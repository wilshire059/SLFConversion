// BTT_GetRandomPointNearStartPosition.cpp
// C++ AI implementation for BTT_GetRandomPointNearStartPosition
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - gets random navigable point near start position

#include "AI/BTT_GetRandomPointNearStartPosition.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTT_GetRandomPointNearStartPosition::UBTT_GetRandomPointNearStartPosition()
{
	NodeName = TEXT("Get Random Point Near Start");
}

EBTNodeResult::Type UBTT_GetRandomPointNearStartPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	// From Blueprint: Get start position from blackboard
	FVector StartPosition = Blackboard->GetValueAsVector(StartPositionKey.SelectedKeyName);

	// From Blueprint: Get random navigable point within radius of start position
	FNavLocation NavLocation;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(ControlledPawn->GetWorld());
	if (NavSys && NavSys->GetRandomReachablePointInRadius(StartPosition, Radius, NavLocation))
	{
		Blackboard->SetValueAsVector(StartPositionKey.SelectedKeyName, NavLocation.Location);
		UE_LOG(LogTemp, Log, TEXT("UBTT_GetRandomPointNearStartPosition::ExecuteTask - Set random point %s near start"),
			*NavLocation.Location.ToString());
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTT_GetRandomPointNearStartPosition::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
