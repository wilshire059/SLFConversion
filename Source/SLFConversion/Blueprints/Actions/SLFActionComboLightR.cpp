// SLFActionComboLightR.cpp
#include "SLFActionComboLightR.h"
#include "GameFramework/Character.h"

USLFActionComboLightR::USLFActionComboLightR()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLightR] Initialized"));
}

void USLFActionComboLightR::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLightR] ExecuteAction"));

	// TODO: Implement stamina requirement check
	// if (!CheckStatRequirement(StaminaTag, StaminaCost)) return;

	if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
	{
		// TODO: Get attack montage from EquipmentManager->GetRightHandWeapon()
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLightR] Playing right-hand attack"));
	}
}
