// BTS_ChaseBounds.cpp
// C++ AI implementation for BTS_ChaseBounds
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - checks if AI is within chase bounds

#include "AI/BTS_ChaseBounds.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AIBehaviorManagerComponent.h"

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
	UAIBehaviorManagerComponent* BehaviorManager = ControlledPawn->FindComponentByClass<UAIBehaviorManagerComponent>();

	// DEBUG: Log chase bounds check
	static int32 DebugCounter = 0;
	if (DebugCounter++ % 60 == 0) // Log once per ~60 ticks
	{
		UE_LOG(LogTemp, Warning, TEXT("[BTS_ChaseBounds] %s: StartPos=%s, ChaseDistance=%.1f, CurrentPos=%s, DistFromStart=%.1f"),
			*ControlledPawn->GetName(),
			*StartPosition.ToString(),
			ChaseDistance,
			*CurrentLocation.ToString(),
			DistanceFromStart);
	}

	// From Blueprint: Branch on "If out of bounds" (distance > chaseDistance)
	if (DistanceFromStart > ChaseDistance)
	{
		// TRUE path: Out of bounds
		UE_LOG(LogTemp, Warning, TEXT("[BTS_CHASEBOUNDS_OOB] %s OUT OF BOUNDS! Dist=%.1f > Chase=%.1f"),
			*ControlledPawn->GetName(), DistanceFromStart, ChaseDistance);

		// Use the configured State (usually OutOfBounds or Investigating)
		ESLFAIStates TargetState = State;

		// Set blackboard enum to the configured State
		Blackboard->SetValueAsEnum(StateKey.SelectedKeyName, static_cast<uint8>(TargetState));

		// CRITICAL: Clear InCombat so BT_Combat stops executing
		Blackboard->SetValueAsBool(FName("InCombat"), false);

		// Also clear Target so combat logic doesn't try to chase
		Blackboard->ClearValue(FName("Target"));

		// Call SetState with the configured state
		if (BehaviorManager)
		{
			BehaviorManager->SetState(TargetState);
			BehaviorManager->SetTarget(nullptr);
		}

		UE_LOG(LogTemp, Warning, TEXT("[BTS_CHASEBOUNDS_OOB] %s state changed to %d, InCombat=false, Target=null"),
			*ControlledPawn->GetName(), static_cast<int32>(TargetState));
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
				BehaviorManager->SetState(ESLFAIStates::Patrolling);
				UE_LOG(LogTemp, Log, TEXT("UBTS_ChaseBounds::TickNode - InverseCondition, switching to Patrolling on %s"), *ControlledPawn->GetName());
			}
		}
		// FALSE path: Do nothing
	}
}
