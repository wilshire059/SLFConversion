// SLFActionScrollWheelLeftHand.cpp
// Logic: Check not guarding, cycle through left hand weapon slots, wield next
#include "SLFActionScrollWheelLeftHand.h"
#include "AC_EquipmentManager.h"
#include "AC_CombatManager.h"
#include "Components/CombatManagerComponent.h"
#include "Components/EquipmentManagerComponent.h"

USLFActionScrollWheelLeftHand::USLFActionScrollWheelLeftHand()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] Initialized"));
}

void USLFActionScrollWheelLeftHand::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] ExecuteAction - Cycle left hand weapon"));

	if (!OwnerActor) return;

	// Check if guarding - don't cycle while guarding
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (CombatMgr && CombatMgr->GetIsGuarding())
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] Cannot cycle while guarding"));
		return;
	}

	// Get equipment manager
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionScrollWheelLeftHand] No equipment manager"));
		return;
	}

	// Get left hand slots
	const FGameplayTagContainer& LeftSlots = EquipMgr->LeftHandSlots;
	if (LeftSlots.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] No left hand slots configured"));
		return;
	}

	// Get current active slot
	FGameplayTag CurrentSlot = EquipMgr->GetActiveWheelSlotForWeapon(false);

	// Build array of OCCUPIED slots only (skip empty slots)
	TArray<FGameplayTag> OccupiedSlots;
	TArray<FGameplayTag> AllSlots;
	LeftSlots.GetGameplayTagArray(AllSlots);

	for (const FGameplayTag& SlotTag : AllSlots)
	{
		if (EquipMgr->IsSlotOccupied(SlotTag))
		{
			OccupiedSlots.Add(SlotTag);
		}
	}

	if (OccupiedSlots.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] No occupied left hand slots"));
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

	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] Cycling from slot %d to %d: %s"),
		CurrentIndex, NextIndex, *NextSlot.ToString());

	// CRITICAL: First unwield/hide the CURRENT slot's weapon
	if (CurrentSlot.IsValid())
	{
		EquipMgr->UnwieldItemAtSlot(CurrentSlot);
		UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] Unwielded previous slot: %s"), *CurrentSlot.ToString());
	}

	// Then wield item at new slot (shows the new weapon)
	EquipMgr->WieldItemAtSlot(NextSlot);
}
