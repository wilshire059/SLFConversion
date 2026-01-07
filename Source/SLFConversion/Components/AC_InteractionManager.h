// AC_InteractionManager.h
// C++ component for AC_InteractionManager
//
// 20-PASS VALIDATION: 2026-01-02 Full Migration
// Source: BlueprintDNA_v2/Component/AC_InteractionManager.json
// Variables: 19 | Functions: 6 | Events: 1 | Dispatchers: 0
//
// Full logic migrated from Blueprint graphs:
// - INTERACTABLE DETECTION & INTERACTION (Tick)
// - RESTING (Event OnRest)
// - TARGET LOCKING (interface calls)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AC_InteractionManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations


UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_InteractionManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_InteractionManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (19)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Interaction")
	double InteractionRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Interaction")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceChannels;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Interaction")
	TEnumAsByte<EDrawDebugTrace::Type> DebugDraw;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<AActor*> NearbyInteractables;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	AActor* NearestInteractable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime", Replicated)
	bool TargetLocked;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime", Replicated)
	AActor* LockedOnTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<AActor*> Targets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Target Lock")
	TArray<FName> TargetLockAcceptedActorTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Target Lock")
	double MaxTargetLockDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Target Lock")
	double TargetLockStrength;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Target Lock")
	double TargetLockTraceDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Target Lock")
	double TargetLockTraceRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Target Lock")
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetLockTraceObjectTypes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Target Lock")
	TEnumAsByte<EDrawDebugTrace::Type> TargetLockDebugDrawType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool AllowTargetSwap;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	AActor* LastRestingPoint; // Type: B_RestingPoint (AActor for compatibility)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle TargetLockDistanceTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	USceneComponent* LockedOnComponent;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (6) + EVENTS (1)
	// ═══════════════════════════════════════════════════════════════════════

	// --- Custom Event: Event OnRest ---
	// Called when player rests at a resting point
	// Sets LastRestingPoint and triggers HUD fade
	UFUNCTION(BlueprintCallable, Category = "AC_InteractionManager|Events")
	void EventOnRest(AActor* RestingPoint);

	// --- Custom Event: Event RestInitialized ---
	// Called when resting is initialized (ready to show rest menu)
	UFUNCTION(BlueprintCallable, Category = "AC_InteractionManager|Events")
	void EventRestInitialized();

	// --- Custom Event: Event RestEnded ---
	// Called when resting ends (player leaves rest menu)
	UFUNCTION(BlueprintCallable, Category = "AC_InteractionManager|Events")
	void EventRestEnded();

	// --- Custom Event: Event Replenishment ---
	// Called to replenish resources at resting point
	UFUNCTION(BlueprintCallable, Category = "AC_InteractionManager|Events")
	void EventReplenishment();

	// --- Custom Event: Event Reset ---
	// Called to reset interaction state
	UFUNCTION(BlueprintCallable, Category = "AC_InteractionManager|Events")
	void EventReset();

	// --- Custom Event: Event Lockon ---
	// Called to trigger lock-on behavior
	UFUNCTION(BlueprintCallable, Category = "AC_InteractionManager|Events")
	void EventLockon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InteractionManager")
	void RemoveFromNearbyInteractables(AActor* Item);
	virtual void RemoveFromNearbyInteractables_Implementation(AActor* Item);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InteractionManager")
	TArray<FHitResult> TargetLockTrace();
	virtual TArray<FHitResult> TargetLockTrace_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InteractionManager")
	void LockOnTarget(AActor* LockOnTarget, bool RotateTowards);
	virtual void LockOnTarget_Implementation(AActor* LockOnTarget, bool RotateTowards);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InteractionManager")
	void ResetLockOn();
	virtual void ResetLockOn_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InteractionManager")
	bool IsTargetLocked();
	virtual bool IsTargetLocked_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InteractionManager")
	void CheckTargetDistance();
	virtual void CheckTargetDistance_Implementation();
};
