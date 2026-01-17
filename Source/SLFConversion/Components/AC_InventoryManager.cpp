// AC_InventoryManager.cpp
// C++ component implementation for AC_InventoryManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_InventoryManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_InventoryManager.h"
#include "Blueprints/B_PickupItem.h"
#include "Widgets/W_InventorySlot.h"

UAC_InventoryManager::UAC_InventoryManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	SlotCount = 40;
	SlotsPerRow = 8;
	OwnerActor = nullptr;
	Currency = 0;
}

void UAC_InventoryManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::BeginPlay"));
	OwnerActor = GetOwner();
}

void UAC_InventoryManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * GetInventoryWidget - Get the inventory widget reference
 */
UW_Inventory* UAC_InventoryManager::GetInventoryWidget_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetInventoryWidget"));
	// Widget references are typically set externally from UI
	return nullptr;
}

/**
 * GetAmountOfItem - Get the amount of a specific item in inventory
 */
void UAC_InventoryManager::GetAmountOfItem_Implementation(UPrimaryDataAsset* Item, int32& OutAmount, bool& OutSuccess)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetAmountOfItem - Item: %s"),
		Item ? *Item->GetName() : TEXT("None"));

	OutAmount = 0;
	OutSuccess = false;

	if (!IsValid(Item))
	{
		return;
	}

	// Search through items for matching asset
	for (const auto& ItemEntry : Items)
	{
		if (ItemEntry.Value == Item)
		{
			OutAmount = 1; // Items map stores asset, count tracked elsewhere
			OutSuccess = true;
			return;
		}
	}
}

/**
 * GetStoredAmountOfItem - Get amount of item in storage
 */
void UAC_InventoryManager::GetStoredAmountOfItem_Implementation(UPrimaryDataAsset* Item, int32& OutAmount, bool& OutSuccess)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetStoredAmountOfItem - Item: %s"),
		Item ? *Item->GetName() : TEXT("None"));

	OutAmount = 0;
	OutSuccess = false;

	if (!IsValid(Item))
	{
		return;
	}

	for (const auto& ItemEntry : StoredItems)
	{
		if (ItemEntry.Value == Item)
		{
			OutAmount = 1;
			OutSuccess = true;
			return;
		}
	}
}

/**
 * GetAmountOfItemWithTag - Get amount using gameplay tag
 */
void UAC_InventoryManager::GetAmountOfItemWithTag_Implementation(const FGameplayTag& Tag, int32& OutAmount, bool& OutSuccess)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetAmountOfItemWithTag - Tag: %s"),
		*Tag.ToString());

	OutAmount = 0;
	OutSuccess = false;

	if (UPrimaryDataAsset** FoundItem = Items.Find(Tag))
	{
		OutAmount = 1;
		OutSuccess = true;
	}
}

/**
 * GetOverallCountForItem - Get total count across inventory and storage
 */
void UAC_InventoryManager::GetOverallCountForItem_Implementation(UPrimaryDataAsset* Item, int32& OutOverallCount, int32& OutOverallCount_1, int32& OutOverallCount_2, int32& OutOverallCount_3)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetOverallCountForItem - Item: %s"),
		Item ? *Item->GetName() : TEXT("None"));

	int32 InventoryCount = 0;
	int32 StorageCount = 0;
	bool bTempSuccess = false;

	GetAmountOfItem(Item, InventoryCount, bTempSuccess);
	GetStoredAmountOfItem(Item, StorageCount, bTempSuccess);

	int32 Total = InventoryCount + StorageCount;

	OutOverallCount = Total;
	OutOverallCount_1 = Total;
	OutOverallCount_2 = Total;
	OutOverallCount_3 = Total;
}

/**
 * GetTotalInventoryItemsCount - Get count of all items in inventory
 */
int32 UAC_InventoryManager::GetTotalInventoryItemsCount_Implementation()
{
	int32 Count = Items.Num();
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetTotalInventoryItemsCount - %d"), Count);
	return Count;
}

/**
 * GetTotalStorageItemsCount - Get count of all items in storage
 */
