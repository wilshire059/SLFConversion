// SLFProceduralCaveManager.cpp
// Single CellFlow + voxel dungeon at runtime.
// Replaces SnapMap nested dungeon architecture.

#include "SLFProceduralCaveManager.h"

#if WITH_EDITOR
#include "Core/Dungeon.h"
#include "Core/DungeonConfig.h"
#include "Core/DungeonBuilder.h"
#include "Core/Volumes/DAVoxelVolume.h"

// CellFlow
#include "Builders/CellFlow/CellFlowBuilder.h"
#include "Builders/CellFlow/CellFlowConfig.h"
#include "Builders/CellFlow/CellFlowAsset.h"
#include "Builders/CellFlow/CellFlowModel.h"

// Layout graph — node query
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/CellFlowLayoutGraph.h"
#include "Frameworks/Flow/Domains/LayoutGraph/Core/FlowAbstractGraph.h"
#include "Frameworks/Flow/Domains/LayoutGraph/Core/FlowAbstractNode.h"

// Voxel + Theme
#include "Frameworks/Voxel/SDFModels/VoxelSDFModel.h"
#include "Frameworks/ThemeEngine/DungeonThemeAsset.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"  // For TActorIterator
#endif // WITH_EDITOR

AProceduralCaveManager::AProceduralCaveManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProceduralCaveManager::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	if (bAutoBuildOnPlay)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this,
			&AProceduralCaveManager::BuildCaveDungeon, 1.0f, false);
		UE_LOG(LogTemp, Warning, TEXT("[CaveDungeon] Auto-build scheduled in 1 second"));
	}
	else if (bHasStoredEntrance)
	{
		// Pre-built dungeon: just teleport player to stored entrance after brief delay
		// (delay lets SaveLoadManager finish so we can override its position)
		FTimerHandle TeleportTimer;
		GetWorldTimerManager().SetTimer(TeleportTimer, [this]()
		{
			TeleportPlayerToEntrance(StoredEntrancePosition);
		}, 0.5f, false);
		UE_LOG(LogTemp, Warning, TEXT("[CaveDungeon] Pre-built cave — teleporting to entrance (%.0f, %.0f, %.0f)"),
			StoredEntrancePosition.X, StoredEntrancePosition.Y, StoredEntrancePosition.Z);
	}
#endif
}

#if WITH_EDITOR

void AProceduralCaveManager::BuildCaveDungeon()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveDungeon] No world!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("=== Building Unified Voxel Cave (CellFlow) ==="));

	// Validate CellFlow asset
	UCellFlowAsset* Flow = CellFlowAsset.LoadSynchronous();
	if (!Flow)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CaveDungeon] CellFlowAsset not assigned!"));
		UE_LOG(LogTemp, Warning, TEXT("  Run: -run=AutoBuildDungeon to generate CF_UnifiedCave"));
		return;
	}

	// ── Spawn single ADungeon ──
	CaveDungeon = World->SpawnActor<ADungeon>(GetActorLocation(), FRotator::ZeroRotator);
	if (!CaveDungeon)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveDungeon] Failed to spawn ADungeon!"));
		return;
	}

	CaveDungeon->SetFolderPath(FName(TEXT("CaveDungeon")));
	CaveDungeon->bDrawDebugData = false; // No "Preview" text

	// Configure CellFlow builder + voxel
	ConfigureDungeon(CaveDungeon);

	// Assign theme — use editor-assigned theme, or fall back to DA bundled themes
	// (matching the working L_Dungeon_01/02/03 code path in SetupOpenWorldCommandlet)
	UDungeonThemeAsset* ThemeToUse = CaveTheme;
	if (!ThemeToUse)
	{
		ThemeToUse = LoadObject<UDungeonThemeAsset>(nullptr,
			TEXT("/Game/DungeonArchitect/Themes/DT_CaveTheme.DT_CaveTheme"));
	}
	if (!ThemeToUse)
	{
		ThemeToUse = LoadObject<UDungeonThemeAsset>(nullptr,
			TEXT("/DungeonArchitect/Showcase/Samples/Themes/Foundry/T_Theme_Founddry.T_Theme_Founddry"));
	}
	if (!ThemeToUse)
	{
		ThemeToUse = LoadObject<UDungeonThemeAsset>(nullptr,
			TEXT("/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleSciFi/DungeonTheme/D_StarterPackTheme.D_StarterPackTheme"));
	}
	if (ThemeToUse)
	{
		CaveDungeon->Themes.Add(ThemeToUse);
		UE_LOG(LogTemp, Warning, TEXT("  Theme: %s"), *ThemeToUse->GetPathName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  WARNING: No theme found! Cave will have no decoration meshes."));
	}

	// Build synchronously
	UE_LOG(LogTemp, Warning, TEXT("  Building dungeon (seed=%d)..."), Seed);
	CaveDungeon->BuildDungeon();

	bool bBuildOk = CaveDungeon->GetBuilder() && CaveDungeon->GetBuilder()->HasBuildSucceeded();
	UE_LOG(LogTemp, Warning, TEXT("  Build: %s"), bBuildOk ? TEXT("OK") : TEXT("FAILED"));

	if (bBuildOk)
	{
		// Force-set rock material on all voxel mesh actors spawned by DA.
		// DA's internal material pipeline may not apply VMS.Material reliably at runtime.
		ForceMaterialOnVoxelMeshes(World);

		OnBuildComplete(CaveDungeon, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveDungeon] Build failed!"));
	}
}

