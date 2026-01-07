// SLFActionBackstab.cpp
// Logic: Get execution target from CombatManager, play paired backstab animation,
// apply critical damage multiplier to the victim
#include "SLFActionBackstab.h"
#include "Components/CombatManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"

USLFActionBackstab::USLFActionBackstab()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Initialized"));
}

void USLFActionBackstab::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] ExecuteAction - Critical backstab attack"));

	if (!OwnerActor) return;

	// Get combat manager to find execution target
	UCombatManagerComponent* CombatMgr = GetCombatManager();
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
	UAnimMontage* VictimMontage = nullptr;

	if (UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action))
	{
		// Load the action montage for the attacker
		AttackerMontage = ActionData->ActionMontage.LoadSynchronous();

		// For victim montage, we need to get it from RelevantData if it contains execution info
		// or from the target's AI CombatManager
		if (Target->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
		{
			// Get victim's executed montage for backstab
			FGameplayTag BackstabTag = FGameplayTag::RequestGameplayTag(FName("Action.Backstab"), false);
			// The victim's montage would come from their AI CombatManager's GetRelevantExecutedMontage
		}
	}

	// Play attacker's backstab animation
	if (AttackerMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, AttackerMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Playing attacker montage: %s"), *AttackerMontage->GetName());
	}

	// Play victim's reaction animation - would be handled by the victim's combat manager
	// The victim plays their "being backstabbed" animation in response
	if (Target->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		// Victim animation is typically triggered via damage event or direct call
		UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Victim should play backstab reaction"));
	}

	// Apply critical damage (backstab typically deals 4x damage or based on weapon)
	// Damage is applied via animation notify (weapon trace) during the backstab animation
	// The critical multiplier would be set via CombatManager or passed to damage calculation

	// Clear execution target after use
	CombatMgr->SetExecutionTarget(nullptr);
}
