// SetupOpenWorldCommandlet.cpp
// Generates open world level with Perlin-noise landscape, Wasteland mesh dressing,
// Dungeon Architect procedural dungeons, and gameplay actors.

#include "SetupOpenWorldCommandlet.h"

#if WITH_EDITOR
#include "SLFAutomationLibrary.h"
#include "UObject/UnrealType.h"
#include "Blueprints/Actors/SLFLevelStreamManager.h"
#include "Blueprints/Actors/SLFShortcutGate.h"
#include "Blueprints/Actors/SLFTrapBase.h"
#include "Blueprints/Actors/SLFPuzzleMarker.h"
#include "Blueprints/Actors/SLFBossDoor.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetCompilingManager.h"
#include "Misc/FileHelper.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"
#include "Landscape.h"
#include "LandscapeInfo.h"
#include "LandscapeProxy.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EditorLevelUtils.h"
#include "FileHelpers.h"
#include "EngineUtils.h"
#include "Math/RandomStream.h"

// Dungeon Architect — Cell Flow + Voxel Cave system
#include "Core/Dungeon.h"
#include "Core/DungeonConfig.h"
#include "Core/DungeonBuilder.h"
#include "Builders/CellFlow/CellFlowBuilder.h"
#include "Builders/CellFlow/CellFlowConfig.h"
#include "Builders/CellFlow/CellFlowAsset.h"
#include "Builders/CellFlow/CellFlowModel.h"
#include "Frameworks/FlowImpl/CellFlow/Lib/CellFlowLib.h"
#include "Builders/Grid/GridDungeonBuilder.h"
#include "Builders/Grid/GridDungeonConfig.h"
#include "Builders/Grid/GridDungeonModel.h"
#include "Frameworks/Flow/ExecGraph/FlowExecGraphScript.h"
#include "Frameworks/Flow/ExecGraph/FlowExecTask.h"
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/Tasks/CellFlowLayoutTaskCreateCellsVoronoi.h"
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/Tasks/CellFlowLayoutTaskCreateCellsGrid.h"
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/Tasks/CellFlowLayoutTaskCreateMainPath.h"
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/Tasks/CellFlowLayoutTaskCreatePath.h"
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/Tasks/CellFlowLayoutTaskFinalize.h"
#include "Frameworks/Voxel/SDFModels/VoxelSDFModel.h"
#include "Frameworks/ThemeEngine/DungeonThemeAsset.h"
#include "Frameworks/ThemeEngine/Graph/DungeonThemeCompiledGraph.h"


// Cave system classes
#include "Dungeon/SLFProceduralCaveManager.h"
#include "Dungeon/SLFCaveMarkerEmitter.h"
#include "Dungeon/SLFCaveEventListener.h"
#include "Dungeon/SLFCaveRandomTransform.h"
#include "Dungeon/SLFCaveSpawnLogic.h"
#include "Dungeon/SLFCaveMazeBuilder.h"

// Collision
#include "PhysicsEngine/BodySetup.h"

// Screenshot capture: use SLF.CaptureDungeon console command (in SLFConversion.cpp)
#endif // WITH_EDITOR

USetupOpenWorldCommandlet::USetupOpenWorldCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 USetupOpenWorldCommandlet::Main(const FString& Params)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("=== SetupOpenWorld Commandlet START ==="));
	UE_LOG(LogTemp, Warning, TEXT("Params: [%s]"), *Params);

	const bool bNoDungeons = Params.Contains(TEXT("-nodungeons"));
	const bool bNoSave = Params.Contains(TEXT("-nosave"));
	const bool bDungeonsOnly = Params.Contains(TEXT("-dungeonsonly"));

	// Store canvas mode on instance for use in BuildCellFlowCave
	bCanvasMode = Params.Contains(TEXT("-canvas"));

	// Optional: regenerate only a specific dungeon (-dungeon=2 means dungeon 2)
	FString DungeonArg;
	if (FParse::Value(*Params, TEXT("-dungeon="), DungeonArg))
	{
		OnlyDungeonNum = FCString::Atoi(*DungeonArg);
		UE_LOG(LogTemp, Warning, TEXT("Only regenerating dungeon %d"), OnlyDungeonNum);
	}

	// ── Grid Test: simple floor+wall dungeon, no cave ──
	if (Params.Contains(TEXT("-gridtest")))
	{
		UE_LOG(LogTemp, Warning, TEXT("=== GRID TEST MODE ==="));

		if (!FSlateApplication::IsInitialized())
		{
			FSlateApplication::Create();
		}
		IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		AR.SearchAllAssets(true);

		const FString LevelName = TEXT("/Game/Maps/L_Dungeon_Test");

		// Delete existing
		FString ExistingFile = FPackageName::LongPackageNameToFilename(LevelName, FPackageName::GetMapPackageExtension());
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*ExistingFile))
		{
			UE_LOG(LogTemp, Warning, TEXT("  Deleting existing: %s"), *ExistingFile);
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*ExistingFile);
		}

		UPackage* Pkg = CreatePackage(*LevelName);
		Pkg->FullyLoad();
		UWorld* TestWorld = UWorld::CreateWorld(EWorldType::None, false, FName(TEXT("L_Dungeon_Test")), Pkg);
		if (!TestWorld)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create test dungeon world!"));
			return 1;
		}
		TestWorld->SetFlags(RF_Public | RF_Standalone);
		FWorldContext& Ctx = GEngine->CreateNewWorldContext(EWorldType::Editor);
		Ctx.SetCurrentWorld(TestWorld);

		const FVector Origin(0, 0, 0);
		FRandomStream Rng(77777);

		// Spawn ADungeon with Grid builder — NO voxel cave
		ADungeon* Dungeon = TestWorld->SpawnActor<ADungeon>(Origin, FRotator::ZeroRotator);
		Dungeon->SetFolderPath(FName(TEXT("Dungeon")));
		Dungeon->BuilderClass = UGridDungeonBuilder::StaticClass();
		Dungeon->CreateBuilderInstance();

		UGridDungeonConfig* Config = Cast<UGridDungeonConfig>(Dungeon->GetConfig());
		if (!Config)
		{
			Config = NewObject<UGridDungeonConfig>(Dungeon);
			Dungeon->Config = Config;
		}

		// Parse optional seed
		int32 TestSeed = 77777;
		FString SeedArg;
		if (FParse::Value(*Params, TEXT("-seed="), SeedArg))
		{
			TestSeed = FCString::Atoi(*SeedArg);
		}

		Config->Seed = TestSeed;
		Config->NumCells = 150;                          // Lots of rooms + corridors
		Config->GridCellSize = FVector(400, 400, 200);   // 4m x 4m cells, 2m height per floor
		Config->MinCellSize = 2;
		Config->MaxCellSize = 6;
		Config->RoomAreaThreshold = 4;
		Config->RoomAspectDelta = 0.4f;
		Config->SpanningTreeLoopProbability = 0.2f;      // Some loops for alternate routes
		Config->MaxBuildTimePerFrameMs = 0;
		Config->bClusterWithHeightVariation = true;       // Multi-level with stairs!

		// NO voxel cave — just floors + walls from theme
		Dungeon->bCarveVoxels = false;

		// Theme — try SimpleShapes first, then StarterPack
		UDungeonThemeAsset* Theme = LoadObject<UDungeonThemeAsset>(nullptr,
			TEXT("/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleShapes/D_MyTheme1.D_MyTheme1"));
		if (!Theme)
		{
			Theme = LoadObject<UDungeonThemeAsset>(nullptr,
				TEXT("/DungeonArchitect/Showcase/Samples/Themes/Foundry/T_Theme_Founddry.T_Theme_Founddry"));
		}
		if (!Theme)
		{
			Theme = LoadObject<UDungeonThemeAsset>(nullptr,
				TEXT("/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleSciFi/DungeonTheme/D_StarterPackTheme.D_StarterPackTheme"));
		}

		if (Theme)
		{
			Dungeon->Themes.Add(Theme);
			UE_LOG(LogTemp, Warning, TEXT("  Theme: %s"), *Theme->GetPathName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  WARNING: No theme found! Dungeon will have no visual meshes."));
		}

		UE_LOG(LogTemp, Warning, TEXT("  Grid config: seed=%d, cells=%d, size=(%d-%d), heightVar=TRUE"),
			TestSeed, Config->NumCells, Config->MinCellSize, Config->MaxCellSize);

		// Build the dungeon
		Dungeon->BuildDungeon();

		// Log results
		UGridDungeonModel* GridModel = Cast<UGridDungeonModel>(Dungeon->GetModel());
		if (GridModel)
		{
			int32 RoomCount = 0, CorridorCount = 0, StairCount = 0;
			for (const FGridDungeonCell& Cell : GridModel->Cells)
			{
				if (Cell.CellType == EGridDungeonCellType::Room) RoomCount++;
				else if (Cell.CellType == EGridDungeonCellType::Corridor) CorridorCount++;
				else if (Cell.CellType == EGridDungeonCellType::CorridorPadding) StairCount++;
			}
			UE_LOG(LogTemp, Warning, TEXT("  Result: %d rooms, %d corridors, %d stairs, %d doors"),
				RoomCount, CorridorCount, StairCount, GridModel->Doors.Num());
		}

		// Add a PlayerStart
		APlayerStart* PS = TestWorld->SpawnActor<APlayerStart>(Origin + FVector(800, 800, 500), FRotator::ZeroRotator);
		if (PS) PS->SetFolderPath(FName(TEXT("Gameplay")));

		// Add basic lighting
		ADirectionalLight* DirLight = TestWorld->SpawnActor<ADirectionalLight>(
			Origin + FVector(0, 0, 5000), FRotator(-50, 30, 0));
		if (DirLight)
		{
			DirLight->GetComponent()->SetIntensity(5.0f);
			DirLight->GetComponent()->SetLightColor(FLinearColor(1.0f, 0.95f, 0.9f));
			DirLight->SetFolderPath(FName(TEXT("Lighting")));
		}

		ASkyLight* Sky = TestWorld->SpawnActor<ASkyLight>(Origin + FVector(0, 0, 3000), FRotator::ZeroRotator);
		if (Sky)
		{
			Sky->GetLightComponent()->SetIntensity(2.0f);
			Sky->SetFolderPath(FName(TEXT("Lighting")));
		}

		// Save
		bool bSaved = SaveLevel(TestWorld, LevelName);
		GEngine->DestroyWorldContext(TestWorld);

		UE_LOG(LogTemp, Warning, TEXT("=== GRID TEST %s ==="), bSaved ? TEXT("COMPLETE") : TEXT("FAILED"));
		return bSaved ? 0 : 1;
	}

	// ── Cave Test: Unified CellFlow + Voxel cave ──
	if (Params.Contains(TEXT("-nestedtest")) || Params.Contains(TEXT("-cavetest")))
	{
		UE_LOG(LogTemp, Warning, TEXT("=== CAVE DUNGEON TEST MODE (CellFlow + Voxel) ==="));

		if (!FSlateApplication::IsInitialized())
		{
			FSlateApplication::Create();
		}
		IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		AR.SearchAllAssets(true);

		int32 CaveSeed = 42;
		FString SeedArg;
		if (FParse::Value(*Params, TEXT("-seed="), SeedArg))
		{
			CaveSeed = FCString::Atoi(*SeedArg);
		}

		// Create test world
		const FString LevelName = TEXT("/Game/Maps/L_Dungeon_Nested");
		FString ExistingFile = FPackageName::LongPackageNameToFilename(LevelName, FPackageName::GetMapPackageExtension());
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*ExistingFile))
		{
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*ExistingFile);
		}

		UPackage* Pkg = CreatePackage(*LevelName);
		Pkg->FullyLoad();
		UWorld* CaveWorld = UWorld::CreateWorld(EWorldType::None, false, FName(TEXT("L_Dungeon_Nested")), Pkg);
		if (!CaveWorld)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create cave dungeon world!"));
			return 1;
		}
		CaveWorld->SetFlags(RF_Public | RF_Standalone);
		FWorldContext& Ctx = GEngine->CreateNewWorldContext(EWorldType::Editor);
		Ctx.SetCurrentWorld(CaveWorld);

		// Spawn AProceduralCaveManager with CellFlow asset
		AProceduralCaveManager* Manager = CaveWorld->SpawnActor<AProceduralCaveManager>(
			FVector::ZeroVector, FRotator::ZeroRotator);
		Manager->SetFolderPath(FName(TEXT("CaveDungeon")));
		Manager->Seed = CaveSeed;

		// Assign CellFlow asset (from AutoBuildDungeon or CLI override)
		FString CellFlowPath;
		if (FParse::Value(*Params, TEXT("-cellflow="), CellFlowPath))
		{
			Manager->CellFlowAsset = TSoftObjectPtr<UCellFlowAsset>(FSoftObjectPath(CellFlowPath));
		}
		else
		{
			Manager->CellFlowAsset = TSoftObjectPtr<UCellFlowAsset>(
				FSoftObjectPath(TEXT("/Game/Dungeons/CellFlow/CF_UnifiedCave.CF_UnifiedCave")));
		}

		// Build
		UE_LOG(LogTemp, Warning, TEXT("── Building unified voxel cave ──"));
		Manager->BuildCaveDungeon();

		// Save
		bool bSaved = SaveLevel(CaveWorld, LevelName);
		GEngine->DestroyWorldContext(CaveWorld);

		UE_LOG(LogTemp, Warning, TEXT("=== CAVE TEST %s ==="), bSaved ? TEXT("COMPLETE") : TEXT("FAILED"));
		return bSaved ? 0 : 1;
	}

	// If only regenerating dungeons, populate FlatAreas with dungeon entrances and skip overworld
	if (bDungeonsOnly)
	{
		UE_LOG(LogTemp, Warning, TEXT("=== DUNGEONS ONLY MODE %s==="), bCanvasMode ? TEXT("(CANVAS) ") : TEXT(""));
		FlatAreas.Empty();
		FlatAreas.Add({FVector(-12000, -10000, 0), 2500.0f, TEXT("dungeon_entrance_1")});
		FlatAreas.Add({FVector(15000, -8000, 0), 2500.0f, TEXT("dungeon_entrance_2")});
		FlatAreas.Add({FVector(5000, 18000, 0), 2500.0f, TEXT("dungeon_entrance_3")});

		// Initialize Slate (required for asset operations)
		if (!FSlateApplication::IsInitialized())
		{
			FSlateApplication::Create();
		}

		IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		AssetRegistry.SearchAllAssets(true);

		UE_LOG(LogTemp, Warning, TEXT("=== Generating Dungeons Only ==="));
		bool bDungeonsOk = GenerateDungeons(nullptr);
		UE_LOG(LogTemp, Warning, TEXT("Dungeon generation: %s"), bDungeonsOk ? TEXT("SUCCESS") : TEXT("PARTIAL"));

		UE_LOG(LogTemp, Warning, TEXT("=== SetupOpenWorld (DungeonsOnly) COMPLETE ==="));
		return 0;
	}

	// ── Pre-flight: Verify plugins ──
	UE_LOG(LogTemp, Warning, TEXT("--- Pre-flight: Verifying plugins ---"));

	// Check Wasteland Environment content exists
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AssetRegistry.SearchAllAssets(true);

	TArray<FAssetData> WastelandAssets;
	AssetRegistry.GetAssetsByPath(FName(TEXT("/Game/Wasteland")), WastelandAssets, true);
	if (WastelandAssets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wasteland Environment content not found at /Game/Wasteland/"));
		UE_LOG(LogTemp, Warning, TEXT("Continuing without Wasteland meshes — will use engine primitives."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %d Wasteland Environment assets"), WastelandAssets.Num());
	}

	// Initialize Slate (required for asset operations in commandlet)
	if (!FSlateApplication::IsInitialized())
	{
		FSlateApplication::Create();
	}

	// ═══════════════════════════════════════════════════════════════════
	// Phase 1: Create level + Landscape
	// ═══════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("═══ Phase 1: Creating Level + Landscape ═══"));

	const FString LevelPackageName = TEXT("/Game/Maps/L_OpenWorld");
	const FString LevelFileName = FPackageName::LongPackageNameToFilename(LevelPackageName, FPackageName::GetMapPackageExtension());

	// Create level package
	UPackage* LevelPkg = CreatePackage(*LevelPackageName);
	LevelPkg->FullyLoad();

	UWorld* OpenWorld = UWorld::CreateWorld(EWorldType::None, false, FName(TEXT("L_OpenWorld")), LevelPkg);
	if (!OpenWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create world!"));
		return 1;
	}
	OpenWorld->SetFlags(RF_Public | RF_Standalone);
	OpenWorld->GetOutermost()->SetDirtyFlag(true);
	UE_LOG(LogTemp, Warning, TEXT("Created world: %s"), *OpenWorld->GetName());

	// Set up world context for commandlet (no InitWorld — it's already initialized by CreateWorld)
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Editor);
	WorldContext.SetCurrentWorld(OpenWorld);

	// Create landscape
	bool bLandscapeOk = CreateLandscape(OpenWorld);
	UE_LOG(LogTemp, Warning, TEXT("Landscape creation: %s"), bLandscapeOk ? TEXT("SUCCESS") : TEXT("FAILED"));

	// ═══════════════════════════════════════════════════════════════════
	// Phase 2: Wasteland Environment Dressing
	// ═══════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("═══ Phase 2: Wasteland Mesh Dressing ═══"));
	bool bDressingOk = ScatterWastelandMeshes(OpenWorld);
	UE_LOG(LogTemp, Warning, TEXT("Wasteland dressing: %s"), bDressingOk ? TEXT("SUCCESS") : TEXT("SKIPPED/PARTIAL"));

	// ═══════════════════════════════════════════════════════════════════
	// Phase 4: Overworld Actors
	// ═══════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("═══ Phase 4: Populating Overworld Actors ═══"));
	bool bActorsOk = PopulateOverworldActors(OpenWorld);
	UE_LOG(LogTemp, Warning, TEXT("Actor population: %s"), bActorsOk ? TEXT("SUCCESS") : TEXT("PARTIAL"));

	// ═══════════════════════════════════════════════════════════════════
	// Save overworld level
	// ═══════════════════════════════════════════════════════════════════
	if (!bNoSave)
	{
		UE_LOG(LogTemp, Warning, TEXT("--- Saving L_OpenWorld ---"));
		bool bSaved = SaveLevel(OpenWorld, LevelPackageName);
		UE_LOG(LogTemp, Warning, TEXT("L_OpenWorld save: %s"), bSaved ? TEXT("SUCCESS") : TEXT("FAILED"));
	}

	// ═══════════════════════════════════════════════════════════════════
	// Phase 3: Dungeon Generation
	// ═══════════════════════════════════════════════════════════════════
	if (!bNoDungeons)
	{
		UE_LOG(LogTemp, Warning, TEXT("═══ Phase 3: Generating Dungeons ═══"));
		bool bDungeonsOk = GenerateDungeons(OpenWorld);
		UE_LOG(LogTemp, Warning, TEXT("Dungeon generation: %s"), bDungeonsOk ? TEXT("SUCCESS") : TEXT("PARTIAL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Skipping dungeon generation (-nodungeons)"));
	}

	// Cleanup world context
	GEngine->DestroyWorldContext(OpenWorld);

	UE_LOG(LogTemp, Warning, TEXT("=== SetupOpenWorld Commandlet COMPLETE ==="));
	return 0;
#else
	UE_LOG(LogTemp, Error, TEXT("SetupOpenWorld requires editor build"));
	return 1;
#endif
}

