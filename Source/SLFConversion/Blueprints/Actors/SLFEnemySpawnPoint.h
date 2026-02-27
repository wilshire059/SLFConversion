// SLFEnemySpawnPoint.h
// Placed in levels to define enemy spawn locations.
// Enemies spawn on level load and respawn when player rests at a resting point.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFEnemySpawnPoint.generated.h"

class USphereComponent;
class UBillboardComponent;
class ASLFSoulslikeEnemy;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFEnemySpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ASLFEnemySpawnPoint();

protected:
	virtual void BeginPlay() override;

public:
	// ── Configuration ──

	/** Enemy class to spawn at this point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config")
	TSubclassOf<ACharacter> EnemyClass;

	/** Zone this spawn point belongs to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config")
	FGameplayTag ZoneTag;

	/** Enemy rank (affects scaling) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config")
	ESLFEnemyRank Rank = ESLFEnemyRank::Regular;

	/** Patrol path actor (optional — enemy patrols if set) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config")
	TObjectPtr<AActor> PatrolPath;

	/** Spawn radius — random offset within this radius for natural placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config", meta = (ClampMin = "0"))
	float SpawnRadius = 50.0f;

	/** Stat scale multiplier applied to enemy base stats */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config")
	float StatScale = 1.0f;

	/** Currency reward on kill */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config")
	int32 CurrencyReward = 100;

	/** Whether this enemy respawns when player rests */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config")
	bool bRespawnOnRest = true;

	/** Whether this enemy is initially spawned on level load */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config")
	bool bSpawnOnBeginPlay = true;

	/** Maximum spawn distance from player (don't spawn if player is too far) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Config")
	float MaxSpawnDistance = 10000.0f;

	// ── Runtime State ──

	/** Currently spawned enemy (nullptr if dead or despawned) */
	UPROPERTY(BlueprintReadOnly, Category = "Spawn|State")
	TObjectPtr<ACharacter> SpawnedEnemy;

	/** Whether the enemy has been killed since last rest */
	UPROPERTY(BlueprintReadOnly, Category = "Spawn|State")
	bool bEnemyKilled = false;

	// ── Functions ──

	/** Spawn the enemy at this point */
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	ACharacter* SpawnEnemy();

	/** Despawn the current enemy (if alive) */
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void DespawnEnemy();

	/** Called when player rests — respawns enemy if configured */
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void OnPlayerRested();

	/** Check if the spawned enemy is alive */
	UFUNCTION(BlueprintPure, Category = "Spawn")
	bool IsEnemyAlive() const;

private:
	/** Editor visualization */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBillboardComponent> EditorSprite;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SpawnRadiusVisual;

	/** Handle enemy death */
	UFUNCTION()
	void OnSpawnedEnemyDeath(AActor* DeadActor);

	/** Get randomized spawn location within radius */
	FVector GetRandomSpawnLocation() const;
};
