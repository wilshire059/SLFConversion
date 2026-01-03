// SLFBTServiceDistanceCheck.cpp
#include "SLFBTServiceDistanceCheck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USLFBTServiceDistanceCheck::USLFBTServiceDistanceCheck()
{
	NodeName = "Distance Check";
	Interval = 0.1f;
}

void USLFBTServiceDistanceCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;

	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!Target) return;

	float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
	Blackboard->SetValueAsFloat(DistanceOutputKey.SelectedKeyName, Distance);
}
