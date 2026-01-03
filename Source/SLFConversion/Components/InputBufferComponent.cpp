// InputBufferComponent.cpp
// C++ implementation for AC_InputBuffer
//
// CRITICAL LOGIC NOTES:
// - BufferOpen = true means we're in an action (attack/dodge), queue for later
// - BufferOpen = false means ready for new action, execute immediately
// - This was a major debugging lesson - verify from JSON before implementing!

#include "InputBufferComponent.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogInputBuffer, Log, All);

UInputBufferComponent::UInputBufferComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bBufferOpen = false;
}

void UInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	UE_LOG(LogInputBuffer, Warning, TEXT("═══════════════════════════════════════════════════════════════"));
	UE_LOG(LogInputBuffer, Warning, TEXT("[InputBuffer] BeginPlay on %s"), Owner ? *Owner->GetName() : TEXT("NULL OWNER"));
	UE_LOG(LogInputBuffer, Warning, TEXT("[InputBuffer] Component: %s"), *GetName());
	UE_LOG(LogInputBuffer, Warning, TEXT("[InputBuffer] Class: %s"), *GetClass()->GetName());
	UE_LOG(LogInputBuffer, Warning, TEXT("[InputBuffer] bBufferOpen: %s"), bBufferOpen ? TEXT("true") : TEXT("false"));
	UE_LOG(LogInputBuffer, Warning, TEXT("═══════════════════════════════════════════════════════════════"));
}

void UInputBufferComponent::QueueAction_Implementation(FGameplayTag QueuedActionTag)
{
	UE_LOG(LogInputBuffer, Verbose, TEXT("QueueAction called with tag: %s, BufferOpen: %s"),
		*QueuedActionTag.ToString(),
		bBufferOpen ? TEXT("true") : TEXT("false"));

	if (bBufferOpen)
	{
		// Buffer is open (during animation) - queue for later
		UE_LOG(LogInputBuffer, Verbose, TEXT("  -> Queuing action for later (buffer open)"));
		QueueNext(QueuedActionTag);
	}
	else
	{
		// Buffer is closed (ready) - execute immediately
		UE_LOG(LogInputBuffer, Verbose, TEXT("  -> Executing immediately (buffer closed)"));
		ConsumeInputBuffer();
	}
}

void UInputBufferComponent::ToggleBuffer_Implementation(bool BufferOpen)
{
	UE_LOG(LogInputBuffer, Verbose, TEXT("ToggleBuffer: %s -> %s"),
		bBufferOpen ? TEXT("open") : TEXT("closed"),
		BufferOpen ? TEXT("open") : TEXT("closed"));

	bBufferOpen = BufferOpen;

	// When buffer closes, process any queued action
	if (!BufferOpen && IncomingActionTag.IsValid())
	{
		UE_LOG(LogInputBuffer, Verbose, TEXT("  -> Buffer closing with queued action, consuming"));
		ConsumeInputBuffer();
	}
}

void UInputBufferComponent::ConsumeInputBuffer_Implementation()
{
	if (IncomingActionTag.IsValid())
	{
		UE_LOG(LogInputBuffer, Log, TEXT("ConsumeInputBuffer: Broadcasting action %s"), *IncomingActionTag.ToString());

		FGameplayTag ActionToExecute = IncomingActionTag;
		IncomingActionTag = FGameplayTag();  // Clear first to prevent re-entry issues

		OnInputBufferConsumed.Broadcast(ActionToExecute);
	}
	else
	{
		UE_LOG(LogInputBuffer, Verbose, TEXT("ConsumeInputBuffer: No action queued"));
	}
}

void UInputBufferComponent::ExecuteActionImmediately_Implementation(FGameplayTag Action)
{
	UE_LOG(LogInputBuffer, Log, TEXT("ExecuteActionImmediately: %s"), *Action.ToString());

	IncomingActionTag = Action;
	ConsumeInputBuffer();
}

void UInputBufferComponent::QueueNext_Implementation(FGameplayTag ActionTag)
{
	// Check if this action should be ignored
	if (IgnoreNextOfActions.HasTag(ActionTag))
	{
		UE_LOG(LogInputBuffer, Verbose, TEXT("QueueNext: Ignoring action %s (in ignore list)"), *ActionTag.ToString());
		return;
	}

	UE_LOG(LogInputBuffer, Verbose, TEXT("QueueNext: Storing action %s"), *ActionTag.ToString());
	IncomingActionTag = ActionTag;

	// Delay until next tick to allow animation notify to complete
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick([WeakThis = TWeakObjectPtr<UInputBufferComponent>(this)]()
		{
			if (UInputBufferComponent* This = WeakThis.Get())
			{
				if (!This->bBufferOpen && This->IncomingActionTag.IsValid())
				{
					UE_LOG(LogInputBuffer, Verbose, TEXT("QueueNext (next tick): Buffer now closed, consuming"));
					This->ConsumeInputBuffer();
				}
			}
		});
	}
}
