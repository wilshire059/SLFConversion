// B_PickupItem.cpp
// C++ implementation for Blueprint B_PickupItem
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_PickupItem.json

#include "Blueprints/B_PickupItem.h"

AB_PickupItem::AB_PickupItem()
{
}

FSLFItemInfo AB_PickupItem::TryGetItemInfo_Implementation()
{
	// Return the cached ItemInfo if available, otherwise try to get from Item asset
	if (Item)
	{
		// If we have a valid Item asset, try to get its ItemInformation
		if (UPDA_Item* ItemAsset = Cast<UPDA_Item>(Item))
		{
			return ItemAsset->ItemInformation;
		}
	}
	return ItemInfo;
}
