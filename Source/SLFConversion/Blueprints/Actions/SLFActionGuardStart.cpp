// SLFActionGuardStart.cpp
#include "SLFActionGuardStart.h"

USLFActionGuardStart::USLFActionGuardStart()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardStart] Initialized"));
}

void USLFActionGuardStart::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardStart] ExecuteAction - Begin blocking"));
	// TODO: Set combat manager guard state, play guard start animation
}
