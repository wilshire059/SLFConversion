// SLFContainerBase.h
// C++ base class for B_Container
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Container
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         5/5 migrated
// Functions:         0/0 migrated (EventGraph logic only)
// Event Dispatchers: 0/0
// Graphs:            2 (EventGraph, UserConstructionScript)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container
//
// PURPOSE: Container actor - chests/lootable containers with open animation
// PARENT: B_Interactable

#pragma once

#include "CoreMinimal.h"
#include "SLFInteractableBase.h"
#include "SLFGameTypes.h"
#include "SLFContainerBase.generated.h"

// Forward declarations
class UAnimMontage;
class UNiagaraSystem;
class ULootDropManagerComponent;

/**
 * Container actor - chest/lootable with open animation
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFContainerBase : public ASLFInteractableBase
{
	GENERATED_BODY()

public:
	ASLFContainerBase();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 5/5 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Open Config (4) ---

	/** [1/5] Animation montage to play when opening */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|Config")
	TSoftObjectPtr<UAnimMontage> OpenMontage;

	/** [2/5] VFX to spawn when opening */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|Config")
	TSoftObjectPtr<UNiagaraSystem> OpenVFX;

	/** [3/5] Distance player moves to when opening */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|Config")
	double MoveDistance;

	/** [4/5] Speed multiplier for open animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|Config")
	double SpeedMultiplier;

	// --- Runtime (1) ---

	/** [5/5] Cached VFX intensity */
	UPROPERTY(BlueprintReadWrite, Category = "Container|Runtime")
	float CachedIntensity;

	/** Whether the container has been opened */
	UPROPERTY(BlueprintReadWrite, Category = "Container|Runtime")
	bool bIsOpen = false;

	/** Lid open rotation (degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|Config")
	float LidOpenAngle = -110.0f;

	// ═══════════════════════════════════════════════════════════════════
	// COMPONENTS (cached from Blueprint SCS)
	// ═══════════════════════════════════════════════════════════════════

	/** Cached reference to Lid mesh component (from Blueprint SCS) */
	UPROPERTY(BlueprintReadOnly, Category = "Container|Components")
	TObjectPtr<UStaticMeshComponent> CachedLid;

	/** Cached reference to NiagaraLocation component (for VFX spawn position) */
	UPROPERTY(BlueprintReadOnly, Category = "Container|Components")
	TObjectPtr<USceneComponent> CachedNiagaraLocation;

	/** Cached reference to ItemSpawn component (for loot spawn position) */
	UPROPERTY(BlueprintReadOnly, Category = "Container|Components")
	TObjectPtr<USceneComponent> CachedItemSpawn;

	/** Cached reference to LootDropManager component (for spawning loot) */
	UPROPERTY(BlueprintReadOnly, Category = "Container|Components")
	TObjectPtr<ULootDropManagerComponent> CachedLootManager;

	// ═══════════════════════════════════════════════════════════════════
	// MESH CONFIGURATION (set in Blueprint CDO)
	// ═══════════════════════════════════════════════════════════════════

	/** Static mesh for the chest box (bottom part) - applied to InteractableSM */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Container|Mesh")
	TSoftObjectPtr<UStaticMesh> ChestBoxMesh;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════

	// --- Interaction Override ---

	virtual void OnInteract_Implementation(AActor* Interactor) override;

	/** Open the lid visually */
	UFUNCTION(BlueprintCallable, Category = "Container")
	void OpenLid();

	/** Handler for when loot is ready to spawn */
	UFUNCTION()
	void HandleItemReadyForSpawn(FSLFLootItem Item);

	/** Spawn the loot pickup at ItemSpawn location */
	void SpawnLootPickup(const FSLFLootItem& Item, int32 Amount);

	/** Spawn a default loot item when no loot is configured */
	void SpawnDefaultLootPickup();

	/** Delay before opening lid and spawning items (seconds) - allows montage to play first
	 * Character animation (AM_SLF_ItemPickup) is 2.33s, chest should open near end (~2.0s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|Config")
	float OpenDelayTime = 2.0f;

private:
	/** Timer handle for delayed lid opening */
	FTimerHandle OpenTimerHandle;

	/** Called after delay to open lid and spawn items */
	void DelayedOpenLidAndSpawnItems();

	// ═══════════════════════════════════════════════════════════════════
	// SMOOTH LID ANIMATION (simple tick-based interpolation)
	// ═══════════════════════════════════════════════════════════════════

	/** Whether lid is currently animating open */
	bool bIsOpeningLid = false;

	/** Current progress of lid opening (0 to 1) */
	float LidOpenProgress = 0.0f;

	/** Starting rotation of the lid (cached when opening begins) */
	FRotator LidStartRotation;

	/** Duration of the lid opening animation (seconds) */
	UPROPERTY(EditAnywhere, Category = "Container|Animation")
	float LidOpenDuration = 5.0f;

	/** Time offset into animation when VFX should spawn (seconds) */
	UPROPERTY(EditAnywhere, Category = "Container|Animation")
	float VFXSpawnTime = 1.0f;

	/** Time offset into animation when items should spawn (seconds) */
	UPROPERTY(EditAnywhere, Category = "Container|Animation")
	float ItemSpawnTime = 2.0f;

	/** Whether VFX has been spawned this opening */
	bool bVFXSpawned = false;

	/** Whether items have been spawned this opening */
	bool bItemsSpawned = false;

	/** Track elapsed time for event timing */
	float OpenElapsedTime = 0.0f;

protected:
	virtual void Tick(float DeltaTime) override;
};
