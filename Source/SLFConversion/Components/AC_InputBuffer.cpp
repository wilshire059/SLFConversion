// AC_InputBuffer.cpp
// C++ component implementation for AC_InputBuffer
//
// 20-PASS VALIDATION: 2026-01-02 - UPDATED
// Source: BlueprintDNA/Component/AC_InputBuffer.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added
// PASS 11-15: Added EventQueueNext with timer-based delay loop

#include "AC_InputBuffer.h"
#include "TimerManager.h"

UAC_InputBuffer::UAC_InputBuffer()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	IncomingActionTag = FGameplayTag();
	BufferOpen = false;
	PendingQueueNextAction = FGameplayTag();
}

void UAC_InputBuffer::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_InputBuffer::BeginPlay"));
}

void UAC_InputBuffer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * QueueAction - Queue an action for later execution
 *
 * Blueprint Logic:
 * 1. Set IncomingActionTag = QueuedActionTag
 * 2. Branch on BufferOpen
 *    - TRUE: Call EventQueueNext (delays until buffer closes)
 *    - FALSE: Immediately consume the buffer
 */
void UAC_InputBuffer::QueueAction_Implementation(const FGameplayTag& QueuedActionTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InputBuffer::QueueAction - Action: %s, BufferOpen: %s"),
		*QueuedActionTag.ToString(), BufferOpen ? TEXT("true") : TEXT("false"));

	// Store the incoming action
	IncomingActionTag = QueuedActionTag;

	// Branch based on buffer state
	if (BufferOpen)
	{
		// Buffer is open - call EventQueueNext to wait for buffer to close
		EventQueueNext(QueuedActionTag);
	}
	else
	{
		// Buffer is closed - consume immediately
		ConsumeInputBuffer();
	}
}

/**
 * ConsumeInputBuffer - Consume the buffered action
 *
 * Blueprint Logic:
 * 1. Broadcast OnInputBufferConsumed with IncomingActionTag
 * 2. Reset IncomingActionTag to empty
 *
 * Comment: "On consumed, resets Incoming Action tag.
 *          The dispatcher is utilised inside B_SoulslikeCharacter."
 */
void UAC_InputBuffer::ConsumeInputBuffer_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InputBuffer::ConsumeInputBuffer - Action: %s"),
		*IncomingActionTag.ToString());

	// Broadcast the consumed action
	OnInputBufferConsumed.Broadcast(IncomingActionTag);

	// Reset the incoming action tag
	IncomingActionTag = FGameplayTag();
}

/**
 * ToggleBuffer - Toggle the buffer state on/off
 *
 * Blueprint Logic:
 * - Set BufferOpen = InBufferOpen
 *
 * Comment: "Toggle buffer on/off"
 */
void UAC_InputBuffer::ToggleBuffer_Implementation(bool InBufferOpen)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InputBuffer::ToggleBuffer - BufferOpen: %s -> %s"),
		BufferOpen ? TEXT("true") : TEXT("false"),
		InBufferOpen ? TEXT("true") : TEXT("false"));

	BufferOpen = InBufferOpen;
}

/**
 * ExecuteActionImmediately - Execute an action bypassing the buffer
 *
 * Blueprint Logic:
 * 1. Set IncomingActionTag = Action
 * 2. Call ConsumeInputBuffer()
 *
 * Comment: "Immediately consumes buffer."
 */
void UAC_InputBuffer::ExecuteActionImmediately_Implementation(const FGameplayTag& Action)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InputBuffer::ExecuteActionImmediately - Action: %s"),
		*Action.ToString());

	// Set the action and immediately consume
	IncomingActionTag = Action;
	ConsumeInputBuffer();
}

/**
 * EventQueueNext - Custom event that waits for buffer to close
 *
 * Blueprint Logic (from JSON graph analysis):
 * 1. Check if ActionTag is in IgnoreNextOfActions (using HasTag)
 * 2. If NOT in ignore list (the NOT condition inverts HasTag result):
 *    - Branch on BufferOpen
 *    - If BufferOpen is TRUE: DelayUntilNextTick, then loop back (recursive)
 *    - If BufferOpen is FALSE: Set IncomingActionTag = ActionTag, call ConsumeInputBuffer
 * 3. If ActionTag IS in ignore list: do nothing (action is ignored)
 */
void UAC_InputBuffer::EventQueueNext(const FGameplayTag& ActionTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InputBuffer::EventQueueNext - Action: %s"), *ActionTag.ToString());

	// Step 1: Check if this action should be ignored
	// HasTag returns true if ActionTag is in IgnoreNextOfActions
	const bool bShouldIgnore = IgnoreNextOfActions.HasTag(ActionTag);

	// Step 2: The Blueprint uses a NOT condition - proceed only if NOT ignored
	if (!bShouldIgnore)
	{
		// Step 3: Check buffer state
		if (BufferOpen)
		{
			// Buffer is still open - DelayUntilNextTick, then loop back
			UE_LOG(LogTemp, Log, TEXT("  Buffer still open, waiting..."));

			// Store the action for the timer callback
			PendingQueueNextAction = ActionTag;

			// Clear any existing timer
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().ClearTimer(QueueNextTimerHandle);

				// Set timer for next tick (DelayUntilNextTick equivalent)
				GetWorld()->GetTimerManager().SetTimerForNextTick(
					FTimerDelegate::CreateUObject(this, &UAC_InputBuffer::OnQueueNextTimerElapsed)
				);
			}
		}
		else
		{
			// Buffer is closed - set the action and consume
			UE_LOG(LogTemp, Log, TEXT("  Buffer closed, consuming action"));
			IncomingActionTag = ActionTag;
			ConsumeInputBuffer();
		}
	}
	else
	{
		// Action is in ignore list - do nothing
		UE_LOG(LogTemp, Log, TEXT("  Action is in IgnoreNextOfActions list, skipping"));
	}
}

/**
 * Timer callback for the delay loop in EventQueueNext
 * This is called after DelayUntilNextTick and loops back to the BufferOpen check
 */
void UAC_InputBuffer::OnQueueNextTimerElapsed()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InputBuffer::OnQueueNextTimerElapsed - PendingAction: %s"),
		*PendingQueueNextAction.ToString());

	// Recursive call back to EventQueueNext with the pending action
	// This loops back to check BufferOpen again (matching the Blueprint loop)
	if (PendingQueueNextAction.IsValid())
	{
		EventQueueNext(PendingQueueNextAction);
	}
}
