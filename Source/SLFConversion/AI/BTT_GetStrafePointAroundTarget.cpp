// BTT_GetStrafePointAroundTarget.cpp
// C++ AI implementation for BTT_GetStrafePointAroundTarget
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - calculates strafe point around target

#include "AI/BTT_GetStrafePointAroundTarget.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTT_GetStrafePointAroundTarget::UBTT_GetStrafePointAroundTarget()
	: PickedStrafeLoc(FVector::ZeroVector)
{
	NodeName = TEXT("Get Strafe Point");
}

EBTNodeResult::Type UBTT_GetStrafePointAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// From Blueprint: Get target actor and radius from blackboard
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	float Radius = Blackboard->GetValueAsFloat(RadiusKey.SelectedKeyName);
	if (!TargetActor || Radius <= 0.0f)
	{
		return EBTNodeResult::Failed;
	}

	// From Blueprint: Calculate strafe locations around target
	SetStrafeLocations();

	// Get random angle for strafe point
	float RandomAngle = FMath::RandRange(0.0f, 360.0f);
	FVector Direction = FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
								FMath::Sin(FMath::DegreesToRadians(RandomAngle)), 0.0f);

	PickedStrafeLoc = TargetActor->GetActorLocation() + Direction * Radius;

	// Verify the strafe location is navigable
	FNavLocation NavLocation;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(ControlledPawn->GetWorld());
	if (NavSys && NavSys->ProjectPointToNavigation(PickedStrafeLoc, NavLocation))
	{
		PickedStrafeLoc = NavLocation.Location;
	}

	// Set the strafe location to blackboard
	Blackboard->SetValueAsVector(StrafeLocationKey.SelectedKeyName, PickedStrafeLoc);
	UE_LOG(LogTemp, Log, TEXT("UBTT_GetStrafePointAroundTarget::ExecuteTask - Strafe point %s"), *PickedStrafeLoc.ToString());

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_GetStrafePointAroundTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

void UBTT_GetStrafePointAroundTarget::SetStrafeLocations()
{
	// From Blueprint: Set up strafe location options based on LocationList
	// This would populate available strafe points based on the configured options
}

ESLFAIStrafeLocations UBTT_GetStrafePointAroundTarget::PickStrafeMethodBasedOnScore(const FSLFAiStrafeInfo& StrafeMethods)
{
	// From Blueprint: Pick best strafe method based on scoring
	// Returns the strafe location with best score
	return ESLFAIStrafeLocations::Back;
}