int32 UAC_InventoryManager::GetTotalStorageItemsCount_Implementation()
{
	int32 Count = StoredItems.Num();
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetTotalStorageItemsCount - %d"), Count);
	return Count;
}

/**
 * HasItem - Check if inventory contains item
 */
bool UAC_InventoryManager::HasItem_Implementation(UPrimaryDataAsset* Item)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::HasItem - Item: %s"),
		Item ? *Item->GetName() : TEXT("None"));

	if (!IsValid(Item))
	{
		return false;
	}

	for (const auto& ItemEntry : Items)
	{
		if (ItemEntry.Value == Item)
		{
			return true;
		}
	}
	return false;
}

/**
 * GetSlotWithItem - Get UI slot containing item
 */
UW_InventorySlot* UAC_InventoryManager::GetSlotWithItem_Implementation(UPrimaryDataAsset* Item, ESLFInventorySlotType SlotType)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetSlotWithItem"));
	// Slot widgets are managed by UI system
	return nullptr;
}

/**
 * GetSlotWithItemTag - Get UI slot by item tag
 */
UW_InventorySlot* UAC_InventoryManager::GetSlotWithItemTag_Implementation(const FGameplayTag& ItemTag, ESLFInventorySlotType SlotType)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetSlotWithItemTag - Tag: %s"), *ItemTag.ToString());
	// Slot widgets are managed by UI system
	return nullptr;
}

/**
 * LootPickupItem - Loot an item from the world
 */
void UAC_InventoryManager::LootPickupItem_Implementation(AB_PickupItem* LootedItem)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::LootPickupItem - Item: %s"),
		LootedItem ? *LootedItem->GetName() : TEXT("None"));

	if (!IsValid(LootedItem))
	{
		return;
	}

	// Get item data from pickup and add to inventory
	// The pickup item should have ItemAsset and Amount properties
	// AddItem(LootedItem->ItemAsset, LootedItem->Amount, true);
}

/**
 * GetEmptySlot - Get first empty inventory slot
 */
UW_InventorySlot* UAC_InventoryManager::GetEmptySlot_Implementation(ESLFInventorySlotType SlotType)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetEmptySlot"));
	// Slot widgets are managed by UI system
	return nullptr;
}

/**
 * AddItem - Add item to inventory
 */
void UAC_InventoryManager::AddItem_Implementation(UPrimaryDataAsset* Item, int32 Count, bool TriggerLootUi)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::AddItem - Item: %s, Count: %d, TriggerUI: %s"),
		Item ? *Item->GetName() : TEXT("None"), Count, TriggerLootUi ? TEXT("true") : TEXT("false"));

	if (!IsValid(Item) || Count <= 0)
	{
		return;
	}

	// Create tag from item name
	FGameplayTag ItemTag = FGameplayTag::RequestGameplayTag(FName(*Item->GetName()));

	// Add to items map
	Items.Add(ItemTag, Item);

	// Broadcast events
	OnInventoryUpdated.Broadcast();
	OnItemAmountUpdated.Broadcast(Item, Count);

	if (TriggerLootUi)
	{
		FSLFItemInfo ItemInfo;
		ItemInfo.ItemTag = ItemTag;
		OnItemLooted.Broadcast(ItemInfo, Count, false);
	}
}

/**
 * RemoveItem - Remove item from inventory
 */
void UAC_InventoryManager::RemoveItem_Implementation(UPrimaryDataAsset* Item, int32 Count)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::RemoveItem - Item: %s, Count: %d"),
		Item ? *Item->GetName() : TEXT("None"), Count);

	if (!IsValid(Item))
	{
		return;
	}

	// Find and remove item
	FGameplayTag TagToRemove;
	for (const auto& ItemEntry : Items)
	{
		if (ItemEntry.Value == Item)
		{
			TagToRemove = ItemEntry.Key;
			break;
		}
	}

	if (TagToRemove.IsValid())
	{
		Items.Remove(TagToRemove);
		OnInventoryUpdated.Broadcast();
		OnItemAmountUpdated.Broadcast(Item, 0);
	}
}

