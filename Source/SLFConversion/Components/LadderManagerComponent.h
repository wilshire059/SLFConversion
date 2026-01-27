// LadderManagerComponent.h
// C++ base class for AC_LadderManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_LadderManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         10/10 migrated
// Functions:         39/39 migrated (excluding ExecuteUbergraph) - many getters/setters
// Event Dispatchers: 0/0
// Graphs:            7 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_LadderManager
//
// PURPOSE: Ladder climbing system - manages climb state, animations, input

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLFEnums.h"
#include "InputActionValue.h"
#include "Animation/AnimMontage.h"
#include "LadderManagerComponent.generated.h"

// Forward declarations
class UDataAsset;
class UAnimMontage;
class UAnimInstance;
class UInputAction;
class UEnhancedInputComponent;

// Types used from SLFEnums.h:
// - ESLFLadderClimbState

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API ULadderManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULadderManagerComponent();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 10/10 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Runtime State (8) ---

	/** [1/10] Whether character is on ground (not climbing) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsOnGround;

	/** [2/10] Whether character is currently climbing */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsClimbing;

	/** [3/10] Whether character is climbing off at top */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsClimbingOffTop;

	/** [4/10] Whether character is climbing down from top */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsClimbingDownFromTop;

	/** [5/10] Whether character is oriented to ladder */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsOrientedToLadder;

	/** [6/10] Current climb animation state */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	ESLFLadderClimbState ClimbAnimState;

	/** [7/10] Currently used ladder actor */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	AActor* CurrentLadder;

	/** [8/10] Whether fast climbing (sprint) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bClimbFast;

	// --- Saved Settings (restored on detach) ---

	/** Original bUseControllerRotationYaw setting before climbing */
	UPROPERTY()
	bool bSavedUseControllerRotationYaw;

	/** Original bOrientRotationToMovement setting before climbing */
	UPROPERTY()
	bool bSavedOrientRotationToMovement;

	// --- Config (2) ---

	/** [9/10] Ladder animation data asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataAsset* LadderAnimset;

	/** [10/10] Sprint speed multiplier for climbing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double ClimbSprintMultiplier;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 39/39 migrated (many are simple getters/setters)
	// ═══════════════════════════════════════════════════════════════════

	// --- State Setters (7) ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Setters")
	void SetCurrentLadder(AActor* Ladder);
	virtual void SetCurrentLadder_Implementation(AActor* Ladder);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Setters")
	void SetIsClimbingDownFromTop(bool Value);
	virtual void SetIsClimbingDownFromTop_Implementation(bool Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Setters")
	void SetIsClimbingOffTop(bool Value);
	virtual void SetIsClimbingOffTop_Implementation(bool Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Setters")
	void SetIsOnGround(bool Value);
	virtual void SetIsOnGround_Implementation(bool Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Setters")
	void SetIsClimbing(bool Value);
	virtual void SetIsClimbing_Implementation(bool Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Setters")
	void SetIsOrientedToLadder(bool Value);
	virtual void SetIsOrientedToLadder_Implementation(bool Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Setters")
	void SetClimbAnimState(ESLFLadderClimbState State);
	virtual void SetClimbAnimState_Implementation(ESLFLadderClimbState State);

	// --- State Getters (7) ---

	UFUNCTION(BlueprintPure, Category = "Ladder Manager|Getters")
	bool GetIsOnGround() const { return bIsOnGround; }

	UFUNCTION(BlueprintPure, Category = "Ladder Manager|Getters")
	bool GetIsClimbing() const { return bIsClimbing; }

	UFUNCTION(BlueprintPure, Category = "Ladder Manager|Getters")
	bool GetIsClimbingOffTop() const { return bIsClimbingOffTop; }

	UFUNCTION(BlueprintPure, Category = "Ladder Manager|Getters")
	bool GetIsClimbingDownFromTop() const { return bIsClimbingDownFromTop; }

	UFUNCTION(BlueprintPure, Category = "Ladder Manager|Getters")
	bool GetIsOrientedToLadder() const { return bIsOrientedToLadder; }

	UFUNCTION(BlueprintPure, Category = "Ladder Manager|Getters")
	ESLFLadderClimbState GetClimbAnimState() const { return ClimbAnimState; }

	UFUNCTION(BlueprintPure, Category = "Ladder Manager|Getters")
	AActor* GetCurrentLadder() const { return CurrentLadder; }

	// --- Climb Control (10) ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void StartClimb(AActor* Ladder, bool bIsTopdown);
	virtual void StartClimb_Implementation(AActor* Ladder, bool bIsTopdown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void StopClimb();
	virtual void StopClimb_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void ClimbUp();
	virtual void ClimbUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void ClimbDown();
	virtual void ClimbDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void StopClimbInput();
	virtual void StopClimbInput_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void MountLadderTop();
	virtual void MountLadderTop_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void MountLadderBottom();
	virtual void MountLadderBottom_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void DismountLadderTop();
	virtual void DismountLadderTop_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void DismountLadderBottom();
	virtual void DismountLadderBottom_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void ToggleFastClimb(bool bFast);
	virtual void ToggleFastClimb_Implementation(bool bFast);

	// --- Animation & Montages (10) ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void PlayClimbUpMontage();
	virtual void PlayClimbUpMontage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void PlayClimbDownMontage();
	virtual void PlayClimbDownMontage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void PlayMountTopMontage();
	virtual void PlayMountTopMontage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void PlayMountBottomMontage();
	virtual void PlayMountBottomMontage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void PlayDismountTopMontage();
	virtual void PlayDismountTopMontage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void PlayDismountBottomMontage();
	virtual void PlayDismountBottomMontage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void StopClimbMontage();
	virtual void StopClimbMontage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void OnMontageEnded();
	virtual void OnMontageEnded_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void OnMontageBlendOut();
	virtual void OnMontageBlendOut_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager|Animation")
	void UpdateClimbPlayRate();
	virtual void UpdateClimbPlayRate_Implementation();

	// --- Utility (5) ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void OrientToLadder();
	virtual void OrientToLadder_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void AttachToLadder();
	virtual void AttachToLadder_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void DetachFromLadder();
	virtual void DetachFromLadder_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void UpdateLadderPosition();
	virtual void UpdateLadderPosition_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder Manager")
	void ResetLadderState();
	virtual void ResetLadderState_Implementation();

public:
	// Tick to handle climbing input
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// BeginPlay to setup input bindings
	virtual void BeginPlay() override;

protected:
	/** Currently playing climb montage - for stopping */
	UPROPERTY(Transient)
	UAnimMontage* CurrentClimbMontage = nullptr;

	/** Cache the last Y input for climbing direction */
	float LastClimbInputY = 0.0f;

	/** Reference to IA_Move input action for Enhanced Input */
	UPROPERTY()
	UInputAction* CachedMoveAction = nullptr;

	/** Helper to get owner character's anim instance */
	UAnimInstance* GetOwnerAnimInstance() const;

	/** Helper to play a soft montage reference
	 * @param Montage - The montage to play
	 * @param PlayRate - Animation playback speed
	 * @param bBindEndCallback - If true, binds HandleMontageEnded to fire when montage completes (for dismount animations)
	 */
	void PlayLadderMontage(const TSoftObjectPtr<UAnimMontage>& Montage, float PlayRate = 1.0f, bool bBindEndCallback = false);

	/** Callback when a montage with end binding finishes (NOT USED - montages don't auto-blend) */
	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** Callback when AnimNotify fires during montage playback
	 *  bp_only uses OnNotifyBegin with "Loop_End" notify to transition states
	 *  because montages don't auto-blend-out, so OnCompleted never fires */
	UFUNCTION()
	void HandleMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	/** Callback when montage starts blending out - fallback for montages without Loop_End notify */
	UFUNCTION()
	void HandleMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	/** Process movement input when climbing */
	void HandleClimbingInput(const FVector2D& InputVector);

	/** Enhanced Input callback for IA_Move */
	void OnMoveInput(const FInputActionValue& Value);

	// ═══════════════════════════════════════════════════════════════════
	// CLIMB EVENTS
	// ═══════════════════════════════════════════════════════════════════

	/** Event called when climbing out at top or bottom */
	UFUNCTION(BlueprintCallable, Category = "Ladder Manager|Events")
	void EventOnClimbOut();

	/** Event called when starting to climb down from top */
	UFUNCTION(BlueprintCallable, Category = "Ladder Manager|Events")
	void EventOnClimbDownFromTop();
};
