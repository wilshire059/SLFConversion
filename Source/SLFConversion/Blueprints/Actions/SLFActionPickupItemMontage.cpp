// SLFActionPickupItemMontage.cpp
#include "SLFActionPickupItemMontage.h"

USLFActionPickupItemMontage::USLFActionPickupItemMontage()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] Initialized"));
}

void USLFActionPickupItemMontage::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] ExecuteAction - Play item pickup animation"));
	// TODO: Play pickup animation, add item to inventory via InteractionManager
}
