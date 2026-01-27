// B_Container.h
// C++ class for Blueprint B_Container
//
// 20-PASS VALIDATION: 2026-01-14
// Source: BlueprintDNA/Blueprint/B_Container.json
// Parent: B_Interactable_C -> AB_Interactable
// Variables: 5 | Components: 6 | Functions: 4 | Dispatchers: 0
//
// Blueprint Logic:
// - OnInteract: Opens the chest - plays timeline, montage, spawns VFX, spawns loot
// - OpenChest timeline: Rotates lid over time, fires SpawnVFX and ItemSpawn events
// - FadeOffTimeline: Fades point light intensity to 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Interactable.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/PointLightComponent.h"
#include "NiagaraSystem.h"
#include "Animation/AnimMontage.h"
#include "B_Container.generated.h"

// Forward declarations
class UAC_LootDropManager;
class AB_PickupItem;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Container : public AB_Interactable
{
	GENERATED_BODY()

public:
	AB_Container();

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENT REFERENCES (found at runtime from Blueprint SCS)
	// NOTE: Blueprint SCS owns the actual components (has mesh assignments)
	// C++ caches references in BeginPlay for runtime access
	// Names prefixed with "Cached" to avoid collision with Blueprint SCS component names
	// ═══════════════════════════════════════════════════════════════════════

	/** Cached reference to Lid mesh component (from Blueprint SCS "Lid") */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CachedLid;

	/** Cached reference to ItemSpawn location (from Blueprint SCS "ItemSpawn") */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* CachedItemSpawn;

	/** Cached reference to PointLight (from Blueprint SCS "PointLight") */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* CachedPointLight;

	/** Cached reference to LootDropManager (from Blueprint SCS "AC_LootDropManager") */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UAC_LootDropManager* CachedLootDropManager;

	/** Cached reference to MoveToLocation (from Blueprint SCS "MoveToLocation") */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* CachedMoveToLocation;

	/** Cached reference to NiagaraLocation (from Blueprint SCS "NiagaraLocation") */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* CachedNiagaraLocation;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5) - From Blueprint Variables
	// ═══════════════════════════════════════════════════════════════════════

	/** Montage to play on interacting character when opening */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Config")
	TSoftObjectPtr<UAnimMontage> OpenMontage;

	/** VFX to spawn when chest opens */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Config")
	TSoftObjectPtr<UNiagaraSystem> OpenVFX;

	/** Distance the lid moves */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Config")
	double MoveDistance;

	/** Speed multiplier for open animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Config")
	double SpeedMultiplier;

	/** Cached point light intensity for fade (runtime, matches Blueprint float type) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	float CachedIntensity;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	/** Called when player interacts with this container */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* InteractingActor);
	virtual void OnInteract_Implementation(AActor* InteractingActor);

	/** Opens the container - triggers timeline animations */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Container")
	void OpenContainer();
	virtual void OpenContainer_Implementation();

	/** Opens lid and spawns item - called from timeline events */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Container")
	void OpenLidAndSpawnItem(bool bSpawnItem, bool bSpawnVFX);
	virtual void OpenLidAndSpawnItem_Implementation(bool bSpawnItem, bool bSpawnVFX);

	/** Initialize states from save data */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Container")
	void InitializeLoadedStates(bool bCanBeTracedState, bool bIsActivatedState);
	virtual void InitializeLoadedStates_Implementation(bool bCanBeTracedState, bool bIsActivatedState);

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Handle item ready for spawn from loot manager */
	UFUNCTION()
	void OnItemReadyForSpawn(FSLFLootItem LootItem);

	/** Handle when spawned pickup item is looted */
	UFUNCTION()
	void OnSpawnedItemLooted();

private:
	// Timeline state
	FTimerHandle OpenChestTimerHandle;
	FTimerHandle FadeOffTimerHandle;
	float OpenChestProgress;
	float FadeOffProgress;
	FRotator InitialLidRotation;
	bool bHasSpawnedVFX;
	bool bHasSpawnedItems;

	/** Update open chest animation (simulates timeline) */
	void UpdateOpenChestTimeline();

	/** Update fade off animation (simulates timeline) */
	void UpdateFadeOffTimeline();

	/** Spawn VFX at niagara location */
	void SpawnOpenVFX();

	/** Spawn loot items */
	void SpawnItems();
};