void AProceduralCaveManager::ConfigureDungeon(ADungeon* Dungeon)
{
	if (!Dungeon) return;

	Dungeon->BuilderClass = UCellFlowBuilder::StaticClass();
	Dungeon->CreateBuilderInstance();

	UCellFlowConfig* Config = Cast<UCellFlowConfig>(Dungeon->GetConfig());
	if (!Config)
	{
		Config = NewObject<UCellFlowConfig>(Dungeon);
		Dungeon->Config = Config;
	}

	Config->CellFlow = CellFlowAsset.LoadSynchronous();
	Config->GridSize = GridSize;
	Config->Seed = Seed;
	Config->MaxBuildTimePerFrameMs = 0; // Synchronous

	// Voxel carving
	Dungeon->bCarveVoxels = true;

	FDAVoxelMeshGenerationSettings& VMS = Dungeon->VoxelMeshSettings;
	VMS.VoxelSize = 50.0f;
	VMS.VoxelChunkSize = 32;
	VMS.bEnableCollision = true;
	VMS.WallThickness = 250.0f;
	VMS.bUseGPU = false;
	VMS.UVScale = 150.0f;
	// Clear DA's default VoxelShapeTheme (DA_Voxel_Core_Shape_Cave_Theme) —
	// it injects placeholder "Preview" material visuals at every marker position.
	// Our own CaveTheme handles all visual markers.
	VMS.VoxelShapeTheme.Reset();

	// Rock material — try multiple paths
	UMaterialInterface* RockMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/Wasteland/Materials/Material_Instances/MI_Rock_03.MI_Rock_03"));
	if (!RockMat)
	{
		UE_LOG(LogTemp, Warning, TEXT("  MI_Rock_03 not found, trying MI_Rock_01..."));
		RockMat = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/WastelandEnvironment/Materials/Material_Instances/MI_Rock_01.MI_Rock_01"));
	}
	if (!RockMat)
	{
		UE_LOG(LogTemp, Warning, TEXT("  MI_Rock_01 not found, trying Wasteland MI_Rock_01..."));
		RockMat = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/Wasteland/Materials/Material_Instances/MI_Rock_01.MI_Rock_01"));
	}
	if (!RockMat)
	{
		// Fallback: use engine basic material
		UE_LOG(LogTemp, Warning, TEXT("  No Wasteland rock material found! Using engine fallback."));
		RockMat = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}
	UE_LOG(LogTemp, Warning, TEXT("  Voxel Material: %s"), RockMat ? *RockMat->GetPathName() : TEXT("NULL — 'Preview' material will show!"));
	if (RockMat)
	{
		VMS.Material = RockMat;
	}

	// Noise
	FDAVoxelNoiseSettings& VNS = Dungeon->VoxelNoiseSettings;
	VNS.NoiseAmplitude = 400.0f;
	VNS.NoiseOctaves = 5;
	VNS.NoiseFloorScale = 0.15f;
	VNS.NoiseCeilingScale = 0.85f;
	VNS.bEnableDomainWarp = true;
	VNS.DomainWarpStrength = 0.25f;
	VNS.NoiseScaleVector = FVector(1, 1, 0.7f);

	// SDF cave model
	UDungeonVoxelSDFModel_Cave* CaveModel = NewObject<UDungeonVoxelSDFModel_Cave>(Dungeon);
	CaveModel->CeilingExtraHeight = 1200.0f;
	CaveModel->bEnableCeilingHoles = true;
	Dungeon->VoxelSDFModel = CaveModel;

	UE_LOG(LogTemp, Warning, TEXT("  Configured: CellFlow + voxel (bCarveVoxels=true, bUseGPU=false, VoxelShapeTheme=cleared)"));
}

