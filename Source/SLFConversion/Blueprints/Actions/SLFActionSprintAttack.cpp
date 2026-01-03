// SLFActionSprintAttack.cpp
#include "SLFActionSprintAttack.h"

USLFActionSprintAttack::USLFActionSprintAttack()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionSprintAttack] Initialized"));
}

void USLFActionSprintAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionSprintAttack] ExecuteAction - Running attack"));
	// TODO: Play sprint attack animation from equipped weapon
}