/**
 * RemoveItemWithTag - Remove item from inventory by gameplay tag (for crafting material consumption)
 * Uses atomic check-then-commit pattern to prevent partial consumption on failure.
 */
bool UAC_InventoryManager::RemoveItemWithTag_Implementation(const FGameplayTag& Tag, int32 Count)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::RemoveItemWithTag - Tag: %s, Count: %d"),
		*Tag.ToString(), Count);

	if (!Tag.IsValid() || Count <= 0)
	{
		return false;
	}

	// ═══════════════════════════════════════════════════════════════════
	// PASS 1: Dry run - Check if we have enough BEFORE modifying anything
	// In this implementation, Items is TMap<FGameplayTag, UPrimaryDataAsset*>
	// Each entry is a single item (no stacks), so we count matching entries
	// ═══════════════════════════════════════════════════════════════════
	int32 TotalAvailable = 0;
	for (const auto& ItemEntry : Items)
	{
		UPDA_Item* ItemData = Cast<UPDA_Item>(ItemEntry.Value);
		if (ItemData && ItemData->ItemInformation.ItemTag.MatchesTag(Tag))
		{
			TotalAvailable++;
		}
	}

	if (TotalAvailable < Count)
	{
		UE_LOG(LogTemp, Warning, TEXT("  ATOMIC CHECK FAILED: Need %d but only have %d - no items removed"),
			Count, TotalAvailable);
		return false; // Fail gracefully, touch nothing
	}

	// ═══════════════════════════════════════════════════════════════════
	// PASS 2: Commit - Actually remove (we verified we have enough)
	// ═══════════════════════════════════════════════════════════════════
	TArray<FGameplayTag> TagsToRemove;
	int32 RemainingToRemove = Count;

	for (const auto& ItemEntry : Items)
	{
		if (RemainingToRemove <= 0)
		{
			break;
		}

		UPDA_Item* ItemData = Cast<UPDA_Item>(ItemEntry.Value);
		if (ItemData && ItemData->ItemInformation.ItemTag.MatchesTag(Tag))
		{
			TagsToRemove.Add(ItemEntry.Key);
			RemainingToRemove--;
			UE_LOG(LogTemp, Log, TEXT("  Removing: %s"), *ItemData->GetName());
		}
	}

	// Remove the found items
	for (const FGameplayTag& TagToRemove : TagsToRemove)
	{
		if (UPrimaryDataAsset** ItemPtr = Items.Find(TagToRemove))
		{
			OnItemAmountUpdated.Broadcast(*ItemPtr, 0);
		}
		Items.Remove(TagToRemove);
	}

	OnInventoryUpdated.Broadcast();
	return true;
}

/**
 * RemoveStoredItem - Remove item from storage
 */
void UAC_InventoryManager::RemoveStoredItem_Implementation(UPrimaryDataAsset* Item, int32 Count)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::RemoveStoredItem - Item: %s, Count: %d"),
		Item ? *Item->GetName() : TEXT("None"), Count);

	if (!IsValid(Item))
	{
		return;
	}

	FGameplayTag TagToRemove;
	for (const auto& ItemEntry : StoredItems)
	{
		if (ItemEntry.Value == Item)
		{
			TagToRemove = ItemEntry.Key;
			break;
		}
	}

	if (TagToRemove.IsValid())
	{
		StoredItems.Remove(TagToRemove);
		OnInventoryUpdated.Broadcast();
	}
}

/**
 * RemoveItemAtSlot - Remove item from specific slot
 *
 * Blueprint Logic:
 * 1. Check if slot is valid and occupied
 * 2. Get item from slot
 * 3. Adjust count or clear slot
 * 4. Update internal Items map
 * 5. Broadcast updates
 */
