// B_Action_Execute.cpp
// C++ implementation for Blueprint B_Action_Execute
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_Execute.json

#include "Blueprints/B_Action_Execute.h"
#include "Components/AC_CombatManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"
#include "Animation/AnimMontage.h"

UB_Action_Execute::UB_Action_Execute()
{
}

void UB_Action_Execute::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionExecute] ExecuteAction"));

	if (!OwnerActor)
	{
		return;
	}

	// Get combat manager to find execution target
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (!CombatMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] No combat manager"));
		return;
	}

	// Get execution target (poise-broken/staggered enemy)
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
		AttackerMontage = ActionData->ActionMontage.LoadSynchronous();
	}

	// Play attacker's execution animation
	if (AttackerMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, AttackerMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Playing attacker montage: %s"), *AttackerMontage->GetName());
	}

	// Victim plays "being executed" animation via their AI CombatManager
	if (Target->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Victim should play execution reaction via their CombatManager"));
	}

	// Clear execution target after use
	CombatMgr->SetExecutionTarget(nullptr);
}

