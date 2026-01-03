// BTS_DistanceCheck.cpp
// C++ AI implementation for BTS_DistanceCheck
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - checks distance to target and sets bool key

#include "AI/BTS_DistanceCheck.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_DistanceCheck::UBTS_DistanceCheck()
{
	NodeName = TEXT("Distance Check");
}

void UBTS_DistanceCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn)
	{
		return;
	}

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return;
	}

	// From Blueprint: Get target actor from blackboard
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!TargetActor)
	{
		return;
	}

	// From Blueprint: Get distance threshold from blackboard
	float DistanceThreshold = Blackboard->GetValueAsFloat(DistanceKey.SelectedKeyName);

	// From Blueprint: Calculate actual distance to target
	float ActualDistance = ControlledPawn->GetDistanceTo(TargetActor);

	// From Blueprint: Compare based on operation type and set result
	bool bResult = false;
	switch (CompareType)
	{
		case ESLFCompareOperation::LessThan:
			bResult = ActualDistance < DistanceThreshold;
			break;
		case ESLFCompareOperation::LessThanOrEqual:
			bResult = ActualDistance <= DistanceThreshold;
			break;
		case ESLFCompareOperation::Equal:
			bResult = FMath::IsNearlyEqual(ActualDistance, DistanceThreshold, 1.0f);
			break;
		case ESLFCompareOperation::GreaterThan:
			bResult = ActualDistance > DistanceThreshold;
			break;
		case ESLFCompareOperation::GreaterThanOrEqual:
			bResult = ActualDistance >= DistanceThreshold;
			break;
	}

	// Set the result to the specified key
	Blackboard->SetValueAsBool(KeyToSet.SelectedKeyName, bResult);
}
