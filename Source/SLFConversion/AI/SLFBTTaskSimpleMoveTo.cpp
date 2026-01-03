// SLFBTTaskSimpleMoveTo.cpp
#include "SLFBTTaskSimpleMoveTo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

USLFBTTaskSimpleMoveTo::USLFBTTaskSimpleMoveTo()
{
	NodeName = "Simple Move To";
}

EBTNodeResult::Type USLFBTTaskSimpleMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	FVector TargetLocation = Blackboard->GetValueAsVector(TargetKey.SelectedKeyName);
	float Radius = Blackboard->GetValueAsFloat(RadiusKey.SelectedKeyName);

	UE_LOG(LogTemp, Log, TEXT("[BTT_SimpleMoveTo] Moving to %s with radius %.0f"),
		*TargetLocation.ToString(), Radius);

	EPathFollowingRequestResult::Type Result = AIController->MoveToLocation(TargetLocation, Radius);

	return Result == EPathFollowingRequestResult::RequestSuccessful ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

FString USLFBTTaskSimpleMoveTo::GetStaticDescription() const
{
	return FString::Printf(TEXT("Move to %s"), *TargetKey.SelectedKeyName.ToString());
}
