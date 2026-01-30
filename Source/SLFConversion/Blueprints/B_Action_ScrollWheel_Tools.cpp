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

	// Set new active tool slot
	EquipMgr->SetActiveToolSlot(NextSlot);
}

