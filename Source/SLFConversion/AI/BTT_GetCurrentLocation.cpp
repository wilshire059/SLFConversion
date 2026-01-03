// BTT_GetCurrentLocation.cpp
// C++ AI implementation for BTT_GetCurrentLocation
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - gets pawn location and stores to blackboard

#include "AI/BTT_GetCurrentLocation.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_GetCurrentLocation::UBTT_GetCurrentLocation()
{
	NodeName = TEXT("Get Current Location");
}

EBTNodeResult::Type UBTT_GetCurrentLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// From Blueprint: Get pawn location and set to blackboard key
	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	Blackboard->SetValueAsVector(CurrentLocationKey.SelectedKeyName, CurrentLocation);

	UE_LOG(LogTemp, Log, TEXT("UBTT_GetCurrentLocation::ExecuteTask - Set location %s to key %s"),
		*CurrentLocation.ToString(), *CurrentLocationKey.SelectedKeyName.ToString());

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_GetCurrentLocation::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