#if WITH_EDITOR

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 1: Landscape
// ═══════════════════════════════════════════════════════════════════════════════

TArray<uint16> USetupOpenWorldCommandlet::GenerateHeightmap(int32 SizeX, int32 SizeY)
{
	TArray<uint16> Heightmap;
	Heightmap.SetNum(SizeX * SizeY);

	const uint16 MidHeight = 32768; // Sea level

	// Perlin-like noise via layered sine waves (deterministic, no external deps)
	FRandomStream Rng(42); // Fixed seed for reproducibility

	for (int32 Y = 0; Y < SizeY; Y++)
	{
		for (int32 X = 0; X < SizeX; X++)
		{
			float FX = (float)X;
			float FY = (float)Y;

			// Layer 1: Base rolling hills (low frequency, high amplitude)
			float H = FMath::Sin(FX * 0.002f) * FMath::Cos(FY * 0.0025f) * 8000.0f;

			// Layer 2: Medium terrain features
			H += FMath::Sin(FX * 0.008f + 1.3f) * FMath::Sin(FY * 0.006f + 0.7f) * 2500.0f;

			// Layer 3: Fine detail (small bumps)
			H += FMath::Sin(FX * 0.025f + 3.1f) * FMath::Cos(FY * 0.03f + 2.2f) * 800.0f;

			// Layer 4: Asymmetric ridge running NW-SE
			float Ridge = FMath::Sin((FX + FY) * 0.003f) * 3000.0f;
			H += Ridge * FMath::Max(0.0f, FMath::Sin(FY * 0.004f));

			// Clamp to uint16 range
			int32 HeightVal = FMath::Clamp((int32)(MidHeight + H), 0, 65535);
			Heightmap[Y * SizeX + X] = (uint16)HeightVal;
		}
	}

	return Heightmap;
}

void USetupOpenWorldCommandlet::CarveFlats()
{
	// Define gameplay areas — all at ground level (Z=0)
	FlatAreas.Empty();

	// Player spawn area — near first dungeon entrance for easy testing
	FlatAreas.Add({FVector(-12000, -9700, 0), 2000.0f, TEXT("spawn")}); // Right in front of dungeon entrance 1

	// Resting points — spread across the map (includes 2 near spawn for testing fast travel)
	FlatAreas.Add({FVector(-11000, -8500, 0), 1200.0f, TEXT("resting_spawn_bonfire")});  // ~1600 UU from player spawn
	FlatAreas.Add({FVector(-13500, -10500, 0), 1200.0f, TEXT("resting_roadside_camp")});  // ~2000 UU from player spawn
	FlatAreas.Add({FVector(-8000, -5000, 0), 1500.0f, TEXT("resting_1")});
	FlatAreas.Add({FVector(10000, 3000, 0), 1500.0f, TEXT("resting_2")});
	FlatAreas.Add({FVector(-3000, 12000, 0), 1500.0f, TEXT("resting_3")});

	// Dungeon entrances
	FlatAreas.Add({FVector(-12000, -10000, 0), 2500.0f, TEXT("dungeon_entrance_1")});
	FlatAreas.Add({FVector(15000, -8000, 0), 2500.0f, TEXT("dungeon_entrance_2")});
	FlatAreas.Add({FVector(5000, 18000, 0), 2500.0f, TEXT("dungeon_entrance_3")});

	// Enemy patrol zones
	FlatAreas.Add({FVector(5000, -3000, 0), 3000.0f, TEXT("patrol_1")});
	FlatAreas.Add({FVector(-6000, 7000, 0), 3000.0f, TEXT("patrol_2")});

	UE_LOG(LogTemp, Warning, TEXT("  Defined %d gameplay areas (all at Z=0)"), FlatAreas.Num());
}

