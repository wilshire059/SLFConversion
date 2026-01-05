// SLFActionTwoHandedStanceL.cpp
// Logic: Toggle two-handed stance for left hand weapon
#include "SLFActionTwoHandedStanceL.h"
#include "Components/EquipmentManagerComponent.h"

USLFActionTwoHandedStanceL::USLFActionTwoHandedStanceL()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStanceL] Initialized"));
}

void USLFActionTwoHandedStanceL::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStanceL] ExecuteAction - Two-hand left weapon"));

	if (!OwnerActor) return;

	// Get equipment manager
	UEquipmentManagerComponent* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionTwoHandedStanceL] No equipment manager"));
		return;
	}

	// Adjust stance for left hand (bRightHand = false)
	EquipMgr->AdjustForTwoHandStance(false);
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStanceL] Adjusted for two-hand stance (left)"));
}
