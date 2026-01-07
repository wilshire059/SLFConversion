// B_Action_Crouch.cpp
// C++ implementation for Blueprint B_Action_Crouch
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_Crouch.json

#include "Blueprints/B_Action_Crouch.h"
#include "Interfaces/BPI_Player.h"

UB_Action_Crouch::UB_Action_Crouch()
{
}

void UB_Action_Crouch::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionCrouch] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionCrouch] No OwnerActor"));
		return;
	}

	// Call ToggleCrouchReplicated via BPI_Player interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
	{
		IBPI_Player::Execute_ToggleCrouchReplicated(OwnerActor);
		UE_LOG(LogTemp, Log, TEXT("[ActionCrouch] Called ToggleCrouchReplicated"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionCrouch] OwnerActor does not implement BPI_Player"));
	}
}

