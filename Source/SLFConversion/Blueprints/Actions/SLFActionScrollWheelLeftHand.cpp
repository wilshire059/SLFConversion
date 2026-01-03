// SLFActionScrollWheelLeftHand.cpp
#include "SLFActionScrollWheelLeftHand.h"

USLFActionScrollWheelLeftHand::USLFActionScrollWheelLeftHand()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] Initialized"));
}

void USLFActionScrollWheelLeftHand::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] ExecuteAction - Cycle left hand weapon"));
	// TODO: Call EquipmentManager->CycleLeftHandSlot()
}
