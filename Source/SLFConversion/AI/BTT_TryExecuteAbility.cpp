// BTT_TryExecuteAbility.cpp
// C++ AI implementation for BTT_TryExecuteAbility
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - tries to execute AI ability
//
// ASYNC TASK FLOW:
// 1. Cast ControlledPawn to ASLFSoulslikeEnemy or AB_Soulslike_Enemy
// 2. Bind to OnAttackEnd delegate
// 3. Call PerformAbility on enemy
// 4. Return InProgress (async)
// 5. When OnAttackEnd fires -> FinishLatentTask(true)

#include "AI/BTT_TryExecuteAbility.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprints/B_Soulslike_Enemy.h"
#include "Blueprints/SLFSoulslikeEnemy.h"

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
		UE_LOG(LogTemp, Warning, TEXT("[BTT_TryExecuteAbility] No controlled pawn"));
		return EBTNodeResult::Failed;
	}

	// Cache reference
	CachedOwnerComp = &OwnerComp;

	// Try casting to ASLFSoulslikeEnemy first (newer class)
	ASLFSoulslikeEnemy* SLFEnemy = Cast<ASLFSoulslikeEnemy>(ControlledPawn);
	if (SLFEnemy)
	{
		CachedSLFEnemy = SLFEnemy;
		CachedEnemy = nullptr;

		// Bind to OnAttackEnd delegate
		SLFEnemy->OnAttackEnd.AddDynamic(this, &UBTT_TryExecuteAbility::OnAttackEndCallback);

		UE_LOG(LogTemp, Log, TEXT("[BTT_TryExecuteAbility] Calling PerformAbility on ASLFSoulslikeEnemy: %s"), *SLFEnemy->GetName());

		// Call PerformAbility
		SLFEnemy->PerformAbility();

		return EBTNodeResult::InProgress;
	}

	// Fallback: Cast to AB_Soulslike_Enemy (older class)
	AB_Soulslike_Enemy* OldEnemy = Cast<AB_Soulslike_Enemy>(ControlledPawn);
	if (OldEnemy)
	{
		CachedEnemy = OldEnemy;
		CachedSLFEnemy = nullptr;

		// Bind to OnAttackEnd delegate
		OldEnemy->OnAttackEnd.AddDynamic(this, &UBTT_TryExecuteAbility::OnAttackEndCallback);

		UE_LOG(LogTemp, Log, TEXT("[BTT_TryExecuteAbility] Calling PerformAbility on AB_Soulslike_Enemy: %s"), *OldEnemy->GetName());

		// Call PerformAbility (BlueprintImplementableEvent)
		OldEnemy->PerformAbility();

		return EBTNodeResult::InProgress;
	}

	UE_LOG(LogTemp, Warning, TEXT("[BTT_TryExecuteAbility] Pawn is neither ASLFSoulslikeEnemy nor AB_Soulslike_Enemy: %s"),
		*ControlledPawn->GetClass()->GetName());
	return EBTNodeResult::Failed;
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
	if (CachedSLFEnemy.IsValid())
	{
		CachedSLFEnemy->OnAttackEnd.RemoveDynamic(this, &UBTT_TryExecuteAbility::OnAttackEndCallback);
		CachedSLFEnemy.Reset();
	}

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