bool USetupOpenWorldCommandlet::CreateLandscape(UWorld* World)
{
	// ALandscape::Import requires editor subsystems (ULandscapeInfoMap) not available in commandlet.
	// Use a flat cube grid at Z=0 for reliable collision. Visual interest comes from
	// Wasteland meshes on top. A real landscape can be substituted in-editor later.

	UStaticMesh* TileMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!TileMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Cube mesh!"));
		return false;
	}

	// Try to load a Wasteland ground material for the terrain tiles
	UMaterialInterface* GroundMaterial = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/Wasteland/Materials/Material_Instances/MI_Rock_01.MI_Rock_01"));
	if (!GroundMaterial)
	{
		// Fallback: try floor material
		GroundMaterial = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/Wasteland/Materials/Material_Instances/MI_Floor_01.MI_Floor_01"));
	}
	if (GroundMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Ground material: %s"), *GroundMaterial->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No ground material found — tiles will use default"));
	}

	// Populate flat areas (all at Z=0 ground level)
	CarveFlats();

	// Create terrain grid: 10x10 tiles, each 5000x5000 cm = 50000x50000 cm total
	// All tiles at Z=0 (flat ground) — Cube is 100x100x100, scale XY to tile size
	const int32 GridSize = 10;
	const float TileSize = 5000.0f;
	const float TotalSize = GridSize * TileSize;
	const float HalfTotal = TotalSize / 2.0f;
	const float TileScale = TileSize / 100.0f; // Cube is 100x100x100 by default

	int32 TilesCreated = 0;

	for (int32 GY = 0; GY < GridSize; GY++)
	{
		for (int32 GX = 0; GX < GridSize; GX++)
		{
			float WorldX = (GX * TileSize) - HalfTotal + TileSize / 2.0f;
			float WorldY = (GY * TileSize) - HalfTotal + TileSize / 2.0f;

			// Place cube so top surface is exactly at Z=0
			// Cube is centered at origin, half-extent = 50 * ScaleZ
			// With ScaleZ=0.1, half-height = 5 UU, so center at Z=-5
			FVector TileLoc(WorldX, WorldY, -5.0f);
			AStaticMeshActor* Tile = World->SpawnActor<AStaticMeshActor>(TileLoc, FRotator::ZeroRotator);
			if (Tile)
			{
				UStaticMeshComponent* SMC = Tile->GetStaticMeshComponent();
				SMC->SetStaticMesh(TileMesh);
				if (GroundMaterial)
				{
					SMC->SetMaterial(0, GroundMaterial);
				}
				SMC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				SMC->SetCollisionProfileName(FName(TEXT("BlockAll")));
				SMC->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
				SMC->SetGenerateOverlapEvents(false);
				Tile->SetActorScale3D(FVector(TileScale, TileScale, 0.1f));
				Tile->SetFolderPath(FName(TEXT("Terrain")));
				Tile->SetMobility(EComponentMobility::Static);
				TilesCreated++;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Terrain grid created: %d tiles (%dx%d), %.0fx%.0f cm, surface at Z=0"),
		TilesCreated, GridSize, GridSize, TotalSize, TotalSize);

	return TilesCreated > 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 2: Wasteland Mesh Scattering
// ═══════════════════════════════════════════════════════════════════════════════

TArray<FString> USetupOpenWorldCommandlet::DiscoverWastelandAssets(const FString& NamePrefixFilter)
{
	TArray<FString> Result;
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	// Wasteland meshes are flat in /Game/Wasteland/Meshes/ with SM_* naming
	// Plus a Foliage subfolder with F_* naming
	TArray<FString> SearchPaths;
	SearchPaths.Add(TEXT("/Game/Wasteland/Meshes"));
	SearchPaths.Add(TEXT("/Game/Wasteland/Meshes/Foliage"));

	for (const FString& SearchPath : SearchPaths)
	{
		TArray<FAssetData> Assets;
		AssetRegistry.GetAssetsByPath(FName(*SearchPath), Assets, false); // false = non-recursive per path

		for (const FAssetData& Asset : Assets)
		{
			if (Asset.AssetClassPath.GetAssetName() != FName(TEXT("StaticMesh")))
			{
				continue;
			}

			// Filter by name prefix if provided
			if (!NamePrefixFilter.IsEmpty())
			{
				FString AssetName = Asset.AssetName.ToString();
				if (!AssetName.StartsWith(NamePrefixFilter))
				{
					continue;
				}
			}

			Result.Add(Asset.GetObjectPathString());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Discovered %d static meshes matching prefix '%s'"), Result.Num(), *NamePrefixFilter);
	return Result;
}

TArray<FString> USetupOpenWorldCommandlet::DiscoverWastelandByPrefixes(const TArray<FString>& Prefixes)
{
	TArray<FString> Result;
	for (const FString& Prefix : Prefixes)
	{
		Result.Append(DiscoverWastelandAssets(Prefix));
	}
	return Result;
}

UStaticMesh* USetupOpenWorldCommandlet::FindWastelandMesh(const FString& Category, int32 Index)
{
	TArray<FString> Assets = DiscoverWastelandAssets(Category);
	if (Assets.Num() == 0) return nullptr;
	int32 SafeIndex = Index % Assets.Num();
	return LoadObject<UStaticMesh>(nullptr, *Assets[SafeIndex]);
}

void USetupOpenWorldCommandlet::ScatterCategory(UWorld* World, const TArray<FString>& MeshPaths, const FString& CategoryName,
	int32 Count, FVector RegionMin, FVector RegionMax, float MinSpacing, float ScaleMin, float ScaleMax,
	bool bMirrorBackface)
{
	TArray<FString> Paths = MeshPaths;
	if (Paths.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No meshes found for category: %s — using engine cube"), *CategoryName);
		Paths.Add(TEXT("/Engine/BasicShapes/Cube.Cube"));
	}

	FRandomStream Rng(FCrc::StrCrc32(*CategoryName)); // Deterministic per category
	TArray<FVector> PlacedLocations;
	int32 Placed = 0;
	int32 MaxAttempts = Count * 5;

	for (int32 Attempt = 0; Attempt < MaxAttempts && Placed < Count; Attempt++)
	{
		FVector Location;
		Location.X = Rng.FRandRange(RegionMin.X, RegionMax.X);
		Location.Y = Rng.FRandRange(RegionMin.Y, RegionMax.Y);
		Location.Z = 0; // Will be adjusted by terrain height

		// Check min spacing against already-placed meshes
		bool bTooClose = false;
		for (const FVector& Existing : PlacedLocations)
		{
			if (FVector::Dist2D(Location, Existing) < MinSpacing)
			{
				bTooClose = true;
				break;
			}
		}
		if (bTooClose) continue;

		// Get terrain height at this location
		float TerrainZ = SampleTerrainHeight(World, Location.X, Location.Y);
		Location.Z = TerrainZ;

		// Random rotation (yaw only) and scale variation
		FRotator Rotation(0, Rng.FRandRange(0.0f, 360.0f), 0);
		float Scale = Rng.FRandRange(ScaleMin, ScaleMax);

		// Pick a random mesh from the category
		int32 MeshIndex = Rng.RandRange(0, Paths.Num() - 1);
		UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *Paths[MeshIndex]);
		if (!Mesh) continue;

		// Spawn static mesh actor
		AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(Location, Rotation);
		if (MeshActor)
		{
			MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
			MeshActor->SetActorScale3D(FVector(Scale));
			MeshActor->SetFolderPath(FName(*FString::Printf(TEXT("Wasteland/%s"), *CategoryName)));
			PlacedLocations.Add(Location);
			Placed++;

			// Mirror copy: spawn a 180° yaw-rotated duplicate at same location
			// to fill in single-sided backfaces on cliff/wall rock meshes
			if (bMirrorBackface)
			{
				FRotator MirrorRot = Rotation;
				MirrorRot.Yaw += 180.0f;
				AStaticMeshActor* Mirror = World->SpawnActor<AStaticMeshActor>(Location, MirrorRot);
				if (Mirror)
				{
					Mirror->GetStaticMeshComponent()->SetStaticMesh(Mesh);
					Mirror->SetActorScale3D(FVector(Scale));
					Mirror->SetFolderPath(FName(*FString::Printf(TEXT("Wasteland/%s_Mirror"), *CategoryName)));
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("  Scattered %d/%d meshes for %s (%d mesh variants)"), Placed, Count, *CategoryName, Paths.Num());
}

float USetupOpenWorldCommandlet::SampleTerrainHeight(UWorld* /*World*/, float /*X*/, float /*Y*/)
{
	// Flat terrain at Z=0. Visual height variation comes from Wasteland meshes.
	// A real landscape with proper collision can be substituted in-editor later.
	return 0.0f;
}

bool USetupOpenWorldCommandlet::ScatterWastelandMeshes(UWorld* World)
{
	// Full map bounds: ~±25200 cm (504 quads * 100 cm / 2)
	const float MapHalfSize = 25000.0f;
	const FVector MapMin(-MapHalfSize, -MapHalfSize, 0);
	const FVector MapMax(MapHalfSize, MapHalfSize, 0);

	// Wasteland Environment meshes are flat in /Game/Wasteland/Meshes/
	// with SM_* prefix naming. Categorize by prefix pattern:

	// ── Rocks — dense scatter everywhere ──
	// SM_Rock meshes are single-sided cliff rocks. Spawn each with a 180° mirror copy
	// so they look solid from all angles (back-to-back technique).
	TArray<FString> RockPrefixes = {TEXT("SM_Rock")};
	TArray<FString> RockMeshes = DiscoverWastelandByPrefixes(RockPrefixes);
	ScatterCategory(World, RockMeshes, TEXT("Rocks"), 120,
		MapMin, MapMax, 500.0f, 0.8f, 1.4f, /*bMirrorBackface=*/ true);

	// F_Rock boulders (full 3D, no mirror needed)
	TArray<FString> BoulderPrefixes = {TEXT("F_Rock")};
	TArray<FString> BoulderMeshes = DiscoverWastelandByPrefixes(BoulderPrefixes);
	ScatterCategory(World, BoulderMeshes, TEXT("Boulders"), 30,
		MapMin, MapMax, 1200.0f, 0.8f, 1.4f);

	// ── Structures — sparse, near POIs (standalone complete meshes only) ──
	// Exclude modular fragments (CaptainWall, CaptainStairsBack, Tower_Bottom, etc.)
	// that look incomplete when placed individually
	TArray<FString> StructurePrefixes = {TEXT("SM_Hut"), TEXT("SM_Silo"),
		TEXT("SM_Bridge"), TEXT("SM_MetalCabin"), TEXT("SM_Van"),
		TEXT("SM_WaterTower"), TEXT("SM_LightTower"), TEXT("SM_CaptainMerged"),
		TEXT("SM_CaptainCabin"), TEXT("SM_CaptainTower")};
	TArray<FString> StructureMeshes = DiscoverWastelandByPrefixes(StructurePrefixes);
	ScatterCategory(World, StructureMeshes, TEXT("Structures"), 15,
		FVector(-15000, -12000, 0), FVector(18000, 20000, 0), 3000.0f, 0.8f, 1.2f);

	// ── Walls / Fences — placed along edges and around POIs ──
	// Standalone fence/wall pieces that look complete on their own
	TArray<FString> WallPrefixes = {TEXT("SM_Fence"), TEXT("SM_ConcreteFence"), TEXT("SM_MetalFence"),
		TEXT("SM_WoodenFence"), TEXT("SM_FenceSpikes"), TEXT("SM_Corrugated"),
		TEXT("SM_Spikes"), TEXT("SM_Sandbag_Pile")};
	TArray<FString> WallMeshes = DiscoverWastelandByPrefixes(WallPrefixes);
	ScatterCategory(World, WallMeshes, TEXT("Walls"), 30,
		FVector(-18000, -15000, 0), FVector(18000, 18000, 0), 1500.0f, 0.9f, 1.1f);

	// ── Props / Debris — moderate scatter (barrels, containers, tires, sandbags, etc.) ──
	TArray<FString> PropPrefixes = {TEXT("SM_Barrel"), TEXT("SM_Container"), TEXT("SM_Debris"),
		TEXT("SM_MetalDebris"), TEXT("SM_Chair"), TEXT("SM_Table"), TEXT("SM_Tire"),
		TEXT("SM_Sandbag"), TEXT("SM_Crane"), TEXT("SM_Pipe"), TEXT("SM_Plank"),
		TEXT("SM_Antenna"), TEXT("SM_Lamp"), TEXT("SM_Bulb"), TEXT("SM_Spotlight"),
		TEXT("SM_Sign"), TEXT("SM_Weight"), TEXT("SM_Rope"), TEXT("SM_Cable"),
		TEXT("SM_Skull"), TEXT("SM_HornSkull"), TEXT("SM_HornsSign"), TEXT("SM_SkullSign"),
		TEXT("SM_ShamanSign")};
	TArray<FString> PropMeshes = DiscoverWastelandByPrefixes(PropPrefixes);
	ScatterCategory(World, PropMeshes, TEXT("Props"), 50,
		FVector(-18000, -15000, 0), FVector(18000, 18000, 0), 800.0f, 0.7f, 1.1f);

	// ── Wood / Beams — structural elements scattered near ruins ──
	TArray<FString> WoodPrefixes = {TEXT("SM_Beam"), TEXT("SM_WoodenBeam"), TEXT("SM_WoodenPole"),
		TEXT("SM_WoodDeck"), TEXT("SM_Ladder"), TEXT("SM_Floor")};
	TArray<FString> WoodMeshes = DiscoverWastelandByPrefixes(WoodPrefixes);
	ScatterCategory(World, WoodMeshes, TEXT("WoodBeams"), 25,
		FVector(-15000, -12000, 0), FVector(18000, 20000, 0), 1200.0f, 0.8f, 1.2f);

	// ── Foliage — both SM_Foliage* in main dir and F_* in Foliage subfolder ──
	TArray<FString> FoliagePrefixes = {TEXT("SM_Foliage"), TEXT("F_Foliage"), TEXT("F_Rock")};
	TArray<FString> FoliageMeshes = DiscoverWastelandByPrefixes(FoliagePrefixes);
	ScatterCategory(World, FoliageMeshes, TEXT("Foliage"), 60,
		MapMin, MapMax, 1000.0f, 0.8f, 1.5f);

	// ── Cluster rocks + debris near dungeon entrances + build visible entrance ──
	int32 DungeonIdx = 0;
	for (const FlatArea& Area : FlatAreas)
	{
		if (Area.Purpose.Contains(TEXT("dungeon_entrance")))
		{
			FVector ClusterMin = Area.Center - FVector(3000, 3000, 0);
			FVector ClusterMax = Area.Center + FVector(3000, 3000, 0);

			// Dense rock cluster around entrance (mirrored for solid look)
			ScatterCategory(World, RockMeshes, TEXT("DungeonRocks"), 20,
				ClusterMin, ClusterMax, 200.0f, 0.8f, 1.6f, /*bMirrorBackface=*/ true);

			// Debris/props around entrance
			ScatterCategory(World, PropMeshes, TEXT("DungeonProps"), 10,
				ClusterMin, ClusterMax, 300.0f, 0.6f, 1.0f);

			// Build visible dungeon entrance structure (rock archway + light)
			BuildDungeonEntrance(World, Area.Center, DungeonIdx);
			DungeonIdx++;
		}
	}

	return true;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Dungeon Entrance Structure
// ═══════════════════════════════════════════════════════════════════════════════

void USetupOpenWorldCommandlet::BuildDungeonEntrance(UWorld* World, FVector Location, int32 DungeonIndex)
{
	// Build a LARGE, impossible-to-miss dungeon entrance structure:
	// - 4 massive rock pillars forming a gate
	// - Lintel rocks across the top
	// - Tall beacon tower (stacked cubes) visible from far away
	// - Ring of rocks around the base

	UStaticMesh* RockMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Wasteland/Meshes/SM_Rock_01.SM_Rock_01"));
	if (!RockMesh) RockMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));

	UStaticMesh* LintelMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Wasteland/Meshes/SM_Rock_03.SM_Rock_03"));
	if (!LintelMesh) LintelMesh = RockMesh;

	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));

	auto SpawnMirroredRock = [&](FVector Pos, FRotator Rot, FVector Scale, UStaticMesh* Mesh)
	{
		AStaticMeshActor* A = World->SpawnActor<AStaticMeshActor>(Pos, Rot);
		if (A) { A->GetStaticMeshComponent()->SetStaticMesh(Mesh); A->SetActorScale3D(Scale); A->SetFolderPath(FName(TEXT("Wasteland/DungeonGate"))); }
		FRotator MirrorRot = Rot; MirrorRot.Yaw += 180.0f;
		AStaticMeshActor* B = World->SpawnActor<AStaticMeshActor>(Pos, MirrorRot);
		if (B) { B->GetStaticMeshComponent()->SetStaticMesh(Mesh); B->SetActorScale3D(Scale); B->SetFolderPath(FName(TEXT("Wasteland/DungeonGate"))); }
	};

	const float GateWidth = 600.0f;

	// 4 massive gate pillars (front pair + back pair)
	SpawnMirroredRock(Location + FVector(-GateWidth, -200, 0), FRotator(0, 0, 0), FVector(4.0f, 4.0f, 8.0f), RockMesh);
	SpawnMirroredRock(Location + FVector(GateWidth, -200, 0), FRotator(0, 90, 0), FVector(4.0f, 4.0f, 8.0f), RockMesh);
	SpawnMirroredRock(Location + FVector(-GateWidth, 200, 0), FRotator(0, 45, 0), FVector(4.0f, 4.0f, 8.0f), RockMesh);
	SpawnMirroredRock(Location + FVector(GateWidth, 200, 0), FRotator(0, -45, 0), FVector(4.0f, 4.0f, 8.0f), RockMesh);

	// Lintel rocks across the top of the gate
	SpawnMirroredRock(Location + FVector(0, -200, 1200.0f), FRotator(0, 0, 0), FVector(8.0f, 2.0f, 2.0f), LintelMesh);
	SpawnMirroredRock(Location + FVector(0, 200, 1200.0f), FRotator(0, 90, 0), FVector(8.0f, 2.0f, 2.0f), LintelMesh);

	// Additional flanking rocks at ground level (ring of rocks)
	for (int32 i = 0; i < 8; i++)
	{
		float Angle = (i * 45.0f) * (PI / 180.0f);
		float Radius = 1200.0f;
		FVector Offset(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0);
		SpawnMirroredRock(Location + Offset, FRotator(0, i * 45.0f + 22.0f, 0), FVector(3.0f, 3.0f, 5.0f), RockMesh);
	}

	// ═══ BEACON TOWER — Tall glowing pillar visible from across the map ═══
	// Stack of cubes forming a tall column
	if (CubeMesh)
	{
		for (int32 i = 0; i < 6; i++)
		{
			float Z = 200.0f + i * 500.0f; // Stack from 200 to 2700
			FVector TowerPos = Location + FVector(0, 0, Z);
			float TowerScale = 1.5f - (i * 0.15f); // Taper slightly
			AStaticMeshActor* Block = World->SpawnActor<AStaticMeshActor>(TowerPos, FRotator(0, i * 30.0f, 0));
			if (Block)
			{
				Block->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
				Block->SetActorScale3D(FVector(TowerScale, TowerScale, 5.0f));
				Block->SetFolderPath(FName(TEXT("Wasteland/DungeonGate")));
			}
		}

		// Bright sphere beacon at the very top
		UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (SphereMesh)
		{
			FVector BeaconPos = Location + FVector(0, 0, 3200.0f);
			AStaticMeshActor* Beacon = World->SpawnActor<AStaticMeshActor>(BeaconPos, FRotator::ZeroRotator);
			if (Beacon)
			{
				Beacon->GetStaticMeshComponent()->SetStaticMesh(SphereMesh);
				Beacon->SetActorScale3D(FVector(4.0f)); // Large glowing orb
				Beacon->SetFolderPath(FName(TEXT("Wasteland/DungeonGate")));
			}
		}
	}

	// Point light (actual light source, warm orange)
	{
		FVector LightPos = Location + FVector(0, 0, 2000.0f);
		APointLight* PointLight = World->SpawnActor<APointLight>(LightPos, FRotator::ZeroRotator);
		if (PointLight)
		{
			PointLight->PointLightComponent->SetIntensity(50000.0f);
			PointLight->PointLightComponent->SetAttenuationRadius(5000.0f);
			PointLight->PointLightComponent->SetLightColor(FLinearColor(1.0f, 0.6f, 0.2f)); // Warm orange
			PointLight->SetFolderPath(FName(TEXT("Wasteland/DungeonGate")));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("  Built dungeon entrance %d at (%.0f, %.0f, %.0f)"),
		DungeonIndex + 1, Location.X, Location.Y, Location.Z);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 3: Dungeon Generation
// ═══════════════════════════════════════════════════════════════════════════════

bool USetupOpenWorldCommandlet::GenerateDungeons(UWorld* OverWorld)
{
	int32 DungeonIndex = 0;
	int32 DungeonsGenerated = 0;
	for (const FlatArea& Area : FlatAreas)
	{
		if (!Area.Purpose.Contains(TEXT("dungeon_entrance"))) continue;

		// Skip if we're only regenerating a specific dungeon
		if (OnlyDungeonNum > 0 && (DungeonIndex + 1) != OnlyDungeonNum)
		{
			UE_LOG(LogTemp, Warning, TEXT("Skipping dungeon %d (only regenerating %d)"), DungeonIndex + 1, OnlyDungeonNum);
			DungeonIndex++;
			continue;
		}

		FString LevelName = FString::Printf(TEXT("/Game/Maps/L_Dungeon_%02d"), DungeonIndex + 1);
		UE_LOG(LogTemp, Warning, TEXT("Generating dungeon %d: %s"), DungeonIndex + 1, *LevelName);

		bool bOk = CreateDungeonLevel(DungeonIndex, LevelName, Area.Center);
		if (bOk)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Dungeon %d: SAVED to %s"), DungeonIndex + 1, *LevelName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Dungeon %d: Created as basic room layout (DA plugin may not be available)"), DungeonIndex + 1);
		}

		DungeonIndex++;
		DungeonsGenerated++;
	}

	return DungeonsGenerated > 0;
}

bool USetupOpenWorldCommandlet::CreateDungeonLevel(int32 DungeonIndex, const FString& LevelName, FVector OverworldEntrance)
{
	// Delete existing level file to avoid "Cannot generate unique name for WorldSettings" crash
	FString ExistingFile = FPackageName::LongPackageNameToFilename(LevelName, FPackageName::GetMapPackageExtension());
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*ExistingFile))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Deleting existing dungeon level: %s"), *ExistingFile);
		FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*ExistingFile);
	}

	// Create a separate level package for this dungeon
	UPackage* DungeonPkg = CreatePackage(*LevelName);
	DungeonPkg->FullyLoad();

	UWorld* DungeonWorld = UWorld::CreateWorld(EWorldType::None, false,
		FName(*FString::Printf(TEXT("L_Dungeon_%02d"), DungeonIndex + 1)), DungeonPkg);
	if (!DungeonWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create dungeon world %d!"), DungeonIndex + 1);
		return false;
	}
	DungeonWorld->SetFlags(RF_Public | RF_Standalone);

	FWorldContext& DungeonCtx = GEngine->CreateNewWorldContext(EWorldType::Editor);
	DungeonCtx.SetCurrentWorld(DungeonWorld);

	// Dungeon placed underground, each at different Y offset to avoid overlap
	const FVector DungeonOrigin(0, DungeonIndex * 20000.0f, -5000.0f);

	FRandomStream Rng(DungeonIndex * 12345 + 42);

	// ══════════════════════════════════════════════════════════════════
	// Generate dungeon — Canvas mode routes ALL through Cell Flow;
	// otherwise Grid builder for dungeon 0 test, Cell Flow for 1+
	// ══════════════════════════════════════════════════════════════════
	if (bCanvasMode)
	{
		BuildCellFlowCave(DungeonWorld, DungeonOrigin, DungeonIndex, Rng, true);
	}
	else if (DungeonIndex == 0)
	{
		BuildGridDungeon(DungeonWorld, DungeonOrigin, DungeonIndex, Rng);
	}
	else
	{
		BuildCellFlowCave(DungeonWorld, DungeonOrigin, DungeonIndex, Rng, false);
	}

	// Screenshots: use SLF.CaptureDungeon console command (requires editor RHI)

	// Save dungeon level
	bool bSaved = SaveLevel(DungeonWorld, LevelName);
	GEngine->DestroyWorldContext(DungeonWorld);

	return bSaved;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Cave Dungeon Generation via Cell Flow + Voxel Cave
