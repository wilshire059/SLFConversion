// SLFZoneManagerComponent.h
// Manages game zones, tracks player zone transitions, and handles zone-specific behavior.
// Attach to the PlayerController or GameState.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
#include "SLFZoneManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnZoneChanged, FGameplayTag, OldZone, FGameplayTag, NewZone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoneDiscovered, FGameplayTag, ZoneTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossDefeated, FGameplayTag, BossTag);

/**
 * Zone Manager - tracks current zone, discovered zones, boss defeats, enemy spawn tables.
 * Lives on the GameState or PlayerController.
 */
UCLASS(ClassGroup = (SLF), meta = (BlueprintSpawnableComponent))
class SLFCONVERSION_API USLFZoneManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLFZoneManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ── Zone Configuration ──

	/** All zones in the game (configured in editor or via data asset) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	TArray<FSLFZoneConfig> ZoneConfigs;

	/** Enemy spawn tables per zone tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|Spawning")
	TMap<FGameplayTag, FSLFEnemySpawnEntry> ZoneEnemyTables;

	// ── Runtime State ──

	/** Current zone the player is in */
	UPROPERTY(BlueprintReadOnly, Category = "Zone|State")
	FGameplayTag CurrentZone;

	/** Set of discovered zones */
	UPROPERTY(BlueprintReadOnly, Category = "Zone|State")
	FGameplayTagContainer DiscoveredZones;

	/** Set of defeated bosses */
	UPROPERTY(BlueprintReadOnly, Category = "Zone|State")
	FGameplayTagContainer DefeatedBosses;

	// ── Events ──

	UPROPERTY(BlueprintAssignable, Category = "Zone|Events")
	FOnZoneChanged OnZoneChanged;

	UPROPERTY(BlueprintAssignable, Category = "Zone|Events")
	FOnZoneDiscovered OnZoneDiscovered;

	UPROPERTY(BlueprintAssignable, Category = "Zone|Events")
	FOnBossDefeated OnBossDefeated;

	// ── Functions ──

	/** Set the player's current zone. Broadcasts OnZoneChanged. */
	UFUNCTION(BlueprintCallable, Category = "Zone")
	void SetCurrentZone(FGameplayTag NewZone);

	/** Mark a zone as discovered. Broadcasts OnZoneDiscovered if first time. */
	UFUNCTION(BlueprintCallable, Category = "Zone")
	void DiscoverZone(FGameplayTag ZoneTag);

	/** Mark a boss as defeated. Broadcasts OnBossDefeated. */
	UFUNCTION(BlueprintCallable, Category = "Zone")
	void DefeatBoss(FGameplayTag BossTag);

	/** Check if a zone has been discovered */
	UFUNCTION(BlueprintPure, Category = "Zone")
	bool IsZoneDiscovered(FGameplayTag ZoneTag) const;

	/** Check if a boss has been defeated */
	UFUNCTION(BlueprintPure, Category = "Zone")
	bool IsBossDefeated(FGameplayTag BossTag) const;

	/** Get config for a specific zone */
	UFUNCTION(BlueprintPure, Category = "Zone")
	bool GetZoneConfig(FGameplayTag ZoneTag, FSLFZoneConfig& OutConfig) const;

	/** Get the difficulty multiplier for the current zone (for stat scaling) */
	UFUNCTION(BlueprintPure, Category = "Zone")
	float GetCurrentZoneDifficultyMultiplier() const;

	/** Check if The Core is accessible (all 4 main bosses defeated) */
	UFUNCTION(BlueprintPure, Category = "Zone")
	bool IsCoreAccessible() const;

	/** Get spawn entries for a specific zone */
	UFUNCTION(BlueprintCallable, Category = "Zone|Spawning")
	TArray<FSLFEnemySpawnEntry> GetSpawnEntriesForZone(FGameplayTag ZoneTag) const;

	// ── Serialization ──

	/** Get zone save data for SaveLoadManager */
	UFUNCTION(BlueprintCallable, Category = "Zone|Save")
	void GetZoneSaveData(FGameplayTag& OutCurrentZone, FGameplayTagContainer& OutDiscovered, FGameplayTagContainer& OutBossesDefeated) const;

	/** Load zone data from save */
	UFUNCTION(BlueprintCallable, Category = "Zone|Save")
	void LoadZoneSaveData(FGameplayTag InCurrentZone, const FGameplayTagContainer& InDiscovered, const FGameplayTagContainer& InBossesDefeated);

private:
	/** Initialize default zone configs if none are set */
	void InitializeDefaultZones();
};