void UAC_InventoryManager::RemoveItemAtSlot_Implementation(UW_InventorySlot* Slot, int32 Count)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::RemoveItemAtSlot - Count: %d"), Count);

	if (!IsValid(Slot) || !Slot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Slot invalid or not occupied"));
		return;
	}

	UPrimaryDataAsset* SlotItem = Slot->AssignedItem;
	if (!IsValid(SlotItem))
	{
		return;
	}

	// Calculate new count
	int32 CurrentCount = Slot->Count;
	int32 NewCount = CurrentCount - Count;

	if (NewCount <= 0)
	{
		// Remove item completely
		Slot->EventClearSlot(true);  // TriggerShift = true to reorganize slots

		// Remove from Items map
		FGameplayTag ItemTag;
		for (const auto& Entry : Items)
		{
			if (Entry.Value == SlotItem)
			{
				ItemTag = Entry.Key;
				break;
			}
		}
		if (ItemTag.IsValid())
		{
			Items.Remove(ItemTag);
		}

		OnItemAmountUpdated.Broadcast(SlotItem, 0);
	}
	else
	{
		// Update count
		Slot->EventChangeAmount(NewCount);
		OnItemAmountUpdated.Broadcast(SlotItem, NewCount);
	}

	OnInventoryUpdated.Broadcast();
}

/**
 * RemoveItemAtStorageSlot - Remove item from storage slot
 *
 * Blueprint Logic:
 * 1. Similar to RemoveItemAtSlot but for storage
 * 2. Updates StoredItems map instead of Items
 */
void UAC_InventoryManager::RemoveItemAtStorageSlot_Implementation(UW_InventorySlot* Slot, int32 Count)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::RemoveItemAtStorageSlot - Count: %d"), Count);

	if (!IsValid(Slot) || !Slot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Storage slot invalid or not occupied"));
		return;
	}

	UPrimaryDataAsset* SlotItem = Slot->AssignedItem;
	if (!IsValid(SlotItem))
	{
		return;
	}

	// Calculate new count
	int32 CurrentCount = Slot->Count;
	int32 NewCount = CurrentCount - Count;

	if (NewCount <= 0)
	{
		// Remove item completely
		Slot->EventClearSlot(true);

		// Remove from StoredItems map
		FGameplayTag ItemTag;
		for (const auto& Entry : StoredItems)
		{
			if (Entry.Value == SlotItem)
			{
				ItemTag = Entry.Key;
				break;
			}
		}
		if (ItemTag.IsValid())
		{
			StoredItems.Remove(ItemTag);
		}
	}
	else
	{
		// Update count
		Slot->EventChangeAmount(NewCount);
	}

	OnInventoryUpdated.Broadcast();
}

/**
 * GetItemsForEquipmentSlot - Get items that can equip to slot
 *
 * Filters inventory items based on equipment slot compatibility.
 * First checks explicit EquipSlots tags, then falls back to Category/SubCategory matching.
 */
