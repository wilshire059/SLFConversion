// SLFActionComboLightL.cpp
#include "SLFActionComboLightL.h"
#include "GameFramework/Character.h"

USLFActionComboLightL::USLFActionComboLightL()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Initialized"));
}

void USLFActionComboLightL::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] ExecuteAction"));

	// TODO: Implement stamina requirement check
	// if (!CheckStatRequirement(StaminaTag, StaminaCost))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] Stat requirement not met"));
	// 	return;
	// }

	// Get animation from equipment manager (left hand weapon)
	if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
	{
		// TODO: Get attack montage from EquipmentManager->GetLeftHandWeapon()
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Playing left-hand attack"));
	}
}
