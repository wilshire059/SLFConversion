// AC_InputBuffer.h
// C++ component for AC_InputBuffer
//
// 20-PASS VALIDATION: 2026-01-02 - UPDATED
// Source: BlueprintDNA/Component/AC_InputBuffer.json
// Variables: 3 | Functions: 5 | Dispatchers: 1
//
// PASS 6-7: Added EventQueueNext (custom event) with timer-based delay loop

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_InputBuffer.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_InputBuffer_OnInputBufferConsumed, FGameplayTag, Action);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_InputBuffer : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_InputBuffer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3 + 2 internal)
	// ═══════════════════════════════════════════════════════════════════════

	// Runtime: Current action waiting in the buffer
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FGameplayTag IncomingActionTag;

	// Config: Tags to ignore when queueing next action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer IgnoreNextOfActions;

	// Runtime: Whether the buffer is open for queueing
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool BufferOpen;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_InputBuffer_OnInputBufferConsumed OnInputBufferConsumed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (5)
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * Queue an action tag - if buffer is open, waits for it to close before consuming.
	 * If buffer is closed, consumes immediately.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InputBuffer")
	void QueueAction(const FGameplayTag& QueuedActionTag);
	virtual void QueueAction_Implementation(const FGameplayTag& QueuedActionTag);

	/**
	 * Consume the buffered action - broadcasts OnInputBufferConsumed and resets IncomingActionTag.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InputBuffer")
	void ConsumeInputBuffer();
	virtual void ConsumeInputBuffer_Implementation();

	/**
	 * Toggle buffer on/off.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InputBuffer")
	void ToggleBuffer(bool InBufferOpen);
	virtual void ToggleBuffer_Implementation(bool InBufferOpen);

	/**
	 * Execute an action immediately, bypassing the buffer.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InputBuffer")
	void ExecuteActionImmediately(const FGameplayTag& Action);
	virtual void ExecuteActionImmediately_Implementation(const FGameplayTag& Action);

	/**
	 * Custom Event: Waits for buffer to close before consuming the action.
	 * Blueprint Logic:
	 * 1. Check if ActionTag is in IgnoreNextOfActions (using HasTag)
	 * 2. If NOT in ignore list:
	 *    - Check BufferOpen?
	 *    - If BufferOpen is TRUE: DelayUntilNextTick, then loop back
	 *    - If BufferOpen is FALSE: Set IncomingActionTag, call ConsumeInputBuffer
	 */
	UFUNCTION(BlueprintCallable, Category = "AC_InputBuffer")
	void EventQueueNext(const FGameplayTag& ActionTag);

private:
	// ═══════════════════════════════════════════════════════════════════════
	// INTERNAL (Timer-based delay loop)
	// ═══════════════════════════════════════════════════════════════════════

	// Timer for DelayUntilNextTick loop in EventQueueNext
	FTimerHandle QueueNextTimerHandle;

	// Cached action tag for the timer callback
	FGameplayTag PendingQueueNextAction;

	/** Timer callback for the delay loop in EventQueueNext */
	void OnQueueNextTimerElapsed();
};
