// SLFActionScrollWheelTools.cpp
// Logic: Cycle through tool slots, set active tool slot
#include "SLFActionScrollWheelTools.h"
#include "AC_EquipmentManager.h"
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
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
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

	// Build array of ONLY occupied tool slots
	TArray<FGameplayTag> AllSlots;
	Tools.GetGameplayTagArray(AllSlots);

	TArray<FGameplayTag> OccupiedSlots;
	for (const FGameplayTag& Slot : AllSlots)
	{
		if (EquipMgr->IsSlotOccupied(Slot))
		{
			OccupiedSlots.Add(Slot);
		}
	}

	if (OccupiedSlots.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] No occupied tool slots to cycle"));
		return;
	}

	if (OccupiedSlots.Num() == 1)
	{
		// Only one tool, nothing to cycle to
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] Only one tool equipped, no cycling needed"));
		return;
	}

	// Find current slot index in occupied slots
	int32 CurrentIndex = -1;
	for (int32 i = 0; i < OccupiedSlots.Num(); i++)
	{
		if (OccupiedSlots[i] == CurrentSlot)
		{
			CurrentIndex = i;
			break;
		}
	}

	// If current slot not found in occupied slots, start from first
	if (CurrentIndex < 0)
	{
		CurrentIndex = 0;
	}

	// Cycle to next occupied slot
	int32 NextIndex = (CurrentIndex + 1) % OccupiedSlots.Num();
	FGameplayTag NextSlot = OccupiedSlots[NextIndex];

	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] Cycling from slot %d to %d: %s (occupied slots: %d)"),
		CurrentIndex, NextIndex, *NextSlot.ToString(), OccupiedSlots.Num());

	// Set active tool slot
	EquipMgr->SetActiveToolSlot(NextSlot);
}
