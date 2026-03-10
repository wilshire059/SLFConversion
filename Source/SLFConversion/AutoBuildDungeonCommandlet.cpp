// AutoBuildDungeonCommandlet.cpp
// Data-driven voxel cave generation using CellFlow builder.
// Single ADungeon with multi-path CellFlow + voxel carving.
// Supports biome presets via -Biome= argument.

#include "AutoBuildDungeonCommandlet.h"

#if WITH_EDITOR

#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/PointLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Framework/Application/SlateApplication.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"

// DA mesh wrapper
#include "Core/Actors/DungeonMesh.h"

// Dungeon Architect — Core
#include "Core/Dungeon.h"
#include "Core/DungeonConfig.h"
#include "Core/DungeonBuilder.h"
#include "Core/Volumes/DAVoxelVolume.h"
#include "Core/Volumes/DungeonThemeOverrideVolume.h"

// Dungeon Architect — CellFlow Builder
#include "Builders/CellFlow/CellFlowBuilder.h"
#include "Builders/CellFlow/CellFlowConfig.h"
#include "Builders/CellFlow/CellFlowAsset.h"
#include "Builders/CellFlow/CellFlowModel.h"

// Dungeon Architect — CellFlow Tasks
#include "Frameworks/Flow/ExecGraph/FlowExecGraphScript.h"
#include "Frameworks/Flow/ExecGraph/FlowExecTask.h"
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/Tasks/CellFlowLayoutTaskCreateCellsVoronoi.h"
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/Tasks/CellFlowLayoutTaskCreateMainPath.h"
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/Tasks/CellFlowLayoutTaskCreatePath.h"
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/Tasks/CellFlowLayoutTaskFinalize.h"

// Dungeon Architect — Layout Graph (node query)
#include "Frameworks/FlowImpl/CellFlow/LayoutGraph/CellFlowLayoutGraph.h"
#include "Frameworks/Flow/Domains/LayoutGraph/Core/FlowAbstractGraph.h"
#include "Frameworks/Flow/Domains/LayoutGraph/Core/FlowAbstractNode.h"
#include "Frameworks/Flow/Domains/LayoutGraph/Core/FlowAbstractLink.h"

// Dungeon Architect — Voxel + Theme
#include "Frameworks/Voxel/SDFModels/VoxelSDFModel.h"
#include "Frameworks/ThemeEngine/DungeonThemeAsset.h"
#include "Frameworks/ThemeEngine/Graph/DungeonThemeCompiledGraph.h"
#include "Frameworks/ThemeEngine/Graph/DungeonThemeGraphNodeData.h"

// Material + Texture imports
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Engine/Texture2D.h"
#include "Factories/TextureFactory.h"
#include "Misc/FileHelper.h"
#include "AssetCompilingManager.h"

// Cave manager
#include "Dungeon/SLFProceduralCaveManager.h"

#endif // WITH_EDITOR

UAutoBuildDungeonCommandlet::UAutoBuildDungeonCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

// ═══════════════════════════════════════════════════════════════════════════
// Biome Presets
// ═══════════════════════════════════════════════════════════════════════════

FDungeonBiomeConfig UAutoBuildDungeonCommandlet::GetBiomePreset(const FString& BiomeName)
{
	FDungeonBiomeConfig Cfg;

	if (BiomeName.Equals(TEXT("lava"), ESearchCase::IgnoreCase))
	{
		Cfg.BiomeName = TEXT("Lava Cavern");
		Cfg.WorldSize = FIntPoint(80, 80);
		Cfg.NumPoints = 1000;
		Cfg.ClusterBlobbiness = 0.8f;
		Cfg.SpokeMinSize = 8;
		Cfg.SpokeMaxSize = 15;
		Cfg.NoiseAmplitude = 300.0f;
		Cfg.WallThickness = 100.0f;
		Cfg.DomainWarpStrength = 0.4f;
		Cfg.MinHubBossDistance = 8000.0f;
		Cfg.MinHubTerminalDistance = 5000.0f;
		Cfg.PathLightColor = FLinearColor(1.0f, 0.4f, 0.1f);
		Cfg.PathLightIntensity = 30000.0f;
		Cfg.TorchColor = FLinearColor(1.0f, 0.3f, 0.05f);
		Cfg.TorchIntensity = 30000.0f;
		Cfg.AmbientColor = FLinearColor(0.4f, 0.1f, 0.05f);
		Cfg.AmbientIntensity = 12000.0f;
		Cfg.SkyLightColor = FLinearColor(0.6f, 0.2f, 0.1f);
		Cfg.DirLightColor = FLinearColor(1.0f, 0.5f, 0.2f);
		Cfg.BiomeTextureDiskPath = TEXT("C:/Users/james/Downloads/cave_materials/extracted/lava_icelandic_rock_cliff_smokagcp_2k");
		Cfg.BiomeMaterialPath = TEXT("/Game/Dungeons/Materials/M_Lava");
		Cfg.FlowAssetPath = TEXT("/Game/Dungeons/CellFlow/CF_LavaCavern");
		Cfg.ThemeAssetPath = TEXT("/Game/Dungeons/Themes/DT_LavaCavern");
		Cfg.MapPath = TEXT("/Game/Maps/L_LavaCavern");
	}
	else if (BiomeName.Equals(TEXT("ice"), ESearchCase::IgnoreCase))
	{
		Cfg.BiomeName = TEXT("Ice Grotto");
		Cfg.WorldSize = FIntPoint(100, 100);
		Cfg.NumPoints = 1300;
		Cfg.NumRelaxIterations = 15;
		Cfg.SpokeMinSize = 10;
		Cfg.SpokeMaxSize = 20;
		Cfg.NoiseAmplitude = 150.0f;
		Cfg.WallThickness = 150.0f;
		Cfg.NoiseOctaves = 4;
		Cfg.bDomainWarp = false;
		Cfg.MinHubBossDistance = 10000.0f;
		Cfg.MinHubTerminalDistance = 7000.0f;
		Cfg.PathLightColor = FLinearColor(0.7f, 0.85f, 1.0f);
		Cfg.PathLightIntensity = 20000.0f;
		Cfg.TorchColor = FLinearColor(0.5f, 0.7f, 1.0f);
		Cfg.TorchIntensity = 15000.0f;
		Cfg.AmbientColor = FLinearColor(0.6f, 0.8f, 1.0f);
		Cfg.AmbientIntensity = 10000.0f;
		Cfg.SkyLightColor = FLinearColor(0.7f, 0.8f, 1.0f);
		Cfg.SkyLightIntensity = 12.0f;
		Cfg.DirLightColor = FLinearColor(0.8f, 0.9f, 1.0f);
		Cfg.DirLightIntensity = 6.0f;
		Cfg.BiomeTextureDiskPath = TEXT("C:/Users/james/Downloads/cave_materials/extracted/snow_qbarr0_2k");
		Cfg.BiomeMaterialPath = TEXT("/Game/Dungeons/Materials/M_Ice");
		Cfg.FlowAssetPath = TEXT("/Game/Dungeons/CellFlow/CF_IceGrotto");
		Cfg.ThemeAssetPath = TEXT("/Game/Dungeons/Themes/DT_IceGrotto");
		Cfg.MapPath = TEXT("/Game/Maps/L_IceGrotto");
	}
	else if (BiomeName.Equals(TEXT("fungal"), ESearchCase::IgnoreCase))
	{
		Cfg.BiomeName = TEXT("Fungal Depths");
		Cfg.WorldSize = FIntPoint(140, 140);
		Cfg.NumPoints = 2000;
		Cfg.NumRelaxIterations = 10;
		Cfg.NoiseAmplitude = 250.0f;
		Cfg.WallThickness = 100.0f;
		Cfg.NoiseCeilingScale = 0.7f;
		Cfg.DomainWarpStrength = 0.35f;
		Cfg.CeilingExtraHeight = 1500.0f;
		Cfg.SpokeMinSize = 15;
		Cfg.SpokeMaxSize = 25;
		Cfg.MinHubBossDistance = 15000.0f;
		Cfg.MinHubTerminalDistance = 10000.0f;
		Cfg.PathLightColor = FLinearColor(0.6f, 0.3f, 0.8f);
		Cfg.PathLightIntensity = 18000.0f;
		Cfg.TorchColor = FLinearColor(0.4f, 0.8f, 0.3f);
		Cfg.TorchIntensity = 25000.0f;
		Cfg.AmbientColor = FLinearColor(0.3f, 0.5f, 0.6f);
		Cfg.AmbientIntensity = 6000.0f;
		Cfg.SkyLightColor = FLinearColor(0.3f, 0.4f, 0.5f);
		Cfg.SkyLightIntensity = 6.0f;
		Cfg.DirLightColor = FLinearColor(0.5f, 0.4f, 0.6f);
		Cfg.DirLightIntensity = 3.0f;
		Cfg.BiomeTextureDiskPath = TEXT("C:/Users/james/Downloads/cave_materials/extracted/mossy_rocky_ground_vcqhcig_2k");
		Cfg.BiomeMaterialPath = TEXT("/Game/Dungeons/Materials/M_Fungal");
		Cfg.FlowAssetPath = TEXT("/Game/Dungeons/CellFlow/CF_FungalDepths");
		Cfg.ThemeAssetPath = TEXT("/Game/Dungeons/Themes/DT_FungalDepths");
		Cfg.MapPath = TEXT("/Game/Maps/L_FungalDepths");
	}
	else if (BiomeName.Equals(TEXT("abyss"), ESearchCase::IgnoreCase))
	{
		Cfg.BiomeName = TEXT("The Abyss");
		Cfg.WorldSize = FIntPoint(60, 60);
		Cfg.NumPoints = 800;
		Cfg.NumRelaxIterations = 8;
		Cfg.EdgeConnectionThreshold = 0.08f;
		Cfg.NoiseAmplitude = 400.0f;
		Cfg.NoiseOctaves = 6;
		Cfg.WallThickness = 200.0f;
		Cfg.bDomainWarp = true;
		Cfg.DomainWarpStrength = 0.5f;
		Cfg.CeilingExtraHeight = 2000.0f;
		Cfg.SpokeMinSize = 6;
		Cfg.SpokeMaxSize = 12;
		Cfg.MinHubBossDistance = 5000.0f;
		Cfg.MinHubTerminalDistance = 3000.0f;
		Cfg.PathLightColor = FLinearColor(0.3f, 0.05f, 0.05f);
		Cfg.PathLightIntensity = 15000.0f;
		Cfg.TorchColor = FLinearColor(0.8f, 0.1f, 0.05f);
		Cfg.TorchIntensity = 12000.0f;
		Cfg.AmbientColor = FLinearColor(0.15f, 0.05f, 0.1f);
		Cfg.AmbientIntensity = 4000.0f;
		Cfg.SkyLightColor = FLinearColor(0.1f, 0.05f, 0.15f);
		Cfg.SkyLightIntensity = 3.0f;
		Cfg.DirLightColor = FLinearColor(0.3f, 0.1f, 0.15f);
		Cfg.DirLightIntensity = 2.0f;
		Cfg.BiomeTextureDiskPath = TEXT("C:/Users/james/Downloads/cave_materials/extracted/slate");
		Cfg.BiomeMaterialPath = TEXT("/Game/Dungeons/Materials/M_Abyss");
		Cfg.FlowAssetPath = TEXT("/Game/Dungeons/CellFlow/CF_Abyss");
		Cfg.ThemeAssetPath = TEXT("/Game/Dungeons/Themes/DT_Abyss");
		Cfg.MapPath = TEXT("/Game/Maps/L_Abyss");
	}
	else // stone (default)
	{
		Cfg.BiomeName = TEXT("Stone Cave");
		// All defaults from the struct initializer
	}

	return Cfg;
}

