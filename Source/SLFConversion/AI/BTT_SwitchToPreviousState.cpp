// BTT_SwitchToPreviousState.cpp
// C++ AI implementation for BTT_SwitchToPreviousState
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - switches AI back to previous state

#include "AI/BTT_SwitchToPreviousState.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AIBehaviorManagerComponent.h"

UBTT_SwitchToPreviousState::UBTT_SwitchToPreviousState()
{
	NodeName = TEXT("Switch To Previous State");
}

EBTNodeResult::Type UBTT_SwitchToPreviousState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	// From Blueprint: Get AI_BehaviorManager and call SwitchToPreviousState
	// NOTE: Blueprint always succeeds even if component is not valid
	UAIBehaviorManagerComponent* BehaviorManager = ControlledPawn->FindComponentByClass<UAIBehaviorManagerComponent>();
	if (BehaviorManager)
	{
		// Use SetState with PreviousState property
		BehaviorManager->SetState(BehaviorManager->PreviousState);
		UE_LOG(LogTemp, Log, TEXT("UBTT_SwitchToPreviousState::ExecuteTask - Switched to previous state on %s"), *ControlledPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTT_SwitchToPreviousState::ExecuteTask - No AI_BehaviorManager on %s, succeeding anyway"), *ControlledPawn->GetName());
	}

	// Always succeed per Blueprint logic (both IsValid paths lead to FinishExecute(true))
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_SwitchToPreviousState::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
