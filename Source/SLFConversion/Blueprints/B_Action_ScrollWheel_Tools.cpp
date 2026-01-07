// B_Action_ScrollWheel_Tools.cpp
// C++ implementation for Blueprint B_Action_ScrollWheel_Tools
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_ScrollWheel_Tools.json

#include "Blueprints/B_Action_ScrollWheel_Tools.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/AC_CombatManager.h"

UB_Action_ScrollWheel_Tools::UB_Action_ScrollWheel_Tools()
{
}

void UB_Action_ScrollWheel_Tools::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] ExecuteAction"));

	if (!OwnerActor)
	{
		return;
	}

	// Check if guarding - don't cycle while guarding
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (CombatMgr && CombatMgr->GetIsGuarding())
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelTools] Cannot cycle while guarding"));
		return;
	}

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
	FGameplayTag CurrentSlot = EquipMgr->GetActiveToolSlot();

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

	// Set new active tool slot
	EquipMgr->SetActiveToolSlot(NextSlot);
}

