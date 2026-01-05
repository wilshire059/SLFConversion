// SLFActionScrollWheelTools.cpp
// Logic: Cycle through tool slots, set active tool slot
#include "SLFActionScrollWheelTools.h"
#include "Components/EquipmentManagerComponent.h"

USLFActionScrollWheelTools::USLFActionScrollWheelTools()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] Initialized"));
}

void USLFActionScrollWheelTools::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] ExecuteAction - Cycle tool/consumable"));

	if (!OwnerActor) return;

	// Get equipment manager
	UEquipmentManagerComponent* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionScrollWheelTools] No equipment manager"));
		return;
	}

	// Get tool slots
	const FGameplayTagContainer& Tools = EquipMgr->ToolSlots;
	if (Tools.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] No tool slots configured"));
		return;
	}

	// Get current active tool slot
	FGameplayTag CurrentSlot = EquipMgr->GetActiveWheelSlotForTool();

	// Find current slot index
	int32 CurrentIndex = -1;
	TArray<FGameplayTag> SlotArray;
	Tools.GetGameplayTagArray(SlotArray);

	for (int32 i = 0; i < SlotArray.Num(); i++)
	{
		if (SlotArray[i] == CurrentSlot)
		{
			CurrentIndex = i;
			break;
		}
	}

	// Cycle to next slot
	int32 NextIndex = (CurrentIndex + 1) % SlotArray.Num();
	FGameplayTag NextSlot = SlotArray[NextIndex];

	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] Cycling from slot %d to %d: %s"),
		CurrentIndex, NextIndex, *NextSlot.ToString());

	// Set active tool slot
	EquipMgr->SetActiveToolSlot(NextSlot);
}
