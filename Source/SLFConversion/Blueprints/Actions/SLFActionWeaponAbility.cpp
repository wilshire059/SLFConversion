// SLFActionWeaponAbility.cpp
#include "SLFActionWeaponAbility.h"

USLFActionWeaponAbility::USLFActionWeaponAbility()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Initialized"));
}

void USLFActionWeaponAbility::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] ExecuteAction - Weapon skill/art"));
	// TODO: Execute weapon-specific ability from equipped weapon data
}
