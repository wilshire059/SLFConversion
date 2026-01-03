// SLFActionComboHeavy.cpp
#include "SLFActionComboHeavy.h"
#include "GameFramework/Character.h"

USLFActionComboHeavy::USLFActionComboHeavy()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Initialized"));
}

void USLFActionComboHeavy::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] ExecuteAction"));

	// TODO: Implement stamina requirement check
	// CheckStatRequirement(StaminaTag, StaminaCost);

	// TODO: Get heavy attack montage from equipped weapon
	UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Playing heavy attack"));
}