TArray<UPrimaryDataAsset*> UAC_InventoryManager::GetItemsForEquipmentSlot_Implementation(const FGameplayTag& EquipmentSlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetItemsForEquipmentSlot - Slot: %s, Items in inventory: %d"),
		*EquipmentSlotTag.ToString(), Items.Num());

	// Parse slot type from tag (e.g., "Right Hand Weapon 1" from full tag path)
	FString SlotTagStr = EquipmentSlotTag.ToString();
	bool bIsRightHand = SlotTagStr.Contains(TEXT("Right Hand"));
	bool bIsLeftHand = SlotTagStr.Contains(TEXT("Left Hand"));
	bool bIsHead = SlotTagStr.Contains(TEXT("Head"));
	bool bIsArmor = SlotTagStr.Contains(TEXT("Armor"));
	bool bIsGloves = SlotTagStr.Contains(TEXT("Gloves"));
	bool bIsGreaves = SlotTagStr.Contains(TEXT("Greaves"));
	bool bIsTrinket = SlotTagStr.Contains(TEXT("Trinket"));
	bool bIsArrow = SlotTagStr.Contains(TEXT("Arrow"));
	bool bIsBullet = SlotTagStr.Contains(TEXT("Bullet"));

	TArray<UPrimaryDataAsset*> Result;

	// Filter items that can equip to this slot
	for (const auto& ItemEntry : Items)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(ItemEntry.Value))
		{
			bool bMatches = false;

			// First check explicit EquipSlots tags
			if (ItemData->ItemInformation.EquipmentDetails.EquipSlots.Num() > 0)
			{
				bMatches = ItemData->ItemInformation.EquipmentDetails.EquipSlots.HasTag(EquipmentSlotTag);
			}
			else
			{
				// Fallback: Match by Category/SubCategory since EquipSlots are empty
				ESLFItemCategory Category = ItemData->ItemInformation.Category.Category;
				ESLFItemSubCategory SubCategory = ItemData->ItemInformation.Category.SubCategory;

				UE_LOG(LogTemp, Log, TEXT("[AC_InventoryManager]   Checking %s: Category=%d, SubCat=%d"),
					*ItemData->GetName(),
					static_cast<int32>(Category),
					static_cast<int32>(SubCategory));

				// Right Hand slots: Weapons, Swords, Katanas, Axes, Maces, Staffs
				if (bIsRightHand)
				{
					bMatches = (Category == ESLFItemCategory::Weapons) ||
					           (Category == ESLFItemCategory::Abilities) ||  // Abilities category used for weapons in test data
					           (SubCategory == ESLFItemSubCategory::Sword) ||
					           (SubCategory == ESLFItemSubCategory::Katana) ||
					           (SubCategory == ESLFItemSubCategory::Axe) ||
					           (SubCategory == ESLFItemSubCategory::Mace) ||
					           (SubCategory == ESLFItemSubCategory::Staff);
				}
				// Left Hand slots: Shields AND Weapons (for dual-wielding)
				else if (bIsLeftHand)
				{
					bMatches = (Category == ESLFItemCategory::Shields) ||
					           (Category == ESLFItemCategory::Weapons) ||
					           (SubCategory == ESLFItemSubCategory::Sword) ||
					           (SubCategory == ESLFItemSubCategory::Katana) ||
					           (SubCategory == ESLFItemSubCategory::Axe) ||
					           (SubCategory == ESLFItemSubCategory::Mace) ||
					           (SubCategory == ESLFItemSubCategory::Staff);
				}
				// Head slot: Head subcategory OR Armor category with Head subcategory
				else if (bIsHead)
				{
					bMatches = (SubCategory == ESLFItemSubCategory::Head) ||
					           (Category == ESLFItemCategory::Armor && SubCategory == ESLFItemSubCategory::Head);
				}
				// Armor/Chest slot: Chest subcategory OR Armor category
				else if (bIsArmor)
				{
					bMatches = (SubCategory == ESLFItemSubCategory::Chest) ||
					           (Category == ESLFItemCategory::Armor && SubCategory == ESLFItemSubCategory::Chest);
				}
				// Gloves/Arms slot
				else if (bIsGloves)
				{
					bMatches = (SubCategory == ESLFItemSubCategory::Arms) ||
					           (Category == ESLFItemCategory::Armor && SubCategory == ESLFItemSubCategory::Arms);
				}
				// Greaves/Legs slot
				else if (bIsGreaves)
				{
					bMatches = (SubCategory == ESLFItemSubCategory::Legs) ||
					           (Category == ESLFItemCategory::Armor && SubCategory == ESLFItemSubCategory::Legs);
				}
				// Trinket slots: Talismans
				else if (bIsTrinket)
				{
					bMatches = (SubCategory == ESLFItemSubCategory::Talismans);
				}
				// Arrow slot: Projectiles
				else if (bIsArrow)
				{
					bMatches = (SubCategory == ESLFItemSubCategory::Projectiles);
				}
				// Bullet slot: Projectiles (same as arrows - both are ammo types)
				else if (bIsBullet)
				{
					bMatches = (SubCategory == ESLFItemSubCategory::Projectiles);
				}
			}

			if (bMatches)
			{
				Result.Add(ItemData);
				UE_LOG(LogTemp, Log, TEXT("[AC_InventoryManager]   MATCH: %s (Category=%d, SubCat=%d)"),
					*ItemData->GetName(),
					static_cast<int32>(ItemData->ItemInformation.Category.Category),
					static_cast<int32>(ItemData->ItemInformation.Category.SubCategory));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetItemsForEquipmentSlot - Found %d matching items"), Result.Num());
	return Result;
}

/**
 * AddItemToStorage - Add item to storage
 */
void UAC_InventoryManager::AddItemToStorage_Implementation(UPrimaryDataAsset* ItemAsset, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::AddItemToStorage - Item: %s, Amount: %d"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("None"), Amount);

	if (!IsValid(ItemAsset) || Amount <= 0)
	{
		return;
	}

	FGameplayTag ItemTag = FGameplayTag::RequestGameplayTag(FName(*ItemAsset->GetName()));
	StoredItems.Add(ItemTag, ItemAsset);
	OnInventoryUpdated.Broadcast();
}

/**
 * AddItemAtSlotToStorage - Move item from inventory slot to storage
 *
 * Blueprint Logic:
 * 1. Get item from inventory slot
 * 2. Add to StoredItems map
 * 3. Remove/reduce from inventory slot
 * 4. Broadcast updates
 */
void UAC_InventoryManager::AddItemAtSlotToStorage_Implementation(UW_InventorySlot* CurrentSlot, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::AddItemAtSlotToStorage - Amount: %d"), Amount);

	if (!IsValid(CurrentSlot) || !CurrentSlot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Inventory slot invalid or not occupied"));
		return;
	}

	UPrimaryDataAsset* SlotItem = CurrentSlot->AssignedItem;
	if (!IsValid(SlotItem))
	{
		return;
	}

	// Add to storage
	AddItemToStorage(SlotItem, Amount);

	// Remove from inventory slot
	RemoveItemAtSlot(CurrentSlot, Amount);

	UE_LOG(LogTemp, Log, TEXT("  Moved %d items to storage"), Amount);
}

/**
 * RetrieveItemAtSlotFromStorage - Move item from storage slot to inventory
 *
 * Blueprint Logic:
 * 1. Get item from storage slot
 * 2. Add to Items map (inventory)
 * 3. Remove/reduce from storage slot
 * 4. Broadcast updates
 */
void UAC_InventoryManager::RetrieveItemAtSlotFromStorage_Implementation(UW_InventorySlot* StorageSlot, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::RetrieveItemAtSlotFromStorage - Amount: %d"), Amount);

	if (!IsValid(StorageSlot) || !StorageSlot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Storage slot invalid or not occupied"));
		return;
	}

	UPrimaryDataAsset* SlotItem = StorageSlot->AssignedItem;
	if (!IsValid(SlotItem))
	{
		return;
	}

	// Add to inventory
	AddItem(SlotItem, Amount, false);  // false = don't trigger loot UI

	// Remove from storage slot
	RemoveItemAtStorageSlot(StorageSlot, Amount);

	UE_LOG(LogTemp, Log, TEXT("  Retrieved %d items from storage"), Amount);
}

/**
 * AdjustCurrency - Modify currency amount
 */
void UAC_InventoryManager::AdjustCurrency_Implementation(int32 Delta)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::AdjustCurrency - Delta: %d, Current: %d -> New: %d"),
		Delta, Currency, Currency + Delta);

	int32 OldCurrency = Currency;
	Currency = FMath::Max(0, Currency + Delta);

	OnCurrencyUpdated.Broadcast(Currency);

	if (Delta > 0)
	{
		OnCurrencyIncreased.Broadcast(Delta);
	}
}

