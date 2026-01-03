// SLFBTServiceIsTargetDead.cpp
// C++ AI implementation for SLFBTServiceIsTargetDead
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated - checks if target is dead

#include "SLFBTServiceIsTargetDead.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CombatManagerComponent.h"
#include "Components/AICombatManagerComponent.h"

USLFBTServiceIsTargetDead::USLFBTServiceIsTargetDead()
{
	NodeName = "Is Target Dead";
	Interval = 0.2f;
}

void USLFBTServiceIsTargetDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;

	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	// Check if target is dead via CombatManager component
	bool bTargetIsDead = (Target == nullptr);
	if (Target)
	{
		// Try CombatManagerComponent first (player)
		UCombatManagerComponent* CombatManager = Target->FindComponentByClass<UCombatManagerComponent>();
		if (CombatManager)
		{
			bTargetIsDead = CombatManager->bIsDead;
		}
		else
		{
			// Fallback: Try AICombatManagerComponent (AI)
			UAICombatManagerComponent* AICombatManager = Target->FindComponentByClass<UAICombatManagerComponent>();
			if (AICombatManager)
			{
				bTargetIsDead = AICombatManager->bIsDead;
			}
		}
	}

	Blackboard->SetValueAsBool(IsDeadOutputKey.SelectedKeyName, bTargetIsDead);
}
