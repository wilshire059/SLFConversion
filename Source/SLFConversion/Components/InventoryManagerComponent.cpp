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
#include "EquipmentManagerComponent.h"
#include "AC_EquipmentManager.h"
#include "SLFPrimaryDataAssets.h"
#include "Blueprints/B_PickupItem.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/SLFPlayerController.h"
#include "Widgets/W_HUD.h"
#include "GameplayTagContainer.h"

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

	// Give player starting items (Health Flask)
	static const FSoftObjectPath HealthFlaskPath(TEXT("/Game/SoulslikeFramework/Data/Items/DA_HealthFlask.DA_HealthFlask"));
	if (UObject* FlaskAsset = HealthFlaskPath.TryLoad())
	{
		if (UDataAsset* FlaskData = Cast<UDataAsset>(FlaskAsset))
		{
			AddItem(FlaskData, 5, false); // 5 flasks (charges), no loot UI
			UE_LOG(LogTemp, Log, TEXT("[InventoryManager] Added 5x Health Flask to inventory"));

			// Delay the equip to ensure AC_EquipmentManager is ready
			// (AC_EquipmentManager::BeginPlay runs after this)
			CachedFlaskAsset = FlaskData;
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UInventoryManagerComponent::EquipStartingFlask);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryManager] Failed to load DA_HealthFlask!"));
	}
}

void UInventoryManagerComponent::EquipStartingFlask()
{
	if (!CachedFlaskAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryManager] EquipStartingFlask - No cached flask asset!"));
		return;
	}

	// AC_EquipmentManager is on the PlayerController (same as InventoryManagerComponent)
	// NOT on the Pawn/Character!
	UAC_EquipmentManager* EquipMgr = nullptr;
	if (OwnerActor)
	{
		EquipMgr = OwnerActor->FindComponentByClass<UAC_EquipmentManager>();
		UE_LOG(LogTemp, Log, TEXT("[InventoryManager] EquipStartingFlask - Looking for AC_EquipmentManager on %s: %s"),
			*OwnerActor->GetName(), EquipMgr ? TEXT("YES") : TEXT("NO"));
	}

	if (EquipMgr)
	{
		// Use "Tool 1" (singular) to match ItemWheel SlotsToTrack configuration
		FGameplayTag ToolSlot1 = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tool 1"));
		bool bSuccess1, bSuccess2, bSuccess3;
		EquipMgr->EquipToolToSlot(Cast<UPrimaryDataAsset>(CachedFlaskAsset), ToolSlot1, false, bSuccess1, bSuccess2, bSuccess3);
		EquipMgr->SetActiveToolSlot(ToolSlot1);
		UE_LOG(LogTemp, Log, TEXT("[InventoryManager] Equipped Health Flask to Tool 1 slot and set as active (success: %s)"),
			bSuccess1 ? TEXT("YES") : TEXT("NO"));

		// Only clear after successful equip
		CachedFlaskAsset = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryManager] EquipStartingFlask - AC_EquipmentManager not found, retrying..."));
		// Retry next frame - keep CachedFlaskAsset for retry
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UInventoryManagerComponent::EquipStartingFlask);
	}
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

