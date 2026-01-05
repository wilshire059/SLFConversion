// SLFActionUseEquippedTool.cpp
// Logic: Get active tool slot, get item at slot, use the item
#include "SLFActionUseEquippedTool.h"
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
	UEquipmentManagerComponent* EquipMgr = GetEquipmentManager();
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

	// Get item at that slot
	UDataAsset* ItemAsset = nullptr;
	FGuid ItemId;
	EquipMgr->GetItemAtSlot(ActiveToolSlot, ItemAsset, ItemId);

	if (!ItemAsset)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] No item at active tool slot"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Using tool: %s"), *ItemAsset->GetName());

	// Use the item via inventory manager
	UInventoryManagerComponent* InvMgr = GetInventoryManager();
	if (InvMgr)
	{
		InvMgr->UseItemFromInventory(ItemAsset);
	}
}
