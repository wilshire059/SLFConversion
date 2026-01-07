// B_Action_GuardEnd.cpp
// C++ implementation for Blueprint B_Action_GuardEnd
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_GuardEnd.json

#include "Blueprints/B_Action_GuardEnd.h"
#include "Interfaces/BPI_GenericCharacter.h"

UB_Action_GuardEnd::UB_Action_GuardEnd()
{
}

void UB_Action_GuardEnd::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionGuardEnd] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionGuardEnd] No OwnerActor"));
		return;
	}

	// Call ToggleGuardReplicated(false, false) via BPI_GenericCharacter interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_ToggleGuardReplicated(OwnerActor, false, false);
		UE_LOG(LogTemp, Log, TEXT("[ActionGuardEnd] Called ToggleGuardReplicated(false, false)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionGuardEnd] OwnerActor does not implement BPI_GenericCharacter"));
	}
}
