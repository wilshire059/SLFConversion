// SLFActionGuardEnd.cpp
#include "SLFActionGuardEnd.h"

USLFActionGuardEnd::USLFActionGuardEnd()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardEnd] Initialized"));
}

void USLFActionGuardEnd::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardEnd] ExecuteAction - End blocking"));
	// TODO: Clear combat manager guard state
}