void AProceduralCaveManager::ForceMaterialOnVoxelMeshes(UWorld* World)
{
	if (!World) return;

	// Load rock material
	UMaterialInterface* RockMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/Wasteland/Materials/Material_Instances/MI_Rock_03.MI_Rock_03"));
	if (!RockMat)
	{
		RockMat = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/WastelandEnvironment/Materials/Material_Instances/MI_Rock_01.MI_Rock_01"));
	}
	if (!RockMat)
	{
		RockMat = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/Wasteland/Materials/Material_Instances/MI_Rock_01.MI_Rock_01"));
	}
	if (!RockMat)
	{
		UE_LOG(LogTemp, Warning, TEXT("  ForceMaterial: No rock material found!"));
		return;
	}

	// DA tags all voxel chunk actors with "da_voxel_mesh"
	// Iterate all actors with that tag and force-set material on their mesh components
	int32 Count = 0;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || !Actor->ActorHasTag(TEXT("da_voxel_mesh")))
		{
			continue;
		}

		TArray<UMeshComponent*> MeshComps;
		Actor->GetComponents<UMeshComponent>(MeshComps);
		for (UMeshComponent* MC : MeshComps)
		{
			if (MC)
			{
				MC->SetMaterial(0, RockMat);
				Count++;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("  ForceMaterial: Applied %s to %d voxel mesh components"),
		*RockMat->GetName(), Count);
}

void AProceduralCaveManager::OnBuildComplete(ADungeon* Dungeon, bool bSuccess)
{
	if (!bSuccess || !Dungeon) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector EntrancePos = FVector::ZeroVector;
	FVector BossPos = FVector::ZeroVector;
	bool bFoundEntrance = false;
	bool bFoundBoss = false;

	QueryPathPositions(Dungeon, EntrancePos, BossPos, bFoundEntrance, bFoundBoss);

	// Spawn boss arena volume
	if (bFoundBoss)
	{
		SpawnBossArenaVolume(World, BossPos);
	}

	// Teleport player to entrance
	if (bFoundEntrance)
	{
		TeleportPlayerToEntrance(EntrancePos);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[CaveDungeon] No entrance found — player stays at spawn"));
	}

	UE_LOG(LogTemp, Warning, TEXT("=== Unified Voxel Cave COMPLETE ==="));
}

