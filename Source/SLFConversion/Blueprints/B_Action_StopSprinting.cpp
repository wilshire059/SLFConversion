// B_Action_StopSprinting.cpp
// C++ implementation for Blueprint B_Action_StopSprinting
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_StopSprinting.json

#include "Blueprints/B_Action_StopSprinting.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/AC_ActionManager.h"

UB_Action_StopSprinting::UB_Action_StopSprinting()
{
}

void UB_Action_StopSprinting::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionStopSprinting] No OwnerActor"));
		return;
	}

	// Set movement mode to Run (default) via BPI_GenericCharacter interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SetMovementMode(OwnerActor, ESLFMovementType::Run);
		UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] MovementMode set to Run"));
	}

	// Stop stamina loss and clear sprinting flag via ActionManager
	UAC_ActionManager* ActionMgr = GetActionManager();
	if (ActionMgr)
	{
		ActionMgr->EventStopStaminaLoss();
		ActionMgr->SetIsSprinting(false);
		UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] Stamina loss stopped, IsSprinting = false"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionStopSprinting] No ActionManager found"));
	}
}
