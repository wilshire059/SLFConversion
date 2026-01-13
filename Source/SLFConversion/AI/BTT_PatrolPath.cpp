// BTT_PatrolPath.cpp
// C++ AI implementation for BTT_PatrolPath
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - follows patrol path points

#include "AI/BTT_PatrolPath.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/AIBehaviorManagerComponent.h"

UBTT_PatrolPath::UBTT_PatrolPath()
{
	NodeName = TEXT("Patrol Path");
}

EBTNodeResult::Type UBTT_PatrolPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	// From Blueprint: Get AI_BehaviorManager and check patrol path
	UAIBehaviorManagerComponent* BehaviorManager = ControlledPawn->FindComponentByClass<UAIBehaviorManagerComponent>();
	if (BehaviorManager && BehaviorManager->PatrolPath)
	{
		// Patrol path exists - use blackboard to get current patrol point
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		if (Blackboard)
		{
			FVector PatrolPoint = Blackboard->GetValueAsVector(FName("PatrolPoint"));
			EPathFollowingRequestResult::Type Result = AIController->MoveToLocation(PatrolPoint);

			if (Result == EPathFollowingRequestResult::RequestSuccessful)
			{
				UE_LOG(LogTemp, Log, TEXT("UBTT_PatrolPath::ExecuteTask - Patrolling to %s"), *PatrolPoint.ToString());
				return EBTNodeResult::InProgress;
			}
			else if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTT_PatrolPath::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AIController->StopMovement();
	}
	return EBTNodeResult::Aborted;
}