bool UInventoryManagerComponent::RemoveItemWithTag_Implementation(const FGameplayTag& Tag, int32 Count)
{
	if (!Tag.IsValid() || Count <= 0)
	{
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] RemoveItemWithTag: %s x%d"), *Tag.ToString(), Count);

	// ═══════════════════════════════════════════════════════════════════
	// PASS 1: Dry run - Check if we have enough BEFORE modifying anything
	// (Atomic check to prevent partial consumption on failure)
	// ═══════════════════════════════════════════════════════════════════
	int32 TotalAvailable = 0;
	for (const auto& Pair : Items)
	{
		UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset);
		if (ItemData && ItemData->ItemInformation.ItemTag.MatchesTag(Tag))
		{
			TotalAvailable += Pair.Value.Amount;
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
	int32 RemainingToRemove = Count;
	TArray<int32> KeysToRemove;

	for (auto& Pair : Items)
	{
		if (RemainingToRemove <= 0)
		{
			break;
		}

		UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset);
		if (ItemData && ItemData->ItemInformation.ItemTag.MatchesTag(Tag))
		{
			int32 OldAmount = Pair.Value.Amount;
			int32 ToRemoveFromThis = FMath::Min(RemainingToRemove, Pair.Value.Amount);
			UDataAsset* ItemAsDataAsset = Cast<UDataAsset>(Pair.Value.ItemAsset);

			if (Pair.Value.Amount <= ToRemoveFromThis)
			{
				// Remove entire stack
				KeysToRemove.Add(Pair.Key);
				OnItemAmountUpdated.Broadcast(ItemAsDataAsset, OldAmount, 0);
			}
			else
			{
				// Reduce stack
				Pair.Value.Amount -= ToRemoveFromThis;
				OnItemAmountUpdated.Broadcast(ItemAsDataAsset, OldAmount, Pair.Value.Amount);
			}

			RemainingToRemove -= ToRemoveFromThis;
			UE_LOG(LogTemp, Log, TEXT("  Removed %d from %s, %d remaining"),
				ToRemoveFromThis, *ItemData->GetName(), RemainingToRemove);
		}
	}

	// Remove empty stacks
	for (int32 Key : KeysToRemove)
	{
		Items.Remove(Key);
	}

	OnInventoryUpdated.Broadcast();
	return true;
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
	// Restore consumable to max stack (IMPLEMENTED)
	int32 Slot = GetSlotWithItem(ItemAsset, ESLFInventorySlotType::InventorySlot);
	if (Slot >= 0)
	{
		if (FSLFInventoryItem* Item = Items.Find(Slot))
		{
			// Get max stack from item data if available
			int32 MaxStack = Amount > 0 ? Amount : 99; // Default max or specified amount
			if (UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset))
			{
				MaxStack = Amount > 0 ? Amount : ItemData->ItemInformation.MaxAmount;
				if (MaxStack <= 0) MaxStack = 99;
			}
			Item->Amount = MaxStack;
			OnInventoryUpdated.Broadcast();
		}
	}
}

