// SLFActionUseEquippedTool.cpp
// Logic: Get active tool slot, get item at slot, use the item
#include "SLFActionUseEquippedTool.h"
#include "AC_EquipmentManager.h"
#include "AC_InventoryManager.h"
#include "AC_ActionManager.h"
#include "SLFPrimaryDataAssets.h"
#include "SLFGameTypes.h"
#include "Components/EquipmentManagerComponent.h"
#include "Components/InventoryManagerComponent.h"

USLFActionUseEquippedTool::USLFActionUseEquippedTool()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Initialized"));
}

void USLFActionUseEquippedTool::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] ExecuteAction - Use currently equipped tool"));

	if (!OwnerActor) return;

	// Get equipment manager
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionUseEquippedTool] No equipment manager"));
		return;
	}

	// Get active tool slot
	FGameplayTag ActiveToolSlot = EquipMgr->GetActiveToolSlot();
	if (!ActiveToolSlot.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] No active tool slot"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Active tool slot: %s"), *ActiveToolSlot.ToString());

	// Get item at that slot
	UPrimaryDataAsset* ItemAsset = nullptr;
	FGuid ItemId;
	EquipMgr->GetItemAtSlotSimple(ActiveToolSlot, ItemAsset, ItemId);

	if (!ItemAsset)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] No item at active tool slot"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Using tool: %s"), *ItemAsset->GetName());

	// Check if this is a flask based on item name/category
	FString ItemName = ItemAsset->GetName();
	UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset);

	bool bIsFlask = ItemName.Contains(TEXT("Flask")) || ItemName.Contains(TEXT("Potion"));

	// Also check category if available
	if (ItemData)
	{
		ESLFItemCategory Category = ItemData->ItemInformation.Category.Category;
		if (Category == ESLFItemCategory::Tools)
		{
			// Tools category can include flasks - check name for flask
			bIsFlask = bIsFlask || ItemName.Contains(TEXT("Flask"));
		}
	}

	if (bIsFlask)
	{
		// Flask item - check charges BEFORE triggering action
		UInventoryManagerComponent* InvMgr = GetInventoryManager();
		if (!InvMgr)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ActionUseEquippedTool] No inventory manager found!"));
			return;
		}

		// Check if flask has charges remaining
		int32 SlotIndex = InvMgr->GetSlotWithItem(ItemAsset, ESLFInventorySlotType::InventorySlot);
		if (SlotIndex < 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ActionUseEquippedTool] Flask not found in inventory!"));
			return;
		}

		// Get current amount from inventory
		TArray<FSLFInventoryItem> AllItems = InvMgr->GetAllItems();
		int32 CurrentCharges = 0;
		for (const FSLFInventoryItem& Item : AllItems)
		{
			if (Item.ItemAsset == ItemAsset)
			{
				CurrentCharges = Item.Amount;
				break;
			}
		}

		if (CurrentCharges <= 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Flask has no charges left!"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Flask has %d charges, using one"), CurrentCharges);

		// Now trigger the DrinkFlask action
		UAC_ActionManager* ActionMgr = GetActionManager();
		if (ActionMgr)
		{
			FGameplayTag DrinkFlaskTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.DrinkFlask.HP"), false);
			if (DrinkFlaskTag.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Flask detected - triggering DrinkFlask.HP action"));
				ActionMgr->EventPerformAction(DrinkFlaskTag);

				// Consume one flask charge
				InvMgr->UseItemFromInventory(ItemAsset);
				UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Consumed 1 flask charge, %d remaining"), CurrentCharges - 1);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[ActionUseEquippedTool] DrinkFlask.HP tag not found!"));
			}
		}
	}
	else
	{
		// Non-flask item - use via inventory manager
		UInventoryManagerComponent* InvMgr = GetInventoryManager();
		if (InvMgr)
		{
			InvMgr->UseItemFromInventory(ItemAsset);
		}
	}
}
