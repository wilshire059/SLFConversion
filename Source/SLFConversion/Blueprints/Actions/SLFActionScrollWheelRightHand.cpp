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

	// Wield item at new slot
	EquipMgr->WieldItemAtSlot(NextSlot);
}
