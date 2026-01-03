// InteractionManagerComponent.h
// C++ base class for AC_InteractionManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_InteractionManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         19/19 migrated
// Functions:         13/13 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 0/0
// Graphs:            7 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_InteractionManager
//
// PURPOSE: Interaction system - traces for interactables, manages target lock

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractionManagerComponent.generated.h"

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UInteractionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionManagerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 19/19 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Interaction Config (3) ---

	/** [1/19] Sphere trace radius for nearby interactables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Interaction")
	float InteractionRadius;

	/** [2/19] Object types to trace for interactables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Interaction")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceChannels;

	/** [3/19] Debug draw mode for interaction traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Interaction")
	TEnumAsByte<EDrawDebugTrace::Type> DebugDraw;

	// --- Target Lock Config (7) ---

	/** [4/19] Actor tags that are valid lock-on targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Target Lock")
	TArray<FName> TargetLockAcceptedActorTags;

	/** [5/19] Maximum distance for maintaining target lock */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Target Lock")
	double MaxTargetLockDistance;

	/** [6/19] Camera rotation strength when locked on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Target Lock")
	double TargetLockStrength;

	/** [7/19] Trace distance for finding lock targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Target Lock")
	double TargetLockTraceDistance;

	/** [8/19] Sphere trace radius for target lock */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Target Lock")
	float TargetLockTraceRadius;

	/** [9/19] Object types for target lock trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Target Lock")
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetLockTraceObjectTypes;

	/** [10/19] Debug draw mode for target lock traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Target Lock")
	TEnumAsByte<EDrawDebugTrace::Type> TargetLockDebugDrawType;

	// --- Runtime State (9) ---

	/** [11/19] Array of nearby interactable actors */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<AActor*> NearbyInteractables;

	/** [12/19] Nearest interactable actor */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	AActor* NearestInteractable;

	/** [13/19] Whether a target is currently locked (replicated) */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Runtime")
	bool bTargetLocked;

	/** [14/19] Currently locked target actor (replicated) */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Runtime")
	AActor* LockedOnTarget;

	/** [15/19] Array of potential lock targets */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<AActor*> Targets;

	/** [16/19] Target index for cycling */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	int32 TargetIndex;

	/** [17/19] Timer for interaction trace */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FTimerHandle InteractionTimer;

	/** [18/19] Camera component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UCameraComponent* PlayerCamera;

	/** [19/19] Last traced NPC (for interaction prompts) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	AActor* LastTracedNPC;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 13/13 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Replication ---
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- Getters (2) ---

	/** [1/13] Get whether target is locked */
	UFUNCTION(BlueprintPure, Category = "Interaction Manager|Getters")
	bool GetTargetLocked() const { return bTargetLocked; }

	/** [2/13] Get the locked on target */
	UFUNCTION(BlueprintPure, Category = "Interaction Manager|Getters")
	AActor* GetLockedOnTarget() const { return LockedOnTarget; }

	// --- Interaction (3) ---

	/** [3/13] Trace for nearby interactable actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager")
	void TraceForInteractables();
	virtual void TraceForInteractables_Implementation();

	/** [4/13] Attempt to interact with nearest interactable */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager")
	void TryInteract();
	virtual void TryInteract_Implementation();

	/** [5/13] Get the nearest interactable from array */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager")
	AActor* GetNearestFromArray(const TArray<AActor*>& Actors);
	virtual AActor* GetNearestFromArray_Implementation(const TArray<AActor*>& Actors);

	// --- Target Lock (5) ---

	/** [6/13] Toggle target lock on/off */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager|Target Lock")
	void ToggleTargetLock();
	virtual void ToggleTargetLock_Implementation();

	/** [7/13] Trace for lockable targets */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager|Target Lock")
	void TraceForTargets();
	virtual void TraceForTargets_Implementation();

	/** [8/13] Lock onto a specific target */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager|Target Lock")
	void LockOnTarget(AActor* Target);
	virtual void LockOnTarget_Implementation(AActor* Target);

	/** [9/13] Unlock from current target */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager|Target Lock")
	void UnlockTarget();
	virtual void UnlockTarget_Implementation();

	/** [10/13] Switch to next/previous target
	 * @param bNext - True for next target, false for previous
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager|Target Lock")
	void SwitchTarget(bool bNext);
	virtual void SwitchTarget_Implementation(bool bNext);

	// --- Utility (3) ---

	/** [11/13] Update target lock camera rotation (called in tick) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager")
	void UpdateTargetLock(float DeltaTime);
	virtual void UpdateTargetLock_Implementation(float DeltaTime);

	/** [12/13] Server RPC to set target lock state */
	UFUNCTION(Server, Reliable, Category = "Interaction Manager")
	void SRV_SetTargetLock(AActor* Target, bool bLocked);
	void SRV_SetTargetLock_Implementation(AActor* Target, bool bLocked);

	/** [13/13] Initialize interaction manager */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Manager")
	void Initialize();
	virtual void Initialize_Implementation();
};
