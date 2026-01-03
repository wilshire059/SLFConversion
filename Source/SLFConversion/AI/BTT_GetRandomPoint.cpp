// BTT_GetRandomPoint.cpp
// C++ AI implementation for BTT_GetRandomPoint
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - gets random navigable point within radius

#include "AI/BTT_GetRandomPoint.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTT_GetRandomPoint::UBTT_GetRandomPoint()
{
	NodeName = TEXT("Get Random Point");
}

EBTNodeResult::Type UBTT_GetRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// From Blueprint: Get random navigable point within radius of pawn
	FNavLocation NavLocation;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(ControlledPawn->GetWorld());
	if (NavSys && NavSys->GetRandomReachablePointInRadius(ControlledPawn->GetActorLocation(), Radius, NavLocation))
	{
		Blackboard->SetValueAsVector(TargetKey.SelectedKeyName, NavLocation.Location);
		UE_LOG(LogTemp, Log, TEXT("UBTT_GetRandomPoint::ExecuteTask - Set random point %s"), *NavLocation.Location.ToString());
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTT_GetRandomPoint::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
