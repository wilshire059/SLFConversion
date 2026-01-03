// BTT_SimpleMoveTo.cpp
// C++ AI implementation for BTT_SimpleMoveTo
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - simple AI move to location
//
// BLUEPRINT LOGIC:
// 1. Get TargetKey as Actor
// 2. If actor valid: AI MoveTo with TargetActor + AcceptanceRadius from RadiusKey
// 3. If actor NOT valid: AI MoveTo with Destination (TargetKey as Vector) + AcceptanceRadius
// 4. Both success AND fail paths call FinishExecute(true) - always succeeds

#include "AI/BTT_SimpleMoveTo.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTT_SimpleMoveTo::UBTT_SimpleMoveTo()
{
	NodeName = TEXT("Simple Move To");
}

EBTNodeResult::Type UBTT_SimpleMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn || !AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTT_SimpleMoveTo::ExecuteTask - No pawn or controller"));
		return EBTNodeResult::Succeeded; // Always succeed per Blueprint design
	}

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTT_SimpleMoveTo::ExecuteTask - No blackboard"));
		return EBTNodeResult::Succeeded; // Always succeed
	}

	// Cache reference for async completion
	CachedOwnerComp = &OwnerComp;

	// Get acceptance radius from blackboard
	float AcceptanceRadius = Blackboard->GetValueAsFloat(RadiusKey.SelectedKeyName);
	if (AcceptanceRadius <= 0.0f)
	{
		AcceptanceRadius = 5.0f; // Default from Blueprint
	}

	// Bind to move completed delegate
	UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
	if (PathComp)
	{
		MoveCompletedDelegateHandle = PathComp->OnRequestFinished.AddUObject(this, &UBTT_SimpleMoveTo::OnMoveCompleted);
	}

	// Try to get TargetKey as Actor first (Blueprint logic: checks IsValid on actor)
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	EPathFollowingRequestResult::Type Result;

	if (IsValid(TargetActor))
	{
		// Actor is valid - move to actor
		UE_LOG(LogTemp, Log, TEXT("UBTT_SimpleMoveTo::ExecuteTask - Moving to actor %s"), *TargetActor->GetName());
		Result = AIController->MoveToActor(
			TargetActor,
			AcceptanceRadius,
			false, // bStopOnOverlap = false per Blueprint
			true,  // Use pathfinding
			false, // bCanStrafe
			TSubclassOf<UNavigationQueryFilter>(),
			true   // bAllowPartialPath
		);
	}
	else
	{
		// Actor is NOT valid - use as vector destination
		FVector TargetLocation = Blackboard->GetValueAsVector(TargetKey.SelectedKeyName);
		UE_LOG(LogTemp, Log, TEXT("UBTT_SimpleMoveTo::ExecuteTask - Moving to location %s"), *TargetLocation.ToString());
		Result = AIController->MoveToLocation(
			TargetLocation,
			AcceptanceRadius,
			false, // bStopOnOverlap = false per Blueprint
			true,  // Use pathfinding
			false, // bCanStrafe
			true,  // bAllowPartialPath
			TSubclassOf<UNavigationQueryFilter>(),
			false  // bProjectDestinationToNavigation
		);
	}

	if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		// Already at destination - cleanup and succeed immediately
		CleanupMoveDelegate();
		UE_LOG(LogTemp, Log, TEXT("UBTT_SimpleMoveTo::ExecuteTask - Already at goal"));
		return EBTNodeResult::Succeeded;
	}
	else if (Result == EPathFollowingRequestResult::RequestSuccessful)
	{
		// Move in progress - wait for completion
		return EBTNodeResult::InProgress;
	}
	else
	{
		// Request failed - but we always succeed per Blueprint design
		CleanupMoveDelegate();
		UE_LOG(LogTemp, Log, TEXT("UBTT_SimpleMoveTo::ExecuteTask - Move request failed, returning success anyway"));
		return EBTNodeResult::Succeeded;
	}
}

void UBTT_SimpleMoveTo::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	UE_LOG(LogTemp, Log, TEXT("UBTT_SimpleMoveTo::OnMoveCompleted - Result: %s"),
		Result.IsSuccess() ? TEXT("Success") : TEXT("Failed"));

	// Cleanup delegate binding
	CleanupMoveDelegate();

	// Always succeed regardless of actual move result (per Blueprint design)
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
	}
}

void UBTT_SimpleMoveTo::CleanupMoveDelegate()
{
	if (CachedOwnerComp.IsValid())
	{
		if (AAIController* AIController = CachedOwnerComp->GetAIOwner())
		{
			if (UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
			{
				PathComp->OnRequestFinished.Remove(MoveCompletedDelegateHandle);
			}
		}
	}
	MoveCompletedDelegateHandle.Reset();
}

EBTNodeResult::Type UBTT_SimpleMoveTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Log, TEXT("UBTT_SimpleMoveTo::AbortTask - Aborting task"));

	// Cleanup delegate binding
	CleanupMoveDelegate();

	// Stop movement
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		AIController->StopMovement();
	}

	return EBTNodeResult::Aborted;
}
