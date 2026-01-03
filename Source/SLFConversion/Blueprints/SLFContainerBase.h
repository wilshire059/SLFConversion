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
#include "SLFContainerBase.generated.h"

// Forward declarations
class UAnimMontage;
class UNiagaraSystem;

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

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 0/0 - EventGraph logic only
	// ═══════════════════════════════════════════════════════════════════

	// --- Interaction Override ---

	virtual void OnInteract_Implementation(AActor* Interactor) override;
};
