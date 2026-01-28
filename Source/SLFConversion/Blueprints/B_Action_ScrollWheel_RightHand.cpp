// B_Action_ScrollWheel_RightHand.cpp
// C++ implementation for Blueprint B_Action_ScrollWheel_RightHand
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_ScrollWheel_RightHand.json

#include "Blueprints/B_Action_ScrollWheel_RightHand.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/AC_CombatManager.h"

UB_Action_ScrollWheel_RightHand::UB_Action_ScrollWheel_RightHand()
{
}

void UB_Action_ScrollWheel_RightHand::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] ExecuteAction"));

	if (!OwnerActor)
	{
		return;
	}

	// Check if guarding - don't cycle while guarding
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (CombatMgr && CombatMgr->GetIsGuarding())
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] Cannot cycle while guarding"));
		return;
	}

	// Get equipment manager
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionScrollWheelRightHand] No equipment manager"));
		return;
	}

	// Get right hand slots
	const FGameplayTagContainer& RightSlots = EquipMgr->RightHandSlots;
	if (RightSlots.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] No right hand slots configured"));
		return;
	}

	// Get current active slot
	FGameplayTag CurrentSlot = EquipMgr->GetActiveWheelSlotForWeapon(true);  // true = right hand

	// Find current slot index
	int32 CurrentIndex = -1;
	TArray<FGameplayTag> SlotArray;
	RightSlots.GetGameplayTagArray(SlotArray);

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

	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] Cycling from slot %d to %d: %s"),
		CurrentIndex, NextIndex, *NextSlot.ToString());

	// CRITICAL: First unwield/hide the CURRENT slot's weapon
	if (CurrentSlot.IsValid())
	{
		EquipMgr->UnwieldItemAtSlot(CurrentSlot);
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] Unwielded previous slot: %s"), *CurrentSlot.ToString());
	}

	// Then wield item at new slot (shows the new weapon)
	EquipMgr->WieldItemAtSlot(NextSlot);
}

