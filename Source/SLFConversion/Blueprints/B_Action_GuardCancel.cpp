// B_Action_GuardCancel.cpp
// C++ implementation for Blueprint B_Action_GuardCancel
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_GuardCancel.json

#include "Blueprints/B_Action_GuardCancel.h"
#include "Interfaces/BPI_GenericCharacter.h"

UB_Action_GuardCancel::UB_Action_GuardCancel()
{
}

void UB_Action_GuardCancel::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardCancel] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionGuardCancel] No OwnerActor"));
		return;
	}

	// Call ToggleGuardReplicated(false, true) - immediately toggle off, IGNORING grace period
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_ToggleGuardReplicated(OwnerActor, false, true);
		UE_LOG(LogTemp, Log, TEXT("[ActionGuardCancel] Called ToggleGuardReplicated(false, true) - ignoring grace period"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionGuardCancel] OwnerActor does not implement BPI_GenericCharacter"));
	}
}
