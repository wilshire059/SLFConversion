// AutoBuildDungeonCommandlet.h
// Data-driven voxel cave generation using CellFlow builder.
//
// Supports biome presets via -Biome= argument:
//   stone, lava, ice, fungal, abyss
//
// Usage:
//   -run=AutoBuildDungeon [-Biome=lava] [-Seed=N] [-OutMap=/Game/Maps/L_MyMap]

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "AutoBuildDungeonCommandlet.generated.h"

class UCellFlowAsset;
class UDungeonThemeAsset;

/** All tunable parameters for a single dungeon generation run. */
struct FDungeonBiomeConfig
{
	// ── Identity ──
	FString BiomeName = TEXT("Stone Cave");

	// ── Canvas ──
	FIntPoint WorldSize = FIntPoint(120, 120);
	int32 NumPoints = 1500;
	int32 NumRelaxIterations = 12;
	float EdgeConnectionThreshold = 0.05f;
	float MinGroupArea = 1.0f;
	float ClusterBlobbiness = 1.0f;
	FVector GridSize = FVector(400, 400, 400);

	// ── Entrance ──
	int32 EntrancePathSize = 5;
	int32 EntranceCeilingMin = 1;
	int32 EntranceCeilingMax = 2;

	// ── Hub ──
	int32 HubMinSize = 2;
	int32 HubMaxSize = 4;
	int32 HubCeilingMin = 3;
	int32 HubCeilingMax = 5;

	// ── Spoke paths ──
	int32 SpokeMinSize = 15;
	int32 SpokeMaxSize = 25;

	// ── Starfish ──
	float MinHubBossDistance = 15000.0f;
	float MinHubTerminalDistance = 10000.0f;
	int32 MaxSeedRetries = 25;

	// ── Voxel ──
	float VoxelSize = 50.0f;
	float WallThickness = 125.0f;
	float NoiseAmplitude = 200.0f;
	int32 NoiseOctaves = 5;
	float NoiseFloorScale = 0.05f;
	float NoiseCeilingScale = 0.85f;
	bool bDomainWarp = true;
	float DomainWarpStrength = 0.25f;
	float CeilingExtraHeight = 1200.0f;

	// ── Lighting ──
	FLinearColor PathLightColor = FLinearColor(1.0f, 0.9f, 0.7f);
	float PathLightIntensity = 25000.0f;
	FLinearColor TorchColor = FLinearColor(1.0f, 0.85f, 0.6f);
	float TorchIntensity = 20000.0f;
	FLinearColor AmbientColor = FLinearColor(0.7f, 0.75f, 0.9f);
	float AmbientIntensity = 8000.0f;
	FLinearColor SkyLightColor = FLinearColor(0.5f, 0.55f, 0.7f);
	float SkyLightIntensity = 8.0f;
	float DirLightIntensity = 5.0f;
	FLinearColor DirLightColor = FLinearColor(0.9f, 0.85f, 0.75f);

	// ── Materials ──
	FString VoxelMaterialPath = TEXT("/Game/Wasteland/Materials/Material_Instances/MI_Rock_03.MI_Rock_03");
	FString FallbackMaterialPath = TEXT("/Game/WastelandEnvironment/Materials/Material_Instances/MI_Rock_01.MI_Rock_01");

	// ── Biome Material (from downloaded texture sets) ──
	// Disk folder containing PBR textures (BaseColor, Normal, Roughness JPGs).
	// If set, the commandlet imports these and creates a material.
	FString BiomeTextureDiskPath;  // e.g., "C:/Users/james/Downloads/cave_materials/extracted/snow_qbarr0_2k"
	FString BiomeMaterialPath;     // e.g., "/Game/Dungeons/Materials/M_Ice"

	// ── Output paths ──
	FString FlowAssetPath = TEXT("/Game/Dungeons/CellFlow/CF_UnifiedCave");
	FString ThemeAssetPath = TEXT("/Game/Dungeons/Themes/DT_UnifiedCave");
	FString MapPath = TEXT("/Game/Maps/L_CaveTest");
};

UCLASS()
class UAutoBuildDungeonCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UAutoBuildDungeonCommandlet();
	virtual int32 Main(const FString& Params) override;

private:
	UCellFlowAsset* CreateCellFlowAsset(const FDungeonBiomeConfig& Cfg);
	UDungeonThemeAsset* GenerateUnifiedTheme(const FDungeonBiomeConfig& Cfg);
	bool GenerateTestLevel(UCellFlowAsset* FlowAsset,
		UDungeonThemeAsset* Theme, int32 Seed, const FDungeonBiomeConfig& Cfg);

	bool SaveAsset(UObject* Asset, const FString& PackagePath);
	bool SaveLevel(UWorld* World, const FString& PackagePath);
	void DeleteExistingAsset(const FString& PackagePath, bool bIsMap = false);

	static FDungeonBiomeConfig GetBiomePreset(const FString& BiomeName);
	UMaterialInterface* CreateBiomeMaterial(const FDungeonBiomeConfig& Cfg);

	UPROPERTY()
	TObjectPtr<UCellFlowAsset> CachedCellFlowAsset;
};
