// SetupOpenWorldCommandlet.cpp
// Generates open world level with Perlin-noise landscape, Wasteland mesh dressing,
// Dungeon Architect procedural dungeons, and gameplay actors.

#include "SetupOpenWorldCommandlet.h"

#if WITH_EDITOR
#include "SLFAutomationLibrary.h"
#include "UObject/UnrealType.h"
#include "Blueprints/Actors/SLFLevelStreamManager.h"
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

// Dungeon Architect
#include "Core/Dungeon.h"
#include "Core/DungeonConfig.h"
#include "Core/DungeonBuilder.h"
#include "Builders/Grid/GridDungeonBuilder.h"
#include "Builders/Grid/GridDungeonConfig.h"
#include "Frameworks/ThemeEngine/DungeonThemeAsset.h"
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
	for (const FlatArea& Area : FlatAreas)
	{
		if (!Area.Purpose.Contains(TEXT("dungeon_entrance"))) continue;

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
	}

	return DungeonIndex > 0;
}

bool USetupOpenWorldCommandlet::CreateDungeonLevel(int32 DungeonIndex, const FString& LevelName, FVector OverworldEntrance)
{
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
	// Try Dungeon Architect (native API — direct class usage, no reflection)
	// ══════════════════════════════════════════════════════════════════
	bool bUsedDA = false;

	// Step 1: Find a theme asset — try multiple paths
	UDungeonThemeAsset* Theme = nullptr;
	{
		// Try known pre-built theme paths (DA plugin content mount)
		const TArray<FString> ThemePaths = {
			TEXT("/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleSciFi/DungeonTheme/D_StarterPackTheme"),
			TEXT("/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleShapes/Dungeons/D_MyTheme1"),
			TEXT("/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleShapes/Dungeons/D_MyTheme_Night"),
			TEXT("/DungeonArchitect/Showcase/Samples/Themes/Foundry/T_Theme_Founddry"),
			TEXT("/DungeonArchitect/Showcase/Samples/Features/GridFlow/DA_GridFlow/Themes/T_GridFlow_Prehistoric"),
		};

		for (const FString& Path : ThemePaths)
		{
			Theme = LoadObject<UDungeonThemeAsset>(nullptr, *Path);
			if (Theme)
			{
				UE_LOG(LogTemp, Warning, TEXT("  Loaded DA theme: %s"), *Path);
				break;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("  Theme not found at: %s"), *Path);
			}
		}

		// Fallback: search asset registry for ANY DungeonThemeAsset
		if (!Theme)
		{
			IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
			TArray<FAssetData> ThemeAssets;
			AssetRegistry.GetAssetsByClass(UDungeonThemeAsset::StaticClass()->GetClassPathName(), ThemeAssets, true);
			UE_LOG(LogTemp, Warning, TEXT("  Asset registry found %d DungeonThemeAsset(s)"), ThemeAssets.Num());

			for (const FAssetData& Asset : ThemeAssets)
			{
				FString AssetPath = Asset.GetObjectPathString();
				UE_LOG(LogTemp, Log, TEXT("    Found theme: %s"), *AssetPath);
				Theme = Cast<UDungeonThemeAsset>(Asset.GetAsset());
				if (Theme)
				{
					UE_LOG(LogTemp, Warning, TEXT("  Using theme from registry: %s"), *AssetPath);
					break;
				}
			}
		}
	}

	if (Theme)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Dungeon Architect: spawning ADungeon with Grid builder"));

		// Step 2: Spawn ADungeon actor
		ADungeon* DungeonActor = DungeonWorld->SpawnActor<ADungeon>(DungeonOrigin, FRotator::ZeroRotator);
		if (DungeonActor)
		{
			// Step 3: Set builder class to Grid and create the builder instance
			DungeonActor->BuilderClass = UGridDungeonBuilder::StaticClass();
			DungeonActor->CreateBuilderInstance();

			// Step 4: Configure the grid dungeon config
			UGridDungeonConfig* GridConfig = Cast<UGridDungeonConfig>(DungeonActor->GetConfig());
			if (GridConfig)
			{
				GridConfig->Seed = DungeonIndex * 12345 + 42;
				GridConfig->NumCells = 80 + DungeonIndex * 30;   // More cells = bigger dungeon
				GridConfig->GridCellSize = FVector(400, 400, 300); // 4m x 4m x 3m per cell
				GridConfig->MinCellSize = 2;
				GridConfig->MaxCellSize = 5;
				GridConfig->RoomAreaThreshold = 5;
				GridConfig->RoomAspectDelta = 0.3f;
				GridConfig->SpanningTreeLoopProbability = 0.15f;  // Few loops, more linear
				GridConfig->MaxBuildTimePerFrameMs = 0;           // Build entire dungeon in one frame (commandlet)
				GridConfig->Instanced = false;                    // Non-instanced for commandlet save

				UE_LOG(LogTemp, Warning, TEXT("  GridConfig: Seed=%d NumCells=%d CellSize=(%.0f,%.0f,%.0f)"),
					GridConfig->Seed, GridConfig->NumCells,
					GridConfig->GridCellSize.X, GridConfig->GridCellSize.Y, GridConfig->GridCellSize.Z);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  Config is not GridDungeonConfig — using defaults"));
				UDungeonConfig* BaseConfig = DungeonActor->GetConfig();
				if (BaseConfig)
				{
					BaseConfig->Seed = DungeonIndex * 12345 + 42;
					BaseConfig->MaxBuildTimePerFrameMs = 0;
				}
			}

			// Step 5: Assign theme
			DungeonActor->Themes.Empty();
			DungeonActor->Themes.Add(Theme);

			// Step 6: Build the dungeon
			UE_LOG(LogTemp, Warning, TEXT("  Calling BuildDungeon()..."));
			DungeonActor->BuildDungeon();

			// Check build result
			UDungeonBuilder* Builder = DungeonActor->GetBuilder();
			if (Builder && Builder->HasBuildSucceeded())
			{
				bUsedDA = true;
				UE_LOG(LogTemp, Warning, TEXT("  Dungeon Architect BuildDungeon() SUCCEEDED"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  Dungeon Architect BuildDungeon() may have failed — checking spawned actors"));
				// Count spawned actors as a heuristic
				int32 SpawnedCount = 0;
				for (TActorIterator<AActor> It(DungeonWorld); It; ++It)
				{
					SpawnedCount++;
				}
				if (SpawnedCount > 5) // DA + config + a few generated actors
				{
					bUsedDA = true;
					UE_LOG(LogTemp, Warning, TEXT("  Found %d actors — DA likely generated content"), SpawnedCount);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("  Only %d actors — DA generation may have failed"), SpawnedCount);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("  Failed to spawn ADungeon actor!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No DA theme found — will use fallback room generator"));
	}

	// ══════════════════════════════════════════════════════════════════
	// Fallback: hand-built rooms with Wasteland meshes + basic geometry
	// ══════════════════════════════════════════════════════════════════
	if (!bUsedDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Building fallback dungeon rooms"));

		UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
		UStaticMesh* FloorMesh = CubeMesh; // Use cube for floor too — has collision unlike Plane

		if (!CubeMesh)
		{
			UE_LOG(LogTemp, Error, TEXT("  Failed to load Cube mesh!"));
			GEngine->DestroyWorldContext(DungeonWorld);
			return false;
		}

		// Try Wasteland meshes for variety
		TArray<FString> WallMeshPaths = DiscoverWastelandByPrefixes({TEXT("SM_WallMetal"), TEXT("SM_Corrugated"), TEXT("SM_Metals")});
		FRandomStream WastelandRng(DungeonIndex * 99 + 7);

		// Room layout
		const int32 NumRooms = 5 + DungeonIndex * 2;
		const float RoomSize = 1500.0f;
		const float WallHeight = 600.0f;
		const float WallThickness = 50.0f;
		const float HallwayWidth = 400.0f;

		struct DungeonRoom
		{
			FVector Center;
			float SizeX, SizeY;
			bool bIsBossRoom;
		};
		TArray<DungeonRoom> Rooms;

		// Entrance room at dungeon origin
		Rooms.Add({DungeonOrigin + FVector(0, 0, 0), RoomSize, RoomSize, false});

		for (int32 R = 1; R < NumRooms; R++)
		{
			int32 ParentIdx = Rng.RandRange(FMath::Max(0, R - 3), R - 1);
			const DungeonRoom& Parent = Rooms[ParentIdx];

			int32 Dir = Rng.RandRange(0, 3);
			FVector Offset;
			switch (Dir)
			{
			case 0: Offset = FVector(RoomSize + HallwayWidth, 0, 0); break;
			case 1: Offset = FVector(0, RoomSize + HallwayWidth, 0); break;
			case 2: Offset = FVector(-(RoomSize + HallwayWidth), 0, 0); break;
			default: Offset = FVector(0, -(RoomSize + HallwayWidth), 0); break;
			}

			float RSizeX = Rng.FRandRange(RoomSize * 0.8f, RoomSize * 1.5f);
			float RSizeY = Rng.FRandRange(RoomSize * 0.8f, RoomSize * 1.5f);
			bool bBoss = (R == NumRooms - 1);
			if (bBoss) { RSizeX *= 1.5f; RSizeY *= 1.5f; }

			Rooms.Add({Parent.Center + Offset, RSizeX, RSizeY, bBoss});
		}

		// Build geometry for each room
		for (int32 R = 0; R < Rooms.Num(); R++)
		{
			const DungeonRoom& Room = Rooms[R];

			// Floor — thick cube slab so it has collision
			{
				AStaticMeshActor* Floor = DungeonWorld->SpawnActor<AStaticMeshActor>(
					Room.Center + FVector(0, 0, -55), FRotator::ZeroRotator);
				if (Floor)
				{
					Floor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
					Floor->SetActorScale3D(FVector(Room.SizeX / 100.0f, Room.SizeY / 100.0f, 0.5f));
					Floor->SetFolderPath(FName(TEXT("Dungeon/Floors")));
				}
			}

			// Ceiling
			{
				AStaticMeshActor* Ceiling = DungeonWorld->SpawnActor<AStaticMeshActor>(
					Room.Center + FVector(0, 0, WallHeight + 25), FRotator::ZeroRotator);
				if (Ceiling)
				{
					Ceiling->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
					Ceiling->SetActorScale3D(FVector(Room.SizeX / 100.0f, Room.SizeY / 100.0f, 0.5f));
					Ceiling->SetFolderPath(FName(TEXT("Dungeon/Ceilings")));
				}
			}

			// 4 Walls
			auto SpawnWall = [&](FVector WallCenter, FVector WallScale)
			{
				AStaticMeshActor* Wall = DungeonWorld->SpawnActor<AStaticMeshActor>(
					WallCenter, FRotator::ZeroRotator);
				if (Wall)
				{
					UStaticMesh* WallMesh = CubeMesh;
					if (WallMeshPaths.Num() > 0)
					{
						int32 Idx = WastelandRng.RandRange(0, WallMeshPaths.Num() - 1);
						UStaticMesh* WM = LoadObject<UStaticMesh>(nullptr, *WallMeshPaths[Idx]);
						if (WM) WallMesh = WM;
					}
					Wall->GetStaticMeshComponent()->SetStaticMesh(WallMesh);
					Wall->SetActorScale3D(WallScale);
					Wall->SetFolderPath(FName(TEXT("Dungeon/Walls")));
				}
			};

			float HalfX = Room.SizeX / 2.0f;
			float HalfY = Room.SizeY / 2.0f;
			float HalfH = WallHeight / 2.0f;

			SpawnWall(Room.Center + FVector(HalfX, 0, HalfH),
				FVector(WallThickness / 100.0f, Room.SizeY / 100.0f, WallHeight / 100.0f));
			SpawnWall(Room.Center + FVector(-HalfX, 0, HalfH),
				FVector(WallThickness / 100.0f, Room.SizeY / 100.0f, WallHeight / 100.0f));
			SpawnWall(Room.Center + FVector(0, HalfY, HalfH),
				FVector(Room.SizeX / 100.0f, WallThickness / 100.0f, WallHeight / 100.0f));
			SpawnWall(Room.Center + FVector(0, -HalfY, HalfH),
				FVector(Room.SizeX / 100.0f, WallThickness / 100.0f, WallHeight / 100.0f));

			// Point light in each room
			{
				APointLight* RoomLight = DungeonWorld->SpawnActor<APointLight>(
					Room.Center + FVector(0, 0, WallHeight * 0.8f), FRotator::ZeroRotator);
				if (RoomLight)
				{
					RoomLight->PointLightComponent->SetIntensity(Room.bIsBossRoom ? 30000.0f : 15000.0f);
					RoomLight->PointLightComponent->SetAttenuationRadius(Room.SizeX * 1.5f);
					RoomLight->PointLightComponent->SetLightColor(FLinearColor(1.0f, 0.7f, 0.4f)); // Warm torch
					RoomLight->SetFolderPath(FName(TEXT("Dungeon/Lights")));
				}
			}

			UE_LOG(LogTemp, Log, TEXT("  Room %d: Center=(%.0f, %.0f, %.0f) Size=%.0fx%.0f %s"),
				R, Room.Center.X, Room.Center.Y, Room.Center.Z,
				Room.SizeX, Room.SizeY, Room.bIsBossRoom ? TEXT("[BOSS]") : TEXT(""));
		}

		// Spawn enemy markers in dungeon rooms (skip entrance room 0)
		for (int32 R = 1; R < Rooms.Num(); R++)
		{
			const DungeonRoom& Room = Rooms[R];
			int32 EnemyCount = Room.bIsBossRoom ? 1 : (Rng.FRandRange(0.0f, 1.0f) > 0.4f ? 1 : 0);
			for (int32 E = 0; E < EnemyCount; E++)
			{
				FVector SpawnPos = Room.Center + FVector(
					Rng.FRandRange(-Room.SizeX * 0.3f, Room.SizeX * 0.3f),
					Rng.FRandRange(-Room.SizeY * 0.3f, Room.SizeY * 0.3f),
					90.0f);

				AActor* Marker = DungeonWorld->SpawnActor(AActor::StaticClass(), &SpawnPos, &FRotator::ZeroRotator);
				if (Marker)
				{
					Marker->Tags.Add(FName(TEXT("EnemySpawnPoint")));
					if (Room.bIsBossRoom) Marker->Tags.Add(FName(TEXT("BossEnemy")));
					Marker->SetFolderPath(FName(TEXT("Dungeon/SpawnPoints")));
				}
				UE_LOG(LogTemp, Warning, TEXT("  Enemy spawn: Room %d at (%.0f, %.0f, %.0f) %s"),
					R, SpawnPos.X, SpawnPos.Y, SpawnPos.Z,
					Room.bIsBossRoom ? TEXT("[BOSS]") : TEXT(""));
			}
		}
	}

	// Add lighting to DA-generated dungeons too (DA themes might not include lights)
	if (bUsedDA)
	{
		// Add ambient point lights throughout the dungeon area
		for (int32 LightIdx = 0; LightIdx < 8; LightIdx++)
		{
			FVector LightPos = DungeonOrigin + FVector(
				Rng.FRandRange(-3000.0f, 3000.0f),
				Rng.FRandRange(-3000.0f, 3000.0f),
				200.0f);
			APointLight* DungeonLight = DungeonWorld->SpawnActor<APointLight>(LightPos, FRotator::ZeroRotator);
			if (DungeonLight)
			{
				DungeonLight->PointLightComponent->SetIntensity(20000.0f);
				DungeonLight->PointLightComponent->SetAttenuationRadius(2500.0f);
				DungeonLight->PointLightComponent->SetLightColor(FLinearColor(1.0f, 0.65f, 0.3f));
				DungeonLight->SetFolderPath(FName(TEXT("Dungeon/Lights")));
			}
		}
	}

	// Save dungeon level
	bool bSaved = SaveLevel(DungeonWorld, LevelName);
	GEngine->DestroyWorldContext(DungeonWorld);

	return bSaved;
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

#endif // WITH_EDITOR
