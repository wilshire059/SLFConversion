// BTT_TryExecuteAbility.cpp
// C++ AI implementation for BTT_TryExecuteAbility
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - tries to execute AI ability
//
// ASYNC TASK FLOW:
// 1. Cast ControlledPawn to AB_Soulslike_Enemy
// 2. Bind to OnAttackEnd delegate
// 3. Call PerformAbility on enemy
// 4. Return InProgress (async)
// 5. When OnAttackEnd fires -> FinishLatentTask(true)

#include "AI/BTT_TryExecuteAbility.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprints/B_Soulslike_Enemy.h"

UBTT_TryExecuteAbility::UBTT_TryExecuteAbility()
{
	NodeName = TEXT("Try Execute Ability");
}

EBTNodeResult::Type UBTT_TryExecuteAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTT_TryExecuteAbility::ExecuteTask - No controlled pawn"));
		return EBTNodeResult::Failed;
	}

	// Cast to B_Soulslike_Enemy
	AB_Soulslike_Enemy* Enemy = Cast<AB_Soulslike_Enemy>(ControlledPawn);
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTT_TryExecuteAbility::ExecuteTask - Pawn is not AB_Soulslike_Enemy"));
		return EBTNodeResult::Failed;
	}

	// Cache references for later use
	CachedEnemy = Enemy;
	CachedOwnerComp = &OwnerComp;

	// Bind to OnAttackEnd delegate
	// Using AddDynamic pattern for multicast delegates
	Enemy->OnAttackEnd.AddDynamic(this, &UBTT_TryExecuteAbility::OnAttackEndCallback);

	UE_LOG(LogTemp, Log, TEXT("UBTT_TryExecuteAbility::ExecuteTask - Bound to OnAttackEnd, calling PerformAbility on %s"), *Enemy->GetName());

	// Call PerformAbility on the enemy
	Enemy->PerformAbility();

	// Return InProgress - task will complete when OnAttackEnd fires
	return EBTNodeResult::InProgress;
}

void UBTT_TryExecuteAbility::OnAttackEndCallback()
{
	UE_LOG(LogTemp, Log, TEXT("UBTT_TryExecuteAbility::OnAttackEndCallback - Attack finished"));

	// Cleanup delegate binding first
	CleanupDelegateBinding();

	// Finish the task successfully
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
	}
}

void UBTT_TryExecuteAbility::CleanupDelegateBinding()
{
	if (CachedEnemy.IsValid())
	{
		CachedEnemy->OnAttackEnd.RemoveDynamic(this, &UBTT_TryExecuteAbility::OnAttackEndCallback);
		CachedEnemy.Reset();
	}
}

EBTNodeResult::Type UBTT_TryExecuteAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Log, TEXT("UBTT_TryExecuteAbility::AbortTask - Aborting task"));

	// Cleanup delegate binding on abort
	CleanupDelegateBinding();

	return EBTNodeResult::Aborted;
}
