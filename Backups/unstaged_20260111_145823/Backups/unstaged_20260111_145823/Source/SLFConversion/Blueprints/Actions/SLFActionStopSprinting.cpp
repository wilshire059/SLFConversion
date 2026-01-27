// SLFActionStopSprinting.cpp
// Logic: SetMovementMode(Walking), StopStaminaLoss, SetIsSprinting(false)
#include "SLFActionStopSprinting.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "AC_ActionManager.h"

USLFActionStopSprinting::USLFActionStopSprinting()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] Initialized"));
}

void USLFActionStopSprinting::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] ExecuteAction"));

	if (!OwnerActor) return;

	// Set movement mode to walking via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SetMovementMode(OwnerActor, ESLFMovementType::Walk);
		UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] MovementMode set to Walk"));
	}

	// Stop stamina loss via ActionManager
	if (UAC_ActionManager* ActionMgr = GetActionManager())
	{
		ActionMgr->EventStopStaminaLoss();
		ActionMgr->SetIsSprinting(false);
		UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] Stamina loss stopped, IsSprinting = false"));
	}
}
