// SLFActionScrollWheelRightHand.cpp
// Logic: Check not guarding, cycle through right hand weapon slots, wield next
#include "SLFActionScrollWheelRightHand.h"
#include "AC_EquipmentManager.h"
#include "AC_CombatManager.h"
#include "Components/CombatManagerComponent.h"
#include "Components/EquipmentManagerComponent.h"

USLFActionScrollWheelRightHand::USLFActionScrollWheelRightHand()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] Initialized"));
}

void USLFActionScrollWheelRightHand::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] ExecuteAction - Cycle right hand weapon"));

	if (!OwnerActor) return;

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
	FGameplayTag CurrentSlot = EquipMgr->GetActiveWheelSlotForWeapon(true);

	// Build array of OCCUPIED slots only (skip empty slots)
	TArray<FGameplayTag> OccupiedSlots;
	TArray<FGameplayTag> AllSlots;
	RightSlots.GetGameplayTagArray(AllSlots);

	for (const FGameplayTag& SlotTag : AllSlots)
	{
		if (EquipMgr->IsSlotOccupied(SlotTag))
		{
			OccupiedSlots.Add(SlotTag);
		}
	}

	if (OccupiedSlots.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelRightHand] No occupied right hand slots"));
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

	// Cycle to next occupied slot
	int32 NextIndex = (CurrentIndex + 1) % OccupiedSlots.Num();
	FGameplayTag NextSlot = OccupiedSlots[NextIndex];

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
