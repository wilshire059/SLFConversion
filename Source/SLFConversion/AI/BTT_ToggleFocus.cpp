// BTT_ToggleFocus.cpp
// C++ AI implementation for BTT_ToggleFocus
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - toggles AI focus on target actor
//
// BLUEPRINT LOGIC:
// 1. Branch on Focus? variable
// 2. If Focus = TRUE:
//    - Check if GetFocusActor is valid (already has focus)
//    - If valid: FinishExecute(true) - already focused, do nothing
//    - If NOT valid: SetFocus to target from blackboard → RotateTowardsTarget(0.25) → FinishExecute(true)
// 3. If Focus = FALSE:
//    - Check if GetFocusActor is valid
//    - If valid: ClearFocus → FinishExecute(true)
//    - If NOT valid: FinishExecute(true) - no focus to clear

#include "AI/BTT_ToggleFocus.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/BPI_Enemy.h"

UBTT_ToggleFocus::UBTT_ToggleFocus()
{
	NodeName = TEXT("Toggle Focus");
}

EBTNodeResult::Type UBTT_ToggleFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTT_ToggleFocus::ExecuteTask - No AIController"));
		return EBTNodeResult::Succeeded; // Always succeed per Blueprint
	}

	APawn* ControlledPawn = AIController->GetPawn();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTT_ToggleFocus::ExecuteTask - No blackboard"));
		return EBTNodeResult::Succeeded;
	}

	// Get current focus actor
	AActor* CurrentFocusActor = AIController->GetFocusActor();

	if (Focus)
	{
		// Focus = TRUE path
		if (IsValid(CurrentFocusActor))
		{
			// Already has focus - do nothing, succeed
			UE_LOG(LogTemp, Log, TEXT("UBTT_ToggleFocus::ExecuteTask - Already has focus on %s"), *CurrentFocusActor->GetName());
		}
		else
		{
			// No current focus - set focus to target from blackboard
			AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
			if (TargetActor)
			{
				AIController->SetFocus(TargetActor);
				UE_LOG(LogTemp, Log, TEXT("UBTT_ToggleFocus::ExecuteTask - Set focus on %s"), *TargetActor->GetName());

				// Call RotateTowardsTarget on ControlledPawn via BPI_Enemy interface
				// Duration 0.25 from Blueprint default
				if (ControlledPawn && ControlledPawn->GetClass()->ImplementsInterface(UBPI_Enemy::StaticClass()))
				{
					IBPI_Enemy::Execute_RotateTowardsTarget(ControlledPawn, 0.25);
					UE_LOG(LogTemp, Log, TEXT("UBTT_ToggleFocus::ExecuteTask - RotateTowardsTarget called with duration 0.25"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UBTT_ToggleFocus::ExecuteTask - No valid target actor from blackboard"));
			}
		}
	}
	else
	{
		// Focus = FALSE path
		if (IsValid(CurrentFocusActor))
		{
			// Has focus - clear it
			AIController->ClearFocus(EAIFocusPriority::Gameplay);
			UE_LOG(LogTemp, Log, TEXT("UBTT_ToggleFocus::ExecuteTask - Cleared focus"));
		}
		else
		{
			// No focus to clear - do nothing
			UE_LOG(LogTemp, Log, TEXT("UBTT_ToggleFocus::ExecuteTask - No focus to clear"));
		}
	}

	// All paths lead to FinishExecute(true)
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_ToggleFocus::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
