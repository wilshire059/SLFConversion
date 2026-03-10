// SLFCaveMarkerEmitter.cpp

#include "SLFCaveMarkerEmitter.h"
#include "Core/DungeonBuilder.h"
#include "Core/DungeonModel.h"
#include "Core/DungeonConfig.h"
#include "Core/DungeonQuery.h"
#include "Builders/CellFlow/CellFlowModel.h"
#include "Builders/CellFlow/CellFlowConfig.h"
#include "Frameworks/FlowImpl/CellFlow/Lib/CellFlowLib.h"

USLFCaveMarkerEmitter::USLFCaveMarkerEmitter()
{
	ExecutionStage = EDungeonMarkerEmitterExecStage::AfterPatternMatcher;
}

void USLFCaveMarkerEmitter::EmitMarkers_Implementation(UDungeonBuilder* Builder, UDungeonModel* Model,
	UDungeonConfig* Config, UDungeonQuery* Query)
{
	if (!Builder || !Model) return;

	UCellFlowModel* CellModel = Cast<UCellFlowModel>(Model);
	if (!CellModel || !CellModel->CellGraph)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLFCaveMarkerEmitter: No CellFlowModel/CellGraph — emitting fallback markers"));
		return;
	}

	UDAFlowCellGraph* CellGraph = CellModel->CellGraph;
	FRandomStream Rng(FPlatformTime::Cycles());

	// Iterate over group nodes (rooms) and emit decoration markers
	for (const FDAFlowCellGroupNode& Group : CellGraph->GroupNodes)
	{
		if (!Group.IsActive()) continue;

		// Compute room center and approximate radius from leaf nodes
		FVector2D RoomCenter2D = FVector2D::ZeroVector;
		float TotalArea = 0.0f;
		int32 LeafCount = 0;

		for (int32 LeafIdx : Group.LeafNodes)
		{
			if (LeafIdx >= 0 && LeafIdx < CellGraph->LeafNodes.Num())
			{
				UDAFlowCellLeafNode* Leaf = CellGraph->LeafNodes[LeafIdx];
				if (Leaf)
				{
					RoomCenter2D += FVector2d(Leaf->GetCenter());
					TotalArea += Leaf->GetArea();
					LeafCount++;
				}
			}
		}

		if (LeafCount == 0) continue;
		RoomCenter2D /= (float)LeafCount;

		// Convert cell-space center to world space using grid size
		FVector RoomCenter(RoomCenter2D.X * GridSize.X, RoomCenter2D.Y * GridSize.Y,
			(float)Group.GroupHeight * GridSize.Z);
		float ApproxRadius = FMath::Sqrt(TotalArea) * GridSize.X * 0.5f;
		float CeilingZ = RoomCenter.Z + (float)Group.GroupHeight * GridSize.Z;
		float FloorZ = RoomCenter.Z;

		// Determine room type from layout node color/path
		bool bIsEntrance = (Group.GroupId == 0); // First group is typically entrance
		bool bIsBoss = Group.GroupColor.Equals(FLinearColor::Red, 0.3f);

		// ── Stalactites (ceiling) ──
		for (int32 i = 0; i < StalactitesPerRoom; i++)
		{
			float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
			float Dist = ApproxRadius * Rng.FRandRange(0.1f, 0.8f);
			FVector Pos(
				RoomCenter.X + FMath::Cos(Angle) * Dist,
				RoomCenter.Y + FMath::Sin(Angle) * Dist,
				CeilingZ - Rng.FRandRange(0.0f, 100.0f)
			);
			FRotator Rot(180.0f, Rng.FRandRange(0.0f, 360.0f), 0.0f);
			Builder->EmitMarker(TEXT("Stalactite"), FTransform(Rot, Pos));
		}

		// ── Stalagmites (floor near walls) ──
		for (int32 i = 0; i < StalagmitesPerRoom; i++)
		{
			float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
			float Dist = ApproxRadius * Rng.FRandRange(0.5f, 0.9f); // Near walls
			FVector Pos(
				RoomCenter.X + FMath::Cos(Angle) * Dist,
				RoomCenter.Y + FMath::Sin(Angle) * Dist,
				FloorZ
			);
			FRotator Rot(0.0f, Rng.FRandRange(0.0f, 360.0f), 0.0f);
			Builder->EmitMarker(TEXT("Stalagmite"), FTransform(Rot, Pos));
		}

		// ── Mushroom clusters (near water, floor) ──
		for (int32 i = 0; i < MushroomClustersPerRoom; i++)
		{
			float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
			float Dist = ApproxRadius * Rng.FRandRange(0.2f, 0.6f);
			FVector Pos(
				RoomCenter.X + FMath::Cos(Angle) * Dist,
				RoomCenter.Y + FMath::Sin(Angle) * Dist,
				FloorZ
			);
			Builder->EmitMarker(TEXT("MushroomCluster"), FTransform(FRotator::ZeroRotator, Pos));
		}

		// ── Cave torches (room perimeter) ──
		for (int32 i = 0; i < TorchesPerRoom; i++)
		{
			float Angle = (float)i * (2.0f * PI / (float)TorchesPerRoom) + Rng.FRandRange(-0.3f, 0.3f);
			float Dist = ApproxRadius * 0.85f;
			FVector Pos(
				RoomCenter.X + FMath::Cos(Angle) * Dist,
				RoomCenter.Y + FMath::Sin(Angle) * Dist,
				RoomCenter.Z + Rng.FRandRange(50.0f, 200.0f)
			);
			Builder->EmitMarker(TEXT("CaveTorch"), FTransform(FRotator::ZeroRotator, Pos));
		}

		// ── Hanging vines (ceiling edges, doorways) ──
		for (int32 i = 0; i < HangingVinesPerRoom; i++)
		{
			float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
			float Dist = ApproxRadius * Rng.FRandRange(0.6f, 0.95f);
			FVector Pos(
				RoomCenter.X + FMath::Cos(Angle) * Dist,
				RoomCenter.Y + FMath::Sin(Angle) * Dist,
				CeilingZ - Rng.FRandRange(0.0f, 50.0f)
			);
			Builder->EmitMarker(TEXT("HangingVines"), FTransform(FRotator::ZeroRotator, Pos));
		}

		// ── Wall growth (wall surfaces) ──
		for (int32 i = 0; i < WallGrowthPerRoom; i++)
		{
			float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
			float Dist = ApproxRadius * Rng.FRandRange(0.7f, 0.95f);
			float Z = RoomCenter.Z + Rng.FRandRange(0.0f, (CeilingZ - FloorZ) * 0.7f);
			FVector Pos(
				RoomCenter.X + FMath::Cos(Angle) * Dist,
				RoomCenter.Y + FMath::Sin(Angle) * Dist,
				Z
			);
			Builder->EmitMarker(TEXT("WallGrowth"), FTransform(FRotator::ZeroRotator, Pos));
		}

		// ── Floor debris (random floor) ──
		for (int32 i = 0; i < FloorDebrisPerRoom; i++)
		{
			float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
			float Dist = ApproxRadius * Rng.FRandRange(0.1f, 0.8f);
			FVector Pos(
				RoomCenter.X + FMath::Cos(Angle) * Dist,
				RoomCenter.Y + FMath::Sin(Angle) * Dist,
				FloorZ
			);
			FRotator Rot(0.0f, Rng.FRandRange(0.0f, 360.0f), 0.0f);
			Builder->EmitMarker(TEXT("FloorDebris"), FTransform(Rot, Pos));
		}

		// ── Ceiling moss (ceiling patches) ──
		for (int32 i = 0; i < CeilingMossPerRoom; i++)
		{
			float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
			float Dist = ApproxRadius * Rng.FRandRange(0.1f, 0.7f);
			FVector Pos(
				RoomCenter.X + FMath::Cos(Angle) * Dist,
				RoomCenter.Y + FMath::Sin(Angle) * Dist,
				CeilingZ
			);
			Builder->EmitMarker(TEXT("CeilingMoss"), FTransform(FRotator::ZeroRotator, Pos));
		}

		// ── Water puddle (40% chance, lowest rooms) ──
		if (Rng.FRandRange(0.0f, 1.0f) < WaterPuddleChance)
		{
			FVector PuddlePos = RoomCenter + FVector(
				Rng.FRandRange(-ApproxRadius * 0.3f, ApproxRadius * 0.3f),
				Rng.FRandRange(-ApproxRadius * 0.3f, ApproxRadius * 0.3f),
				0.0f // Floor level
			);
			PuddlePos.Z = FloorZ;
			Builder->EmitMarker(TEXT("WaterPuddle"), FTransform(FRotator::ZeroRotator, PuddlePos));

			// Paired drip above puddle
			FVector DripPos = PuddlePos;
			DripPos.Z = CeilingZ - 50.0f;
			Builder->EmitMarker(TEXT("CaveDrip"), FTransform(FRotator::ZeroRotator, DripPos));
		}

		// ── Cave smoke (50% chance) ──
		if (Rng.FRandRange(0.0f, 1.0f) < CaveSmokeChance)
		{
			Builder->EmitMarker(TEXT("CaveSmoke"), FTransform(FRotator::ZeroRotator, RoomCenter));
		}

		// ── Special room lights ──
		if (bIsBoss)
		{
			// Boss room: center + 4 cardinal lights
			Builder->EmitMarker(TEXT("BossRoomLight"), FTransform(FRotator::ZeroRotator, RoomCenter));
			for (int32 i = 0; i < 4; i++)
			{
				float Angle = (float)i * PI * 0.5f;
				FVector LightPos = RoomCenter + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0) * ApproxRadius * 0.6f;
				Builder->EmitMarker(TEXT("BossRoomLight"), FTransform(FRotator::ZeroRotator, LightPos));
			}
		}
		else if (bIsEntrance)
		{
			Builder->EmitMarker(TEXT("EntranceLight"), FTransform(FRotator::ZeroRotator,
				RoomCenter + FVector(0, 0, 100)));
			Builder->EmitMarker(TEXT("EntranceLight"), FTransform(FRotator::ZeroRotator,
				RoomCenter + FVector(ApproxRadius * 0.5f, 0, 100)));
		}

		// ── Cave pillars (large rooms only) ──
		if (ApproxRadius > 800.0f)
		{
			int32 NumPillars = FMath::Clamp(FMath::RoundToInt32(ApproxRadius / 500.0f), 1, 3);
			for (int32 i = 0; i < NumPillars; i++)
			{
				float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
				float Dist = ApproxRadius * Rng.FRandRange(0.2f, 0.5f);
				FVector Pos(
					RoomCenter.X + FMath::Cos(Angle) * Dist,
					RoomCenter.Y + FMath::Sin(Angle) * Dist,
					FloorZ
				);
				Builder->EmitMarker(TEXT("CavePillar"), FTransform(FRotator::ZeroRotator, Pos));
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("SLFCaveMarkerEmitter: Emitted decoration markers for %d room groups"),
		CellGraph->GroupNodes.Num());
}
