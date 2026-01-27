// SLFActionExecute.cpp
// Logic: Get execution target from CombatManager (staggered/poise-broken enemy),
// play paired execution/riposte animation on both attacker and victim
#include "SLFActionExecute.h"
#include "AC_CombatManager.h"
#include "Components/CombatManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"

USLFActionExecute::USLFActionExecute()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Initialized"));
}

void USLFActionExecute::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionExecute] ExecuteAction - Riposte/execution attack"));

	if (!OwnerActor) return;

	// Get combat manager to find execution target
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (!CombatMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] No combat manager"));
		return;
	}

	// Get execution target (should be a poise-broken/staggered enemy)
	AActor* Target = CombatMgr->ExecutionTarget;
	if (!Target)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionExecute] No execution target set"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Executing riposte on: %s"), *Target->GetName());

	// Get execution montage from action data asset
	UAnimMontage* AttackerMontage = nullptr;

	if (UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action))
	{
		// Load the action montage for the attacker
		AttackerMontage = ActionData->ActionMontage.LoadSynchronous();
	}

	// Play attacker's execution animation
	if (AttackerMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, AttackerMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Playing attacker montage: %s"), *AttackerMontage->GetName());
	}

	// For the victim (target), they need to play their "being executed" animation
	// This is typically retrieved from the target's AI CombatManager via GetRelevantExecutedMontage
	if (Target->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		FGameplayTag ExecuteTag = FGameplayTag::RequestGameplayTag(FName("Action.Execute"), false);
		// The victim's AI CombatManager would have ExecutedMontages data table
		// which maps execution tags to victim montages
		UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Victim should play execution reaction via their CombatManager"));
	}

	// Position the attacker relative to target for the paired animation
	// In a full implementation, this would warp the attacker to the proper position
	// for the synchronized paired animation to play correctly

	// Apply critical damage (execution typically deals massive damage)
	// Damage is applied via animation notify (weapon trace) during the execution animation
	// The critical multiplier would be set via CombatManager

	// Clear execution target after use
	CombatMgr->SetExecutionTarget(nullptr);
}
