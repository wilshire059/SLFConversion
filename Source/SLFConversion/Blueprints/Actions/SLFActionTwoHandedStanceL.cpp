// SLFActionTwoHandedStanceL.cpp
#include "SLFActionTwoHandedStanceL.h"

USLFActionTwoHandedStanceL::USLFActionTwoHandedStanceL()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStanceL] Initialized"));
}

void USLFActionTwoHandedStanceL::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStanceL] ExecuteAction - Two-hand left weapon"));
	// TODO: Toggle two-handed mode for left hand weapon via EquipmentManager
}
