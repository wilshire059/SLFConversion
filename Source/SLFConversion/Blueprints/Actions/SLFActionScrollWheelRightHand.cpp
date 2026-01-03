// SLFActionScrollWheelRightHand.cpp
#include "SLFActionScrollWheelRightHand.h"

USLFActionScrollWheelRightHand::USLFActionScrollWheelRightHand()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] Initialized"));
}

void USLFActionScrollWheelRightHand::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] ExecuteAction - Cycle right hand weapon"));
	// TODO: Call EquipmentManager->CycleRightHandSlot()
}
