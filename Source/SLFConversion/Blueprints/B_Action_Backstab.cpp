// B_Action_Backstab.cpp
// C++ implementation for Blueprint B_Action_Backstab
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_Backstab.json

#include "Blueprints/B_Action_Backstab.h"
#include "Components/AC_CombatManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"
#include "Animation/AnimMontage.h"

UB_Action_Backstab::UB_Action_Backstab()
{
}

void UB_Action_Backstab::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] ExecuteAction"));

	if (!OwnerActor)
	{
		return;
	}

	// Get combat manager to find execution target
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (!CombatMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] No combat manager"));
		return;
	}

	// Get execution target
	AActor* Target = CombatMgr->ExecutionTarget;
	if (!Target)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] No execution target set"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Executing backstab on: %s"), *Target->GetName());

	// Get backstab montage from action data asset
	UAnimMontage* AttackerMontage = nullptr;

	if (UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action))
	{
		AttackerMontage = ActionData->ActionMontage.LoadSynchronous();
	}

	// Play attacker's backstab animation
	if (AttackerMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, AttackerMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Playing attacker montage: %s"), *AttackerMontage->GetName());
	}

	// Victim animation is typically triggered via the target's AI CombatManager
	if (Target->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Victim should play backstab reaction via their CombatManager"));
	}

	// Clear execution target after use
	CombatMgr->SetExecutionTarget(nullptr);
}

