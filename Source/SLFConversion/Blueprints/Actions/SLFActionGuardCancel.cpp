// SLFActionGuardCancel.cpp
#include "SLFActionGuardCancel.h"

USLFActionGuardCancel::USLFActionGuardCancel()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardCancel] Initialized"));
}

void USLFActionGuardCancel::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardCancel] ExecuteAction - Cancel guard"));
	// TODO: Force stop guard animation, clear guard state
}
