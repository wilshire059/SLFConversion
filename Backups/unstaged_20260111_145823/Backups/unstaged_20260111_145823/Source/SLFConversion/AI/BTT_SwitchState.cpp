// BTT_SwitchState.cpp
// C++ AI implementation for BTT_SwitchState
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - switches AI state via AI_BehaviorManager

#include "AI/BTT_SwitchState.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AIBehaviorManagerComponent.h"

UBTT_SwitchState::UBTT_SwitchState()
{
	NodeName = TEXT("Switch State");
}

EBTNodeResult::Type UBTT_SwitchState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	// From Blueprint: Get AI_BehaviorManager and call SetState
	// NOTE: Blueprint always succeeds even if component is not valid
	UAIBehaviorManagerComponent* BehaviorManager = ControlledPawn->FindComponentByClass<UAIBehaviorManagerComponent>();
	if (BehaviorManager)
	{
		BehaviorManager->SetState(NewState);
		UE_LOG(LogTemp, Log, TEXT("UBTT_SwitchState::ExecuteTask - Set state %d on %s"), (int32)NewState, *ControlledPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTT_SwitchState::ExecuteTask - No AI_BehaviorManager on %s, succeeding anyway"), *ControlledPawn->GetName());
	}

	// Always succeed per Blueprint logic (both IsValid paths lead to FinishExecute(true))
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_SwitchState::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
