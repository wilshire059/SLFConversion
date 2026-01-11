// SLFActionTwoHandedStanceR.cpp
// Logic: Toggle two-handed stance for right hand weapon
#include "SLFActionTwoHandedStanceR.h"
#include "AC_EquipmentManager.h"
#include "Components/EquipmentManagerComponent.h"

USLFActionTwoHandedStanceR::USLFActionTwoHandedStanceR()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStanceR] Initialized"));
}

void USLFActionTwoHandedStanceR::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStanceR] ExecuteAction - Two-hand right weapon"));

	if (!OwnerActor) return;

	// Get equipment manager
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionTwoHandedStanceR] No equipment manager"));
		return;
	}

	// Adjust stance for right hand (bRightHand = true)
	EquipMgr->AdjustForTwoHandStance(true);
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStanceR] Adjusted for two-hand stance (right)"));
}
