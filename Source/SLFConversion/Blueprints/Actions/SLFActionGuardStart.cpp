// SLFActionGuardStart.cpp
// Logic: Stop active montage, then toggle guard on
#include "SLFActionGuardStart.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/BPI_GenericCharacter.h"

USLFActionGuardStart::USLFActionGuardStart()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardStart] Initialized"));
}

void USLFActionGuardStart::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardStart] ExecuteAction"));

	if (!OwnerActor) return;

	// Get anim instance and check for active montage
	if (UAnimInstance* AnimInstance = GetOwnerAnimInstance())
	{
		UAnimMontage* ActiveMontage = AnimInstance->GetCurrentActiveMontage();
		if (IsValid(ActiveMontage))
		{
			// Stop active montage with 0.2s blend out
			AnimInstance->Montage_Stop(0.2f, ActiveMontage);
			UE_LOG(LogTemp, Log, TEXT("[ActionGuardStart] Stopped active montage: %s"), *ActiveMontage->GetName());
		}
	}

	// Toggle guard on via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_ToggleGuardReplicated(OwnerActor, true, false);
		UE_LOG(LogTemp, Log, TEXT("[ActionGuardStart] Guard toggled ON"));
	}
}
