// BTS_ChaseBounds.cpp
// C++ AI implementation for BTS_ChaseBounds
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - checks if AI is within chase bounds

#include "AI/BTS_ChaseBounds.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AC_AI_BehaviorManager.h"

UBTS_ChaseBounds::UBTS_ChaseBounds()
	: InverseCondition(false)
{
	NodeName = TEXT("Chase Bounds");
}

void UBTS_ChaseBounds::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	// From Blueprint: Get start position and chase distance from blackboard
	FVector StartPosition = Blackboard->GetValueAsVector(StartPositionKey.SelectedKeyName);
	float ChaseDistance = Blackboard->GetValueAsFloat(ChaseDistanceKey.SelectedKeyName);

	// From Blueprint: Calculate distance from start position
	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	double DistanceFromStart = FVector::Dist(CurrentLocation, StartPosition);

	// Get BehaviorManager for state changes
	UAC_AI_BehaviorManager* BehaviorManager = ControlledPawn->FindComponentByClass<UAC_AI_BehaviorManager>();

	// From Blueprint: Branch on "If out of bounds" (distance > chaseDistance)
	if (DistanceFromStart > ChaseDistance)
	{
		// TRUE path: Out of bounds
		// Set blackboard enum to the configured State
		Blackboard->SetValueAsEnum(StateKey.SelectedKeyName, static_cast<uint8>(State));

		// Then call SetState with Investigating (NewEnumerator3 = index 3)
		if (BehaviorManager)
		{
			BehaviorManager->SetState(ESLFAIStates::Investigating, FInstancedStruct());
			UE_LOG(LogTemp, Log, TEXT("UBTS_ChaseBounds::TickNode - Out of bounds, switching to Investigating on %s"), *ControlledPawn->GetName());
		}
	}
	else
	{
		// FALSE path: In bounds
		// Branch on InverseCondition
		if (InverseCondition)
		{
			// TRUE path: Call SetState with Patrolling (NewEnumerator2 = index 2)
			if (BehaviorManager)
			{
				BehaviorManager->SetState(ESLFAIStates::Patrolling, FInstancedStruct());
				UE_LOG(LogTemp, Log, TEXT("UBTS_ChaseBounds::TickNode - InverseCondition, switching to Patrolling on %s"), *ControlledPawn->GetName());
			}
		}
		// FALSE path: Do nothing
	}
}