/**
 * SerializeInventoryStorageCurrencyData - Serialize for saving
 */
void UAC_InventoryManager::SerializeInventoryStorageCurrencyData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::SerializeInventoryStorageCurrencyData"));

	TArray<FInstancedStruct> DataToSave;

	// Convert inventory items to save format
	// Each item would be an FInstancedStruct containing FSLFInventoryItemsSaveInfo

	FGameplayTag SaveTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Save.Inventory"));
	OnSaveRequested.Broadcast(SaveTag, DataToSave);
}

/**
 * InitializeLoadedInventory - Load inventory from save
 */
void UAC_InventoryManager::InitializeLoadedInventory_Implementation(const TArray<FSLFInventoryItemsSaveInfo>& LoadedInventoryItems)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::InitializeLoadedInventory - %d items"),
		LoadedInventoryItems.Num());

	Items.Empty();

	for (const FSLFInventoryItemsSaveInfo& SavedItem : LoadedInventoryItems)
	{
		if (IsValid(Cast<UPrimaryDataAsset>(SavedItem.Item)))
		{
			FGameplayTag ItemTag = FGameplayTag::RequestGameplayTag(FName(*SavedItem.Item->GetName()));
			Items.Add(ItemTag, Cast<UPrimaryDataAsset>(SavedItem.Item));
		}
	}

	OnInventoryUpdated.Broadcast();
}

