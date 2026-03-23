// SetupBatchEnemyCommandlet.h
// Generic commandlet for importing any Ashborne enemy from the Meshy AI + Blender pipeline.
// Usage: -run=SetupBatchEnemy -name=withered_wanderer [-skipmesh] [-skipanims]

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "SetupBatchEnemyCommandlet.generated.h"

UCLASS()
class USetupBatchEnemyCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	USetupBatchEnemyCommandlet();
	virtual int32 Main(const FString& Params) override;

private:
	// Convert snake_case enemy name to PascalCase (e.g., "withered_wanderer" -> "WitheredWanderer")
	static FString ToPascalCase(const FString& SnakeName);

	// Categorize Elden Ring animation ID into role (Attack, Idle, Walk, Run, HitReact, Death, Dodge, Guard, PoiseBreak)
	static FString CategorizeAnimID(const FString& AnimID);

	// Import mesh from FBX via UAssetImportTask
	bool ImportMesh(const FString& FBXPath, const FString& DestDir, const FString& DestName);

	// Import PBR textures and create material
	bool ImportTexturesAndCreateMaterial(const FString& EnemyDir, const FString& DestDir, const FString& PascalName);

	// Import all animations from fbx/ directory
	int32 ImportAnimations(const FString& FBXDir, const FString& DestAnimDir, const FString& PascalName, USkeleton* Skeleton);

	// Create montages from imported animation sequences
	void CreateMontages(const FString& DestDir, const FString& AnimDir, const FString& PascalName,
		const TMap<FString, FString>& AnimCategoryMap);

	// Create blend space from locomotion animations
	void CreateBlendSpace(const FString& DestDir, const FString& AnimDir, const FString& PascalName, const FString& SkeletonPath);

	// Create AnimBP by duplicating and retargeting
	void CreateAnimBP(const FString& DestDir, const FString& AnimDir, const FString& PascalName, const FString& SkeletonPath);

	// Create data assets (AnimData, WeaponAnimset, CombatReaction, PoiseBreak, AI Abilities)
	void CreateDataAssets(const FString& DestDir, const FString& AnimDir, const FString& PascalName, const FString& EnemySnakeName);

	// Add sockets to skeleton
	void AddSockets(const FString& SkeletonPath);

	// Add weapon traces to attack montages (tries TAE JSON first, falls back to heuristic)
	void AddWeaponTraces(const FString& DestDir, const FString& PascalName, const FString& EnemySnakeName);

	// Add weapon traces from TAE-extracted hitbox JSON. Returns true if JSON found and applied.
	bool AddWeaponTracesFromTAE(const FString& DestDir, const FString& PascalName, const FString& EnemySnakeName, TSet<FString>* OutProcessedMontages = nullptr);

	// Helper: save asset
	static bool SaveAsset(UObject* Asset, UPackage* Pkg);

	// Helper: prepare package (clear existing)
	static UPackage* PreparePackage(const FString& PkgName, const FString& ObjName);

	// Source FBX directory
	static constexpr const TCHAR* SourceBaseDir = TEXT("C:/scripts/elden_ring_tools/test_meshes");
};
