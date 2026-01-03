// BTS_SetMovementModeBasedOnDistance.cpp
// C++ AI implementation for BTS_SetMovementModeBasedOnDistance
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - sets movement mode based on distance to target

#include "AI/BTS_SetMovementModeBasedOnDistance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"

UBTS_SetMovementModeBasedOnDistance::UBTS_SetMovementModeBasedOnDistance()
{
	NodeName = TEXT("Set Movement Mode Based On Distance");
}

void UBTS_SetMovementModeBasedOnDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	// From Blueprint: Get target actor and distance threshold from blackboard
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	float DistanceThreshold = Blackboard->GetValueAsFloat(DistanceKey.SelectedKeyName);

	if (!TargetActor)
	{
		return;
	}

	// From Blueprint: Calculate distance to target
	float DistanceToTarget = ControlledPawn->GetDistanceTo(TargetActor);

	// From Blueprint: Set movement mode based on distance
	// If close to target, walk; if far, run
	ESLFMovementType MovementMode = (DistanceToTarget <= DistanceThreshold) ?
		ESLFMovementType::Walk : ESLFMovementType::Run;

	// From Blueprint: Apply movement mode via interface
	if (ControlledPawn->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SetMovementMode(ControlledPawn, MovementMode);
	}
}