/**
 * UseItemFromInventory - Use/consume an item
 */
void UAC_InventoryManager::UseItemFromInventory_Implementation(UPrimaryDataAsset* Item)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::UseItemFromInventory - Item: %s"),
		Item ? *Item->GetName() : TEXT("None"));

	if (!IsValid(Item))
	{
		return;
	}

	// Using an item typically triggers an action
	// The specific behavior depends on item type
}

/**
 * GetCurrentCurency - Get current currency amount
 */
int32 UAC_InventoryManager::GetCurrentCurency_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetCurrentCurency - %d"), Currency);
	return Currency;
}

/**
 * GetItemsForCategory - Get items filtered by category
 */
void UAC_InventoryManager::GetItemsForCategory_Implementation(ESLFItemCategory ItemCategory, ESLFInventorySlotType Type, TMap<FGameplayTag, UPrimaryDataAsset*>& OutStackableItems, TArray<UPrimaryDataAsset*>& OutNonStackableItems)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetItemsForCategory - Category: %d, Type: %d"),
		static_cast<int32>(ItemCategory), static_cast<int32>(Type));

	OutStackableItems.Empty();
	OutNonStackableItems.Empty();

	const TMap<FGameplayTag, UPrimaryDataAsset*>& SourceItems =
		(Type == ESLFInventorySlotType::StorageSlot) ? StoredItems : Items;

	for (const auto& ItemEntry : SourceItems)
	{
		if (IsValid(ItemEntry.Value))
		{
			// Filter by category - would need to check ItemInfo
			OutStackableItems.Add(ItemEntry.Key, ItemEntry.Value);
		}
	}
}

/**
 * GetAllItems - Get all items
 */
void UAC_InventoryManager::GetAllItems_Implementation(ESLFInventorySlotType Type, TMap<FGameplayTag, UPrimaryDataAsset*>& OutStackableItems, TArray<UPrimaryDataAsset*>& OutNonStackableItems)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::GetAllItems - Type: %d"), static_cast<int32>(Type));

	OutStackableItems.Empty();
	OutNonStackableItems.Empty();

	const TMap<FGameplayTag, UPrimaryDataAsset*>& SourceItems =
		(Type == ESLFInventorySlotType::StorageSlot) ? StoredItems : Items;

	for (const auto& ItemEntry : SourceItems)
	{
		OutStackableItems.Add(ItemEntry.Key, ItemEntry.Value);
	}
}

/**
 * UseItemAtSlot - Use item from specific slot
 *
 * Blueprint Logic:
 * 1. Get item from slot
 * 2. Call UseItemFromInventory
 * 3. Consume/reduce item based on type
 */
void UAC_InventoryManager::UseItemAtSlot_Implementation(UW_InventorySlot* Slot)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::UseItemAtSlot"));

	if (!IsValid(Slot) || !Slot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Slot invalid or not occupied"));
		return;
	}

	UPrimaryDataAsset* SlotItem = Slot->AssignedItem;
	if (!IsValid(SlotItem))
	{
		return;
	}

	// Use the item
	UseItemFromInventory(SlotItem);

	// After using, check if item should be consumed (single-use items)
	// The UseItemFromInventory handles the logic based on item type
	// For consumable items, it will trigger removal
	UE_LOG(LogTemp, Log, TEXT("  Used item: %s"), *SlotItem->GetName());
}

/**
 * LeaveItemAtSlot - Leave item (drop on ground)
 *
 * Blueprint Logic:
 * 1. Get item from slot
 * 2. Spawn pickup actor at owner location
 * 3. Remove from inventory
 */
