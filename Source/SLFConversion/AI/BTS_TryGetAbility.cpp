// BTS_TryGetAbility.cpp
// C++ AI implementation for BTS_TryGetAbility
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - checks if AI can use ability and sets blackboard key

#include "AI/BTS_TryGetAbility.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AC_AI_CombatManager.h"

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

	// From Blueprint: Get AC_AI_CombatManager from controlled pawn
	UAC_AI_CombatManager* AICombatManager = ControlledPawn->FindComponentByClass<UAC_AI_CombatManager>();
	if (!AICombatManager)
	{
		return;
	}

	// From Blueprint: Call TryGetAbility and check if result is valid
	// The function has multiple output params but Blueprint only uses the first one
	UPrimaryDataAsset* SelectedAbility = nullptr;
	UPrimaryDataAsset* Unused1 = nullptr;
	UPrimaryDataAsset* Unused2 = nullptr;
	UPrimaryDataAsset* Unused3 = nullptr;
	UPrimaryDataAsset* Unused4 = nullptr;
	AICombatManager->TryGetAbility(SelectedAbility, Unused1, Unused2, Unused3, Unused4);

	// From Blueprint: Set bCanAttack = IsValid(SelectedAbility)
	bool bCanAttack = IsValid(SelectedAbility);

	// From Blueprint: Set result to blackboard
	Blackboard->SetValueAsBool(CanAttackKey.SelectedKeyName, bCanAttack);

	UE_LOG(LogTemp, Log, TEXT("UBTS_TryGetAbility::TickNode - CanAttack=%s on %s"),
		bCanAttack ? TEXT("true") : TEXT("false"), *ControlledPawn->GetName());
}