int32 UAutoBuildDungeonCommandlet::Main(const FString& Params)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT(""));
	UE_LOG(LogTemp, Warning, TEXT("========================================================"));
	UE_LOG(LogTemp, Warning, TEXT("   AutoBuildDungeon — Data-Driven Voxel Cave (CellFlow)"));
	UE_LOG(LogTemp, Warning, TEXT("========================================================"));
	UE_LOG(LogTemp, Warning, TEXT(""));

	if (!FSlateApplication::IsInitialized())
	{
		FSlateApplication::Create();
	}
	IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AR.SearchAllAssets(true);

	// ── Parse arguments ──
	int32 Seed = 42;
	FString SeedArg;
	if (FParse::Value(*Params, TEXT("-seed="), SeedArg))
	{
		Seed = FCString::Atoi(*SeedArg);
	}

	FString BiomeName = TEXT("stone");
	FParse::Value(*Params, TEXT("-Biome="), BiomeName);

	FDungeonBiomeConfig Cfg = GetBiomePreset(BiomeName);

	// Allow -OutMap= override
	FString OutMapOverride;
	if (FParse::Value(*Params, TEXT("-OutMap="), OutMapOverride))
	{
		Cfg.MapPath = OutMapOverride;
	}

	UE_LOG(LogTemp, Warning, TEXT("  Biome:  %s"), *Cfg.BiomeName);
	UE_LOG(LogTemp, Warning, TEXT("  Seed:   %d"), Seed);
	UE_LOG(LogTemp, Warning, TEXT("  Canvas: %dx%d (%d pts)"), Cfg.WorldSize.X, Cfg.WorldSize.Y, Cfg.NumPoints);
	UE_LOG(LogTemp, Warning, TEXT("  Spokes: %d-%d nodes"), Cfg.SpokeMinSize, Cfg.SpokeMaxSize);
	UE_LOG(LogTemp, Warning, TEXT("  Output: %s"), *Cfg.MapPath);
	UE_LOG(LogTemp, Warning, TEXT(""));

	// ════════════════════════════════════════════════════
	// STEP 1: CellFlow Asset
	// ════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- STEP 1: CellFlow Asset ---"));

	// Reset cache so each biome gets its own flow asset
	CachedCellFlowAsset = nullptr;
	UCellFlowAsset* FlowAsset = CreateCellFlowAsset(Cfg);

	UE_LOG(LogTemp, Warning, TEXT("  CellFlow: %s"), FlowAsset ? TEXT("OK") : TEXT("FAILED"));

	if (!FlowAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("CellFlow asset generation failed. Aborting."));
		return 1;
	}

	// ════════════════════════════════════════════════════
	// STEP 2: Theme
	// ════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- STEP 2: Theme ---"));

	UDungeonThemeAsset* Theme = GenerateUnifiedTheme(Cfg);

	UE_LOG(LogTemp, Warning, TEXT("  Theme: %s"), Theme ? TEXT("OK") : TEXT("FAILED"));

	// ════════════════════════════════════════════════════
	// STEP 3: Generate Level
	// ════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- STEP 3: Level ---"));

	bool bTestOk = GenerateTestLevel(FlowAsset, Theme, Seed, Cfg);

	UE_LOG(LogTemp, Warning, TEXT("  Level: %s"), bTestOk ? TEXT("OK") : TEXT("FAILED"));

	// ════════════════════════════════════════════════════
	// Summary
	// ════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT(""));
	UE_LOG(LogTemp, Warning, TEXT("========================================================"));
	UE_LOG(LogTemp, Warning, TEXT("  %s — Generation Complete"), *Cfg.BiomeName);
	UE_LOG(LogTemp, Warning, TEXT("  CellFlow: %s"), *Cfg.FlowAssetPath);
	UE_LOG(LogTemp, Warning, TEXT("  Theme:    %s"), *Cfg.ThemeAssetPath);
	UE_LOG(LogTemp, Warning, TEXT("  Map:      %s"), *Cfg.MapPath);
	UE_LOG(LogTemp, Warning, TEXT("  Seed:     %d"), Seed);
	UE_LOG(LogTemp, Warning, TEXT("========================================================"));

	return bTestOk ? 0 : 1;
#else
	return 0;
#endif
}

#if WITH_EDITOR

// ═══════════════════════════════════════════════════════════════════════════
// Asset Save/Delete Helpers
// ═══════════════════════════════════════════════════════════════════════════

bool UAutoBuildDungeonCommandlet::SaveAsset(UObject* Asset, const FString& PackagePath)
{
	if (!Asset) return false;

	UPackage* Pkg = Asset->GetOutermost();
	Pkg->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(Asset);

	FString FileName = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());

	FSavePackageArgs Args;
	Args.TopLevelFlags = RF_Standalone;
	return UPackage::SavePackage(Pkg, Asset, *FileName, Args);
}

bool UAutoBuildDungeonCommandlet::SaveLevel(UWorld* World, const FString& PackagePath)
{
	if (!World) return false;

	UPackage* Pkg = World->GetOutermost();
	Pkg->MarkPackageDirty();

	FString FileName = FPackageName::LongPackageNameToFilename(PackagePath, TEXT(".umap"));

	FSavePackageArgs Args;
	Args.TopLevelFlags = RF_Standalone;
	return UPackage::SavePackage(Pkg, World, *FileName, Args);
}

