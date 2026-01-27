// SLFRestingPointBase.h
// C++ base class for B_RestingPoint
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_RestingPoint (FULL EVENTGRAPH MIGRATION 2026-01-16)
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         10/10 migrated
// Functions:         4/4 migrated (GetReplenishData, OnInteract, OnTraced, DiscoverPoint)
// Event Dispatchers: 2/2 migrated
// Components:        8 (SittingZone, Scene, CameraArm, LookatCamera, UnlockEffect, Effect, EffectLight, EnvironmentLight)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint
//
// EVENTGRAPH LOGIC:
// - OnInteract: Branch on IsActivated -> DiscoverPoint (first time) or Rest (subsequent)
// - OnTraced: Set InteractionText based on IsActivated ("Discover Resting Point" vs "Rest")
// - DiscoverPoint: Activate effect, play montage, save game, set IsActivated=true
// - PositionSittingActor: Move player to SittingZone after 1s delay, broadcast OnReady

#pragma once

#include "CoreMinimal.h"
#include "SLFInteractableBase.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PointLightComponent.h"
#include "NiagaraComponent.h"
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
	// DefaultSceneRoot is inherited from SLFInteractableBase

	ASLFRestingPointBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnConstruction(const FTransform& Transform) override;

	// ═══════════════════════════════════════════════════════════════════
	// CACHED COMPONENT REFERENCES (from Blueprint SCS)
	// ═══════════════════════════════════════════════════════════════════
	// NOTE: These are NOT UPROPERTY to avoid naming conflicts with Blueprint SCS.
	// Blueprint already defines these components in its SimpleConstructionScript.
	// We cache references in BeginPlay for C++ logic access.
	// Blueprint access is via the SCS components directly.

protected:
	/** Cached reference to SittingZone billboard from Blueprint SCS */
	UBillboardComponent* CachedSittingZone;

	/** Cached reference to Scene component from Blueprint SCS */
	USceneComponent* CachedSpawnScene;

	/** Cached reference to CameraArm from Blueprint SCS */
	USpringArmComponent* CachedCameraArm;

	/** Cached reference to LookatCamera from Blueprint SCS */
	UCameraComponent* CachedLookatCamera;

	/** Cached reference to UnlockEffect niagara from Blueprint SCS */
	UNiagaraComponent* CachedUnlockEffect;

	/** Cached reference to Effect niagara from Blueprint SCS */
	UNiagaraComponent* CachedEffect;

	/** Cached reference to EffectLight from Blueprint SCS */
	UPointLightComponent* CachedEffectLight;

	/** Cached reference to EnvironmentLight from Blueprint SCS */
	UPointLightComponent* CachedEnvironmentLight;

	// ═══════════════════════════════════════════════════════════════════
	// C++ CREATED COMPONENTS (not from Blueprint SCS)
	// ═══════════════════════════════════════════════════════════════════

	/** Interaction collision sphere - created in C++ for trace detection
	 *  B_RestingPoint doesn't have a mesh, so we need this for AC_InteractionManager
	 *  to detect this actor via sphere trace.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* InteractionCollision;

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

	// ═══════════════════════════════════════════════════════════════════
	// INTERFACE OVERRIDES (EventGraph Logic)
	// ═══════════════════════════════════════════════════════════════════

	/** OnInteract: Branch on IsActivated -> DiscoverPoint or Rest flow */
	virtual void OnInteract_Implementation(AActor* Interactor) override;

	/** OnTraced: Set InteractionText based on IsActivated */
	virtual void OnTraced_Implementation(AActor* TracedBy) override;

	// ═══════════════════════════════════════════════════════════════════
	// CUSTOM FUNCTIONS (EventGraph Logic)
	// ═══════════════════════════════════════════════════════════════════

	/** Called when player discovers this resting point for the first time */
	UFUNCTION(BlueprintCallable, Category = "RestingPoint")
	void DiscoverPoint(AActor* DiscoveringActor);

protected:
	/** Timer handle for delayed positioning after rest */
	FTimerHandle PositionTimerHandle;

	/** Called after 1s delay to position sitting actor and broadcast OnReady */
	void PositionSittingActor();
};
