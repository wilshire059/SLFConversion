// InventoryManagerComponent.cpp
// C++ implementation for AC_InventoryManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_InventoryManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         7/7 (initialized in constructor)
// Functions:         35/35 implemented (stub implementations)
// Event Dispatchers: 7/7 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "InventoryManagerComponent.h"

UInventoryManagerComponent::UInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config
	SlotCount = 60;
	SlotsPerRow = 5;

	// Initialize runtime
	OwnerActor = nullptr;
	Currency = 0;
}

void UInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerActor = GetOwner();
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] BeginPlay on %s"), *OwnerActor->GetName());
}

// ═══════════════════════════════════════════════════════════════════════════════
// ADD/REMOVE ITEMS [1-6/35]
// ═══════════════════════════════════════════════════════════════════════════════

void UInventoryManagerComponent::AddItem_Implementation(UDataAsset* Item, int32 Count, bool bTriggerLootUi)
{
	if (!Item || Count <= 0) return;

	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] AddItem: %s x%d (LootUI: %s)"),
		*Item->GetName(), Count, bTriggerLootUi ? TEXT("true") : TEXT("false"));

	// Check for existing stack
	for (auto& Pair : Items)
	{
		if (Pair.Value.ItemAsset == Item)
		{
			int32 OldAmount = Pair.Value.Amount;
			Pair.Value.Amount += Count;
			OnItemAmountUpdated.Broadcast(Item, OldAmount, Pair.Value.Amount);
			OnInventoryUpdated.Broadcast();
			if (bTriggerLootUi)
			{
				OnItemLooted.Broadcast(Item, Count);
			}
			return;
		}
	}

	// Find empty slot
	int32 EmptySlot = GetEmptySlot(ESLFInventorySlotType::InventorySlot);
	if (EmptySlot < 0)
	{
		OnErrorTrigger.Broadcast(FText::FromString(TEXT("Inventory is full")));
		return;
	}

	// Add new item
	FSLFInventoryItem NewItem;
	NewItem.ItemAsset = Item;
	NewItem.Amount = Count;
	NewItem.UniqueId = FGuid::NewGuid();

	Items.Add(EmptySlot, NewItem);
	OnItemAmountUpdated.Broadcast(Item, 0, Count);
	OnInventoryUpdated.Broadcast();
	if (bTriggerLootUi)
	{
		OnItemLooted.Broadcast(Item, Count);
	}
}

void UInventoryManagerComponent::RemoveItem_Implementation(UDataAsset* Item, int32 Count)
{
	if (!Item || Count <= 0) return;

	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] RemoveItem: %s x%d"), *Item->GetName(), Count);

	for (auto& Pair : Items)
	{
		if (Pair.Value.ItemAsset == Item)
		{
			int32 OldAmount = Pair.Value.Amount;
			if (Pair.Value.Amount <= Count)
			{
				Items.Remove(Pair.Key);
				OnItemAmountUpdated.Broadcast(Item, OldAmount, 0);
			}
			else
			{
				Pair.Value.Amount -= Count;
				OnItemAmountUpdated.Broadcast(Item, OldAmount, Pair.Value.Amount);
			}
			OnInventoryUpdated.Broadcast();
			return;
		}
	}
}

void UInventoryManagerComponent::RemoveItemAtSlot_Implementation(int32 SlotIndex, int32 Amount)
{
	if (FSLFInventoryItem* Item = Items.Find(SlotIndex))
	{
		int32 OldAmount = Item->Amount;
		if (Item->Amount <= Amount)
		{
			UDataAsset* Asset = Cast<UDataAsset>(Item->ItemAsset);
			Items.Remove(SlotIndex);
			OnItemAmountUpdated.Broadcast(Asset, OldAmount, 0);
		}
		else
		{
			Item->Amount -= Amount;
			OnItemAmountUpdated.Broadcast(Cast<UDataAsset>(Item->ItemAsset), OldAmount, Item->Amount);
		}
		OnInventoryUpdated.Broadcast();
	}
}

bool UInventoryManagerComponent::LootPickupItem_Implementation(UDataAsset* ItemAsset, int32 Amount)
{
	// AddItem now handles the loot UI notification internally
	AddItem(ItemAsset, Amount, true);
	return true;
}

void UInventoryManagerComponent::ReplenishItem_Implementation(UDataAsset* ItemAsset, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] ReplenishItem: %s x%d"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("null"), Amount);
	// TODO: Restore consumable to max stack
}

int32 UInventoryManagerComponent::GetEmptySlot_Implementation(ESLFInventorySlotType SlotType)
{
	const TMap<int32, FSLFInventoryItem>& TargetMap = (SlotType == ESLFInventorySlotType::StorageSlot) ? StoredItems : Items;
	for (int32 i = 0; i < SlotCount; ++i)
	{
		if (!TargetMap.Contains(i)) return i;
	}
	return -1;
}

