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

	// Find current slot index
	int32 CurrentIndex = -1;
	TArray<FGameplayTag> SlotArray;
	LeftSlots.GetGameplayTagArray(SlotArray);

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

	UE_LOG(LogTemp, Log, TEXT("[ActionScrollWheelLeftHand] Cycling from slot %d to %d: %s"),
		CurrentIndex, NextIndex, *NextSlot.ToString());

	// Wield item at new slot
	EquipMgr->WieldItemAtSlot(NextSlot);
}
