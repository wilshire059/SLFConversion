// BTT_ClearKey.cpp
// C++ AI implementation for BTT_ClearKey
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - clears a blackboard key value

#include "AI/BTT_ClearKey.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_ClearKey::UBTT_ClearKey()
{
	NodeName = TEXT("Clear Key");
}

EBTNodeResult::Type UBTT_ClearKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	// From Blueprint: Clear the blackboard value for the specified key
	Blackboard->ClearValue(Key.SelectedKeyName);

	UE_LOG(LogTemp, Log, TEXT("UBTT_ClearKey::ExecuteTask - Cleared key %s"), *Key.SelectedKeyName.ToString());

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_ClearKey::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