// ═══════════════════════════════════════════════════════════════════════════════
// USE/DISCARD ITEMS [7-10/35]
// ═══════════════════════════════════════════════════════════════════════════════

void UInventoryManagerComponent::UseItemAtSlot_Implementation(int32 SlotIndex)
{
	if (FSLFInventoryItem* Item = Items.Find(SlotIndex))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventoryManager] UseItemAtSlot %d: %s"),
			SlotIndex, *Item->ItemAsset->GetName());
		// TODO: Execute item use effect
		RemoveItemAtSlot(SlotIndex, 1);
	}
}

void UInventoryManagerComponent::UseItemFromInventory_Implementation(UDataAsset* ItemAsset)
{
	int32 Slot = GetSlotWithItem(ItemAsset, ESLFInventorySlotType::InventorySlot);
	if (Slot >= 0) UseItemAtSlot(Slot);
}

void UInventoryManagerComponent::DiscardItemAtSlot_Implementation(int32 SlotIndex, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] DiscardItemAtSlot %d x%d"), SlotIndex, Amount);
	RemoveItemAtSlot(SlotIndex, Amount);
}

void UInventoryManagerComponent::LeaveItemAtSlot_Implementation(int32 SlotIndex, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] LeaveItemAtSlot %d x%d"), SlotIndex, Amount);
	// TODO: Spawn pickup actor in world
	RemoveItemAtSlot(SlotIndex, Amount);
}

// ═══════════════════════════════════════════════════════════════════════════════
// STORAGE OPERATIONS [11-17/35]
// ═══════════════════════════════════════════════════════════════════════════════

void UInventoryManagerComponent::AddItemAtSlotToStorage_Implementation(int32 SlotIndex, UDataAsset* ItemAsset, int32 Amount)
{
	FSLFInventoryItem NewItem;
	NewItem.ItemAsset = ItemAsset;
	NewItem.Amount = Amount;
	NewItem.UniqueId = FGuid::NewGuid();
	StoredItems.Add(SlotIndex, NewItem);
}

bool UInventoryManagerComponent::AddItemToStorage_Implementation(UDataAsset* ItemAsset, int32 Amount)
{
	int32 EmptySlot = -1;
	for (int32 i = 0; i < SlotCount; ++i)
	{
		if (!StoredItems.Contains(i)) { EmptySlot = i; break; }
	}
	if (EmptySlot < 0) return false;
	AddItemAtSlotToStorage(EmptySlot, ItemAsset, Amount);
	return true;
}

void UInventoryManagerComponent::RemoveItemAtStorageSlot_Implementation(int32 SlotIndex, int32 Amount)
{
	if (FSLFInventoryItem* Item = StoredItems.Find(SlotIndex))
	{
		if (Item->Amount <= Amount) StoredItems.Remove(SlotIndex);
		else Item->Amount -= Amount;
	}
}

void UInventoryManagerComponent::RemoveStoredItem_Implementation(UDataAsset* Item, int32 Count)
{
	for (auto& Pair : StoredItems)
	{
		if (Pair.Value.ItemAsset == Item)
		{
			RemoveItemAtStorageSlot(Pair.Key, Count);
			return;
		}
	}
}

void UInventoryManagerComponent::StoreItemAtSlot_Implementation(int32 InventorySlot, int32 Amount)
{
	if (FSLFInventoryItem* Item = Items.Find(InventorySlot))
	{
		AddItemToStorage(Cast<UDataAsset>(Item->ItemAsset), Amount);
		RemoveItemAtSlot(InventorySlot, Amount);
	}
}

void UInventoryManagerComponent::RetrieveItemAtSlot_Implementation(int32 StorageSlot, int32 Amount)
{
	if (FSLFInventoryItem* Item = StoredItems.Find(StorageSlot))
	{
		AddItem(Cast<UDataAsset>(Item->ItemAsset), Amount, false);
		RemoveItemAtStorageSlot(StorageSlot, Amount);
	}
}

