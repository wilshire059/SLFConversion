// SLFActionGuardCancel.cpp
// Logic: Immediately toggle guard off, IGNORING grace period
#include "SLFActionGuardCancel.h"
#include "Interfaces/BPI_GenericCharacter.h"

USLFActionGuardCancel::USLFActionGuardCancel()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardCancel] Initialized"));
}

void USLFActionGuardCancel::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardCancel] ExecuteAction"));

	if (!OwnerActor) return;

	// Toggle guard off via interface (ignores grace period for immediate cancel)
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_ToggleGuardReplicated(OwnerActor, false, true);
		UE_LOG(LogTemp, Log, TEXT("[ActionGuardCancel] Guard CANCELLED (ignores grace period)"));
	}
}
