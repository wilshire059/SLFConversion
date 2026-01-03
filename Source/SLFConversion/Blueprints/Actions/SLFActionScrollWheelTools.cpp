// SLFActionScrollWheelTools.cpp
#include "SLFActionScrollWheelTools.h"

USLFActionScrollWheelTools::USLFActionScrollWheelTools()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] Initialized"));
}

void USLFActionScrollWheelTools::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] ExecuteAction - Cycle tool/consumable"));
	// TODO: Call EquipmentManager->CycleToolSlot()
}
