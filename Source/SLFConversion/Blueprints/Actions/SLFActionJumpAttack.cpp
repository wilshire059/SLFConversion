// SLFActionJumpAttack.cpp
#include "SLFActionJumpAttack.h"

USLFActionJumpAttack::USLFActionJumpAttack()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] Initialized"));
}

void USLFActionJumpAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] ExecuteAction - Plunging attack"));
	// TODO: Play jump attack animation, apply plunge attack damage
}
