// InventoryManagerComponent.h
// C++ base class for AC_InventoryManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_InventoryManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         7/7 migrated (excluding 7 dispatchers counted separately)
// Functions:         35/35 migrated (excluding ExecuteUbergraph and async callbacks)
// Event Dispatchers: 7/7 migrated
// Graphs:            43 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_InventoryManager
//
// PURPOSE: Inventory system - manages items, storage, currency, loot

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "InventoryManagerComponent.generated.h"

// Forward declarations
class UDataAsset;
class UUserWidget;

// Types used from SLFEnums.h/SLFGameTypes.h:
// - ESLFItemCategory
// - FSLFInventoryItem (if defined there, otherwise keep local)

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 7/7 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/7] Called when inventory contents change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

/** [2/7] Called when currency amount changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrencyUpdated, int32, NewAmount);

/** [3/7] Called when item is looted */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemLooted, UDataAsset*, ItemAsset, int32, Amount);

/** [4/7] Called when save is requested */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventorySaveRequested);

/** [5/7] Called when item amount changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemAmountUpdated, UDataAsset*, ItemAsset, int32, OldAmount, int32, NewAmount);

/** [6/7] Called when an error occurs (full inventory, etc.) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryErrorTrigger, FText, ErrorMessage);

/** [7/7] Called when currency is increased (for UI feedback) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrencyIncreased, int32, AmountAdded);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 7/7 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Config (3) ---

	/** [1/7] Available item categories */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FGameplayTag> Categories;

	/** [2/7] Maximum inventory slot count */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 SlotCount;

	/** [3/7] Number of slots per row (for UI) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 SlotsPerRow;

	// --- Runtime (4) ---

	/** [4/7] Owner actor reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	AActor* OwnerActor;

	/** [5/7] Main inventory items by slot index */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<int32, FSLFInventoryItem> Items;

	/** [6/7] Stored items (chest/stash) by slot index */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<int32, FSLFInventoryItem> StoredItems;

	/** [7/7] Current currency amount */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	int32 Currency;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 7/7 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/7] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventoryUpdated OnInventoryUpdated;

	/** [2/7] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCurrencyUpdated OnCurrencyUpdated;

	/** [3/7] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemLooted OnItemLooted;

	/** [4/7] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventorySaveRequested OnSaveRequested;

	/** [5/7] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemAmountUpdated OnItemAmountUpdated;

	/** [6/7] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventoryErrorTrigger OnErrorTrigger;

	/** [7/7] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCurrencyIncreased OnCurrencyIncreased;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 35/35 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Add/Remove Items (6) ---

	/** [1/35] Add item to inventory */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Items")
	void AddItem(UDataAsset* Item, int32 Count, bool bTriggerLootUi);
	virtual void AddItem_Implementation(UDataAsset* Item, int32 Count, bool bTriggerLootUi);

	/** [2/35] Remove item from inventory by asset */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Items")
	void RemoveItem(UDataAsset* Item, int32 Count);
	virtual void RemoveItem_Implementation(UDataAsset* Item, int32 Count);

	/** [3/35] Remove item at specific slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Items")
	void RemoveItemAtSlot(int32 SlotIndex, int32 Amount);
	virtual void RemoveItemAtSlot_Implementation(int32 SlotIndex, int32 Amount);

	/** [4/35] Loot pickup item (from world) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Items")
	bool LootPickupItem(UDataAsset* ItemAsset, int32 Amount);
	virtual bool LootPickupItem_Implementation(UDataAsset* ItemAsset, int32 Amount);

	/** [5/35] Replenish item (restore consumable) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Items")
	void ReplenishItem(UDataAsset* ItemAsset, int32 Amount);
	virtual void ReplenishItem_Implementation(UDataAsset* ItemAsset, int32 Amount);

	/** [6/35] Get empty slot index for slot type */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Items")
	int32 GetEmptySlot(ESLFInventorySlotType SlotType);
	virtual int32 GetEmptySlot_Implementation(ESLFInventorySlotType SlotType);

	// --- Use/Discard Items (4) ---

	/** [7/35] Use item at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Use")
	void UseItemAtSlot(int32 SlotIndex);
	virtual void UseItemAtSlot_Implementation(int32 SlotIndex);

	/** [8/35] Use item from inventory by asset */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Use")
	void UseItemFromInventory(UDataAsset* ItemAsset);
	virtual void UseItemFromInventory_Implementation(UDataAsset* ItemAsset);

	/** [9/35] Discard item at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Use")
	void DiscardItemAtSlot(int32 SlotIndex, int32 Amount);
	virtual void DiscardItemAtSlot_Implementation(int32 SlotIndex, int32 Amount);

	/** [10/35] Leave item at slot (drop to world) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Use")
	void LeaveItemAtSlot(int32 SlotIndex, int32 Amount);
	virtual void LeaveItemAtSlot_Implementation(int32 SlotIndex, int32 Amount);

	// --- Storage Operations (7) ---

	/** [11/35] Add item to storage at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Storage")
	void AddItemAtSlotToStorage(int32 SlotIndex, UDataAsset* ItemAsset, int32 Amount);
	virtual void AddItemAtSlotToStorage_Implementation(int32 SlotIndex, UDataAsset* ItemAsset, int32 Amount);

	/** [12/35] Add item to storage (auto-slot) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Storage")
	bool AddItemToStorage(UDataAsset* ItemAsset, int32 Amount);
	virtual bool AddItemToStorage_Implementation(UDataAsset* ItemAsset, int32 Amount);

	/** [13/35] Remove item at storage slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Storage")
	void RemoveItemAtStorageSlot(int32 SlotIndex, int32 Amount);
	virtual void RemoveItemAtStorageSlot_Implementation(int32 SlotIndex, int32 Amount);

	/** [14/35] Remove stored item by asset */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Storage")
	void RemoveStoredItem(UDataAsset* Item, int32 Count);
	virtual void RemoveStoredItem_Implementation(UDataAsset* Item, int32 Count);

	/** [15/35] Store item from inventory slot to storage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Storage")
	void StoreItemAtSlot(int32 InventorySlot, int32 Amount);
	virtual void StoreItemAtSlot_Implementation(int32 InventorySlot, int32 Amount);

	/** [16/35] Retrieve item from storage slot to inventory */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Storage")
	void RetrieveItemAtSlot(int32 StorageSlot, int32 Amount);
	virtual void RetrieveItemAtSlot_Implementation(int32 StorageSlot, int32 Amount);

	/** [17/35] Retrieve item from storage to specific inventory slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Storage")
	void RetrieveItemAtSlotFromStorage(int32 StorageSlot, int32 InventorySlot, int32 Amount);
	virtual void RetrieveItemAtSlotFromStorage_Implementation(int32 StorageSlot, int32 InventorySlot, int32 Amount);

	// --- Query - Counts (7) ---

	/** [18/35] Get amount of item in inventory */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	void GetAmountOfItem(UDataAsset* Item, int32& Amount, bool& bSuccess);
	virtual void GetAmountOfItem_Implementation(UDataAsset* Item, int32& Amount, bool& bSuccess);

	/** [19/35] Get amount of item with specific tag */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	void GetAmountOfItemWithTag(FGameplayTag Tag, int32& Amount, bool& bSuccess);
	virtual void GetAmountOfItemWithTag_Implementation(FGameplayTag Tag, int32& Amount, bool& bSuccess);

	/** [20/35] Get stored amount of item */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	void GetStoredAmountOfItem(UDataAsset* Item, int32& Amount, bool& bSuccess);
	virtual void GetStoredAmountOfItem_Implementation(UDataAsset* Item, int32& Amount, bool& bSuccess);

	/** [21/35] Get overall count (inventory + storage) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	int32 GetOverallCountForItem(UDataAsset* ItemAsset);
	virtual int32 GetOverallCountForItem_Implementation(UDataAsset* ItemAsset);

	/** [22/35] Get total inventory items count */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	int32 GetTotalInventoryItemsCount();
	virtual int32 GetTotalInventoryItemsCount_Implementation();

	/** [23/35] Get total storage items count */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	int32 GetTotalStorageItemsCount();
	virtual int32 GetTotalStorageItemsCount_Implementation();

	/** [24/35] Check if has item */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	bool HasItem(UDataAsset* ItemAsset);
	virtual bool HasItem_Implementation(UDataAsset* ItemAsset);

	// --- Query - Slots (4) ---

	/** [25/35] Get slot containing item */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	int32 GetSlotWithItem(UDataAsset* Item, ESLFInventorySlotType SlotType);
	virtual int32 GetSlotWithItem_Implementation(UDataAsset* Item, ESLFInventorySlotType SlotType);

	/** [26/35] Get slot containing item with tag */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	int32 GetSlotWithItemTag(FGameplayTag ItemTag, ESLFInventorySlotType SlotType);
	virtual int32 GetSlotWithItemTag_Implementation(FGameplayTag ItemTag, ESLFInventorySlotType SlotType);

	/** [27/35] Get all items as array */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	TArray<FSLFInventoryItem> GetAllItems();
	virtual TArray<FSLFInventoryItem> GetAllItems_Implementation();

	/** [28/35] Get items for specific category */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	TArray<FSLFInventoryItem> GetItemsForCategory(FGameplayTag CategoryTag);
	virtual TArray<FSLFInventoryItem> GetItemsForCategory_Implementation(FGameplayTag CategoryTag);

	// --- Query - Equipment (1) ---

	/** [29/35] Get items valid for equipment slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Query")
	TArray<FSLFInventoryItem> GetItemsForEquipmentSlot(FGameplayTag SlotTag);
	virtual TArray<FSLFInventoryItem> GetItemsForEquipmentSlot_Implementation(FGameplayTag SlotTag);

	// --- Currency (2) ---

	/** [30/35] Adjust currency amount */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Currency")
	void AdjustCurrency(int32 Amount);
	virtual void AdjustCurrency_Implementation(int32 Amount);

	/** [31/35] Get current currency */
	UFUNCTION(BlueprintPure, Category = "Inventory|Currency")
	int32 GetCurrentCurrency() const { return Currency; }

	// --- Serialization (3) ---

	/** [32/35] Serialize inventory, storage, and currency data for saving */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Save")
	void SerializeInventoryStorageCurrencyData(TArray<FInstancedStruct>& OutData);
	virtual void SerializeInventoryStorageCurrencyData_Implementation(TArray<FInstancedStruct>& OutData);

	/** [33/35] Initialize loaded inventory data */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Save")
	void InitializeLoadedInventory(const TArray<FInstancedStruct>& InData);
	virtual void InitializeLoadedInventory_Implementation(const TArray<FInstancedStruct>& InData);

	/** [34/35] Initialize loaded storage data */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Save")
	void InitializeLoadedStorage(const TArray<FInstancedStruct>& InData);
	virtual void InitializeLoadedStorage_Implementation(const TArray<FInstancedStruct>& InData);

	// --- UI (1) ---

	/** [35/35] Get inventory widget reference */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|UI")
	UUserWidget* GetInventoryWidget();
	virtual UUserWidget* GetInventoryWidget_Implementation();
};
