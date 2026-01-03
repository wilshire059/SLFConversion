// SLFActionBackstab.cpp
#include "SLFActionBackstab.h"

USLFActionBackstab::USLFActionBackstab()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Initialized"));
}

void USLFActionBackstab::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] ExecuteAction - Critical backstab attack"));
	// TODO: Play paired backstab animation, apply critical damage multiplier
}