void UAutoBuildDungeonCommandlet::DeleteExistingAsset(const FString& PackagePath, bool bIsMap)
{
	FString Ext = bIsMap ? TEXT(".umap") : FPackageName::GetAssetPackageExtension();
	FString FilePath = FPackageName::LongPackageNameToFilename(PackagePath, Ext);
	if (FPaths::FileExists(FilePath))
	{
		IFileManager::Get().Delete(*FilePath);
		UE_LOG(LogTemp, Warning, TEXT("  Deleted existing: %s"), *FilePath);
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// STEP 1: CellFlow Asset — Multi-Path Voronoi Layout
// ═══════════════════════════════════════════════════════════════════════════

UCellFlowAsset* UAutoBuildDungeonCommandlet::CreateCellFlowAsset(const FDungeonBiomeConfig& Cfg)
{
	if (CachedCellFlowAsset) return CachedCellFlowAsset;

	DeleteExistingAsset(Cfg.FlowAssetPath);

	UPackage* FlowPkg = CreatePackage(*Cfg.FlowAssetPath);
	FlowPkg->FullyLoad();

	FString AssetName = FPaths::GetBaseFilename(Cfg.FlowAssetPath);
	UCellFlowAsset* FlowAsset = NewObject<UCellFlowAsset>(FlowPkg, *AssetName, RF_Public | RF_Standalone);
	UFlowExecScript* ExecScript = NewObject<UFlowExecScript>(FlowAsset);
	UFlowExecScriptGraph* ScriptGraph = NewObject<UFlowExecScriptGraph>(ExecScript);
	ExecScript->ScriptGraph = ScriptGraph;
	FlowAsset->ExecScript = ExecScript;

	// ── Task 1: Voronoi cell generation ──
	UCellFlowLayoutTaskCreateCellsVoronoi* TaskVoronoi = NewObject<UCellFlowLayoutTaskCreateCellsVoronoi>(FlowAsset);
	TaskVoronoi->WorldSize = Cfg.WorldSize;
	TaskVoronoi->NumPoints = Cfg.NumPoints;
	TaskVoronoi->NumRelaxIterations = Cfg.NumRelaxIterations;
	TaskVoronoi->bDisableBoundaryCells = true;
	TaskVoronoi->EdgeConnectionThreshold = Cfg.EdgeConnectionThreshold;
	TaskVoronoi->MinGroupArea = Cfg.MinGroupArea;
	TaskVoronoi->ClusterBlobbiness = Cfg.ClusterBlobbiness;

	// ── Task 2: Entrance tunnel ──
	UCellFlowLayoutTaskCreateMainPath* TaskMain = NewObject<UCellFlowLayoutTaskCreateMainPath>(FlowAsset);
	TaskMain->PathSize = Cfg.EntrancePathSize;
	TaskMain->PathName = TEXT("main");
	TaskMain->StartMarkerName = TEXT("SpawnPoint");
	TaskMain->GoalMarkerName = TEXT("BossGate");
	TaskMain->StartNodePathName = TEXT("main_start");
	TaskMain->GoalNodePathName = TEXT("main_goal");
	TaskMain->CeilingHeightMin = Cfg.EntranceCeilingMin;
	TaskMain->CeilingHeightMax = Cfg.EntranceCeilingMax;

	// ── Task 3: Central hub ──
	UCellFlowLayoutTaskCreatePath* TaskHub = NewObject<UCellFlowLayoutTaskCreatePath>(FlowAsset);
	TaskHub->MinPathSize = Cfg.HubMinSize;
	TaskHub->MaxPathSize = Cfg.HubMaxSize;
	TaskHub->PathName = TEXT("hub");
	TaskHub->StartFromPath = TEXT("main");
	TaskHub->StartNodePathNameOverride = TEXT("main_goal");
	TaskHub->CeilingHeightMin = Cfg.HubCeilingMin;
	TaskHub->CeilingHeightMax = Cfg.HubCeilingMax;

	// ── Tasks 4-8: Spoke paths ──
	UCellFlowLayoutTaskCreatePath* TaskCorridor = NewObject<UCellFlowLayoutTaskCreatePath>(FlowAsset);
	TaskCorridor->MinPathSize = Cfg.SpokeMinSize;
	TaskCorridor->MaxPathSize = Cfg.SpokeMaxSize;
	TaskCorridor->PathName = TEXT("corridor");
	TaskCorridor->StartFromPath = TEXT("hub");
	TaskCorridor->CeilingHeightMin = 1;
	TaskCorridor->CeilingHeightMax = 2;

	UCellFlowLayoutTaskCreatePath* TaskBoss = NewObject<UCellFlowLayoutTaskCreatePath>(FlowAsset);
	TaskBoss->MinPathSize = Cfg.SpokeMinSize;
	TaskBoss->MaxPathSize = Cfg.SpokeMaxSize;
	TaskBoss->PathName = TEXT("boss");
	TaskBoss->StartFromPath = TEXT("hub");
	TaskBoss->EndNodePathNameOverride = TEXT("boss_arena");
	TaskBoss->CeilingHeightMin = 1;
	TaskBoss->CeilingHeightMax = 2;

	UCellFlowLayoutTaskCreatePath* TaskTreasure = NewObject<UCellFlowLayoutTaskCreatePath>(FlowAsset);
	TaskTreasure->MinPathSize = Cfg.SpokeMinSize;
	TaskTreasure->MaxPathSize = Cfg.SpokeMaxSize;
	TaskTreasure->PathName = TEXT("treasure");
	TaskTreasure->StartFromPath = TEXT("hub");
	TaskTreasure->EndNodePathNameOverride = TEXT("treasure_room");
	TaskTreasure->CeilingHeightMin = 1;
	TaskTreasure->CeilingHeightMax = 2;

	UCellFlowLayoutTaskCreatePath* TaskTrap = NewObject<UCellFlowLayoutTaskCreatePath>(FlowAsset);
	TaskTrap->MinPathSize = Cfg.SpokeMinSize;
	TaskTrap->MaxPathSize = Cfg.SpokeMaxSize;
	TaskTrap->PathName = TEXT("trap");
	TaskTrap->StartFromPath = TEXT("hub");
	TaskTrap->EndNodePathNameOverride = TEXT("trap_room");
	TaskTrap->CeilingHeightMin = 1;
	TaskTrap->CeilingHeightMax = 2;

	UCellFlowLayoutTaskCreatePath* TaskMiniBoss = NewObject<UCellFlowLayoutTaskCreatePath>(FlowAsset);
	TaskMiniBoss->MinPathSize = Cfg.SpokeMinSize;
	TaskMiniBoss->MaxPathSize = Cfg.SpokeMaxSize;
	TaskMiniBoss->PathName = TEXT("miniboss");
	TaskMiniBoss->StartFromPath = TEXT("hub");
	TaskMiniBoss->EndNodePathNameOverride = TEXT("miniboss_arena");
	TaskMiniBoss->CeilingHeightMin = 1;
	TaskMiniBoss->CeilingHeightMax = 2;

	// ── Task 9: Finalize ──
	UCellFlowLayoutTaskFinalize* TaskFinalize = NewObject<UCellFlowLayoutTaskFinalize>(FlowAsset);
	{
		auto SetIntProp = [](UObject* Obj, FName PropName, int32 Value) {
			if (FIntProperty* Prop = CastField<FIntProperty>(Obj->GetClass()->FindPropertyByName(PropName)))
			{
				Prop->SetPropertyValue_InContainer(Obj, Value);
			}
		};
		SetIntProp(TaskFinalize, TEXT("MinHeight"), 0);
		SetIntProp(TaskFinalize, TEXT("MaxHeight"), 0);

		if (FBoolProperty* SepProp = CastField<FBoolProperty>(
			UCellFlowLayoutTaskFinalize::StaticClass()->FindPropertyByName(TEXT("bSeparateAdjacentRooms"))))
		{
			SepProp->SetPropertyValue_InContainer(TaskFinalize, true);
		}
	}

	// ── Wire task graph ──
	UFlowExecScriptTaskNode* NodeVoronoi = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeVoronoi->NodeId = FGuid::NewGuid();
	NodeVoronoi->Task = TaskVoronoi;

	UFlowExecScriptTaskNode* NodeMain = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeMain->NodeId = FGuid::NewGuid();
	NodeMain->Task = TaskMain;

	UFlowExecScriptTaskNode* NodeHub = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeHub->NodeId = FGuid::NewGuid();
	NodeHub->Task = TaskHub;

	UFlowExecScriptTaskNode* NodeCorridor = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeCorridor->NodeId = FGuid::NewGuid();
	NodeCorridor->Task = TaskCorridor;

	UFlowExecScriptTaskNode* NodeBoss = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeBoss->NodeId = FGuid::NewGuid();
	NodeBoss->Task = TaskBoss;

	UFlowExecScriptTaskNode* NodeTreasure = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeTreasure->NodeId = FGuid::NewGuid();
	NodeTreasure->Task = TaskTreasure;

	UFlowExecScriptTaskNode* NodeTrap = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeTrap->NodeId = FGuid::NewGuid();
	NodeTrap->Task = TaskTrap;

	UFlowExecScriptTaskNode* NodeMiniBoss = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeMiniBoss->NodeId = FGuid::NewGuid();
	NodeMiniBoss->Task = TaskMiniBoss;

	UFlowExecScriptTaskNode* NodeFinalize = NewObject<UFlowExecScriptTaskNode>(ScriptGraph);
	NodeFinalize->NodeId = FGuid::NewGuid();
	NodeFinalize->Task = TaskFinalize;

	UFlowExecScriptResultNode* NodeResult = NewObject<UFlowExecScriptResultNode>(ScriptGraph);
	NodeResult->NodeId = FGuid::NewGuid();
	ExecScript->ResultNode = NodeResult;

	// Chain: Voronoi → Main → Hub → Corridor → Boss → Treasure → Trap → MiniBoss → Finalize → Result
	NodeVoronoi->OutgoingNodes.Add(NodeMain);
	NodeMain->IncomingNodes.Add(NodeVoronoi);

	NodeMain->OutgoingNodes.Add(NodeHub);
	NodeHub->IncomingNodes.Add(NodeMain);

	NodeHub->OutgoingNodes.Add(NodeCorridor);
	NodeCorridor->IncomingNodes.Add(NodeHub);

	NodeCorridor->OutgoingNodes.Add(NodeBoss);
	NodeBoss->IncomingNodes.Add(NodeCorridor);

	NodeBoss->OutgoingNodes.Add(NodeTreasure);
	NodeTreasure->IncomingNodes.Add(NodeBoss);

	NodeTreasure->OutgoingNodes.Add(NodeTrap);
	NodeTrap->IncomingNodes.Add(NodeTreasure);

	NodeTrap->OutgoingNodes.Add(NodeMiniBoss);
	NodeMiniBoss->IncomingNodes.Add(NodeTrap);

	NodeMiniBoss->OutgoingNodes.Add(NodeFinalize);
	NodeFinalize->IncomingNodes.Add(NodeMiniBoss);

	NodeFinalize->OutgoingNodes.Add(NodeResult);
	NodeResult->IncomingNodes.Add(NodeFinalize);

	ScriptGraph->Nodes.Add(NodeVoronoi);
	ScriptGraph->Nodes.Add(NodeMain);
	ScriptGraph->Nodes.Add(NodeHub);
	ScriptGraph->Nodes.Add(NodeCorridor);
	ScriptGraph->Nodes.Add(NodeBoss);
	ScriptGraph->Nodes.Add(NodeTreasure);
	ScriptGraph->Nodes.Add(NodeTrap);
	ScriptGraph->Nodes.Add(NodeMiniBoss);
	ScriptGraph->Nodes.Add(NodeFinalize);
	ScriptGraph->Nodes.Add(NodeResult);

	UE_LOG(LogTemp, Warning, TEXT("  Tasks: Voronoi(%dx%d,%dpts) -> Main(%d) -> Hub(%d-%d) -> [5 spokes](%d-%d) -> Finalize"),
		Cfg.WorldSize.X, Cfg.WorldSize.Y, Cfg.NumPoints,
		Cfg.EntrancePathSize, Cfg.HubMinSize, Cfg.HubMaxSize,
		Cfg.SpokeMinSize, Cfg.SpokeMaxSize);

	bool bSaved = SaveAsset(FlowAsset, Cfg.FlowAssetPath);
	UE_LOG(LogTemp, Warning, TEXT("  CellFlow asset: %s (%s)"), *Cfg.FlowAssetPath, bSaved ? TEXT("saved") : TEXT("FAILED"));

	CachedCellFlowAsset = FlowAsset;
	return FlowAsset;
}

// ═══════════════════════════════════════════════════════════════════════════
// STEP 2: Unified Theme
// ═══════════════════════════════════════════════════════════════════════════

UDungeonThemeAsset* UAutoBuildDungeonCommandlet::GenerateUnifiedTheme(const FDungeonBiomeConfig& Cfg)
{
	DeleteExistingAsset(Cfg.ThemeAssetPath);

	UPackage* Pkg = CreatePackage(*Cfg.ThemeAssetPath);
	Pkg->FullyLoad();

	FString AssetName = FPaths::GetBaseFilename(Cfg.ThemeAssetPath);
	UDungeonThemeAsset* Theme = NewObject<UDungeonThemeAsset>(Pkg, *AssetName, RF_Public | RF_Standalone);
	Theme->Version = static_cast<int32>(EDungeonThemeAssetVersion::V2_MovedToCompiledData);

	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));

	auto LoadRock = [](const TCHAR* Path) -> UStaticMesh*
	{
		FString PathStr(Path);
		FString FullPath = PathStr + TEXT(".") + FPaths::GetBaseFilename(PathStr);
		return LoadObject<UStaticMesh>(nullptr, *FullPath);
	};

	UStaticMesh* Rock01 = LoadRock(TEXT("/Game/Wasteland/Meshes/SM_Rock_01"));
	UStaticMesh* Rock03 = LoadRock(TEXT("/Game/Wasteland/Meshes/SM_Rock_03"));
	UStaticMesh* Rock05 = LoadRock(TEXT("/Game/Wasteland/Meshes/SM_Rock_05"));
	UStaticMesh* Rock07 = LoadRock(TEXT("/Game/Wasteland/Meshes/SM_Rock_07"));

	FDungeonThemeCompiledGraph& Graph = Theme->CompiledThemeGraph;

	auto WrapMesh = [&Theme](UStaticMesh* InMesh) -> UDungeonMesh*
	{
		if (!InMesh) return nullptr;
		UDungeonMesh* DM = NewObject<UDungeonMesh>(Theme);
		DM->StaticMesh = InMesh;
		DM->bCanEverAffectNavigation = false;
		DM->CalculateHashCode();
		return DM;
	};

	auto AddMarkerVisual = [&](const FString& MarkerName, UStaticMesh* Mesh,
		const FVector& Scale, const FRotator& Rot = FRotator::ZeroRotator, float Probability = 1.0f) -> FGuid
	{
		FDungeonThemeMarkerNodeData Marker;
		Marker.NodeGuid = FGuid::NewGuid();
		Marker.MarkerName = MarkerName;
		Graph.MarkerNodes.Add(Marker);

		UStaticMesh* MeshToUse = Mesh ? Mesh : CubeMesh;
		UDungeonMesh* WrappedMesh = WrapMesh(MeshToUse);
		if (WrappedMesh)
		{
			FDungeonThemeVisualNodeData Visual;
			Visual.NodeGuid = FGuid::NewGuid();
			Visual.ParentNodes.Add(Marker.NodeGuid);
			Visual.AssetObject = WrappedMesh;
			Visual.Probability = Probability;
			Visual.ExecutionOrder = 0.0f;
			Visual.ConsumeOnAttach = false;
			Visual.Offset = FTransform(FQuat(Rot), FVector::ZeroVector, Scale);
			Graph.VisualNodes.Add(Visual);
		}

		return Marker.NodeGuid;
	};

	// Ground
	FGuid GroundMarkerGuid = AddMarkerVisual(TEXT("Ground"), Rock01, FVector(2.0f, 2.0f, 0.3f));

	// Wall
	FGuid WallMarkerGuid = AddMarkerVisual(TEXT("Wall"), Rock05, FVector(1.5f, 0.5f, 2.0f));

	// Wall → Torch light (biome-colored)
	{
		UPointLightComponent* LightTemplate = NewObject<UPointLightComponent>(Theme, TEXT("TorchLightTemplate"));
		LightTemplate->SetMobility(EComponentMobility::Movable);
		LightTemplate->SetIntensity(Cfg.TorchIntensity);
		LightTemplate->SetLightColor(Cfg.TorchColor);
		LightTemplate->SetAttenuationRadius(2500.0f);
		LightTemplate->SetCastShadows(false);

		FDungeonThemeVisualNodeData LightNode;
		LightNode.NodeGuid = FGuid::NewGuid();
		LightNode.ParentNodes.Add(WallMarkerGuid);
		LightNode.AssetObject = LightTemplate;
		LightNode.Probability = 0.5f;
		LightNode.Offset = FTransform(FRotator::ZeroRotator, FVector(0, 50, 250));
		LightNode.ConsumeOnAttach = false;
		LightNode.ExecutionOrder = 1.0f;
		Graph.VisualNodes.Add(LightNode);
	}

	// Ground → Ambient fill light (biome-colored)
	{
		UPointLightComponent* AmbientTemplate = NewObject<UPointLightComponent>(Theme, TEXT("AmbientLightTemplate"));
		AmbientTemplate->SetMobility(EComponentMobility::Movable);
		AmbientTemplate->SetIntensity(Cfg.AmbientIntensity);
		AmbientTemplate->SetLightColor(Cfg.AmbientColor);
		AmbientTemplate->SetAttenuationRadius(2000.0f);
		AmbientTemplate->SetCastShadows(false);

		FDungeonThemeVisualNodeData AmbientNode;
		AmbientNode.NodeGuid = FGuid::NewGuid();
		AmbientNode.ParentNodes.Add(GroundMarkerGuid);
		AmbientNode.AssetObject = AmbientTemplate;
		AmbientNode.Probability = 0.35f;
		AmbientNode.Offset = FTransform(FRotator::ZeroRotator, FVector(0, 0, 150));
		AmbientNode.ConsumeOnAttach = false;
		AmbientNode.ExecutionOrder = 1.0f;
		Graph.VisualNodes.Add(AmbientNode);
	}

	// Fence
	AddMarkerVisual(TEXT("Fence"), Rock07, FVector(1.0f, 0.5f, 1.5f));

	// Door
	AddMarkerVisual(TEXT("Door"), CubeMesh, FVector(2.0f, 0.1f, 2.5f));

	// Stair
	UStaticMesh* RockSlab = LoadRock(TEXT("/Game/Wasteland/Meshes/SM_Rock_10"));
	if (!RockSlab) RockSlab = Rock01;
	AddMarkerVisual(TEXT("Stair"), RockSlab, FVector(3.0f, 3.0f, 0.25f), FRotator(10, 0, 0));

	// SpawnPoint marker (no visual)
	{
		FDungeonThemeMarkerNodeData SpawnMarker;
		SpawnMarker.NodeGuid = FGuid::NewGuid();
		SpawnMarker.MarkerName = TEXT("SpawnPoint");
		Graph.MarkerNodes.Add(SpawnMarker);
	}

	bool bSaved = SaveAsset(Theme, Cfg.ThemeAssetPath);
	UE_LOG(LogTemp, Warning, TEXT("  Theme: %d markers, %d visuals (%s)"),
		Graph.MarkerNodes.Num(), Graph.VisualNodes.Num(), bSaved ? TEXT("saved") : TEXT("FAILED"));

	return bSaved ? Theme : nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// Biome Material — Import PBR textures from disk, create UMaterial
// ═══════════════════════════════════════════════════════════════════════════

UMaterialInterface* UAutoBuildDungeonCommandlet::CreateBiomeMaterial(const FDungeonBiomeConfig& Cfg)
{
	if (Cfg.BiomeTextureDiskPath.IsEmpty() || Cfg.BiomeMaterialPath.IsEmpty())
	{
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("  Creating biome material from: %s"), *Cfg.BiomeTextureDiskPath);

	// ── Find texture files (support both JPG and PNG, Quixel and Slate naming) ──
	TArray<FString> ImageFiles;
	IFileManager::Get().FindFiles(ImageFiles, *Cfg.BiomeTextureDiskPath, TEXT("*.jpg"));
	TArray<FString> PNGFiles;
	IFileManager::Get().FindFiles(PNGFiles, *Cfg.BiomeTextureDiskPath, TEXT("*.png"));
	ImageFiles.Append(PNGFiles);

	if (ImageFiles.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("  No image files found in %s"), *Cfg.BiomeTextureDiskPath);
		return nullptr;
	}

	FString BaseColorFile, NormalFile, RoughnessFile, MetallicFile;
	for (const FString& File : ImageFiles)
	{
		FString Lower = File.ToLower();
		if (Lower.Contains(TEXT("basecolor")) || Lower.Contains(TEXT("base_color")))
			BaseColorFile = FPaths::Combine(Cfg.BiomeTextureDiskPath, File);
		else if (Lower.Contains(TEXT("normal")))
			NormalFile = FPaths::Combine(Cfg.BiomeTextureDiskPath, File);
		else if (Lower.Contains(TEXT("roughness")))
			RoughnessFile = FPaths::Combine(Cfg.BiomeTextureDiskPath, File);
		else if (Lower.Contains(TEXT("metallic")))
			MetallicFile = FPaths::Combine(Cfg.BiomeTextureDiskPath, File);
	}

	if (BaseColorFile.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("  No BaseColor texture found in %s"), *Cfg.BiomeTextureDiskPath);
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("  BaseColor: %s"), *FPaths::GetCleanFilename(BaseColorFile));
	UE_LOG(LogTemp, Warning, TEXT("  Normal:    %s"), NormalFile.IsEmpty() ? TEXT("(none)") : *FPaths::GetCleanFilename(NormalFile));
	UE_LOG(LogTemp, Warning, TEXT("  Roughness: %s"), RoughnessFile.IsEmpty() ? TEXT("(none)") : *FPaths::GetCleanFilename(RoughnessFile));
	UE_LOG(LogTemp, Warning, TEXT("  Metallic:  %s"), MetallicFile.IsEmpty() ? TEXT("(none)") : *FPaths::GetCleanFilename(MetallicFile));

	// ── Texture destination path (same folder as material) ──
	FString TexDestPath = FPaths::GetPath(Cfg.BiomeMaterialPath);
	FString MaterialName = FPaths::GetBaseFilename(Cfg.BiomeMaterialPath);

	// ── Import helper — sRGB for all (Bug #33 workaround) ──
	auto ImportTexture = [&](const FString& FilePath, const FString& AssetName) -> UTexture2D*
	{
		if (FilePath.IsEmpty()) return nullptr;

		FString PackagePath = TexDestPath / AssetName;

		// Delete existing asset first (Bug #21)
		FString ExistingFile = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
		if (FPaths::FileExists(ExistingFile))
		{
			IFileManager::Get().Delete(*ExistingFile);
		}

		UPackage* Package = CreatePackage(*PackagePath);
		if (!Package) return nullptr;

		TArray<uint8> FileData;
		if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
		{
			UE_LOG(LogTemp, Error, TEXT("  Failed to read: %s"), *FilePath);
			return nullptr;
		}

		UTextureFactory* Factory = NewObject<UTextureFactory>();
		Factory->SuppressImportOverwriteDialog();

		// Determine file extension
		FString Ext = FPaths::GetExtension(FilePath).ToLower();

		const uint8* DataPtr = FileData.GetData();
		UObject* ImportedObj = Factory->FactoryCreateBinary(
			UTexture2D::StaticClass(),
			Package,
			FName(*AssetName),
			RF_Public | RF_Standalone,
			nullptr,
			*Ext,
			DataPtr,
			DataPtr + FileData.Num(),
			GWarn);

		UTexture2D* Texture = Cast<UTexture2D>(ImportedObj);
		if (Texture)
		{
			// Force platform texture data generation (DDC)
			Texture->UpdateResource();
			FAssetCompilingManager::Get().FinishAllCompilation();

			// Wait for texture streaming to complete
			Texture->WaitForStreaming();

			Package->MarkPackageDirty();

			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
			UPackage::SavePackage(Package, Texture,
				*FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension()), SaveArgs);

			UE_LOG(LogTemp, Warning, TEXT("  Imported: %s (%dx%d)"), *AssetName, Texture->GetSizeX(), Texture->GetSizeY());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("  Failed to import: %s"), *FilePath);
		}
		return Texture;
	};

	// ── Import textures ──
	FString Prefix = FString::Printf(TEXT("T_%s"), *MaterialName.Replace(TEXT("M_"), TEXT("")));
	UTexture2D* T_BaseColor = ImportTexture(BaseColorFile, Prefix + TEXT("_BaseColor"));
	UTexture2D* T_Normal = ImportTexture(NormalFile, Prefix + TEXT("_Normal"));
	UTexture2D* T_Roughness = ImportTexture(RoughnessFile, Prefix + TEXT("_Roughness"));
	UTexture2D* T_Metallic = ImportTexture(MetallicFile, Prefix + TEXT("_Metallic"));

	if (!T_BaseColor)
	{
		UE_LOG(LogTemp, Error, TEXT("  Base color import failed — aborting material creation"));
		return nullptr;
	}

	// ── Create MaterialInstanceConstant (inherits compiled shaders from parent) ──
	// UMaterial requires RHI for shader compilation, which commandlets don't have.
	// MIC inherits shaders from parent → works immediately without compilation.
	FString MatPackagePath = Cfg.BiomeMaterialPath;
	FString ExistingMatFile = FPackageName::LongPackageNameToFilename(MatPackagePath, FPackageName::GetAssetPackageExtension());
	if (FPaths::FileExists(ExistingMatFile))
	{
		IFileManager::Get().Delete(*ExistingMatFile);
	}

	// Load MI_Rock_03's parent material, or use a known base material
	UMaterialInterface* ParentMat = LoadObject<UMaterialInterface>(nullptr, *Cfg.VoxelMaterialPath);
	if (!ParentMat)
	{
		ParentMat = LoadObject<UMaterialInterface>(nullptr, *Cfg.FallbackMaterialPath);
	}
	if (!ParentMat)
	{
		UE_LOG(LogTemp, Error, TEXT("  No parent material found for MIC"));
		return nullptr;
	}

	// Find the root UMaterial so we know what texture parameters exist
	UMaterial* RootMat = ParentMat->GetMaterial();
	if (!RootMat)
	{
		UE_LOG(LogTemp, Error, TEXT("  Could not get root UMaterial from %s"), *ParentMat->GetPathName());
		return nullptr;
	}

	// Log available texture parameters
	TArray<FMaterialParameterInfo> TexParamInfos;
	TArray<FGuid> TexParamGuids;
	ParentMat->GetAllTextureParameterInfo(TexParamInfos, TexParamGuids);
	UE_LOG(LogTemp, Warning, TEXT("  Parent material: %s (%d texture params)"),
		*ParentMat->GetPathName(), TexParamInfos.Num());
	for (const FMaterialParameterInfo& Info : TexParamInfos)
	{
		UE_LOG(LogTemp, Warning, TEXT("    Texture param: '%s'"), *Info.Name.ToString());
	}

	// Also log scalar params
	TArray<FMaterialParameterInfo> ScalarParamInfos;
	TArray<FGuid> ScalarParamGuids;
	ParentMat->GetAllScalarParameterInfo(ScalarParamInfos, ScalarParamGuids);
	for (const FMaterialParameterInfo& Info : ScalarParamInfos)
	{
		UE_LOG(LogTemp, Warning, TEXT("    Scalar param: '%s'"), *Info.Name.ToString());
	}

	UPackage* MatPackage = CreatePackage(*MatPackagePath);
	UMaterialInstanceConstant* MIC = NewObject<UMaterialInstanceConstant>(
		MatPackage, FName(*MaterialName), RF_Public | RF_Standalone);
	MIC->SetParentEditorOnly(ParentMat);

	// Set texture parameter overrides (try common parameter names)
	// MI_Rock_03 params: "BC" (BaseColor), "N" (Normal), "ORM" (packed), "2-BC", "Opacity - A Channel"
	// Match by exact name first, then by Contains
	auto TrySetTexture = [&](UTexture2D* Tex, const TArray<FString>& ExactNames, const TArray<FString>& ContainsNames)
	{
		if (!Tex) return;
		// Exact match
		for (const FString& Name : ExactNames)
		{
			for (const FMaterialParameterInfo& Info : TexParamInfos)
			{
				if (Info.Name.ToString().Equals(Name, ESearchCase::IgnoreCase))
				{
					MIC->SetTextureParameterValueEditorOnly(Info.Name, Tex);
					UE_LOG(LogTemp, Warning, TEXT("  Set texture param '%s' = %s"),
						*Info.Name.ToString(), *Tex->GetName());
					return;
				}
			}
		}
		// Contains match
		for (const FString& Name : ContainsNames)
		{
			for (const FMaterialParameterInfo& Info : TexParamInfos)
			{
				if (Info.Name.ToString().Contains(Name, ESearchCase::IgnoreCase))
				{
					MIC->SetTextureParameterValueEditorOnly(Info.Name, Tex);
					UE_LOG(LogTemp, Warning, TEXT("  Set texture param '%s' = %s (contains '%s')"),
						*Info.Name.ToString(), *Tex->GetName(), *Name);
					return;
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("  No matching param for texture: %s"), *Tex->GetName());
	};

	TrySetTexture(T_BaseColor, {TEXT("BC")}, {TEXT("BaseColor"), TEXT("Diffuse"), TEXT("Albedo")});
	TrySetTexture(T_Normal, {TEXT("N")}, {TEXT("Normal")});
	TrySetTexture(T_Roughness, {TEXT("ORM")}, {TEXT("Roughness"), TEXT("Rough")});
	TrySetTexture(T_Metallic, {}, {TEXT("Metallic"), TEXT("Metal")});

	MIC->PostEditChange();
	MatPackage->MarkPackageDirty();

	{
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		UPackage::SavePackage(MatPackage, MIC,
			*FPackageName::LongPackageNameToFilename(MatPackagePath, FPackageName::GetAssetPackageExtension()), SaveArgs);
	}

	UE_LOG(LogTemp, Warning, TEXT("  Biome MIC created: %s (parent=%s)"),
		*MaterialName, *ParentMat->GetName());
	return MIC;
}

// ═══════════════════════════════════════════════════════════════════════════
// STEP 3: Test Level — Single ADungeon + Boss Volume
// ═══════════════════════════════════════════════════════════════════════════

bool UAutoBuildDungeonCommandlet::GenerateTestLevel(UCellFlowAsset* FlowAsset,
	UDungeonThemeAsset* Theme, int32 Seed, const FDungeonBiomeConfig& Cfg)
{
	DeleteExistingAsset(Cfg.MapPath, true);

	UPackage* Pkg = CreatePackage(*Cfg.MapPath);
	Pkg->FullyLoad();

	FString LevelName = FPaths::GetBaseFilename(Cfg.MapPath);
	UWorld* World = UWorld::CreateWorld(EWorldType::None, false, FName(*LevelName), Pkg);
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to create world!"));
		return false;
	}
	World->SetFlags(RF_Public | RF_Standalone);

	FWorldContext& WC = GEngine->CreateNewWorldContext(EWorldType::Editor);
	WC.SetCurrentWorld(World);

	// ── 3a. Spawn AProceduralCaveManager ──
	AProceduralCaveManager* Manager = World->SpawnActor<AProceduralCaveManager>(
		FVector::ZeroVector, FRotator::ZeroRotator);
	if (Manager)
	{
		Manager->SetFolderPath(FName(TEXT("DungeonSystem")));
		Manager->Seed = Seed;
		Manager->bAutoBuildOnPlay = true;

		if (FlowAsset)
		{
			Manager->CellFlowAsset = TSoftObjectPtr<UCellFlowAsset>(
				FSoftObjectPath(FlowAsset->GetPathName()));
		}
		UDungeonThemeAsset* BundledTheme = LoadObject<UDungeonThemeAsset>(nullptr,
			TEXT("/DungeonArchitect/Showcase/Samples/Themes/Foundry/T_Theme_Founddry.T_Theme_Founddry"));
		if (!BundledTheme)
		{
			BundledTheme = LoadObject<UDungeonThemeAsset>(nullptr,
				TEXT("/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleSciFi/DungeonTheme/D_StarterPackTheme.D_StarterPackTheme"));
		}
		if (BundledTheme)
		{
			Manager->CaveTheme = BundledTheme;
		}

		UE_LOG(LogTemp, Warning, TEXT("  CaveManager spawned (seed=%d)"), Seed);
	}

	// ── 3b. Build dungeon WITH voxels ──
	ADungeon* Dungeon = World->SpawnActor<ADungeon>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (Dungeon)
	{
		Dungeon->SetFolderPath(FName(TEXT("DungeonSystem/Cave")));
		Dungeon->bDrawDebugData = false;
		Dungeon->BuilderClass = UCellFlowBuilder::StaticClass();
		Dungeon->CreateBuilderInstance();

		UCellFlowConfig* Config = Cast<UCellFlowConfig>(Dungeon->GetConfig());
		if (!Config)
		{
			Config = NewObject<UCellFlowConfig>(Dungeon);
			Dungeon->Config = Config;
		}
		Config->CellFlow = FlowAsset;
		Config->GridSize = Cfg.GridSize;
		Config->Seed = Seed;
		Config->MaxBuildTimePerFrameMs = 0;

		// Voxel config
		Dungeon->bCarveVoxels = true;

		FDAVoxelMeshGenerationSettings& VMS = Dungeon->VoxelMeshSettings;
		VMS.VoxelSize = Cfg.VoxelSize;
		VMS.VoxelChunkSize = 32;
		VMS.bEnableCollision = true;
		VMS.WallThickness = Cfg.WallThickness;
		VMS.bUseGPU = false;
		VMS.UVScale = 150.0f;
		VMS.VoxelShapeTheme.Reset();

		// Biome material (import PBR textures) or fallback rock material
		UMaterialInterface* VoxelMat = nullptr;
		if (!Cfg.BiomeTextureDiskPath.IsEmpty())
		{
			VoxelMat = CreateBiomeMaterial(Cfg);
		}
		if (!VoxelMat)
		{
			VoxelMat = LoadObject<UMaterialInterface>(nullptr, *Cfg.VoxelMaterialPath);
		}
		if (!VoxelMat)
		{
			VoxelMat = LoadObject<UMaterialInterface>(nullptr, *Cfg.FallbackMaterialPath);
		}
		if (VoxelMat)
		{
			VMS.Material = VoxelMat;
			UE_LOG(LogTemp, Warning, TEXT("  VMS.Material set to: %s (class=%s)"),
				*VoxelMat->GetPathName(), *VoxelMat->GetClass()->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("  VMS.Material is NULL — voxels will use default!"));
		}

		// Voxel noise
		FDAVoxelNoiseSettings& VNS = Dungeon->VoxelNoiseSettings;
		VNS.NoiseAmplitude = Cfg.NoiseAmplitude;
		VNS.NoiseOctaves = Cfg.NoiseOctaves;
		VNS.NoiseFloorScale = Cfg.NoiseFloorScale;
		VNS.NoiseCeilingScale = Cfg.NoiseCeilingScale;
		VNS.bEnableDomainWarp = Cfg.bDomainWarp;
		VNS.DomainWarpStrength = Cfg.DomainWarpStrength;
		VNS.NoiseScaleVector = FVector(1, 1, 0.7f);

		// Cave SDF model
		UDungeonVoxelSDFModel_Cave* CaveModel = NewObject<UDungeonVoxelSDFModel_Cave>(Dungeon);
		CaveModel->CeilingExtraHeight = Cfg.CeilingExtraHeight;
		CaveModel->bEnableCeilingHoles = true;
		Dungeon->VoxelSDFModel = CaveModel;

		// Theme
		if (Manager && Manager->CaveTheme)
		{
			Dungeon->Themes.Add(Manager->CaveTheme);
		}
		else if (Theme)
		{
			Dungeon->Themes.Add(Theme);
		}

		// ── Two-pass build with STARFISH distance enforcement ──
		bool bBuildOk = false;
		int32 CurrentSeed = Seed;
		float BestMinDistance = 0.0f;
		int32 BestSeed = Seed;

		for (int32 Retry = 0; Retry < Cfg.MaxSeedRetries; ++Retry)
		{
			Config->Seed = CurrentSeed;
			UE_LOG(LogTemp, Warning, TEXT("  Pass 1: Building layout (seed=%d, attempt %d/%d)..."),
				CurrentSeed, Retry + 1, Cfg.MaxSeedRetries);
			Dungeon->BuildDungeon();

			UDungeonBuilder* Builder = Dungeon->GetBuilder();
			bBuildOk = Builder && Builder->HasBuildSucceeded();
			UCellFlowModel* CheckModel = bBuildOk ? Cast<UCellFlowModel>(Dungeon->GetModel()) : nullptr;
			if (!bBuildOk || !CheckModel || !CheckModel->LayoutGraph)
			{
				UE_LOG(LogTemp, Warning, TEXT("  seed=%d: BUILD FAILED, skipping"), CurrentSeed);
				CurrentSeed++;
				bBuildOk = false;
				continue;
			}

			// Find hub + all terminal positions
			FVector HubPos = FVector::ZeroVector;
			FVector BossPos_Check = FVector::ZeroVector;
			FVector TreasurePos_Check = FVector::ZeroVector;
			FVector TrapPos_Check = FVector::ZeroVector;
			FVector MiniBossPos_Check = FVector::ZeroVector;
			bool bHub = false, bBoss = false, bTreasure = false, bTrap = false, bMiniBoss = false;

			for (UFlowAbstractNode* Node : CheckModel->LayoutGraph->GraphNodes)
			{
				if (!Node || !Node->bActive) continue;
				FVector WP = Node->Coord * Config->GridSize;

				if (!bHub && Node->PathName == TEXT("hub"))
				{
					HubPos = WP;
					bHub = true;
				}
				if (!bBoss && (Node->PathName == TEXT("boss_arena") ||
					(Node->PathName == TEXT("boss") && Node->PathIndex == Node->PathLength - 1)))
				{
					BossPos_Check = WP;
					bBoss = true;
				}
				if (!bTreasure && (Node->PathName == TEXT("treasure_room") ||
					(Node->PathName == TEXT("treasure") && Node->PathIndex == Node->PathLength - 1)))
				{
					TreasurePos_Check = WP;
					bTreasure = true;
				}
				if (!bTrap && (Node->PathName == TEXT("trap_room") ||
					(Node->PathName == TEXT("trap") && Node->PathIndex == Node->PathLength - 1)))
				{
					TrapPos_Check = WP;
					bTrap = true;
				}
				if (!bMiniBoss && (Node->PathName == TEXT("miniboss_arena") ||
					(Node->PathName == TEXT("miniboss") && Node->PathIndex == Node->PathLength - 1)))
				{
					MiniBossPos_Check = WP;
					bMiniBoss = true;
				}
			}

			if (!bHub || !bBoss || !bTreasure || !bTrap || !bMiniBoss)
			{
				UE_LOG(LogTemp, Warning, TEXT("  seed=%d: MISSING NODES, skipping"), CurrentSeed);
				CurrentSeed++;
				bBuildOk = false;
				continue;
			}

			float DistBoss = FVector::Dist(HubPos, BossPos_Check);
			float DistTreasure = FVector::Dist(HubPos, TreasurePos_Check);
			float DistTrap = FVector::Dist(HubPos, TrapPos_Check);
			float DistMiniBoss = FVector::Dist(HubPos, MiniBossPos_Check);
			float CurrentMinDist = FMath::Min(FMath::Min(DistBoss, DistTreasure), FMath::Min(DistTrap, DistMiniBoss));

			if (CurrentMinDist > BestMinDistance)
			{
				BestMinDistance = CurrentMinDist;
				BestSeed = CurrentSeed;
			}

			bool bBossOk = DistBoss >= Cfg.MinHubBossDistance;
			bool bTreasureOk = DistTreasure >= Cfg.MinHubTerminalDistance;
			bool bTrapOk = DistTrap >= Cfg.MinHubTerminalDistance;
			bool bMiniBossOk = DistMiniBoss >= Cfg.MinHubTerminalDistance;

			UE_LOG(LogTemp, Warning, TEXT("  seed=%d: Boss=%.0f%s Treasure=%.0f%s Trap=%.0f%s MiniBoss=%.0f%s min=%.0f"),
				CurrentSeed,
				DistBoss, bBossOk ? TEXT("OK") : TEXT(""),
				DistTreasure, bTreasureOk ? TEXT("OK") : TEXT(""),
				DistTrap, bTrapOk ? TEXT("OK") : TEXT(""),
				DistMiniBoss, bMiniBossOk ? TEXT("OK") : TEXT(""),
				CurrentMinDist);

			if (bBossOk && bTreasureOk && bTrapOk && bMiniBossOk)
			{
				UE_LOG(LogTemp, Warning, TEXT("  STARFISH ACCEPTED (seed=%d, attempt %d)"), CurrentSeed, Retry + 1);
				break;
			}

			CurrentSeed++;
			bBuildOk = false;
		}

		if (!bBuildOk)
		{
			UE_LOG(LogTemp, Warning, TEXT("  No perfect starfish in %d attempts. Using BEST seed=%d (min spoke=%.0f)"),
				Cfg.MaxSeedRetries, BestSeed, BestMinDistance);
			Config->Seed = BestSeed;
			Dungeon->BuildDungeon();
			bBuildOk = Dungeon->GetBuilder() && Dungeon->GetBuilder()->HasBuildSucceeded();
		}

		// ── Door carving volumes ──
		TArray<ADAVoxelVolume*> DoorCarveVolumes;
		if (bBuildOk)
		{
			UCellFlowModel* Pass1Model = Cast<UCellFlowModel>(Dungeon->GetModel());
			if (Pass1Model && Pass1Model->LayoutGraph)
			{
				TMap<FGuid, UFlowAbstractNode*> NodeMap;
				for (UFlowAbstractNode* Node : Pass1Model->LayoutGraph->GraphNodes)
				{
					if (Node) NodeMap.Add(Node->NodeId, Node);
				}

				for (UFlowAbstractLink* Link : Pass1Model->LayoutGraph->GraphLinks)
				{
					if (!Link) continue;
					UFlowAbstractNode* NodeA = NodeMap.FindRef(Link->Source);
					UFlowAbstractNode* NodeB = NodeMap.FindRef(Link->Destination);
					if (!NodeA || !NodeB || !NodeA->bActive || !NodeB->bActive) continue;
					if (NodeA->PathName.IsEmpty() || NodeB->PathName.IsEmpty()) continue;
					if (NodeA->PathName == NodeB->PathName && NodeA->PathIndex != -1 && NodeB->PathIndex != -1)
					{
						if (FMath::Abs(NodeA->PathIndex - NodeB->PathIndex) <= 1) continue;
					}

					FVector PosA = NodeA->Coord * Config->GridSize;
					FVector PosB = NodeB->Coord * Config->GridSize;
					FVector DoorPos = (PosA + PosB) * 0.5f;

					ADAVoxelVolume* DoorVol = World->SpawnActor<ADAVoxelVolume>(
						DoorPos + FVector(0, 0, 200), FRotator::ZeroRotator);
					if (DoorVol)
					{
						DoorVol->BoxExtent = FVector(300, 200, 250);
						DoorVol->bSubtract = true;
						DoorVol->Priority = 100;
						DoorVol->bOverrideNoiseSettings = true;
						DoorVol->NoiseSettingsOverride.NoiseAmplitude = 30.0f;
						DoorVol->NoiseSettingsOverride.NoiseFloorScale = 0.02f;
						DoorVol->NoiseSettingsOverride.NoiseCeilingScale = 0.5f;
						DoorVol->NoiseSettingsOverride.NoiseOctaves = 2;
						DoorVol->NoiseSettingsOverride.bEnableDomainWarp = false;
						DoorVol->SetFolderPath(FName(TEXT("DungeonSystem/DoorVolumes")));
						DoorCarveVolumes.Add(DoorVol);
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("  Spawned %d door carving volumes"), DoorCarveVolumes.Num());
			}
		}

		// Pass 2: Rebuild with door volumes in world
		if (DoorCarveVolumes.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Pass 2: Rebuilding with %d door carving volumes..."), DoorCarveVolumes.Num());
			Dungeon->BuildDungeon();
			bBuildOk = Dungeon->GetBuilder() && Dungeon->GetBuilder()->HasBuildSucceeded();
			UE_LOG(LogTemp, Warning, TEXT("  Pass 2: %s"), bBuildOk ? TEXT("OK") : TEXT("FAILED"));
		}

		// ── 3c. Query path positions, spawn arenas and beacons ──
		if (bBuildOk)
		{
			UCellFlowModel* Model = Cast<UCellFlowModel>(Dungeon->GetModel());
			if (Model && Model->LayoutGraph)
			{
				FVector BossPos = FVector::ZeroVector;
				FVector EntrancePos = FVector::ZeroVector;
				bool bFoundBoss = false;
				bool bFoundEntrance = false;
				TArray<FVector> AllNodePositions;

				// Helper: stamp a terminal arena volume
				auto StampArena = [&World](const FVector& Pos, FVector Extent, float ZOffset,
					float Amplitude, const FName& Folder, const TCHAR* Label)
				{
					ADAVoxelVolume* Vol = World->SpawnActor<ADAVoxelVolume>(
						Pos + FVector(0, 0, ZOffset), FRotator::ZeroRotator);
					if (Vol)
					{
						Vol->BoxExtent = Extent;
						Vol->bSubtract = true;
						Vol->Priority = 50;
						Vol->bOverrideNoiseSettings = true;
						Vol->NoiseSettingsOverride.NoiseAmplitude = Amplitude;
						Vol->NoiseSettingsOverride.NoiseFloorScale = 0.05f;
						Vol->NoiseSettingsOverride.NoiseCeilingScale = 0.5f;
						Vol->NoiseSettingsOverride.NoiseOctaves = 3;
						Vol->NoiseSettingsOverride.bEnableDomainWarp = false;
						Vol->SetFolderPath(Folder);
						UE_LOG(LogTemp, Warning, TEXT("  %s stamped at (%.0f,%.0f,%.0f) — %.0fm x %.0fm"),
							Label, Pos.X, Pos.Y, Pos.Z, Extent.X * 2.0f / 100.0f, Extent.Y * 2.0f / 100.0f);
					}
				};

				int32 ArenaCount = 0;

				// Helper: spawn beacon lights
				auto SpawnBeaconLight = [&World, &Cfg](const FVector& Pos, FLinearColor Color,
					float Intensity, float Radius, const FName& Folder, int32 Count = 1, float RingRadius = 0.0f)
				{
					for (int32 i = 0; i < Count; ++i)
					{
						FVector Offset = FVector::ZeroVector;
						if (Count > 1 && RingRadius > 0.0f)
						{
							float Angle = (2.0f * PI * i) / Count;
							Offset = FVector(FMath::Cos(Angle) * RingRadius, FMath::Sin(Angle) * RingRadius, 0);
						}
						APointLight* Light = World->SpawnActor<APointLight>(
							Pos + FVector(0, 0, 500) + Offset, FRotator::ZeroRotator);
						if (Light && Light->PointLightComponent)
						{
							Light->PointLightComponent->SetMobility(EComponentMobility::Movable);
							Light->PointLightComponent->SetIntensity(Intensity);
							Light->PointLightComponent->SetLightColor(Color);
							Light->PointLightComponent->SetAttenuationRadius(Radius);
							Light->PointLightComponent->SetCastShadows(false);
							Light->PointLightComponent->SetMaxDrawDistance(15000.0f);
							Light->SetFolderPath(Folder);
						}
					}
				};

				for (UFlowAbstractNode* Node : Model->LayoutGraph->GraphNodes)
				{
					if (!Node || !Node->bActive) continue;
					FVector WorldPos = Node->Coord * Config->GridSize;
					AllNodePositions.Add(WorldPos);

					// Entrance
					if (Node->PathName == TEXT("main_start") ||
						(Node->PathName == TEXT("main") && Node->PathIndex == 0))
					{
						EntrancePos = WorldPos;
						bFoundEntrance = true;
						SpawnBeaconLight(WorldPos, FLinearColor(0.0f, 1.0f, 0.2f), 200000.0f, 5000.0f,
							FName(TEXT("Lighting/EntranceBeacon")));
					}

					// Hub
					if (Node->PathName == TEXT("hub"))
					{
						ADAVoxelVolume* HubVol = World->SpawnActor<ADAVoxelVolume>(
							WorldPos + FVector(0, 0, 800), FRotator::ZeroRotator);
						if (HubVol)
						{
							HubVol->BoxExtent = FVector(1500, 1500, 800);
							HubVol->bSubtract = false;
							HubVol->Priority = 30;
							HubVol->bOverrideNoiseSettings = true;
							HubVol->NoiseSettingsOverride.NoiseAmplitude = 100.0f;
							HubVol->NoiseSettingsOverride.NoiseFloorScale = 0.05f;
							HubVol->NoiseSettingsOverride.NoiseCeilingScale = 0.5f;
							HubVol->NoiseSettingsOverride.NoiseOctaves = 2;
							HubVol->NoiseSettingsOverride.bEnableDomainWarp = false;
							HubVol->SetFolderPath(FName(TEXT("DungeonSystem/Hub")));
						}
						SpawnBeaconLight(WorldPos, FLinearColor(1.0f, 1.0f, 1.0f), 300000.0f, 6000.0f,
							FName(TEXT("Lighting/HubBeacon")));
					}

					// Boss
					if (Node->PathName == TEXT("boss_arena") ||
					(Node->PathName == TEXT("boss") && Node->PathIndex == Node->PathLength - 1))
					{
						BossPos = WorldPos;
						bFoundBoss = true;
						StampArena(WorldPos, FVector(5000, 5000, 2000), 2000.0f, 50.0f,
							FName(TEXT("DungeonSystem/BossArena")), TEXT("Boss arena"));
						SpawnBeaconLight(WorldPos, FLinearColor(1.0f, 0.0f, 0.5f), 500000.0f, 8000.0f,
							FName(TEXT("Lighting/BossBeacon")), 6, 1500.0f);
						ArenaCount++;
					}

					// Treasure
					if (Node->PathName == TEXT("treasure_room") ||
					(Node->PathName == TEXT("treasure") && Node->PathIndex == Node->PathLength - 1))
					{
						StampArena(WorldPos, FVector(600, 600, 400), 400.0f, 100.0f,
							FName(TEXT("DungeonSystem/TreasureRooms")), TEXT("Treasure room"));
						SpawnBeaconLight(WorldPos, FLinearColor(1.0f, 0.85f, 0.0f), 200000.0f, 5000.0f,
							FName(TEXT("Lighting/TreasureBeacon")));
						ArenaCount++;
					}

					// Trap
					if (Node->PathName == TEXT("trap_room") ||
					(Node->PathName == TEXT("trap") && Node->PathIndex == Node->PathLength - 1))
					{
						StampArena(WorldPos, FVector(1000, 1000, 500), 500.0f, 100.0f,
							FName(TEXT("DungeonSystem/TrapRooms")), TEXT("Trap room"));
						SpawnBeaconLight(WorldPos, FLinearColor(1.0f, 0.0f, 0.0f), 200000.0f, 5000.0f,
							FName(TEXT("Lighting/TrapBeacon")));
						ArenaCount++;
					}

					// Mini-Boss
					if (Node->PathName == TEXT("miniboss_arena") ||
					(Node->PathName == TEXT("miniboss") && Node->PathIndex == Node->PathLength - 1))
					{
						StampArena(WorldPos, FVector(1500, 1500, 600), 600.0f, 100.0f,
							FName(TEXT("DungeonSystem/MiniBossArenas")), TEXT("Mini-boss arena"));
						SpawnBeaconLight(WorldPos, FLinearColor(0.0f, 0.3f, 1.0f), 200000.0f, 5000.0f,
							FName(TEXT("Lighting/MiniBossBeacon")));
						ArenaCount++;
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("  Terminal arenas stamped: %d"), ArenaCount);

				// Path lights (biome-colored)
				int32 LightCount = 0;
				for (const FVector& NodePos : AllNodePositions)
				{
					APointLight* CaveLight = World->SpawnActor<APointLight>(
						NodePos + FVector(0, 0, 300), FRotator::ZeroRotator);
					if (CaveLight)
					{
						UPointLightComponent* PLC = CaveLight->PointLightComponent;
						if (PLC)
						{
							PLC->SetMobility(EComponentMobility::Movable);
							PLC->SetIntensity(Cfg.PathLightIntensity);
							PLC->SetLightColor(Cfg.PathLightColor);
							PLC->SetAttenuationRadius(1500.0f);
							PLC->SetCastShadows(false);
							PLC->SetMaxDrawDistance(6000.0f);
						}
						CaveLight->SetFolderPath(FName(TEXT("Lighting/CaveLights")));
						LightCount++;
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("  Placed %d path lights"), LightCount);

				// PlayerStart
				if (bFoundEntrance)
				{
					FVector EntranceWorld = EntrancePos + FVector(0, 0, 200);
					APlayerStart* PS = World->SpawnActor<APlayerStart>(
						EntranceWorld, FRotator::ZeroRotator);
					if (PS) PS->SetFolderPath(FName(TEXT("Gameplay")));
					UE_LOG(LogTemp, Warning, TEXT("  PlayerStart at (%.0f, %.0f, %.0f)"),
						EntranceWorld.X, EntranceWorld.Y, EntranceWorld.Z);

					if (Manager)
					{
						Manager->StoredEntrancePosition = EntranceWorld;
						Manager->bHasStoredEntrance = true;
					}
				}
				else
				{
					APlayerStart* PS = World->SpawnActor<APlayerStart>(
						FVector(0, 0, 5000), FRotator::ZeroRotator);
					if (PS) PS->SetFolderPath(FName(TEXT("Gameplay")));
					UE_LOG(LogTemp, Warning, TEXT("  PlayerStart at fallback (0, 0, 5000)"));
				}
			}

		}
		else
		{
			APlayerStart* PS = World->SpawnActor<APlayerStart>(
				FVector(0, 0, 5000), FRotator::ZeroRotator);
			if (PS) PS->SetFolderPath(FName(TEXT("Gameplay")));
		}

		// ── Force material on voxel mesh chunks (use same VoxelMat from pre-build) ──
		{
			UMaterialInterface* ForceMat = VoxelMat;
			if (!ForceMat)
			{
				ForceMat = LoadObject<UMaterialInterface>(nullptr,
					TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
			}
			int32 MatCount = 0;
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				AActor* Actor = *It;
				if (!Actor) continue;

				bool bIsVoxelChunk = Actor->ActorHasTag(TEXT("da_voxel_mesh"));
				if (!bIsVoxelChunk)
				{
					TArray<UMeshComponent*> Meshes;
					Actor->GetComponents<UMeshComponent>(Meshes);
					for (UMeshComponent* MC : Meshes)
					{
						if (MC && MC->GetMaterial(0) == UMaterial::GetDefaultMaterial(MD_Surface))
						{
							bIsVoxelChunk = true;
							break;
						}
					}
				}

				if (bIsVoxelChunk && ForceMat)
				{
					TArray<UMeshComponent*> Meshes;
					Actor->GetComponents<UMeshComponent>(Meshes);
					for (UMeshComponent* MC : Meshes)
					{
						if (MC)
						{
							MC->SetMaterial(0, ForceMat);
							MatCount++;
						}
					}
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("  Force-set material on %d mesh components"), MatCount);
		}

		UE_LOG(LogTemp, Warning, TEXT("  Dungeon KEPT in level (pre-built with voxels)"));
	}

	// Disable runtime auto-build
	if (Manager)
	{
		Manager->bAutoBuildOnPlay = false;
		if (Dungeon)
		{
			UCellFlowModel* FinalModel = Cast<UCellFlowModel>(Dungeon->GetModel());
			UCellFlowConfig* FinalConfig = Cast<UCellFlowConfig>(Dungeon->GetConfig());
			if (FinalModel && FinalModel->LayoutGraph && FinalConfig)
			{
				for (UFlowAbstractNode* Node : FinalModel->LayoutGraph->GraphNodes)
				{
					if (!Node || !Node->bActive) continue;
					if (Node->PathName == TEXT("main_start") ||
						(Node->PathName == TEXT("main") && Node->PathIndex == 0))
					{
						Manager->StoredEntrancePosition = Node->Coord * FinalConfig->GridSize;
						Manager->bHasStoredEntrance = true;
						break;
					}
				}
			}
		}
	}

	// ── 3d. Lighting (biome-colored) ──
	ADirectionalLight* DirLight = World->SpawnActor<ADirectionalLight>(
		FVector(0, 0, 5000), FRotator(-45, 30, 0));
	if (DirLight)
	{
		DirLight->GetComponent()->SetMobility(EComponentMobility::Movable);
		DirLight->GetComponent()->SetIntensity(Cfg.DirLightIntensity);
		DirLight->GetComponent()->SetLightColor(Cfg.DirLightColor);
		DirLight->SetFolderPath(FName(TEXT("Lighting")));
	}

	ASkyLight* Sky = World->SpawnActor<ASkyLight>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (Sky)
	{
		Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
		Sky->GetLightComponent()->SetIntensity(Cfg.SkyLightIntensity);
		Sky->GetLightComponent()->SetLightColor(Cfg.SkyLightColor);
		Sky->GetLightComponent()->SetCastShadows(false);
		Sky->SetFolderPath(FName(TEXT("Lighting")));
	}

	// Bake NavMesh
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (NavSys)
	{
		NavSys->Build();
		UE_LOG(LogTemp, Warning, TEXT("  NavMesh built and baked into level."));
	}

	bool bSaved = SaveLevel(World, Cfg.MapPath);
	GEngine->DestroyWorldContext(World);
	return bSaved;
}

#endif // WITH_EDITOR
