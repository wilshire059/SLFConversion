// B_Action_UseEquippedTool.cpp
// C++ implementation for Blueprint B_Action_UseEquippedTool
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_UseEquippedTool.json

#include "Blueprints/B_Action_UseEquippedTool.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/AC_InventoryManager.h"

UB_Action_UseEquippedTool::UB_Action_UseEquippedTool()
{
}

void UB_Action_UseEquippedTool::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] ExecuteAction"));

	// Get equipment manager
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionUseEquippedTool] No EquipmentManager found"));
		return;
	}

	// Get inventory manager
	UAC_InventoryManager* InvMgr = GetInventoryManager();
	if (!InvMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionUseEquippedTool] No InventoryManager found"));
		return;
	}

	// Get active tool slot
	FGameplayTag ActiveToolSlot = EquipMgr->GetActiveToolSlot();
	UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] ActiveToolSlot: %s"), *ActiveToolSlot.ToString());

	// Get item at slot
	FSLFItemInfo ItemData;
	UPrimaryDataAsset* ItemAsset = nullptr;
	FGuid ItemId;
	FSLFItemInfo ItemData_1;
	UPrimaryDataAsset* ItemAsset_1 = nullptr;
	FGuid ItemId_1;

	EquipMgr->GetItemAtSlot(ActiveToolSlot, ItemData, ItemAsset, ItemId, ItemData_1, ItemAsset_1, ItemId_1);

	// Use item from inventory
	if (ItemAsset)
	{
		InvMgr->UseItemFromInventory(ItemAsset);
		UE_LOG(LogTemp, Log, TEXT("[ActionUseEquippedTool] Used item: %s"), *ItemAsset->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionUseEquippedTool] No item found at active tool slot"));
	}
}
