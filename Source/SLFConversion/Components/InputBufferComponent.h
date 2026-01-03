// InputBufferComponent.h
// C++ base class for AC_InputBuffer
// Variables: 4, Functions: 6, Event Dispatchers: 1
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer
//
// BUFFER SEMANTICS (CRITICAL):
// - BufferOpen = true  -> Queue action for later (during attack/animation)
// - BufferOpen = false -> Execute action immediately (buffer is closed/available)
// - ToggleBuffer(false) processes the queued action when buffer closes

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InputBufferComponent.generated.h"

// Event Dispatcher: Called when buffered input is consumed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputBufferConsumed, FGameplayTag, Action);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInputBufferComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 4/4 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** Currently queued action tag waiting to be consumed */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FGameplayTag IncomingActionTag;

	/** Tags of actions to ignore on next queue attempt */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer IgnoreNextOfActions;

	/** Buffer state: true = queuing (during animation), false = ready to execute */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bBufferOpen;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** Called when a buffered action is consumed and should be executed */
	UPROPERTY(BlueprintAssignable, Category = "Runtime")
	FOnInputBufferConsumed OnInputBufferConsumed;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 6/6 migrated (excluding ExecuteUbergraph)
	// ═══════════════════════════════════════════════════════════════════

	/**
	 * Queue an action for later execution when buffer opens
	 * Logic: If BufferOpen -> store action for later
	 *        If BufferClosed -> execute immediately via ConsumeInputBuffer
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input Buffer")
	void QueueAction(FGameplayTag QueuedActionTag);
	virtual void QueueAction_Implementation(FGameplayTag QueuedActionTag);

	/**
	 * Toggle the buffer state
	 * When closing (bOpen = false), processes any queued action
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input Buffer", meta = (DisplayName = "Toggle Buffer"))
	void ToggleBuffer(UPARAM(DisplayName = "Buffer Open?") bool BufferOpen);
	virtual void ToggleBuffer_Implementation(bool BufferOpen);

	/**
	 * Consume and broadcast the currently buffered action
	 * Clears IncomingActionTag and broadcasts OnInputBufferConsumed
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input Buffer")
	void ConsumeInputBuffer();
	virtual void ConsumeInputBuffer_Implementation();

	/**
	 * Execute an action immediately, bypassing buffer logic
	 * Sets IncomingActionTag and calls ConsumeInputBuffer
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input Buffer")
	void ExecuteActionImmediately(FGameplayTag Action);
	virtual void ExecuteActionImmediately_Implementation(FGameplayTag Action);

protected:
	/**
	 * Internal event called when queuing an action for next frame
	 * Checks IgnoreNextOfActions before processing
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Input Buffer")
	void QueueNext(FGameplayTag ActionTag);
	virtual void QueueNext_Implementation(FGameplayTag ActionTag);
};
