// SLFBTTaskGetRandomPoint.cpp
#include "SLFBTTaskGetRandomPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

USLFBTTaskGetRandomPoint::USLFBTTaskGetRandomPoint()
{
	NodeName = "Get Random Point";
}

EBTNodeResult::Type USLFBTTaskGetRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	FNavLocation RandomLocation;
	if (NavSys->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), Radius, RandomLocation))
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		if (Blackboard)
		{
			Blackboard->SetValueAsVector(OutputKey.SelectedKeyName, RandomLocation.Location);
			UE_LOG(LogTemp, Log, TEXT("[BTT_GetRandomPoint] Found: %s"), *RandomLocation.Location.ToString());
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