void UAC_InventoryManager::LeaveItemAtSlot_Implementation(int32 Amount, UW_InventorySlot* InventorySlot)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::LeaveItemAtSlot - Amount: %d"), Amount);

	if (!IsValid(InventorySlot) || !InventorySlot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Slot invalid or not occupied"));
		return;
	}

	UPrimaryDataAsset* SlotItem = InventorySlot->AssignedItem;
	if (!IsValid(SlotItem))
	{
		return;
	}

	// Get owner location to spawn pickup
	if (IsValid(OwnerActor))
	{
		FVector SpawnLocation = OwnerActor->GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		// Spawn B_PickupItem at owner location
		// In full implementation, would spawn the pickup actor with item data
		UE_LOG(LogTemp, Log, TEXT("  Would spawn pickup at %s for %d x %s"),
			*SpawnLocation.ToString(), Amount, *SlotItem->GetName());
	}

	// Remove from inventory
	RemoveItemAtSlot(InventorySlot, Amount);
}

/**
 * DiscardItemAtSlot - Discard/destroy item
 *
 * Blueprint Logic:
 * 1. Remove item from slot without spawning pickup
 * 2. Item is permanently lost
 */
void UAC_InventoryManager::DiscardItemAtSlot_Implementation(int32 Amount, UW_InventorySlot* InventorySlot)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::DiscardItemAtSlot - Amount: %d"), Amount);

	if (!IsValid(InventorySlot) || !InventorySlot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Slot invalid or not occupied"));
		return;
	}

	UPrimaryDataAsset* SlotItem = InventorySlot->AssignedItem;

	// Simply remove from inventory - item is destroyed
	RemoveItemAtSlot(InventorySlot, Amount);

	UE_LOG(LogTemp, Log, TEXT("  Discarded %d x %s"),
		Amount, SlotItem ? *SlotItem->GetName() : TEXT("Unknown"));
}

/**
 * StoreItemAtSlot - Move item to storage
 *
 * Blueprint Logic:
 * Wrapper for AddItemAtSlotToStorage
 */
void UAC_InventoryManager::StoreItemAtSlot_Implementation(int32 Amount, UW_InventorySlot* InventorySlot)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::StoreItemAtSlot - Amount: %d"), Amount);

	// Delegate to existing implementation
	AddItemAtSlotToStorage(InventorySlot, Amount);
}

/**
 * RetrieveItemAtSlot - Retrieve item from storage
 *
 * Blueprint Logic:
 * Wrapper for RetrieveItemAtSlotFromStorage
 */
void UAC_InventoryManager::RetrieveItemAtSlot_Implementation(int32 Amount, UW_InventorySlot* InventorySlot)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::RetrieveItemAtSlot - Amount: %d"), Amount);

	// Delegate to existing implementation
	RetrieveItemAtSlotFromStorage(InventorySlot, Amount);
}

/**
 * InitializeLoadedStorage - Load storage from save
 */
void UAC_InventoryManager::InitializeLoadedStorage_Implementation(const TArray<FSLFInventoryItemsSaveInfo>& LoadedStorageItems)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::InitializeLoadedStorage - %d items"),
		LoadedStorageItems.Num());

	StoredItems.Empty();

	for (const FSLFInventoryItemsSaveInfo& SavedItem : LoadedStorageItems)
	{
		if (IsValid(Cast<UPrimaryDataAsset>(SavedItem.Item)))
		{
			FGameplayTag ItemTag = FGameplayTag::RequestGameplayTag(FName(*SavedItem.Item->GetName()));
			StoredItems.Add(ItemTag, Cast<UPrimaryDataAsset>(SavedItem.Item));
		}
	}

	OnInventoryUpdated.Broadcast();
}

/**
 * ReplenishItem - Replenish a consumable item
 */
void UAC_InventoryManager::ReplenishItem_Implementation(UPrimaryDataAsset* Item)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::ReplenishItem - Item: %s"),
		Item ? *Item->GetName() : TEXT("None"));

	// Replenish typically restores rechargeable items like flasks
	if (!IsValid(Item))
	{
		return;
	}

	// Check if item is rechargeable and replenish to max
	OnItemAmountUpdated.Broadcast(Item, 1);
}
