// SLFCaveEventListener.cpp
// Elden Ring-style cave dungeon layout:
//   Entrance → visible locked boss door → explore branches (enemies/traps/loot)
//   → find lever (deepest room) → shortcut back to entrance → boss fight

#include "SLFCaveEventListener.h"
#include "Core/Dungeon.h"
#include "Core/DungeonModel.h"
#include "Builders/CellFlow/CellFlowModel.h"
#include "Builders/Grid/GridDungeonModel.h"
#include "Frameworks/FlowImpl/CellFlow/Lib/CellFlowLib.h"
#include "Blueprints/Actors/SLFShortcutGate.h"
#include "Blueprints/Actors/SLFTrapBase.h"
#include "Blueprints/Actors/SLFPuzzleMarker.h"
#include "Blueprints/Actors/SLFBossDoor.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PointLight.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PointLightComponent.h"
#include "SLFEnums.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"

void USLFCaveEventListener::OnPostDungeonBuild_Implementation(ADungeon* Dungeon)
{
	UE_LOG(LogTemp, Warning, TEXT("SLFCaveEventListener: OnPostDungeonBuild (Dungeon=%s)"),
		Dungeon ? *Dungeon->GetName() : TEXT("NULL"));

	if (!Dungeon) return;

	UWorld* World = Dungeon->GetWorld();
	if (!World) return;

	FRandomStream Rng(DungeonIndex * 7777 + 42);
	FVector DungeonOrigin = Dungeon->GetActorLocation();

	// ════════════════════════════════════════════════════════════════
	// Build room list from dungeon model (supports Grid and Cell Flow)
	// ════════════════════════════════════════════════════════════════

	struct FRoomInfo
	{
		FVector Center;
		float Radius;
		int32 GroupId;
		int32 RoomIndex;    // Sequential index in Rooms array
		float DepthRatio;   // 0.0 = entrance, 1.0 = deepest
		int32 LeafCount;    // Number of cells in this room
		float TotalArea;    // Total cell area (grid units²)
	};

	TArray<FRoomInfo> Rooms;

	// Try Grid model first, then Cell Flow
	UGridDungeonModel* GridModel = Cast<UGridDungeonModel>(Dungeon->GetModel());
	UCellFlowModel* CellModel = GridModel ? nullptr : Cast<UCellFlowModel>(Dungeon->GetModel());

	if (GridModel)
	{
		// Grid builder: extract rooms from cells with CellType == Room
		FVector GridCellSize(400, 400, 200);  // Must match GridDungeonConfig::GridCellSize

		for (const FGridDungeonCell& Cell : GridModel->Cells)
		{
			if (Cell.CellType != EGridDungeonCellType::Room) continue;

			FRoomInfo Room;
			float CenterX = (Cell.Bounds.Location.X + Cell.Bounds.Size.X * 0.5f) * GridCellSize.X;
			float CenterY = (Cell.Bounds.Location.Y + Cell.Bounds.Size.Y * 0.5f) * GridCellSize.Y;
			Room.Center = DungeonOrigin + FVector(CenterX, CenterY, 0.0f);
			Room.Radius = FMath::Max(Cell.Bounds.Size.X, Cell.Bounds.Size.Y) * GridCellSize.X * 0.5f;
			Room.GroupId = Cell.Id;
			Room.RoomIndex = Rooms.Num();
			Room.DepthRatio = 0.0f;
			Room.LeafCount = Cell.Bounds.Size.X * Cell.Bounds.Size.Y;
			Room.TotalArea = (float)Room.LeafCount;
			Rooms.Add(Room);
		}
		UE_LOG(LogTemp, Warning, TEXT("  Grid model: %d room cells extracted"), Rooms.Num());
	}
	else if (CellModel && CellModel->CellGraph)
	{
		// Cell Flow builder: extract rooms from group nodes
		UDAFlowCellGraph* CellGraph = CellModel->CellGraph;
		FVector GridSize(300, 300, 250);  // Must match commandlet GridSize

		for (int32 GIdx = 0; GIdx < CellGraph->GroupNodes.Num(); GIdx++)
		{
			const FDAFlowCellGroupNode& Group = CellGraph->GroupNodes[GIdx];
			if (!Group.IsActive()) continue;

			FVector2D Center2D = FVector2D::ZeroVector;
			float TotalArea = 0.0f;
			int32 LeafCount = 0;

			for (int32 LeafIdx : Group.LeafNodes)
			{
				if (LeafIdx >= 0 && LeafIdx < CellGraph->LeafNodes.Num())
				{
					UDAFlowCellLeafNode* Leaf = CellGraph->LeafNodes[LeafIdx];
					if (Leaf)
					{
						Center2D += FVector2d(Leaf->GetCenter());
						TotalArea += Leaf->GetArea();
						LeafCount++;
					}
				}
			}

			if (LeafCount == 0) continue;
			Center2D /= (float)LeafCount;

			FRoomInfo Room;
			Room.Center = DungeonOrigin + FVector(Center2D.X * GridSize.X, Center2D.Y * GridSize.Y,
				(float)Group.GroupHeight * GridSize.Z);
			Room.Radius = FMath::Sqrt(TotalArea) * GridSize.X * 0.5f;
			Room.GroupId = Group.GroupId;
			Room.RoomIndex = Rooms.Num();
			Room.DepthRatio = 0.0f;
			Room.LeafCount = LeafCount;
			Room.TotalArea = TotalArea;
			Rooms.Add(Room);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SLFCaveEventListener: No supported model found — skipping"));
		return;
	}

	if (Rooms.Num() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLFCaveEventListener: Only %d rooms — skipping"), Rooms.Num());
		return;
	}

	// Calculate depth ratio: distance from entrance (room 0) along room chain
	const FVector EntrancePos = Rooms[0].Center;
	float MaxDist = 0.0f;
	for (FRoomInfo& Room : Rooms)
	{
		float Dist = FVector::Dist(EntrancePos, Room.Center);
		MaxDist = FMath::Max(MaxDist, Dist);
	}
	if (MaxDist > 0.0f)
	{
		for (FRoomInfo& Room : Rooms)
		{
			Room.DepthRatio = FVector::Dist(EntrancePos, Room.Center) / MaxDist;
		}
	}

	// Identify key rooms
	const int32 EntranceIdx = 0;
	int32 BossIdx = Rooms.Num() - 1;  // Last room = boss arena

	// Find the deepest non-boss room for the lever
	int32 LeverIdx = -1;
	float MaxLeverDepth = 0.0f;
	for (int32 R = 1; R < BossIdx; R++)
	{
		if (Rooms[R].DepthRatio > MaxLeverDepth)
		{
			MaxLeverDepth = Rooms[R].DepthRatio;
			LeverIdx = R;
		}
	}
	if (LeverIdx < 0) LeverIdx = FMath::Max(1, BossIdx - 1);

	UE_LOG(LogTemp, Warning, TEXT("  %d rooms: entrance=%d, lever=%d (depth=%.2f), boss=%d"),
		Rooms.Num(), EntranceIdx, LeverIdx, Rooms[LeverIdx].DepthRatio, BossIdx);

	// ════════════════════════════════════════════════════════════════
	// PlayerStart at entrance room (high above floor so pawn doesn't clip geometry)
	// ════════════════════════════════════════════════════════════════
	{
		FVector PSPos = Rooms[EntranceIdx].Center + FVector(0, 0, 500.0f);
		APlayerStart* PS = World->SpawnActor<APlayerStart>(PSPos, FRotator::ZeroRotator);
		if (PS)
		{
			PS->SetFolderPath(FName(TEXT("Dungeon/Gameplay")));
			UE_LOG(LogTemp, Warning, TEXT("  PlayerStart at (%.0f, %.0f, %.0f)"),
				PSPos.X, PSPos.Y, PSPos.Z);
		}
	}

	// ════════════════════════════════════════════════════════════════
	// TEST MODE: Place text labels in each room (DungeonIndex == 0 only)
	// ════════════════════════════════════════════════════════════════
	if (DungeonIndex == 0)
	{
		for (int32 R = 0; R < Rooms.Num(); R++)
		{
			const FRoomInfo& Room = Rooms[R];
			float WorldSize = Room.Radius * 2.0f;  // Approximate diameter in cm

			FString Label = FString::Printf(TEXT("Room %d\n%d cells\nArea: %.0f\nRadius: %.0f cm\n~%.0fx%.0f m"),
				R, Room.LeafCount, Room.TotalArea, Room.Radius,
				WorldSize / 100.0f, WorldSize / 100.0f);

			FVector TextPos = Room.Center + FVector(0, 0, 200.0f);
			ATextRenderActor* TextActor = World->SpawnActor<ATextRenderActor>(TextPos, FRotator::ZeroRotator);
			if (TextActor)
			{
				UTextRenderComponent* TextComp = TextActor->GetTextRender();
				TextComp->SetText(FText::FromString(Label));
				TextComp->SetWorldSize(50.0f);
				TextComp->SetTextRenderColor(FColor::Yellow);
				TextComp->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
				TextActor->SetFolderPath(FName(TEXT("Dungeon/TestLabels")));

				UE_LOG(LogTemp, Warning, TEXT("  Room %d: %d cells, area=%.0f, radius=%.0f cm (~%.0fx%.0f m)"),
					R, Room.LeafCount, Room.TotalArea, Room.Radius, WorldSize / 100.0f, WorldSize / 100.0f);
			}

			// Bright light so room is visible
			APointLight* Light = World->SpawnActor<APointLight>(Room.Center + FVector(0, 0, 300.0f), FRotator::ZeroRotator);
			if (Light)
			{
				Light->PointLightComponent->SetIntensity(200000.0f);
				Light->PointLightComponent->SetAttenuationRadius(FMath::Max(Room.Radius * 2.0f, 2000.0f));
				Light->PointLightComponent->SetLightColor(FLinearColor(1.0f, 0.9f, 0.7f));  // Warm white
				Light->SetFolderPath(FName(TEXT("Dungeon/TestLabels")));
			}
		}
	}

	// ════════════════════════════════════════════════════════════════
	// 1. Boss fog gate — visible from entrance, LOCKED
	//    Placed between entrance room and boss room (near entrance side)
	// ════════════════════════════════════════════════════════════════
	{
		// Find the room on the main path closest to entrance that leads toward boss
		// Place boss door between room 1 and room 2 (visible early but blocks progress)
		int32 DoorRoomA = EntranceIdx;
		int32 DoorRoomB = FMath::Min(1, BossIdx);

		FVector BossGatePos = Rooms[DoorRoomA].Center * 0.3f + Rooms[BossIdx].Center * 0.7f;
		// Place it between entrance area and the next room toward boss
		if (Rooms.Num() >= 3)
		{
			BossGatePos = (Rooms[DoorRoomB].Center + Rooms[BossIdx].Center) * 0.5f;
		}
		FVector GateDir = (Rooms[BossIdx].Center - Rooms[EntranceIdx].Center).GetSafeNormal();

		ASLFBossDoor* BossGate = World->SpawnActor<ASLFBossDoor>(BossGatePos, GateDir.Rotation());
		if (BossGate)
		{
			BossGate->bRequiresPuzzle = true;
			BossGate->DungeonTier = static_cast<ESLFDungeonTier>(FMath::Min(DungeonIndex, 2));
			BossGate->SetFolderPath(FName(TEXT("Dungeon/BossGates")));
			UE_LOG(LogTemp, Warning, TEXT("  Boss fog gate (locked, tier %d)"), DungeonIndex);
		}
	}

	// ════════════════════════════════════════════════════════════════
	// 3. Lever/puzzle in the deepest non-boss room
	// ════════════════════════════════════════════════════════════════
	{
		ASLFPuzzleMarker* Lever = World->SpawnActor<ASLFPuzzleMarker>(
			Rooms[LeverIdx].Center, FRotator::ZeroRotator);
		if (Lever)
		{
			Lever->PuzzleType = static_cast<ESLFPuzzleType>(0); // Lever type
			Lever->RequiredObjectives = 1; // Single lever pull
			Lever->SetFolderPath(FName(TEXT("Dungeon/Puzzles")));
			UE_LOG(LogTemp, Warning, TEXT("  Lever in room %d (depth %.0f%%)"),
				LeverIdx, Rooms[LeverIdx].DepthRatio * 100.0f);
		}
	}

	// ════════════════════════════════════════════════════════════════
	// 4. Shortcut gate — deep dungeon back to near entrance
	//    The "earned reward" for exploring deep
	// ════════════════════════════════════════════════════════════════
	if (Rooms.Num() >= 5)
	{
		// Find a deep room (>60% depth) that isn't the lever or boss room
		int32 ShortcutFromIdx = -1;
		for (int32 R = Rooms.Num() - 2; R >= 1; R--)
		{
			if (R != LeverIdx && R != BossIdx && Rooms[R].DepthRatio > 0.5f)
			{
				ShortcutFromIdx = R;
				break;
			}
		}
		if (ShortcutFromIdx < 0) ShortcutFromIdx = FMath::Max(1, LeverIdx - 1);

		// Shortcut connects to room near entrance (room 1 or 2)
		int32 ShortcutToIdx = FMath::Min(1, Rooms.Num() - 1);

		FVector GatePos = (Rooms[ShortcutFromIdx].Center + Rooms[ShortcutToIdx].Center) * 0.5f;
		FVector GateDir = (Rooms[ShortcutToIdx].Center - Rooms[ShortcutFromIdx].Center).GetSafeNormal();

		ASLFShortcutGate* Gate = World->SpawnActor<ASLFShortcutGate>(GatePos, GateDir.Rotation());
		if (Gate)
		{
			Gate->SetFolderPath(FName(TEXT("Dungeon/Shortcuts")));
			UE_LOG(LogTemp, Warning, TEXT("  Shortcut: room %d (%.0f%% deep) -> room %d (near entrance)"),
				ShortcutFromIdx, Rooms[ShortcutFromIdx].DepthRatio * 100.0f, ShortcutToIdx);
		}
	}

	// ════════════════════════════════════════════════════════════════
	// 5. Enemies — ambush-style, escalating by depth
	//    Entrance: 0, early rooms: 1-2, deep rooms: 2-3, boss: 1 boss
	// ════════════════════════════════════════════════════════════════
	int32 TotalEnemies = 0;
	for (int32 R = 0; R < Rooms.Num(); R++)
	{
		const FRoomInfo& Room = Rooms[R];

		if (R == EntranceIdx) continue; // Safe zone

		int32 Count;
		if (R == BossIdx)
		{
			Count = 1; // Boss enemy
		}
		else
		{
			// Escalating: 1 enemy in shallow rooms, 2-3 in deeper rooms
			float Difficulty = Room.DepthRatio;
			int32 BaseCount = 1 + FMath::FloorToInt32(Difficulty * 2.0f); // 1-3
			Count = FMath::Clamp(BaseCount + Rng.RandRange(-1, 0), 1, 3);
		}

		for (int32 E = 0; E < Count; E++)
		{
			// Ambush positioning: enemies at room edges (behind pillars, walls)
			float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
			float Dist = Room.Radius * Rng.FRandRange(0.4f, 0.7f); // Near walls, not center
			FVector SpawnPos = Room.Center + FVector(
				FMath::Cos(Angle) * Dist,
				FMath::Sin(Angle) * Dist,
				90.0f
			);

			AActor* Marker = World->SpawnActor(AActor::StaticClass(), &SpawnPos, &FRotator::ZeroRotator);
			if (Marker)
			{
				Marker->Tags.Add(FName(TEXT("EnemySpawnPoint")));
				if (R == BossIdx) Marker->Tags.Add(FName(TEXT("BossEnemy")));
				Marker->SetFolderPath(FName(TEXT("Dungeon/SpawnPoints")));
				TotalEnemies++;
			}
		}
	}

	// ════════════════════════════════════════════════════════════════
	// 6. Dead-end content — each non-main-path room gets something:
	//    treasure marker, ambush, or traps (never empty)
	// ════════════════════════════════════════════════════════════════
	int32 TreasurePlaced = 0, TrapsPlaced = 0;
	for (int32 R = 1; R < Rooms.Num(); R++)
	{
		if (R == BossIdx || R == LeverIdx) continue; // Already have purpose

		const FRoomInfo& Room = Rooms[R];

		// Roll what this room contains (weighted by depth)
		float Roll = Rng.FRandRange(0.0f, 1.0f);
		if (Roll < 0.35f)
		{
			// Treasure dead end — item pickup marker
			FVector TreasurePos = Room.Center + FVector(
				Rng.FRandRange(-Room.Radius * 0.2f, Room.Radius * 0.2f),
				Rng.FRandRange(-Room.Radius * 0.2f, Room.Radius * 0.2f),
				50.0f
			);
			AActor* Treasure = World->SpawnActor(AActor::StaticClass(), &TreasurePos, &FRotator::ZeroRotator);
			if (Treasure)
			{
				Treasure->Tags.Add(FName(TEXT("TreasurePickup")));
				// Scale loot quality by depth
				if (Room.DepthRatio > 0.7f) Treasure->Tags.Add(FName(TEXT("RareLoot")));
				else if (Room.DepthRatio > 0.4f) Treasure->Tags.Add(FName(TEXT("UncommonLoot")));
				else Treasure->Tags.Add(FName(TEXT("CommonLoot")));
				Treasure->SetFolderPath(FName(TEXT("Dungeon/Treasure")));
				TreasurePlaced++;
			}
		}
		else if (Roll < 0.65f)
		{
			// Trap room — 1-2 traps scaling with depth
			int32 TrapCount = 1 + (Room.DepthRatio > 0.5f ? 1 : 0);
			for (int32 TC = 0; TC < TrapCount; TC++)
			{
				float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
				float Dist = Room.Radius * Rng.FRandRange(0.2f, 0.5f);
				FVector TrapPos = Room.Center + FVector(
					FMath::Cos(Angle) * Dist,
					FMath::Sin(Angle) * Dist,
					50.0f
				);

				ASLFTrapBase* Trap = World->SpawnActor<ASLFTrapBase>(TrapPos, FRotator::ZeroRotator);
				if (Trap)
				{
					// Trap type escalates with depth
					int32 MaxType = FMath::Clamp(FMath::FloorToInt32(Room.DepthRatio * 5.0f), 0, 4);
					Trap->TrapType = static_cast<ESLFTrapType>(Rng.RandRange(0, MaxType));
					Trap->Damage = TrapBaseDamage + (DungeonIndex * 15.0f) + (Room.DepthRatio * 20.0f);
					Trap->SetFolderPath(FName(TEXT("Dungeon/Traps")));
					TrapsPlaced++;
				}
			}
		}
		// else: the room already has enemies from step 5 (ambush dead end)
	}

	// ════════════════════════════════════════════════════════════════
	// 7. Lighting — atmospheric, with color progression by depth
	// ════════════════════════════════════════════════════════════════
	int32 LightsPlaced = 0;
	for (int32 R = 0; R < Rooms.Num(); R++)
	{
		const FRoomInfo& Room = Rooms[R];

		FLinearColor LightColor;
		float Intensity;
		float Attenuation;
		int32 NumLights;

		if (R == BossIdx)
		{
			LightColor = FLinearColor(0.4f, 0.2f, 0.6f); // Ominous purple
			Intensity = 150000.0f;
			Attenuation = 4000.0f;
			NumLights = 5;
		}
		else if (R == EntranceIdx)
		{
			LightColor = FLinearColor(0.8f, 0.7f, 0.5f); // Warm amber (safe)
			Intensity = 200000.0f;
			Attenuation = 5000.0f;
			NumLights = 4;
		}
		else if (R == LeverIdx)
		{
			LightColor = FLinearColor(0.6f, 0.5f, 0.2f); // Warm gold (reward)
			Intensity = 120000.0f;
			Attenuation = 3500.0f;
			NumLights = 3;
		}
		else
		{
			// Color shifts from blue-green (shallow) to deeper blue-purple (deep)
			float Depth = Room.DepthRatio;
			LightColor = FLinearColor(
				0.15f + Depth * 0.15f,   // R: 0.15 → 0.30
				0.5f - Depth * 0.25f,    // G: 0.50 → 0.25
				0.4f + Depth * 0.2f      // B: 0.40 → 0.60
			);
			Intensity = 60000.0f + Depth * 30000.0f; // Brighter in deeper areas
			Attenuation = 2500.0f;
			NumLights = FMath::Clamp(FMath::RoundToInt32(Room.Radius / 400.0f), 2, 4);
		}

		// Center light
		FVector CenterLightPos = Room.Center + FVector(0, 0, 250.0f);
		APointLight* CenterLight = World->SpawnActor<APointLight>(CenterLightPos, FRotator::ZeroRotator);
		if (CenterLight)
		{
			CenterLight->PointLightComponent->SetIntensity(Intensity);
			CenterLight->PointLightComponent->SetAttenuationRadius(Attenuation);
			CenterLight->PointLightComponent->SetLightColor(LightColor);
			CenterLight->PointLightComponent->SetCastShadows(R == BossIdx);
			CenterLight->SetFolderPath(FName(TEXT("Dungeon/Lights")));
			LightsPlaced++;
		}

		// Perimeter lights
		for (int32 L = 1; L < NumLights; L++)
		{
			float Angle = (float)L * (2.0f * PI / (float)NumLights) + Rng.FRandRange(-0.3f, 0.3f);
			float Dist = Room.Radius * 0.55f;
			FVector LightPos = Room.Center + FVector(
				FMath::Cos(Angle) * Dist,
				FMath::Sin(Angle) * Dist,
				Rng.FRandRange(100.0f, 350.0f)
			);

			APointLight* PL = World->SpawnActor<APointLight>(LightPos, FRotator::ZeroRotator);
			if (PL)
			{
				PL->PointLightComponent->SetIntensity(Intensity * 0.7f);
				PL->PointLightComponent->SetAttenuationRadius(Attenuation * 0.8f);
				PL->PointLightComponent->SetLightColor(LightColor);
				PL->PointLightComponent->SetCastShadows(false);
				PL->SetFolderPath(FName(TEXT("Dungeon/Lights")));
				LightsPlaced++;
			}
		}
	}

	// ════════════════════════════════════════════════════════════════
	// 8. Atmospheric fog
	// ════════════════════════════════════════════════════════════════
	{
		FVector FogPos = DungeonOrigin + FVector(0, 0, -200);
		AExponentialHeightFog* DungeonFog = World->SpawnActor<AExponentialHeightFog>(FogPos, FRotator::ZeroRotator);
		if (DungeonFog)
		{
			DungeonFog->GetComponent()->SetFogDensity(0.025f);
			DungeonFog->GetComponent()->SetFogHeightFalloff(0.12f);
			DungeonFog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.08f, 0.12f, 0.18f));
			DungeonFog->GetComponent()->SetStartDistance(100.0f);
			DungeonFog->SetFolderPath(FName(TEXT("Dungeon/Atmosphere")));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("SLFCaveEventListener: dungeon %d — %d rooms, %d enemies, %d treasure, %d traps, %d lights"),
		DungeonIndex + 1, Rooms.Num(), TotalEnemies, TreasurePlaced, TrapsPlaced, LightsPlaced);
}
