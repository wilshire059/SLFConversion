// B_Action_GuardStart.cpp
// C++ implementation for Blueprint B_Action_GuardStart
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_GuardStart.json

#include "Blueprints/B_Action_GuardStart.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/BPI_GenericCharacter.h"

UB_Action_GuardStart::UB_Action_GuardStart()
{
}

void UB_Action_GuardStart::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardStart] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionGuardStart] No OwnerActor"));
		return;
	}

	// Get anim instance and check for active montage
	UAnimInstance* AnimInstance = nullptr;
	UAnimInstance* AnimInstance1 = nullptr;
	GetOwnerAnimInstance(AnimInstance, AnimInstance1);

	if (AnimInstance)
	{
		UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
		if (IsValid(CurrentMontage))
		{
			// Stop montage with 0.2s blend out
			AnimInstance->Montage_Stop(0.2f, CurrentMontage);
			UE_LOG(LogTemp, Log, TEXT("[ActionGuardStart] Stopped montage: %s"), *CurrentMontage->GetName());
		}
	}

	// Call ToggleGuardReplicated(true, false) via BPI_GenericCharacter interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_ToggleGuardReplicated(OwnerActor, true, false);
		UE_LOG(LogTemp, Log, TEXT("[ActionGuardStart] Called ToggleGuardReplicated(true, false)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionGuardStart] OwnerActor does not implement BPI_GenericCharacter"));
	}
}

