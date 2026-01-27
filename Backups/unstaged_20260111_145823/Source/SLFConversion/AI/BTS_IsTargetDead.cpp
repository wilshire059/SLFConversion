// BTS_IsTargetDead.cpp
// C++ AI implementation for BTS_IsTargetDead
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - checks if target is dead and switches state

#include "AI/BTS_IsTargetDead.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AIBehaviorManagerComponent.h"
#include "Components/AC_CombatManager.h"

UBTS_IsTargetDead::UBTS_IsTargetDead()
{
	NodeName = TEXT("Is Target Dead");
}

void UBTS_IsTargetDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	// From Blueprint: Get AC_CombatManager from target actor and check IsDead? property
	UAC_CombatManager* CombatManager = TargetActor->FindComponentByClass<UAC_CombatManager>();
	if (!CombatManager)
	{
		return;
	}

	// From Blueprint: Check if target is dead (property name is "IsDead" without "b" prefix)
	bool bIsTargetDead = CombatManager->IsDead;

	// From Blueprint: If target is dead, switch to specified state
	if (bIsTargetDead)
	{
		UAIBehaviorManagerComponent* BehaviorManager = ControlledPawn->FindComponentByClass<UAIBehaviorManagerComponent>();
		if (BehaviorManager)
		{
			BehaviorManager->SetState(StateToSwitchTo);
			UE_LOG(LogTemp, Log, TEXT("UBTS_IsTargetDead::TickNode - Target dead, switching state on %s"), *ControlledPawn->GetName());
		}
	}
}
