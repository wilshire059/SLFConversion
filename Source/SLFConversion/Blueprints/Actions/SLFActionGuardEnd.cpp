// SLFActionGuardEnd.cpp
// Logic: Toggle guard off (respects grace period)
#include "SLFActionGuardEnd.h"
#include "Interfaces/BPI_GenericCharacter.h"

USLFActionGuardEnd::USLFActionGuardEnd()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardEnd] Initialized"));
}

void USLFActionGuardEnd::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardEnd] ExecuteAction"));

	if (!OwnerActor) return;

	// Toggle guard off via interface (respects grace period)
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_ToggleGuardReplicated(OwnerActor, false, false);
		UE_LOG(LogTemp, Log, TEXT("[ActionGuardEnd] Guard toggled OFF (respects grace period)"));
	}
}
