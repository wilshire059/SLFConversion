// SLFProceduralCaveManager.h
// Single-dungeon runtime manager for unified voxel cave.
//
// Spawns one ADungeon (CellFlow + bCarveVoxels=true) at play time.
// After build completes, queries path positions for:
//   - Entrance (main_start) → teleport player
//   - Boss arena (boss path end) → spawn ADAVoxelVolume with flat noise
//
// Configurable via editor properties: CellFlowAsset, CaveTheme, Seed, GridSize.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFProceduralCaveManager.generated.h"

class ADungeon;
class UCellFlowAsset;
class UDungeonThemeAsset;

UCLASS()
class SLFCONVERSION_API AProceduralCaveManager : public AActor
{
	GENERATED_BODY()

public:
	AProceduralCaveManager();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Cave Dungeon")
	void BuildCaveDungeon();

	UPROPERTY(EditAnywhere, Category = "Cave Dungeon")
	bool bAutoBuildOnPlay = true;

	UPROPERTY(EditAnywhere, Category = "Cave Dungeon")
	TSoftObjectPtr<UCellFlowAsset> CellFlowAsset;

	UPROPERTY(EditAnywhere, Category = "Cave Dungeon")
	TObjectPtr<UDungeonThemeAsset> CaveTheme;

	UPROPERTY(EditAnywhere, Category = "Cave Dungeon")
	int32 Seed = 42;

	UPROPERTY(EditAnywhere, Category = "Cave Dungeon")
	FVector GridSize = FVector(400, 400, 400);

	UPROPERTY(EditAnywhere, Category = "Cave Dungeon|Boss Arena")
	FVector BossArenaExtent = FVector(1500, 1500, 800);

	// Pre-built entrance position (set by commandlet, used for teleport without rebuild)
	UPROPERTY(EditAnywhere, Category = "Cave Dungeon")
	FVector StoredEntrancePosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Cave Dungeon")
	bool bHasStoredEntrance = false;

private:
	void ConfigureDungeon(ADungeon* Dungeon);
	void ForceMaterialOnVoxelMeshes(UWorld* World);
	void OnBuildComplete(ADungeon* Dungeon, bool bSuccess);
	void SpawnBossArenaVolume(UWorld* World, const FVector& BossPos);
	void TeleportPlayerToEntrance(const FVector& EntrancePos);
	void QueryPathPositions(ADungeon* Dungeon, FVector& OutEntrance, FVector& OutBoss,
		bool& bFoundEntrance, bool& bFoundBoss);

	UPROPERTY()
	TObjectPtr<ADungeon> CaveDungeon;
};