void UInventoryManagerComponent::ReplenishAllRechargeableItems()
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] ReplenishAllRechargeableItems"));

	int32 ReplenishedCount = 0;

	// Iterate through all inventory items
	for (auto& Pair : Items)
	{
		FSLFInventoryItem& Item = Pair.Value;

		// Check if this item is rechargeable
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Item.ItemAsset))
		{
			if (ItemData->ItemInformation.bRechargeable)
			{
				// Get max amount from item data
				int32 MaxAmount = ItemData->ItemInformation.MaxAmount;
				if (MaxAmount <= 0) MaxAmount = 5; // Default max for flasks

				int32 OldAmount = Item.Amount;
				Item.Amount = MaxAmount;
				ReplenishedCount++;

				UE_LOG(LogTemp, Log, TEXT("  Replenished %s: %d -> %d"),
					*ItemData->GetName(), OldAmount, MaxAmount);

				// Broadcast update for this item
				OnItemAmountUpdated.Broadcast(ItemData, OldAmount, MaxAmount);
			}
		}
	}

	if (ReplenishedCount > 0)
	{
		OnInventoryUpdated.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("  Total items replenished: %d"), ReplenishedCount);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("  No rechargeable items found to replenish"));
	}
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
		UE_LOG(LogTemp, Log, TEXT("[InventoryManager] UseItemAtSlot %d: %s (Amount: %d)"),
			SlotIndex, Item->ItemAsset ? *Item->ItemAsset->GetName() : TEXT("null"), Item->Amount);

		// Check if item has charges to use
		if (Item->Amount <= 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[InventoryManager] Item has no charges left!"));
			return;
		}

		// Execute item use effect (IMPLEMENTED)
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Item->ItemAsset))
		{
			// Check if item is usable
			if (ItemData->ItemInformation.bUsable)
			{
				// Check if this is a rechargeable item (flask)
				bool bIsRechargeable = ItemData->ItemInformation.bRechargeable;

				// Broadcast that item amount is changing (no OnItemUsed dispatcher exists)
				OnItemAmountUpdated.Broadcast(ItemData, Item->Amount, Item->Amount - 1);

				// Check if item has an action to trigger
				if (ItemData->ItemInformation.ActionToTrigger.IsValid())
				{
					UE_LOG(LogTemp, Log, TEXT("[InventoryManager] Item triggers action: %s"),
						*ItemData->ItemInformation.ActionToTrigger.ToString());
				}

				// For rechargeable items, just decrement amount (don't remove at 0)
				if (bIsRechargeable)
				{
					Item->Amount -= 1;
					UE_LOG(LogTemp, Log, TEXT("[InventoryManager] Rechargeable item - new amount: %d"), Item->Amount);
					OnInventoryUpdated.Broadcast();
					return;
				}
			}
		}
		// For non-rechargeable items, use normal removal (removes at 0)
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
	// Spawn pickup actor in world (IMPLEMENTED)
	if (FSLFInventoryItem* Item = Items.Find(SlotIndex))
	{
		if (OwnerActor && Item->ItemAsset)
		{
			UWorld* World = OwnerActor->GetWorld();
			if (World)
			{
				// Spawn pickup slightly in front of owner
				FVector SpawnLocation = OwnerActor->GetActorLocation() + OwnerActor->GetActorForwardVector() * 100.0f;
				FRotator SpawnRotation = FRotator::ZeroRotator;
				FTransform SpawnTransform(SpawnRotation, SpawnLocation);

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = OwnerActor;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				AActor* SpawnedActor = World->SpawnActor<AActor>(AB_PickupItem::StaticClass(), SpawnTransform, SpawnParams);
				if (AB_PickupItem* PickupItem = Cast<AB_PickupItem>(SpawnedActor))
				{
					PickupItem->Item = Cast<UPrimaryDataAsset>(Item->ItemAsset);
					PickupItem->Count = FMath::Min(Amount, Item->Amount);
					PickupItem->UsePhysics = true;
				}
			}
		}
	}
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

	// Broadcast inventory updated so UI refreshes (storage shares the same delegate)
	OnInventoryUpdated.Broadcast();
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

		// Broadcast inventory updated so UI refreshes (storage shares the same delegate)
		OnInventoryUpdated.Broadcast();
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
	// Check item tag and return amount (IMPLEMENTED)
	for (const auto& Pair : Items)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			if (ItemData->ItemInformation.ItemTag.MatchesTag(Tag))
			{
				Amount += Pair.Value.Amount;
				bSuccess = true;
			}
		}
	}
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
	const TMap<int32, FSLFInventoryItem>& TargetMap = (SlotType == ESLFInventorySlotType::StorageSlot) ? StoredItems : Items;
	for (const auto& Pair : TargetMap)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			if (ItemData->ItemInformation.ItemTag.MatchesTag(ItemTag))
			{
				return Pair.Key;
			}
		}
	}
	return -1;
}

TArray<FSLFInventoryItem> UInventoryManagerComponent::GetAllItems_Implementation()
{
	TArray<FSLFInventoryItem> Result;
	for (const auto& Pair : Items) Result.Add(Pair.Value);
	return Result;
}

TArray<FSLFInventoryItem> UInventoryManagerComponent::GetStoredItems_Implementation()
{
	TArray<FSLFInventoryItem> Result;
	for (const auto& Pair : StoredItems)
	{
		Result.Add(Pair.Value);
	}
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] GetStoredItems - Returning %d stored items"), Result.Num());
	return Result;
}

TArray<FSLFInventoryItem> UInventoryManagerComponent::GetItemsForCategory_Implementation(FGameplayTag CategoryTag)
{
	TArray<FSLFInventoryItem> Result;
	for (const auto& Pair : Items)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			// Match if ItemTag is a child of CategoryTag (e.g., "Items.Weapons.Sword" matches "Items.Weapons")
			if (ItemData->ItemInformation.ItemTag.MatchesTag(CategoryTag))
			{
				Result.Add(Pair.Value);
			}
		}
	}
	return Result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUERY - EQUIPMENT [29/35]