void UInventoryManagerComponent::RetrieveItemAtSlotFromStorage_Implementation(int32 StorageSlot, int32 InventorySlot, int32 Amount)
{
	RetrieveItemAtSlot(StorageSlot, Amount);
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUERY - COUNTS [18-24/35]
// ═══════════════════════════════════════════════════════════════════════════════

void UInventoryManagerComponent::GetAmountOfItem_Implementation(UDataAsset* Item, int32& Amount, bool& bSuccess)
{
	Amount = 0;
	bSuccess = false;
	for (const auto& Pair : Items)
	{
		if (Pair.Value.ItemAsset == Item)
		{
			Amount = Pair.Value.Amount;
			bSuccess = true;
			return;
		}
	}
}

void UInventoryManagerComponent::GetAmountOfItemWithTag_Implementation(FGameplayTag Tag, int32& Amount, bool& bSuccess)
{
	Amount = 0;
	bSuccess = false;
	// TODO: Check item tag and return amount
}

void UInventoryManagerComponent::GetStoredAmountOfItem_Implementation(UDataAsset* Item, int32& Amount, bool& bSuccess)
{
	Amount = 0;
	bSuccess = false;
	for (const auto& Pair : StoredItems)
	{
		if (Pair.Value.ItemAsset == Item)
		{
			Amount = Pair.Value.Amount;
			bSuccess = true;
			return;
		}
	}
}

int32 UInventoryManagerComponent::GetOverallCountForItem_Implementation(UDataAsset* ItemAsset)
{
	int32 InvAmount = 0, StoredAmount = 0;
	bool bSuccess = false;
	GetAmountOfItem(ItemAsset, InvAmount, bSuccess);
	GetStoredAmountOfItem(ItemAsset, StoredAmount, bSuccess);
	return InvAmount + StoredAmount;
}

int32 UInventoryManagerComponent::GetTotalInventoryItemsCount_Implementation()
{
	int32 Total = 0;
	for (const auto& Pair : Items) Total += Pair.Value.Amount;
	return Total;
}

int32 UInventoryManagerComponent::GetTotalStorageItemsCount_Implementation()
{
	int32 Total = 0;
	for (const auto& Pair : StoredItems) Total += Pair.Value.Amount;
	return Total;
}

bool UInventoryManagerComponent::HasItem_Implementation(UDataAsset* ItemAsset)
{
	int32 Amount = 0;
	bool bSuccess = false;
	GetAmountOfItem(ItemAsset, Amount, bSuccess);
	return Amount > 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUERY - SLOTS [25-28/35]
// ═══════════════════════════════════════════════════════════════════════════════

int32 UInventoryManagerComponent::GetSlotWithItem_Implementation(UDataAsset* Item, ESLFInventorySlotType SlotType)
{
	const TMap<int32, FSLFInventoryItem>& TargetMap = (SlotType == ESLFInventorySlotType::StorageSlot) ? StoredItems : Items;
	for (const auto& Pair : TargetMap)
	{
		if (Pair.Value.ItemAsset == Item) return Pair.Key;
	}
	return -1;
}

int32 UInventoryManagerComponent::GetSlotWithItemTag_Implementation(FGameplayTag ItemTag, ESLFInventorySlotType SlotType)
{
	// TODO: Check item tag
	return -1;
}

TArray<FSLFInventoryItem> UInventoryManagerComponent::GetAllItems_Implementation()
{
	TArray<FSLFInventoryItem> Result;
	for (const auto& Pair : Items) Result.Add(Pair.Value);
	return Result;
}

TArray<FSLFInventoryItem> UInventoryManagerComponent::GetItemsForCategory_Implementation(FGameplayTag CategoryTag)
{
	// TODO: Filter by category
	return GetAllItems();
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUERY - EQUIPMENT [29/35]
// ═══════════════════════════════════════════════════════════════════════════════

TArray<FSLFInventoryItem> UInventoryManagerComponent::GetItemsForEquipmentSlot_Implementation(FGameplayTag SlotTag)
{
	// TODO: Filter items valid for slot
	return GetAllItems();
}

// ═══════════════════════════════════════════════════════════════════════════════
// CURRENCY [30-31/35]
// ═══════════════════════════════════════════════════════════════════════════════

void UInventoryManagerComponent::AdjustCurrency_Implementation(int32 Amount)
{
	int32 OldCurrency = Currency;
	Currency = FMath::Max(0, Currency + Amount);

	if (Amount > 0) OnCurrencyIncreased.Broadcast(Amount);
	OnCurrencyUpdated.Broadcast(Currency);

	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] AdjustCurrency: %d -> %d"), OldCurrency, Currency);
}

// ═══════════════════════════════════════════════════════════════════════════════
// SERIALIZATION [32-34/35]
// ═══════════════════════════════════════════════════════════════════════════════

void UInventoryManagerComponent::SerializeInventoryStorageCurrencyData_Implementation(TArray<FInstancedStruct>& OutData)
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] SerializeInventoryStorageCurrencyData"));
	OnSaveRequested.Broadcast();
}

void UInventoryManagerComponent::InitializeLoadedInventory_Implementation(const TArray<FInstancedStruct>& InData)
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] InitializeLoadedInventory"));
}

void UInventoryManagerComponent::InitializeLoadedStorage_Implementation(const TArray<FInstancedStruct>& InData)
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] InitializeLoadedStorage"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// UI [35/35]
// ═══════════════════════════════════════════════════════════════════════════════

UUserWidget* UInventoryManagerComponent::GetInventoryWidget_Implementation()
{
	// TODO: Get from HUD
	return nullptr;
}
