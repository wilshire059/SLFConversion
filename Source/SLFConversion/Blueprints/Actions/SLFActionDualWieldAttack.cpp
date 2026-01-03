// SLFActionDualWieldAttack.cpp
#include "SLFActionDualWieldAttack.h"

USLFActionDualWieldAttack::USLFActionDualWieldAttack()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDualWieldAttack] Initialized"));
}

void USLFActionDualWieldAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDualWieldAttack] ExecuteAction - Both hands attack"));
	// TODO: Play dual wield attack animation using both equipped weapons
}