void AProceduralCaveManager::QueryPathPositions(ADungeon* Dungeon,
	FVector& OutEntrance, FVector& OutBoss,
	bool& bFoundEntrance, bool& bFoundBoss)
{
	bFoundEntrance = false;
	bFoundBoss = false;

	UCellFlowModel* Model = Cast<UCellFlowModel>(Dungeon->GetModel());
	if (!Model || !Model->LayoutGraph)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No CellFlowModel/LayoutGraph to query"));
		return;
	}

	UCellFlowConfig* Config = Cast<UCellFlowConfig>(Dungeon->GetConfig());
	FVector ConfigGridSize = Config ? Config->GridSize : GridSize;

	for (UFlowAbstractNode* Node : Model->LayoutGraph->GraphNodes)
	{
		if (!Node || !Node->bActive) continue;
		FVector WorldPos = Node->Coord * ConfigGridSize;

		// Entrance: main path start node
		if (Node->PathName == TEXT("main_start") ||
			(Node->PathName == TEXT("main") && Node->PathIndex == 0))
		{
			OutEntrance = WorldPos;
			bFoundEntrance = true;
			UE_LOG(LogTemp, Warning, TEXT("  Entrance: path='%s' idx=%d coord=(%.1f,%.1f,%.1f) world=(%.0f,%.0f,%.0f)"),
				*Node->PathName, Node->PathIndex,
				Node->Coord.X, Node->Coord.Y, Node->Coord.Z,
				WorldPos.X, WorldPos.Y, WorldPos.Z);
		}

		// Boss: end of boss path or named boss_arena
		if (Node->PathName == TEXT("boss_arena") ||
			(Node->PathName == TEXT("boss") && Node->PathIndex == Node->PathLength - 1))
		{
			OutBoss = WorldPos;
			bFoundBoss = true;
			UE_LOG(LogTemp, Warning, TEXT("  Boss arena: path='%s' idx=%d coord=(%.1f,%.1f,%.1f) world=(%.0f,%.0f,%.0f)"),
				*Node->PathName, Node->PathIndex,
				Node->Coord.X, Node->Coord.Y, Node->Coord.Z,
				WorldPos.X, WorldPos.Y, WorldPos.Z);
		}
	}
}

void AProceduralCaveManager::SpawnBossArenaVolume(UWorld* World, const FVector& BossPos)
{
	ADAVoxelVolume* BossVolume = World->SpawnActor<ADAVoxelVolume>(
		BossPos + FVector(0, 0, 200), FRotator::ZeroRotator);
	if (!BossVolume) return;

	BossVolume->BoxExtent = BossArenaExtent;
	BossVolume->bSubtract = false;
	BossVolume->Priority = 50;
	BossVolume->bOverrideNoiseSettings = true;
	BossVolume->NoiseSettingsOverride.NoiseAmplitude = 50.0f;
	BossVolume->NoiseSettingsOverride.NoiseFloorScale = 0.02f;
	BossVolume->NoiseSettingsOverride.NoiseCeilingScale = 0.3f;
	BossVolume->NoiseSettingsOverride.NoiseOctaves = 2;
	BossVolume->NoiseSettingsOverride.bEnableDomainWarp = false;
	BossVolume->MaterialID = 1;
	BossVolume->SetFolderPath(FName(TEXT("CaveDungeon/BossArena")));

	UE_LOG(LogTemp, Warning, TEXT("  Boss ADAVoxelVolume at (%.0f,%.0f,%.0f) extent=(%.0f,%.0f,%.0f)"),
		BossPos.X, BossPos.Y, BossPos.Z + 200,
		BossArenaExtent.X, BossArenaExtent.Y, BossArenaExtent.Z);
}

void AProceduralCaveManager::TeleportPlayerToEntrance(const FVector& EntrancePos)
{
	UWorld* World = GetWorld();
	if (!World) return;

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (PlayerChar)
	{
		FVector SpawnPos = EntrancePos + FVector(0, 0, 200); // Above floor for capsule
		PlayerChar->SetActorLocation(SpawnPos, false, nullptr, ETeleportType::ResetPhysics);
		UE_LOG(LogTemp, Warning, TEXT("  Teleported player to entrance (%.0f, %.0f, %.0f)"),
			SpawnPos.X, SpawnPos.Y, SpawnPos.Z);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No player pawn to teleport (entrance: %.0f, %.0f, %.0f)"),
			EntrancePos.X, EntrancePos.Y, EntrancePos.Z);
	}
}

#else // !WITH_EDITOR

void AProceduralCaveManager::BuildCaveDungeon()
{
	UE_LOG(LogTemp, Warning, TEXT("[CaveDungeon] BuildCaveDungeon requires editor (WITH_EDITOR)."));
}

#endif // WITH_EDITOR