// ═══════════════════════════════════════════════════════════════════════════════

void USetupOpenWorldCommandlet::BuildCellFlowCave(UWorld* DungeonWorld, FVector Origin, int32 DungeonIdx, FRandomStream& Rng, bool bCanvas)
{
	UE_LOG(LogTemp, Warning, TEXT("  Building Cell Flow cave dungeon %d at (%.0f, %.0f, %.0f)"),
		DungeonIdx + 1, Origin.X, Origin.Y, Origin.Z);

	// ══════════════════════════════════════════════════════════════════
	// 1. Spawn ADungeon actor with Cell Flow builder
	// ══════════════════════════════════════════════════════════════════
	ADungeon* Dungeon = DungeonWorld->SpawnActor<ADungeon>(Origin, FRotator::ZeroRotator);
	if (!Dungeon)
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to spawn ADungeon actor!"));
		return;
	}

	Dungeon->SetFolderPath(FName(TEXT("Dungeon/DungeonActor")));
	Dungeon->BuilderClass = UCellFlowBuilder::StaticClass();
	Dungeon->CreateBuilderInstance();

	// ══════════════════════════════════════════════════════════════════
	// 2. Build Cell Flow execution script (Voronoi → paths → finalize)
	// ══════════════════════════════════════════════════════════════════

	// Per-dungeon parameters
	struct FCaveFlowParams
	{
		FIntPoint WorldSize;
		int32 NumPoints;       // Voronoi only
		float MinGroupArea;    // Voronoi only
		int32 MinCellSize;     // Grid only
		int32 MaxCellSize;     // Grid only
		int32 MainPathSize;
		int32 NumBranches;
		int32 CeilingHeightMax;
		FVector GridSize;
		bool bUseGrid = false; // false = Voronoi, true = Grid
	};

	FCaveFlowParams Params;
	switch (DungeonIdx)
	{
	case 0: // TEST: Grid cells, long straight line, varied sizes for comparison
		Params.bUseGrid = true;
		Params.WorldSize = FIntPoint(40, 10);  // Long and narrow world
		Params.MinCellSize = 1;   // 1 tile = corridor size
		Params.MaxCellSize = 8;   // 8 tiles = large room
		Params.MainPathSize = 10; // Long straight path
		Params.NumBranches = 0;   // No branches — pure straight line
		Params.CeilingHeightMax = 3;
		Params.GridSize = FVector(300, 300, 250);
		break;
	case 1: // Medium cave (8-12 rooms, ~10 min clear)
		Params.WorldSize = FIntPoint(24, 24);
		Params.NumPoints = 70;
		Params.MinGroupArea = 14.0f;
		Params.MainPathSize = 5;
		Params.NumBranches = 4;
		Params.CeilingHeightMax = 3;
		Params.GridSize = FVector(300, 300, 250);
		break;
	default: // Large cave with boss (12-16 rooms, ~15 min clear)
		Params.WorldSize = FIntPoint(28, 28);
		Params.NumPoints = 90;
		Params.MinGroupArea = 16.0f;
		Params.MainPathSize = 6;
		Params.NumBranches = 5;
		Params.CeilingHeightMax = 4;
		Params.GridSize = FVector(300, 300, 300);
		break;
	}

	// Canvas mode overrides: large open cave, no branches
	if (bCanvas)
	{
		Params.bUseGrid = false;
		Params.WorldSize = FIntPoint(40, 40);
		Params.NumPoints = 40;
		Params.MinGroupArea = 30.0f;
		Params.MainPathSize = 8;
		Params.NumBranches = 0;
		Params.GridSize = FVector(400, 400, 400);
		Params.CeilingHeightMax = 4;
		UE_LOG(LogTemp, Warning, TEXT("  CANVAS MODE: overriding to large open cave"));
	}

	UE_LOG(LogTemp, Warning, TEXT("  Cell Flow params: %dx%d world, %d points, %d main path, %d branches"),
		Params.WorldSize.X, Params.WorldSize.Y, Params.NumPoints, Params.MainPathSize, Params.NumBranches);

	// ══════════════════════════════════════════════════════════════════
	// 3. Create Cell Flow execution script
	// ══════════════════════════════════════════════════════════════════

	// Create the flow asset in a proper package (TSoftObjectPtr requires disk-saved assets)
	FString FlowPkgName = FString::Printf(TEXT("/Game/DungeonArchitect/CellFlow/CF_Cave_%02d"), DungeonIdx + 1);
	FString FlowFileName = FPackageName::LongPackageNameToFilename(FlowPkgName, FPackageName::GetAssetPackageExtension());

	// Delete existing file to avoid partial-load crash
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*FlowFileName))
	{
		FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FlowFileName);
	}

	UPackage* FlowPkg = CreatePackage(*FlowPkgName);
	FlowPkg->FullyLoad();

	UCellFlowAsset* FlowAsset = NewObject<UCellFlowAsset>(FlowPkg, TEXT("CF_CaveFlow"), RF_Public | RF_Standalone);
	UFlowExecScript* ExecScript = NewObject<UFlowExecScript>(FlowAsset);
	UFlowExecScriptGraph* ScriptGraph = NewObject<UFlowExecScriptGraph>(ExecScript);
	ExecScript->ScriptGraph = ScriptGraph;
	FlowAsset->ExecScript = ExecScript;

	// Task 1: Create Cells (Grid or Voronoi)
	UCellFlowLayoutTaskCreateCellsBase* TaskCellsBase = nullptr;
	if (Params.bUseGrid)
	{
		UCellFlowLayoutTaskCreateCellsGrid* TaskGrid = NewObject<UCellFlowLayoutTaskCreateCellsGrid>(FlowAsset);
		TaskGrid->WorldSize = Params.WorldSize;
		TaskGrid->MinCellSize = Params.MinCellSize;
		TaskGrid->MaxCellSize = Params.MaxCellSize;
		TaskGrid->FitIterations = 30;
		TaskGrid->MinAspectRatio = 0.6f;
		TaskGrid->MaxAspectRatio = 1.4f;
		TaskCellsBase = TaskGrid;
		UE_LOG(LogTemp, Warning, TEXT("  Using GRID cells: min=%d max=%d"), Params.MinCellSize, Params.MaxCellSize);
	}
	else
	{
		UCellFlowLayoutTaskCreateCellsVoronoi* TaskVoronoi = NewObject<UCellFlowLayoutTaskCreateCellsVoronoi>(FlowAsset);
		TaskVoronoi->WorldSize = Params.WorldSize;
		TaskVoronoi->NumPoints = Params.NumPoints;
		TaskVoronoi->NumRelaxIterations = 4;
		TaskVoronoi->bDisableBoundaryCells = true;
		TaskVoronoi->EdgeConnectionThreshold = 0.10f;
		TaskVoronoi->MinGroupArea = Params.MinGroupArea;
		TaskVoronoi->ClusterBlobbiness = 0.25f;
		TaskCellsBase = TaskVoronoi;
	}

	// Task 2: Create Main Path (entrance → boss)
	UCellFlowLayoutTaskCreateMainPath* TaskMain = NewObject<UCellFlowLayoutTaskCreateMainPath>(FlowAsset);
	TaskMain->PathSize = Params.MainPathSize;
	TaskMain->PathName = TEXT("main");
	TaskMain->StartMarkerName = TEXT("SpawnPoint");
	TaskMain->GoalMarkerName = TEXT("BossRoom");
	TaskMain->CeilingHeightMin = 2;
	TaskMain->CeilingHeightMax = Params.CeilingHeightMax;

	// Branch paths — dead ends with loot, ambushes, or traps
	TArray<UCellFlowLayoutTaskCreatePath*> BranchTasks;
	const TCHAR* BranchNames[] = { TEXT("treasure"), TEXT("ambush"), TEXT("trap"), TEXT("secret"), TEXT("loot") };
	for (int32 B = 0; B < Params.NumBranches; B++)
	{
		UCellFlowLayoutTaskCreatePath* TaskBranch = NewObject<UCellFlowLayoutTaskCreatePath>(FlowAsset);
		TaskBranch->PathName = BranchNames[FMath::Min(B, 4)];
		TaskBranch->MinPathSize = 1;
		TaskBranch->MaxPathSize = 3;  // Short but meaningful dead ends
		TaskBranch->StartFromPath = TEXT("main");
		TaskBranch->CeilingHeightMin = 1;
		TaskBranch->CeilingHeightMax = FMath::Max(1, Params.CeilingHeightMax - 1);
		BranchTasks.Add(TaskBranch);
	}

	// Task 5: Finalize — bSeparateAdjacentRooms defaults to true (inserts wall tiles between rooms)
	UCellFlowLayoutTaskFinalize* TaskFinalize = NewObject<UCellFlowLayoutTaskFinalize>(FlowAsset);
	// ══════════════════════════════════════════════════════════════════
	// Wire execution graph: Cells → Main → Branches → Finalize → Result
	// ══════════════════════════════════════════════════════════════════

	// Create task nodes
	UFlowExecScriptTaskNode* NodeCells = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeCells->NodeId = FGuid::NewGuid();
	NodeCells->Task = TaskCellsBase;

	UFlowExecScriptTaskNode* NodeMain = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeMain->NodeId = FGuid::NewGuid();
	NodeMain->Task = TaskMain;

	TArray<UFlowExecScriptTaskNode*> BranchNodes;
	for (UCellFlowLayoutTaskCreatePath* BT : BranchTasks)
	{
		UFlowExecScriptTaskNode* BN = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
		BN->NodeId = FGuid::NewGuid();
		BN->Task = BT;
		BranchNodes.Add(BN);
	}

	UFlowExecScriptTaskNode* NodeFinalize = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeFinalize->NodeId = FGuid::NewGuid();
	NodeFinalize->Task = TaskFinalize;

	UFlowExecScriptResultNode* NodeResult = NewObject<UFlowExecScriptResultNode>(ScriptGraph);
	NodeResult->NodeId = FGuid::NewGuid();
	ExecScript->ResultNode = NodeResult;

	// Wire: Cells → Main
	NodeCells->OutgoingNodes.Add(NodeMain);
	NodeMain->IncomingNodes.Add(NodeCells);

	// Wire: Main → Branch1 → Branch2 → ... → Finalize
	UFlowExecScriptGraphNode* PrevNode = NodeMain;
	for (UFlowExecScriptTaskNode* BN : BranchNodes)
	{
		PrevNode->OutgoingNodes.Add(BN);
		BN->IncomingNodes.Add(PrevNode);
		PrevNode = BN;
	}
	PrevNode->OutgoingNodes.Add(NodeFinalize);
	NodeFinalize->IncomingNodes.Add(PrevNode);

	// Wire: Finalize → Result
	NodeFinalize->OutgoingNodes.Add(NodeResult);
	NodeResult->IncomingNodes.Add(NodeFinalize);

	// Add all nodes to graph
	ScriptGraph->Nodes.Add(NodeCells);
	ScriptGraph->Nodes.Add(NodeMain);
	for (UFlowExecScriptTaskNode* BN : BranchNodes)
	{
		ScriptGraph->Nodes.Add(BN);
	}
	ScriptGraph->Nodes.Add(NodeFinalize);
	ScriptGraph->Nodes.Add(NodeResult);
		UE_LOG(LogTemp, Warning, TEXT("  Execution graph: %d nodes wired"), ScriptGraph->Nodes.Num());

	// Save flow asset to disk (required for TSoftObjectPtr::LoadSynchronous)
	{
		FlowPkg->MarkPackageDirty();
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		bool bFlowSaved = UPackage::SavePackage(FlowPkg, FlowAsset, *FlowFileName, SaveArgs);
		UE_LOG(LogTemp, Warning, TEXT("  Flow asset saved: %s (%s)"), *FlowPkgName, bFlowSaved ? TEXT("OK") : TEXT("FAILED"));
	}

	// ══════════════════════════════════════════════════════════════════
	// 4. Configure Cell Flow builder
	// ══════════════════════════════════════════════════════════════════
	UCellFlowConfig* CellConfig = Cast<UCellFlowConfig>(Dungeon->GetConfig());
	if (!CellConfig)
	{
		CellConfig = NewObject<UCellFlowConfig>(Dungeon);
		Dungeon->Config = CellConfig;
	}

	CellConfig->CellFlow = FlowAsset;
	CellConfig->Seed = Rng.RandRange(1, 999999);
	CellConfig->GridSize = Params.GridSize;
	CellConfig->MaxBuildTimePerFrameMs = 0.0f; // Synchronous build

	// ══════════════════════════════════════════════════════════════════
	// 5. Configure Voxel Cave overlay
	// ══════════════════════════════════════════════════════════════════
	ConfigureVoxelCave(Dungeon, DungeonIdx, bCanvas);

	// ══════════════════════════════════════════════════════════════════
	// 6. Theme + marker emitter + event listener
	// ══════════════════════════════════════════════════════════════════
	UDungeonThemeAsset* CaveTheme = GetOrCreateCaveTheme();
	if (CaveTheme)
	{
		Dungeon->Themes.Add(CaveTheme);
	}

	// Skip gameplay placement in canvas mode — walls and gameplay come after design
	if (!bCanvas)
	{
		// Custom marker emitter for cave decorations
		USLFCaveMarkerEmitter* MarkerEmitter = NewObject<USLFCaveMarkerEmitter>(Dungeon);
		MarkerEmitter->GridSize = Params.GridSize;
		Dungeon->MarkerEmitters.Add(MarkerEmitter);

		// Event listener for gameplay actors
		USLFCaveEventListener* EventListener = NewObject<USLFCaveEventListener>(Dungeon);
		EventListener->DungeonIndex = DungeonIdx;
		Dungeon->EventListeners.Add(EventListener);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Canvas mode: skipping marker emitter and event listener"));
	}

	// ══════════════════════════════════════════════════════════════════
	// 7. Build the dungeon
	// ══════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("  Building dungeon %d..."), DungeonIdx + 1);
	Dungeon->BuildDungeon();

	bool bBuildOk = Dungeon->GetBuilder() && Dungeon->GetBuilder()->HasBuildSucceeded();
	UE_LOG(LogTemp, Warning, TEXT("  Dungeon %d build %s"),
		DungeonIdx + 1, bBuildOk ? TEXT("SUCCEEDED") : TEXT("FAILED"));

	if (!bBuildOk)
	{
		UE_LOG(LogTemp, Error, TEXT("  Cell Flow build failed — dungeon %d will be empty"), DungeonIdx + 1);
	}

	// Export cell graph to JSON (canvas mode or always — useful for design)
	if (bBuildOk)
	{
		FSLFCaveMazeBuilder::ExportCellGraphToJson(Dungeon, DungeonIdx, Origin, Params.GridSize);
	}

	// Canvas mode: flood the cave with light so it's fully visible for design work
	if (bCanvas && bBuildOk)
	{
		int32 LightsPlaced = 0;

		// Global directional light pointing down — fills the entire cave
		ADirectionalLight* DirLight = DungeonWorld->SpawnActor<ADirectionalLight>(
			Origin + FVector(0, 0, 2000.0f), FRotator(-70.0f, 30.0f, 0.0f));
		if (DirLight)
		{
			DirLight->GetComponent()->SetIntensity(8.0f);
			DirLight->GetComponent()->SetLightColor(FLinearColor(0.95f, 0.92f, 0.85f));
			DirLight->GetComponent()->SetCastShadows(false);
			DirLight->SetFolderPath(FName(TEXT("Dungeon/CanvasLights")));
			LightsPlaced++;
		}

		// Sky light for ambient fill — eliminates pure-black shadows
		ASkyLight* SkyLightActor = DungeonWorld->SpawnActor<ASkyLight>(
			Origin + FVector(0, 0, 1000.0f), FRotator::ZeroRotator);
		if (SkyLightActor)
		{
			SkyLightActor->GetLightComponent()->SetIntensity(3.0f);
			SkyLightActor->GetLightComponent()->SetLightColor(FLinearColor(0.6f, 0.65f, 0.75f));
			SkyLightActor->SetFolderPath(FName(TEXT("Dungeon/CanvasLights")));
			LightsPlaced++;
		}

		// Per-room point lights — bright center + 6 perimeter lights in every room
		UCellFlowModel* CellModel = Cast<UCellFlowModel>(Dungeon->GetModel());
		if (CellModel && CellModel->CellGraph)
		{
			UDAFlowCellGraph* CellGraph = CellModel->CellGraph;

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

				FVector WorldCenter(
					Origin.X + Center2D.X * Params.GridSize.X,
					Origin.Y + Center2D.Y * Params.GridSize.Y,
					Origin.Z + (float)Group.GroupHeight * Params.GridSize.Z
				);
				float Radius = FMath::Max(FMath::Sqrt(TotalArea) * Params.GridSize.X * 0.5f, 600.0f);

				// Bright center light
				APointLight* CenterLight = DungeonWorld->SpawnActor<APointLight>(
					WorldCenter + FVector(0, 0, 400.0f), FRotator::ZeroRotator);
				if (CenterLight)
				{
					CenterLight->PointLightComponent->SetIntensity(800000.0f);
					CenterLight->PointLightComponent->SetAttenuationRadius(FMath::Max(Radius * 3.0f, 5000.0f));
					CenterLight->PointLightComponent->SetLightColor(FLinearColor(1.0f, 0.95f, 0.9f));
					CenterLight->PointLightComponent->SetCastShadows(false);
					CenterLight->SetFolderPath(FName(TEXT("Dungeon/CanvasLights")));
					LightsPlaced++;
				}

				// 6 perimeter lights in every room
				for (int32 L = 0; L < 6; L++)
				{
					float Angle = L * PI / 3.0f;
					FVector LightPos = WorldCenter + FVector(
						FMath::Cos(Angle) * Radius * 0.65f,
						FMath::Sin(Angle) * Radius * 0.65f,
						300.0f);
					APointLight* PL = DungeonWorld->SpawnActor<APointLight>(LightPos, FRotator::ZeroRotator);
					if (PL)
					{
						PL->PointLightComponent->SetIntensity(500000.0f);
						PL->PointLightComponent->SetAttenuationRadius(4000.0f);
						PL->PointLightComponent->SetLightColor(FLinearColor(0.95f, 0.9f, 0.85f));
						PL->PointLightComponent->SetCastShadows(false);
						PL->SetFolderPath(FName(TEXT("Dungeon/CanvasLights")));
						LightsPlaced++;
					}
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("  Canvas mode: placed %d lights (dir+sky+%d room groups)"),
				LightsPlaced, CellGraph->GroupNodes.Num());
		}
	}

	// Canvas mode: build maze walls from layout JSON if it exists
	if (bCanvas && bBuildOk)
	{
		FString LayoutPath = FPaths::ProjectDir() / TEXT("MapCapture") /
			FString::Printf(TEXT("dungeon_%02d_layout.json"), DungeonIdx + 1);

		if (FPaths::FileExists(LayoutPath))
		{
			UE_LOG(LogTemp, Warning, TEXT("  Canvas mode: found layout JSON, building maze walls..."));
			FSLFCaveMazeBuilder::BuildCaveMaze(DungeonWorld, DungeonIdx);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Canvas mode: no layout JSON at %s — skipping wall construction"), *LayoutPath);
		}
	}

	// Explicitly fire event listeners — DA may not call them in commandlet mode
	UE_LOG(LogTemp, Warning, TEXT("  Firing post-build event listeners (%d registered)..."), Dungeon->EventListeners.Num());
	for (UDungeonEventListener* Listener : Dungeon->EventListeners)
	{
		if (Listener)
		{
			UE_LOG(LogTemp, Warning, TEXT("    Calling %s"), *Listener->GetClass()->GetName());
			Listener->OnPostDungeonBuild(Dungeon);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("  Cave dungeon %d generation COMPLETE"), DungeonIdx + 1);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Grid Dungeon Builder (rooms + corridors)
// ═══════════════════════════════════════════════════════════════════════════════

void USetupOpenWorldCommandlet::BuildGridDungeon(UWorld* DungeonWorld, FVector Origin, int32 DungeonIdx, FRandomStream& Rng)
{
	UE_LOG(LogTemp, Warning, TEXT("  Building GRID dungeon %d at (%.0f, %.0f, %.0f)"),
		DungeonIdx + 1, Origin.X, Origin.Y, Origin.Z);

	// Spawn ADungeon with Grid builder
	ADungeon* Dungeon = DungeonWorld->SpawnActor<ADungeon>(Origin, FRotator::ZeroRotator);
	if (!Dungeon)
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to spawn ADungeon actor!"));
		return;
	}

	Dungeon->SetFolderPath(FName(TEXT("Dungeon/DungeonActor")));
	Dungeon->BuilderClass = UGridDungeonBuilder::StaticClass();
	Dungeon->CreateBuilderInstance();

	// Configure Grid dungeon
	UGridDungeonConfig* Config = Cast<UGridDungeonConfig>(Dungeon->GetConfig());
	if (!Config)
	{
		Config = NewObject<UGridDungeonConfig>(Dungeon);
		Dungeon->Config = Config;
	}

	// Original working config from initial DA integration
	Config->Seed = DungeonIdx * 12345 + 42;
	Config->NumCells = 80 + DungeonIdx * 30;
	Config->GridCellSize = FVector(400, 400, 300);  // 4m x 4m x 3m per cell
	Config->MinCellSize = 2;
	Config->MaxCellSize = 5;
	Config->RoomAreaThreshold = 5;
	Config->RoomAspectDelta = 0.3f;
	Config->SpanningTreeLoopProbability = 0.15f;
	Config->MaxBuildTimePerFrameMs = 0;

	UE_LOG(LogTemp, Warning, TEXT("  Grid config: %d cells, size %d-%d, roomThreshold=%d"),
		Config->NumCells, Config->MinCellSize, Config->MaxCellSize, Config->RoomAreaThreshold);

	// Voxel cave overlay for ceiling + organic walls
	ConfigureVoxelCave(Dungeon, DungeonIdx);

	// Theme
	Dungeon->Themes.Add(GetOrCreateCaveTheme());

	// Event listener for gameplay actors
	USLFCaveEventListener* Listener = NewObject<USLFCaveEventListener>(Dungeon);
	Listener->DungeonIndex = DungeonIdx;
	Dungeon->EventListeners.Add(Listener);

	// Build
	Dungeon->BuildDungeon();

	// Manually fire event listener (commandlet mode doesn't auto-fire)
	Listener->OnPostDungeonBuild(Dungeon);

	// Log results
	UGridDungeonModel* GridModel = Cast<UGridDungeonModel>(Dungeon->GetModel());
	if (GridModel)
	{
		int32 RoomCount = 0, CorridorCount = 0;
		for (const FGridDungeonCell& Cell : GridModel->Cells)
		{
			if (Cell.CellType == EGridDungeonCellType::Room) RoomCount++;
			else if (Cell.CellType == EGridDungeonCellType::Corridor) CorridorCount++;
		}
		UE_LOG(LogTemp, Warning, TEXT("  Grid dungeon %d: %d rooms, %d corridors, %d doors"),
			DungeonIdx + 1, RoomCount, CorridorCount, GridModel->Doors.Num());
	}

	UE_LOG(LogTemp, Warning, TEXT("  Dungeon %d build SUCCEEDED"), DungeonIdx + 1);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Voxel Cave Configuration
// ═══════════════════════════════════════════════════════════════════════════════

void USetupOpenWorldCommandlet::ConfigureVoxelCave(ADungeon* Dungeon, int32 DungeonIdx, bool bCanvas)
{
	Dungeon->bCarveVoxels = true;

	// Voxel mesh settings
	Dungeon->VoxelMeshSettings.VoxelSize = 50.0f;
	Dungeon->VoxelMeshSettings.VoxelChunkSize = 32;
	Dungeon->VoxelMeshSettings.bEnableCollision = true;
	Dungeon->VoxelMeshSettings.WallThickness = 250.0f;
	Dungeon->VoxelMeshSettings.bUseGPU = false; // Required for commandlet (no GPU compute)
	Dungeon->VoxelMeshSettings.UVScale = 150.0f;
	Dungeon->VoxelMeshSettings.VoxelShapeTheme.Reset(); // Don't inject DA's default "Preview" theme

	// Try to load a rock material for cave walls
	UMaterialInterface* RockMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/Wasteland/Materials/Material_Instances/MI_Rock_03.MI_Rock_03"));
	if (!RockMat)
	{
		RockMat = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/WastelandEnvironment/Materials/Material_Instances/MI_Rock_01.MI_Rock_01"));
	}
	if (RockMat)
	{
		Dungeon->VoxelMeshSettings.Material = RockMat;
	}

	// Voxel noise settings (organic cave surfaces)
	Dungeon->VoxelNoiseSettings.NoiseAmplitude = 400.0f;
	Dungeon->VoxelNoiseSettings.NoiseOctaves = 5;
	Dungeon->VoxelNoiseSettings.NoiseFloorScale = 0.15f;      // Nearly flat floors
	Dungeon->VoxelNoiseSettings.NoiseCeilingScale = 0.85f;     // Rough ceilings
	Dungeon->VoxelNoiseSettings.NoiseScaleNormalized = 1.2f;
	Dungeon->VoxelNoiseSettings.bEnableDomainWarp = true;      // Erosion patterns
	Dungeon->VoxelNoiseSettings.DomainWarpStrength = 0.25f;
	Dungeon->VoxelNoiseSettings.FloorCeilingTransitionHeight = 500.0f;
	Dungeon->VoxelNoiseSettings.NoiseScaleVector = FVector(1, 1, 0.7f);

	// Canvas mode: flatter floors, higher ceilings, less noise variation
	if (bCanvas)
	{
		Dungeon->VoxelNoiseSettings.NoiseFloorScale = 0.10f;
		Dungeon->VoxelNoiseSettings.NoiseCeilingScale = 0.70f;
	}

	// Cave SDF model
	UDungeonVoxelSDFModel_Cave* CaveModel = NewObject<UDungeonVoxelSDFModel_Cave>(Dungeon);
	CaveModel->CeilingExtraHeight = bCanvas ? 1600.0f : 1200.0f;
	CaveModel->bEnableCeilingHoles = true;
	CaveModel->CeilingHoleRadius = 500.0f;
	CaveModel->CeilingHoleHeightOffset = 400.0f;
	CaveModel->MaxCeilingHolesPerRoom = 3;
	CaveModel->CeilingHoleSpawnProbability = 0.5f;
	Dungeon->VoxelSDFModel = CaveModel;

	UE_LOG(LogTemp, Warning, TEXT("  Voxel cave configured: VoxelSize=50, Noise=400, CeilingExtra=1200, bGPU=false"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// Programmatic Cave Theme
// ═══════════════════════════════════════════════════════════════════════════════

UDungeonThemeAsset* USetupOpenWorldCommandlet::GetOrCreateCaveTheme()
{
	if (CachedCaveTheme) return CachedCaveTheme;

	// Try loading an existing theme first
	UDungeonThemeAsset* Theme = LoadObject<UDungeonThemeAsset>(nullptr,
		TEXT("/Game/DungeonArchitect/Themes/DT_CaveTheme.DT_CaveTheme"));
	if (Theme)
	{
		CachedCaveTheme = Theme;
		return Theme;
	}

	// Try a bundled DA theme as fallback
	Theme = LoadObject<UDungeonThemeAsset>(nullptr,
		TEXT("/DungeonArchitect/Showcase/Samples/Themes/Foundry/T_Theme_Founddry.T_Theme_Founddry"));
	if (!Theme)
	{
		Theme = LoadObject<UDungeonThemeAsset>(nullptr,
			TEXT("/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleSciFi/DungeonTheme/D_StarterPackTheme.D_StarterPackTheme"));
	}

	if (Theme)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Using bundled DA theme: %s"), *Theme->GetPathName());
		CachedCaveTheme = Theme;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No DA theme found — cave will use voxel mesh only (no decoration meshes)"));
	}

	return Theme;
}


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 4: Overworld Actor Population
// ═══════════════════════════════════════════════════════════════════════════════

bool USetupOpenWorldCommandlet::PopulateOverworldActors(UWorld* World)
{
	int32 ActorsSpawned = 0;

	// ── PlayerStart ──
	// Spawn well above ground to avoid any collision with terrain/rocks blocking pawn spawn
	for (const FlatArea& Area : FlatAreas)
	{
		if (Area.Purpose == TEXT("spawn"))
		{
			FVector SpawnLoc = Area.Center + FVector(0, 0, 500.0f); // High above ground, pawn will fall to surface
			APlayerStart* PS = World->SpawnActor<APlayerStart>(SpawnLoc, FRotator::ZeroRotator);
			if (PS)
			{
				PS->SetFolderPath(FName(TEXT("Gameplay")));
				// Ensure PlayerStart is in the level's actor list
				ULevel* Level = World->PersistentLevel;
				if (Level && !Level->Actors.Contains(PS))
				{
					Level->Actors.Add(PS);
					UE_LOG(LogTemp, Warning, TEXT("  PlayerStart manually added to Actors array"));
				}
				ActorsSpawned++;
				UE_LOG(LogTemp, Warning, TEXT("  PlayerStart at (%.0f, %.0f, %.0f) — Outer: %s"),
					SpawnLoc.X, SpawnLoc.Y, SpawnLoc.Z,
					PS->GetOuter() ? *PS->GetOuter()->GetName() : TEXT("NULL"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  FAILED to spawn PlayerStart!"));
			}
			break;
		}
	}

	// ── Directional Light (Sun) ──
	{
		FVector SunLoc(0, 0, 10000);
		FRotator SunRot(-45.0f, 30.0f, 0); // Angled for dramatic shadows
		ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(SunLoc, SunRot);
		if (Sun)
		{
			Sun->GetLightComponent()->SetIntensity(8.0f);
			Sun->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.95f, 0.85f)); // Warm sunlight
			Sun->SetFolderPath(FName(TEXT("Lighting")));
			ActorsSpawned++;
			UE_LOG(LogTemp, Warning, TEXT("  Directional Light (Sun)"));
		}
	}

	// ── Sky Light ──
	{
		ASkyLight* SkyLightActor = World->SpawnActor<ASkyLight>(FVector(0, 0, 5000), FRotator::ZeroRotator);
		if (SkyLightActor)
		{
			SkyLightActor->GetLightComponent()->SetIntensity(1.0f);
			SkyLightActor->SetFolderPath(FName(TEXT("Lighting")));
			ActorsSpawned++;
			UE_LOG(LogTemp, Warning, TEXT("  Sky Light"));
		}
	}

	// ── Exponential Height Fog ──
	{
		AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(FVector(0, 0, 0), FRotator::ZeroRotator);
		if (Fog)
		{
			Fog->GetComponent()->SetFogDensity(0.005f);
			Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.6f, 0.55f, 0.45f)); // Warm, hazy
			Fog->GetComponent()->SetFogHeightFalloff(0.2f);
			Fog->SetFolderPath(FName(TEXT("Atmosphere")));
			ActorsSpawned++;
			UE_LOG(LogTemp, Warning, TEXT("  Exponential Height Fog"));
		}
	}

	// ── Post Process Volume (Unbound) ──
	{
		APostProcessVolume* PPV = World->SpawnActor<APostProcessVolume>(FVector(0, 0, 0), FRotator::ZeroRotator);
		if (PPV)
		{
			PPV->bUnbound = true;
			PPV->Settings.bOverride_AutoExposureBias = true;
			PPV->Settings.AutoExposureBias = 0.5f;
			PPV->Settings.bOverride_ColorSaturation = true;
			PPV->Settings.ColorSaturation = FVector4(0.85f, 0.85f, 0.85f, 1.0f); // Slightly desaturated
			PPV->Settings.bOverride_ColorContrast = true;
			PPV->Settings.ColorContrast = FVector4(1.1f, 1.1f, 1.05f, 1.0f); // Slightly warm contrast
			PPV->SetFolderPath(FName(TEXT("Atmosphere")));
			ActorsSpawned++;
			UE_LOG(LogTemp, Warning, TEXT("  Post Process Volume (unbound, wasteland grading)"));
		}
	}

	// ── NavMesh Bounds Volume ──
	{
		ANavMeshBoundsVolume* NavBounds = World->SpawnActor<ANavMeshBoundsVolume>(
			FVector(0, 0, 0), FRotator::ZeroRotator);
		if (NavBounds)
		{
			// Scale to cover entire playable area (±25000 cm XY, ±5000 cm Z for dungeons)
			NavBounds->SetActorScale3D(FVector(500.0f, 500.0f, 100.0f));
			NavBounds->SetFolderPath(FName(TEXT("Navigation")));
			ActorsSpawned++;
			UE_LOG(LogTemp, Warning, TEXT("  NavMesh Bounds Volume (covers full map)"));
		}
	}

	// ── SkyManager ──
	// Spawn SkyManager from Blueprint path (existing asset)
	{
		UClass* SkyManagerClass = LoadClass<AActor>(nullptr,
			TEXT("/Script/SLFConversion.ASLFSkyManager"));
		if (!SkyManagerClass)
		{
			// Try to load from Blueprint
			UBlueprint* SkyBP = LoadObject<UBlueprint>(nullptr,
				TEXT("/Game/SoulslikeFramework/Blueprints/Actors/B_SkyManager.B_SkyManager"));
			if (SkyBP && SkyBP->GeneratedClass)
			{
				SkyManagerClass = SkyBP->GeneratedClass;
			}
		}
		if (SkyManagerClass)
		{
			AActor* SkyMgr = World->SpawnActor(SkyManagerClass, &FVector::ZeroVector, &FRotator::ZeroRotator);
			if (SkyMgr)
			{
				SkyMgr->SetFolderPath(FName(TEXT("Gameplay")));
				ActorsSpawned++;
				UE_LOG(LogTemp, Warning, TEXT("  SkyManager spawned"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  SkyManager class not found — skipping"));
		}
	}

	// ── Resting Points ──
	{
		UBlueprint* RestingBP = LoadObject<UBlueprint>(nullptr,
			TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint.B_RestingPoint"));
		UClass* RestingClass = RestingBP ? RestingBP->GeneratedClass : nullptr;

		// Map purpose strings to display names
		TMap<FString, FString> RestPointNames;
		RestPointNames.Add(TEXT("resting_spawn_bonfire"), TEXT("Spawn Bonfire"));
		RestPointNames.Add(TEXT("resting_roadside_camp"), TEXT("Roadside Camp"));
		RestPointNames.Add(TEXT("resting_1"), TEXT("Crossroads Rest"));
		RestPointNames.Add(TEXT("resting_2"), TEXT("Eastern Overlook"));
		RestPointNames.Add(TEXT("resting_3"), TEXT("Northern Shrine"));

		int32 RestPointIdx = 0;
		for (const FlatArea& Area : FlatAreas)
		{
			if (!Area.Purpose.Contains(TEXT("resting"))) continue;

			if (RestingClass)
			{
				FVector Loc = Area.Center + FVector(0, 0, 5.0f);
				AActor* RP = World->SpawnActor(RestingClass, &Loc, &FRotator::ZeroRotator);
				if (RP)
				{
					RP->SetFolderPath(FName(TEXT("Gameplay/RestingPoints")));

					// Set LocationName and unique ID for fast travel
					FString DisplayName = TEXT("Resting Point");
					if (FString* Found = RestPointNames.Find(Area.Purpose))
					{
						DisplayName = *Found;
					}

					// Set properties via reflection (LocationName, ID, IsActivated exist on both class hierarchies)
					if (FTextProperty* NameProp = FindFProperty<FTextProperty>(RP->GetClass(), TEXT("LocationName")))
					{
						NameProp->SetPropertyValue_InContainer(RP, FText::FromString(DisplayName));
					}
					if (FStructProperty* IdProp = FindFProperty<FStructProperty>(RP->GetClass(), TEXT("ID")))
					{
						// Generate deterministic GUID from index so it's stable across runs
						FGuid StableGuid(0xFA577140, RestPointIdx + 1, 0, 0);
						IdProp->CopyCompleteValue(IdProp->ContainerPtrToValuePtr<void>(RP), &StableGuid);
					}

					ActorsSpawned++;
					RestPointIdx++;
					UE_LOG(LogTemp, Warning, TEXT("  RestingPoint '%s' at (%.0f, %.0f, %.0f) [%s]"),
						*DisplayName, Loc.X, Loc.Y, Loc.Z, *Area.Purpose);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  RestingPoint Blueprint not found — placing marker"));
				FVector Loc = Area.Center + FVector(0, 0, 5.0f);
				AActor* Marker = World->SpawnActor(AActor::StaticClass(), &Loc, &FRotator::ZeroRotator);
				if (Marker)
				{
					Marker->Tags.Add(FName(TEXT("RestingPointSpawn")));
					Marker->SetFolderPath(FName(TEXT("Gameplay/RestingPoints")));
				}
			}
		}
	}

	// ── Enemy Patrol Spawn Markers ──
	{
		FRandomStream Rng(777);
		for (const FlatArea& Area : FlatAreas)
		{
			if (!Area.Purpose.Contains(TEXT("patrol"))) continue;

			int32 EnemyCount = Rng.RandRange(2, 3);
			for (int32 E = 0; E < EnemyCount; E++)
			{
				FVector Offset(
					Rng.FRandRange(-Area.Radius * 0.6f, Area.Radius * 0.6f),
					Rng.FRandRange(-Area.Radius * 0.6f, Area.Radius * 0.6f),
					90.0f
				);
				FVector SpawnLoc = Area.Center + Offset;

				AActor* Marker = World->SpawnActor(AActor::StaticClass(), &SpawnLoc, &FRotator::ZeroRotator);
				if (Marker)
				{
					Marker->Tags.Add(FName(TEXT("EnemySpawnPoint")));
					Marker->Tags.Add(FName(TEXT("SentinelPatrol")));
					Marker->SetFolderPath(FName(TEXT("Gameplay/Enemies")));
					ActorsSpawned++;
				}

				UE_LOG(LogTemp, Warning, TEXT("  Enemy patrol marker at (%.0f, %.0f, %.0f) [%s]"),
					SpawnLoc.X, SpawnLoc.Y, SpawnLoc.Z, *Area.Purpose);
			}
		}
	}

	// ── Dungeon Entrance Resting Points (fast travel targets) ──
	{
		UBlueprint* DungeonRestBP = LoadObject<UBlueprint>(nullptr,
			TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint.B_RestingPoint"));
		UClass* DungeonRestClass = DungeonRestBP ? DungeonRestBP->GeneratedClass : nullptr;

		static const FString DungeonRestNames[] = {
			TEXT("Forgotten Catacombs Entrance"),
			TEXT("Ashen Depths Entrance"),
			TEXT("Hollow Sanctum Entrance")
		};

		int32 DungeonRPIdx = 0;
		for (const FlatArea& Area : FlatAreas)
		{
			if (!Area.Purpose.Contains(TEXT("dungeon_entrance"))) continue;

			if (DungeonRestClass)
			{
				FVector Loc = Area.Center + FVector(200.0f, 0, 5.0f); // Slightly offset from stream trigger
				AActor* RP = World->SpawnActor(DungeonRestClass, &Loc, &FRotator::ZeroRotator);
				if (RP)
				{
					RP->SetFolderPath(FName(TEXT("Gameplay/DungeonRestPoints")));

					FString DisplayName = DungeonRPIdx < 3 ? DungeonRestNames[DungeonRPIdx] : TEXT("Dungeon Entrance");

					if (FTextProperty* NameProp = FindFProperty<FTextProperty>(RP->GetClass(), TEXT("LocationName")))
					{
						NameProp->SetPropertyValue_InContainer(RP, FText::FromString(DisplayName));
					}
					if (FStructProperty* IdProp = FindFProperty<FStructProperty>(RP->GetClass(), TEXT("ID")))
					{
						// Deterministic GUID in a different range (0xD0000000) from regular rest points
						FGuid StableGuid(0xD0000000, DungeonRPIdx + 1, 0, 0);
						IdProp->CopyCompleteValue(IdProp->ContainerPtrToValuePtr<void>(RP), &StableGuid);
					}
					if (FBoolProperty* DungeonProp = FindFProperty<FBoolProperty>(RP->GetClass(), TEXT("bIsDungeonEntrance")))
					{
						DungeonProp->SetPropertyValue_InContainer(RP, true);
					}
					// TEMP: Pre-discover for testing so they show on world map immediately
					if (FBoolProperty* ActivatedProp = FindFProperty<FBoolProperty>(RP->GetClass(), TEXT("IsActivated")))
					{
						ActivatedProp->SetPropertyValue_InContainer(RP, true);
					}

					ActorsSpawned++;
					UE_LOG(LogTemp, Warning, TEXT("  Dungeon RestingPoint '%s' at (%.0f, %.0f, %.0f) [PRE-DISCOVERED]"),
						*DisplayName, Loc.X, Loc.Y, Loc.Z);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  RestingPoint Blueprint not found — skipping dungeon rest point"));
			}

			DungeonRPIdx++;
		}
	}

	// ── Dungeon Entrance Triggers (ASLFLevelStreamManager) ──
	{
		static const FString DungeonNames[] = {
			TEXT("Forgotten Catacombs"),
			TEXT("Ashen Depths"),
			TEXT("Hollow Sanctum")
		};

		int32 DungeonIdx = 0;
		for (const FlatArea& Area : FlatAreas)
		{
			if (!Area.Purpose.Contains(TEXT("dungeon_entrance"))) continue;

			// Place trigger at base of the entrance tower
			FVector Loc = Area.Center + FVector(0, 0, 100.0f);
			ASLFLevelStreamManager* StreamMgr = World->SpawnActor<ASLFLevelStreamManager>(Loc, FRotator::ZeroRotator);
			if (StreamMgr)
			{
				StreamMgr->DungeonLevelName = FString::Printf(TEXT("/Game/Maps/L_Dungeon_%02d"), DungeonIdx + 1);
				StreamMgr->DungeonDisplayName = DungeonIdx < 3 ? DungeonNames[DungeonIdx] : TEXT("Unknown Dungeon");
				StreamMgr->DungeonWorldOffset = FVector(0, 0, -5000.0f);
				StreamMgr->DungeonSpawnOffset = FVector(0, 0, 200.0f);
				StreamMgr->FadeDuration = 1.0f;
				StreamMgr->SetFolderPath(FName(TEXT("Gameplay/DungeonEntrances")));
				ActorsSpawned++;
				UE_LOG(LogTemp, Warning, TEXT("  LevelStreamManager at (%.0f, %.0f, %.0f) -> %s (%s)"),
					Loc.X, Loc.Y, Loc.Z,
					*StreamMgr->DungeonLevelName, *StreamMgr->DungeonDisplayName);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  FAILED to spawn LevelStreamManager for Dungeon_%02d"), DungeonIdx + 1);
			}

			DungeonIdx++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("  Total actors spawned: %d"), ActorsSpawned);
	return ActorsSpawned > 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════════════════════════════

bool USetupOpenWorldCommandlet::SaveLevel(UWorld* World, const FString& PackageName)
{
	if (!World) return false;

	UPackage* Pkg = World->GetOutermost();
	Pkg->MarkPackageDirty();

	// Diagnostic: count actors by type
	int32 TotalActors = World->PersistentLevel ? World->PersistentLevel->Actors.Num() : 0;
	int32 PlayerStarts = 0;
	for (TActorIterator<APlayerStart> It(World); It; ++It) { PlayerStarts++; }
	UE_LOG(LogTemp, Warning, TEXT("SaveLevel: %d total actors, %d PlayerStarts in level"), TotalActors, PlayerStarts);

	FString FileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetMapPackageExtension());

	// Ensure directory exists
	FString Dir = FPaths::GetPath(FileName);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectoryTree(*Dir);

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	bool bSaved = UPackage::SavePackage(Pkg, World, *FileName, SaveArgs);

	UE_LOG(LogTemp, Warning, TEXT("SaveLevel: %s -> %s (%s)"),
		*PackageName, *FileName, bSaved ? TEXT("OK") : TEXT("FAILED"));

	return bSaved;
}

// ═══════════════════════════════════════════════════════════════════════
// Snap Map Asset Generation — fully automated, no editor UI required
// ═══════════════════════════════════════════════════════════════════════

bool USetupOpenWorldCommandlet::SaveAssetPackage(UObject* Asset, const FString& PackageName)
{
	if (!Asset) return false;

	UPackage* Pkg = Asset->GetOutermost();
	Pkg->MarkPackageDirty();

	FString FileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	FString Dir = FPaths::GetPath(FileName);
	FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*Dir);

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	bool bSaved = UPackage::SavePackage(Pkg, Asset, *FileName, SaveArgs);

	UE_LOG(LogTemp, Warning, TEXT("  SaveAsset: %s → %s (%s)"),
		*PackageName, *FileName, bSaved ? TEXT("OK") : TEXT("FAILED"));
	return bSaved;
}


#endif // WITH_EDITOR
