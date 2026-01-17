// AC_InventoryManager.h
// C++ component for AC_InventoryManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_InventoryManager.json
// Variables: 7 | Functions: 35 | Dispatchers: 7

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_InventoryManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class UW_Inventory;
class UW_InventorySlot;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAC_InventoryManager_OnInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_InventoryManager_OnCurrencyUpdated, int32, NewCurrency);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAC_InventoryManager_OnItemLooted, FSLFItemInfo, Item, int32, Amount, bool, Exists);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAC_InventoryManager_OnSaveRequested, FGameplayTag, SaveGameTag, TArray<FInstancedStruct>, InventoryItemsToSave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAC_InventoryManager_OnItemAmountUpdated, UPrimaryDataAsset*, Item, int32, NewCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_InventoryManager_OnErrorTrigger, FText, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_InventoryManager_OnCurrencyIncreased, int32, IncreaseAmount);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_InventoryManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_InventoryManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TArray<FSLFInventoryCategory> Categories;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	int32 SlotCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	int32 SlotsPerRow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	AActor* OwnerActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UPrimaryDataAsset*> Items;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UPrimaryDataAsset*> StoredItems;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	int32 Currency;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_InventoryManager_OnInventoryUpdated OnInventoryUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_InventoryManager_OnCurrencyUpdated OnCurrencyUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_InventoryManager_OnItemLooted OnItemLooted;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_InventoryManager_OnSaveRequested OnSaveRequested;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_InventoryManager_OnItemAmountUpdated OnItemAmountUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_InventoryManager_OnErrorTrigger OnErrorTrigger;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_InventoryManager_OnCurrencyIncreased OnCurrencyIncreased;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (35)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	UW_Inventory* GetInventoryWidget();
	virtual UW_Inventory* GetInventoryWidget_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void GetAmountOfItem(UPrimaryDataAsset* Item, int32& OutAmount, bool& OutSuccess);
	virtual void GetAmountOfItem_Implementation(UPrimaryDataAsset* Item, int32& OutAmount, bool& OutSuccess);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void GetStoredAmountOfItem(UPrimaryDataAsset* Item, int32& OutAmount, bool& OutSuccess);
	virtual void GetStoredAmountOfItem_Implementation(UPrimaryDataAsset* Item, int32& OutAmount, bool& OutSuccess);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void GetAmountOfItemWithTag(const FGameplayTag& Tag, int32& OutAmount, bool& OutSuccess);
	virtual void GetAmountOfItemWithTag_Implementation(const FGameplayTag& Tag, int32& OutAmount, bool& OutSuccess);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void GetOverallCountForItem(UPrimaryDataAsset* Item, int32& OutOverallCount, int32& OutOverallCount_1, int32& OutOverallCount_2, int32& OutOverallCount_3);
	virtual void GetOverallCountForItem_Implementation(UPrimaryDataAsset* Item, int32& OutOverallCount, int32& OutOverallCount_1, int32& OutOverallCount_2, int32& OutOverallCount_3);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	int32 GetTotalInventoryItemsCount();
	virtual int32 GetTotalInventoryItemsCount_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	int32 GetTotalStorageItemsCount();
	virtual int32 GetTotalStorageItemsCount_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	bool HasItem(UPrimaryDataAsset* Item);
	virtual bool HasItem_Implementation(UPrimaryDataAsset* Item);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	UW_InventorySlot* GetSlotWithItem(UPrimaryDataAsset* Item, ESLFInventorySlotType SlotType);
	virtual UW_InventorySlot* GetSlotWithItem_Implementation(UPrimaryDataAsset* Item, ESLFInventorySlotType SlotType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	UW_InventorySlot* GetSlotWithItemTag(const FGameplayTag& ItemTag, ESLFInventorySlotType SlotType);
	virtual UW_InventorySlot* GetSlotWithItemTag_Implementation(const FGameplayTag& ItemTag, ESLFInventorySlotType SlotType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void LootPickupItem(AB_PickupItem* LootedItem);
	virtual void LootPickupItem_Implementation(AB_PickupItem* LootedItem);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	UW_InventorySlot* GetEmptySlot(ESLFInventorySlotType SlotType);
	virtual UW_InventorySlot* GetEmptySlot_Implementation(ESLFInventorySlotType SlotType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void AddItem(UPrimaryDataAsset* Item, int32 Count, bool TriggerLootUi);
	virtual void AddItem_Implementation(UPrimaryDataAsset* Item, int32 Count, bool TriggerLootUi);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void RemoveItem(UPrimaryDataAsset* Item, int32 Count);
	virtual void RemoveItem_Implementation(UPrimaryDataAsset* Item, int32 Count);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	bool RemoveItemWithTag(const FGameplayTag& Tag, int32 Count);
	virtual bool RemoveItemWithTag_Implementation(const FGameplayTag& Tag, int32 Count);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void RemoveStoredItem(UPrimaryDataAsset* Item, int32 Count);
	virtual void RemoveStoredItem_Implementation(UPrimaryDataAsset* Item, int32 Count);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void RemoveItemAtSlot(UW_InventorySlot* Slot, int32 Count);
	virtual void RemoveItemAtSlot_Implementation(UW_InventorySlot* Slot, int32 Count);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void RemoveItemAtStorageSlot(UW_InventorySlot* Slot, int32 Count);
	virtual void RemoveItemAtStorageSlot_Implementation(UW_InventorySlot* Slot, int32 Count);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	TArray<UPrimaryDataAsset*> GetItemsForEquipmentSlot(const FGameplayTag& EquipmentSlotTag);
	virtual TArray<UPrimaryDataAsset*> GetItemsForEquipmentSlot_Implementation(const FGameplayTag& EquipmentSlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void AddItemToStorage(UPrimaryDataAsset* ItemAsset, int32 Amount);
	virtual void AddItemToStorage_Implementation(UPrimaryDataAsset* ItemAsset, int32 Amount);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void AddItemAtSlotToStorage(UW_InventorySlot* CurrentSlot, int32 Amount);
	virtual void AddItemAtSlotToStorage_Implementation(UW_InventorySlot* CurrentSlot, int32 Amount);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void RetrieveItemAtSlotFromStorage(UW_InventorySlot* StorageSlot, int32 Amount);
	virtual void RetrieveItemAtSlotFromStorage_Implementation(UW_InventorySlot* StorageSlot, int32 Amount);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void AdjustCurrency(int32 Delta);
	virtual void AdjustCurrency_Implementation(int32 Delta);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void SerializeInventoryStorageCurrencyData();
	virtual void SerializeInventoryStorageCurrencyData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void InitializeLoadedInventory(const TArray<FSLFInventoryItemsSaveInfo>& LoadedInventoryItems);
	virtual void InitializeLoadedInventory_Implementation(const TArray<FSLFInventoryItemsSaveInfo>& LoadedInventoryItems);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void UseItemFromInventory(UPrimaryDataAsset* Item);
	virtual void UseItemFromInventory_Implementation(UPrimaryDataAsset* Item);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	int32 GetCurrentCurency();
	virtual int32 GetCurrentCurency_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void GetItemsForCategory(ESLFItemCategory ItemCategory, ESLFInventorySlotType Type, TMap<FGameplayTag, UPrimaryDataAsset*>& OutStackableItems, TArray<UPrimaryDataAsset*>& OutNonStackableItems);
	virtual void GetItemsForCategory_Implementation(ESLFItemCategory ItemCategory, ESLFInventorySlotType Type, TMap<FGameplayTag, UPrimaryDataAsset*>& OutStackableItems, TArray<UPrimaryDataAsset*>& OutNonStackableItems);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void GetAllItems(ESLFInventorySlotType Type, TMap<FGameplayTag, UPrimaryDataAsset*>& OutStackableItems, TArray<UPrimaryDataAsset*>& OutNonStackableItems);
	virtual void GetAllItems_Implementation(ESLFInventorySlotType Type, TMap<FGameplayTag, UPrimaryDataAsset*>& OutStackableItems, TArray<UPrimaryDataAsset*>& OutNonStackableItems);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void UseItemAtSlot(UW_InventorySlot* Slot);
	virtual void UseItemAtSlot_Implementation(UW_InventorySlot* Slot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void LeaveItemAtSlot(int32 Amount, UW_InventorySlot* InventorySlot);
	virtual void LeaveItemAtSlot_Implementation(int32 Amount, UW_InventorySlot* InventorySlot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void DiscardItemAtSlot(int32 Amount, UW_InventorySlot* InventorySlot);
	virtual void DiscardItemAtSlot_Implementation(int32 Amount, UW_InventorySlot* InventorySlot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void StoreItemAtSlot(int32 Amount, UW_InventorySlot* InventorySlot);
	virtual void StoreItemAtSlot_Implementation(int32 Amount, UW_InventorySlot* InventorySlot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void RetrieveItemAtSlot(int32 Amount, UW_InventorySlot* InventorySlot);
	virtual void RetrieveItemAtSlot_Implementation(int32 Amount, UW_InventorySlot* InventorySlot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void InitializeLoadedStorage(const TArray<FSLFInventoryItemsSaveInfo>& LoadedStorageItems);
	virtual void InitializeLoadedStorage_Implementation(const TArray<FSLFInventoryItemsSaveInfo>& LoadedStorageItems);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_InventoryManager")
	void ReplenishItem(UPrimaryDataAsset* Item);
	virtual void ReplenishItem_Implementation(UPrimaryDataAsset* Item);
};
