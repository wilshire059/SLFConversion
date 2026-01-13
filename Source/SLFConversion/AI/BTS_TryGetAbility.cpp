// BTS_TryGetAbility.cpp
// C++ AI implementation for BTS_TryGetAbility
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - checks if AI can use ability and sets blackboard key

#include "AI/BTS_TryGetAbility.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AC_AI_CombatManager.h"
#include "Components/AICombatManagerComponent.h"

UBTS_TryGetAbility::UBTS_TryGetAbility()
{
	NodeName = TEXT("Try Get Ability");
}

void UBTS_TryGetAbility::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	// Try to get combat manager - check both C++ and Blueprint component types
	bool bCanAttack = false;

	// First try C++ component (UAICombatManagerComponent)
	UAICombatManagerComponent* CppCombatMgr = ControlledPawn->FindComponentByClass<UAICombatManagerComponent>();
	if (CppCombatMgr)
	{
		UDataAsset* SelectedAbility = nullptr;
		bCanAttack = CppCombatMgr->TryGetAbility(SelectedAbility);
		UE_LOG(LogTemp, Log, TEXT("[BTS_TryGetAbility] C++ CombatManager found, TryGetAbility=%s, Ability=%s"),
			bCanAttack ? TEXT("true") : TEXT("false"),
			SelectedAbility ? *SelectedAbility->GetName() : TEXT("null"));
	}
	else
	{
		// Fallback: try Blueprint component (UAC_AI_CombatManager)
		UAC_AI_CombatManager* BpCombatMgr = ControlledPawn->FindComponentByClass<UAC_AI_CombatManager>();
		if (BpCombatMgr)
		{
			UPrimaryDataAsset* SelectedAbility = nullptr;
			UPrimaryDataAsset* Unused1 = nullptr;
			UPrimaryDataAsset* Unused2 = nullptr;
			UPrimaryDataAsset* Unused3 = nullptr;
			UPrimaryDataAsset* Unused4 = nullptr;
			BpCombatMgr->TryGetAbility(SelectedAbility, Unused1, Unused2, Unused3, Unused4);
			bCanAttack = IsValid(SelectedAbility);
			UE_LOG(LogTemp, Log, TEXT("[BTS_TryGetAbility] BP CombatManager found, CanAttack=%s"),
				bCanAttack ? TEXT("true") : TEXT("false"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[BTS_TryGetAbility] No CombatManager found on %s!"), *ControlledPawn->GetName());
		}
	}

	// From Blueprint: Set result to blackboard
	Blackboard->SetValueAsBool(CanAttackKey.SelectedKeyName, bCanAttack);

	UE_LOG(LogTemp, Log, TEXT("UBTS_TryGetAbility::TickNode - CanAttack=%s on %s"),
		bCanAttack ? TEXT("true") : TEXT("false"), *ControlledPawn->GetName());
}