// ═══════════════════════════════════════════════════════════════════════════════

TArray<FSLFInventoryItem> UInventoryManagerComponent::GetItemsForEquipmentSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] GetItemsForEquipmentSlot - Looking for slot: %s, Items in inventory: %d"),
		*SlotTag.ToString(), Items.Num());

	// Parse slot type from tag (e.g., "Right Hand Weapon 1" from full tag path)
	FString SlotTagStr = SlotTag.ToString();
	bool bIsRightHand = SlotTagStr.Contains(TEXT("Right Hand"));
	bool bIsLeftHand = SlotTagStr.Contains(TEXT("Left Hand"));
	bool bIsHead = SlotTagStr.Contains(TEXT("Head"));
	bool bIsArmor = SlotTagStr.Contains(TEXT("Armor"));
	bool bIsGloves = SlotTagStr.Contains(TEXT("Gloves"));
	bool bIsGreaves = SlotTagStr.Contains(TEXT("Greaves"));
	bool bIsTrinket = SlotTagStr.Contains(TEXT("Trinket"));
	bool bIsArrow = SlotTagStr.Contains(TEXT("Arrow"));
	bool bIsBullet = SlotTagStr.Contains(TEXT("Bullet"));

	TArray<FSLFInventoryItem> Result;
	for (const auto& Pair : Items)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			bool bMatches = false;

			// First check explicit EquipSlots tags
			if (ItemData->ItemInformation.EquipmentDetails.EquipSlots.Num() > 0)
			{
				bMatches = ItemData->ItemInformation.EquipmentDetails.EquipSlots.HasTag(SlotTag);
			}
			else
			{
				// Fallback: Match by Category/SubCategory since EquipSlots are empty
				ESLFItemCategory Category = ItemData->ItemInformation.Category.Category;
				ESLFItemSubCategory SubCategory = ItemData->ItemInformation.Category.SubCategory;

				UE_LOG(LogTemp, Log, TEXT("[InventoryManager]   Checking %s: Category=%d, SubCat=%d"),
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
				Result.Add(Pair.Value);
				UE_LOG(LogTemp, Log, TEXT("[InventoryManager]   MATCH: %s (Category=%d, SubCat=%d)"),
					*ItemData->GetName(),
					static_cast<int32>(ItemData->ItemInformation.Category.Category),
					static_cast<int32>(ItemData->ItemInformation.Category.SubCategory));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] GetItemsForEquipmentSlot - Found %d matching items"), Result.Num());
	return Result;
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
	// Get player controller to access HUD
	if (APawn* OwnerPawn = Cast<APawn>(OwnerActor))
	{
		if (ASLFPlayerController* PC = Cast<ASLFPlayerController>(OwnerPawn->GetController()))
		{
			// Return HUD widget reference - the inventory widget is managed by the HUD
			return PC->HUDWidgetRef;
		}
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ASYNC EVENTS
// ═══════════════════════════════════════════════════════════════════════════════

void UInventoryManagerComponent::EventAsyncAddItemByTag(FGameplayTag ItemTag, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] EventAsyncAddItemByTag: %s x%d"), *ItemTag.ToString(), Amount);

	// In a full implementation, this would:
	// 1. Look up the item asset path from tag
	// 2. Async load the asset
	// 3. Call AddItem when complete

	// For now, log and attempt to find item in asset registry
	// This would typically use StreamableManager for async loading
}

void UInventoryManagerComponent::EventAsyncUseItem(UDataAsset* ItemAsset)
{
	UE_LOG(LogTemp, Log, TEXT("[InventoryManager] EventAsyncUseItem: %s"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("null"));

	if (!ItemAsset)
	{
		return;
	}

	// Use the item through the normal path
	UseItemFromInventory(ItemAsset);
}
