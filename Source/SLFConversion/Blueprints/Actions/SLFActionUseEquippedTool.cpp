// SLFActionUseEquippedTool.cpp
#include "SLFActionUseEquippedTool.h"

USLFActionUseEquippedTool::USLFActionUseEquippedTool()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Initialized"));
}

void USLFActionUseEquippedTool::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] ExecuteAction - Use currently equipped tool"));
	// TODO: Get equipped tool from EquipmentManager, execute tool action
}
