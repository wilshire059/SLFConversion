// SLFRestingPointBase.h
// C++ base class for B_RestingPoint
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_RestingPoint
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         10/10 migrated (excluding 2 dispatchers counted separately)
// Functions:         1/1 migrated (excluding UserConstructionScript and EventGraph)
// Event Dispatchers: 2/2 migrated
// Graphs:            5 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint
//
// PURPOSE: Resting point actor - bonfires/checkpoints with stat replenishment
// PARENT: B_Interactable

#pragma once

#include "CoreMinimal.h"
#include "SLFInteractableBase.h"
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"
#include "SLFRestingPointBase.generated.h"

// Forward declarations
class UAnimMontage;
class UDataAsset;

/**
 * Data returned from resting point for replenishment
 */
USTRUCT(BlueprintType)
struct FSLFReplenishData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint")
	TSet<UDataAsset*> ItemsToReplenish;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint")
	FGameplayTagContainer StatsToReplenish;
};

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 2/2 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/2] Called when resting point is ready (player seated) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRestingPointReady);

/** [2/2] Called when player exits resting point */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRestingPointExited);

/**
 * Resting point actor - bonfire/checkpoint with stat replenishment
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFRestingPointBase : public ASLFInteractableBase
{
	GENERATED_BODY()

public:
	/** Default scene root for Blueprint components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

public:
	ASLFRestingPointBase();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 10/10 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Point Settings (1) ---

	/** [1/10] Display name for this location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint|Config")
	FText LocationName;

	// --- Animation (1) ---

	/** [2/10] Montage to play when discovering this point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint|Config")
	TSoftObjectPtr<UAnimMontage> DiscoverInteractionMontage;

	// --- View Settings (5) ---

	/** [3/10] Camera angle when sitting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint|View")
	double SittingAngle;

	/** [4/10] Whether to force camera to face sitting actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint|View")
	bool bForceFaceSittingActor;

	/** [5/10] Camera distance from resting point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint|View")
	float CameraDistance;

	/** [6/10] Camera position offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint|View")
	FVector CameraOffset;

	/** [7/10] Camera rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint|View")
	FRotator CameraRotation;

	// --- Replenish Settings (2) ---

	/** [8/10] Set of items to replenish when resting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint|Replenish")
	TSet<UDataAsset*> ItemsToReplenish;

	/** [9/10] Stat tags to replenish when resting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestingPoint|Replenish")
	FGameplayTagContainer StatsToReplenish;

	// --- Runtime (1) ---

	/** [10/10] Currently sitting actor (player) */
	UPROPERTY(BlueprintReadWrite, Category = "RestingPoint|Runtime")
	AActor* SittingActor;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRestingPointReady OnReady;

	/** [2/2] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRestingPointExited OnExited;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Get replenish data for this resting point
	 * @return Struct containing items and stats to replenish
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RestingPoint")
	FSLFReplenishData GetReplenishData();
	virtual FSLFReplenishData GetReplenishData_Implementation();

	// --- Interaction Override ---

	virtual void OnInteract_Implementation(AActor* Interactor) override;
};
