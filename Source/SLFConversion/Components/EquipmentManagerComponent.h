// EquipmentManagerComponent.h
// C++ base class for AC_EquipmentManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_EquipmentManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         19/19 migrated (excluding 9 dispatchers counted separately)
// Functions:         41/41 migrated (excluding ExecuteUbergraph and async callbacks)
// Event Dispatchers: 9/9 migrated
// Graphs:            53 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager
//
// PURPOSE: Equipment system - manages weapon slots, armor, tools, overlays, stance

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "EquipmentManagerComponent.generated.h"

// Forward declarations
class UDataTable;
class UDataAsset;
class UAnimSequenceBase;
class UBuffManagerComponent;

// Types used from SLFEnums.h/SLFGameTypes.h:
// - FSLFCurrentEquipment
// - FSLFEquipmentItemsSaveInfo
// - ESLFOverlayState
// - ESLFWeaponAbilityHandle

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 9/9 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/9] Called when item is equipped to a slot */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemEquippedToSlot, FGameplayTag, SlotTag, UDataAsset*, ItemAsset);

/** [2/9] Called when item is unequipped from a slot */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUnequippedFromSlot, FGameplayTag, SlotTag, UDataAsset*, ItemAsset);

/** [3/9] Called when weapon ability is updated */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponAbilityUpdated);

/** [4/9] Called when weapon ability is removed */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponAbilityRemoved);

/** [5/9] Called when save is requested */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentSaveRequested);

/** [6/9] Called when weapon stat check fails */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponStatCheckFailed, UDataAsset*, ItemAsset);

/** [7/9] Called when item effect is removed */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEffectRemoved, FGameplayTag, SlotTag);

/** [8/9] Called when item effect is added */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEffectAdded, FGameplayTag, SlotTag);

/** [9/9] Called when stance changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStanceChanged);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 19/19 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Config (4) ---

	/** [1/19] Equipment slot configuration data table */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataTable* SlotTable;

	/** [2/19] Weapon ability slot hand (left/right) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	ESLFWeaponAbilityHandle WeaponAbilitySlot;

	/** [3/19] Left hand equipment slots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer LeftHandSlots;

	/** [4/19] Right hand equipment slots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer RightHandSlots;

	// --- Runtime State - Equipment Maps (4) ---

	/** [5/19] All currently equipped items by slot tag */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FSLFCurrentEquipment> AllEquippedItems;

	/** [6/19] Hidden items (temporarily unequipped) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FSLFCurrentEquipment> HiddenItems;

	/** [7/19] Spawned item actors at each slot */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, AActor*> SpawnedItemsAtSlots;

	/** [8/19] Spawned buffs at each slot */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UObject*> SpawnedBuffAtSlot;

	// --- Runtime State - Overlay (6) ---

	/** [9/19] Current active overlay state (replicated) */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Runtime")
	ESLFOverlayState ActiveOverlayState;

	/** [10/19] Left hand overlay state */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	ESLFOverlayState LeftHandOverlayState;

	/** [11/19] Right hand overlay state */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	ESLFOverlayState RightHandOverlayState;

	/** [12/19] Left hand overlay tags */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FGameplayTagContainer LeftHandOverlayTags;

	/** [13/19] Right hand overlay tags */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FGameplayTagContainer RightHandOverlayTags;

	/** [14/19] Granted gameplay tags from equipment */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FGameplayTagContainer GrantedTags;

	// --- Runtime State - Tools (3) ---

	/** [15/19] Tool equipment slots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer ToolSlots;

	/** [16/19] Currently active tool slot */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FGameplayTag ActiveToolSlot;

	// --- Runtime State - Combat (3) ---

	/** [17/19] Active guard/block animation sequence */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UAnimSequenceBase* ActiveBlockSequence;

	/** [18/19] Whether async weapon spawn is busy */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsAsyncWeaponBusy;

	/** [19/19] Cached equipment data for loading */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<FSLFEquipmentItemsSaveInfo> Cached_LoadedEquipment;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 9/9 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/9] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemEquippedToSlot OnItemEquippedToSlot;

	/** [2/9] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemUnequippedFromSlot OnItemUnequippedFromSlot;

	/** [3/9] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponAbilityUpdated OnWeaponAbilityUpdated;

	/** [4/9] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponAbilityRemoved OnWeaponAbilityRemoved;

	/** [5/9] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEquipmentSaveRequested OnSaveRequested;

	/** [6/9] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponStatCheckFailed OnWeaponStatCheckFailed;

	/** [7/9] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemEffectRemoved OnItemEffectRemoved;

	/** [8/9] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemEffectAdded OnItemEffectAdded;

	/** [9/9] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStanceChanged OnStanceChanged;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 41/41 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Equip/Unequip Weapons (4) ---

	/** [1/41] Equip weapon to slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Weapons")
	void EquipWeaponToSlot(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);
	virtual void EquipWeaponToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);

	/** [2/41] Unequip weapon at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Weapons")
	void UnequipWeaponAtSlot(FGameplayTag SlotTag);
	virtual void UnequipWeaponAtSlot_Implementation(FGameplayTag SlotTag);

	/** [3/41] Wield item at slot (make visible) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Weapons")
	void WieldItemAtSlot(FGameplayTag SlotTag);
	virtual void WieldItemAtSlot_Implementation(FGameplayTag SlotTag);

	/** [4/41] Unwield item at slot (hide but keep equipped) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Weapons")
	void UnwieldItemAtSlot(FGameplayTag SlotTag);
	virtual void UnwieldItemAtSlot_Implementation(FGameplayTag SlotTag);

	// --- Equip/Unequip Armor (2) ---

	/** [5/41] Equip armor to slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Armor")
	void EquipArmorToSlot(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);
	virtual void EquipArmorToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);

	/** [6/41] Unequip armor at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Armor")
	void UnequipArmorAtSlot(FGameplayTag SlotTag);
	virtual void UnequipArmorAtSlot_Implementation(FGameplayTag SlotTag);

	// --- Equip/Unequip Tools (4) ---

	/** [7/41] Equip tool to slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Tools")
	void EquipToolToSlot(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);
	virtual void EquipToolToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);

	/** [8/41] Unequip tool at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Tools")
	void UnequipToolAtSlot(FGameplayTag SlotTag);
	virtual void UnequipToolAtSlot_Implementation(FGameplayTag SlotTag);

	/** [9/41] Unequip tool from other slots when equipping to new slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Tools")
	void UnequipToolFromOtherSlots(UDataAsset* ItemAsset);
	virtual void UnequipToolFromOtherSlots_Implementation(UDataAsset* ItemAsset);

	/** [10/41] Hide item at slot (visual only) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Tools")
	void HideItemAtSlot(FGameplayTag SlotTag);
	virtual void HideItemAtSlot_Implementation(FGameplayTag SlotTag);

	// --- Equip/Unequip Talismans (2) ---

	/** [11/41] Equip talisman to slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Talismans")
	void EquipTalismanToSlot(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);
	virtual void EquipTalismanToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);

	/** [12/41] Unequip talisman at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Talismans")
	void UnequipTalismanAtSlot(FGameplayTag SlotTag);
	virtual void UnequipTalismanAtSlot_Implementation(FGameplayTag SlotTag);

	// --- Generic Equip/Unequip (2) ---

	/** [13/41] Generic equip item to slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	void EquipItemToSlot(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);
	virtual void EquipItemToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats);

	/** [14/41] Generic unequip item at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	void UnequipItemAtSlot(FGameplayTag SlotTag);
	virtual void UnequipItemAtSlot_Implementation(FGameplayTag SlotTag);

	// --- Slot Queries (7) ---

	/** [15/41] Get item data at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Query")
	void GetItemAtSlot(FGameplayTag SlotTag, UDataAsset*& OutItemAsset, FGuid& OutId);
	virtual void GetItemAtSlot_Implementation(FGameplayTag SlotTag, UDataAsset*& OutItemAsset, FGuid& OutId);

	/** [16/41] Get spawned item actor at slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Query")
	AActor* GetSpawnedItemAtSlot(FGameplayTag SlotTag);
	virtual AActor* GetSpawnedItemAtSlot_Implementation(FGameplayTag SlotTag);

	/** [17/41] Check if slot is occupied */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Query")
	bool IsSlotOccupied(FGameplayTag SlotTag);
	virtual bool IsSlotOccupied_Implementation(FGameplayTag SlotTag);

	/** [18/41] Check if specific item is equipped anywhere */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Query")
	bool IsItemEquipped(UDataAsset* ItemAsset);
	virtual bool IsItemEquipped_Implementation(UDataAsset* ItemAsset);

	/** [19/41] Check if specific item is equipped to specific slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Query")
	bool IsItemEquippedToSlot(FGameplayTag SlotTag, UDataAsset* ItemAsset);
	virtual bool IsItemEquippedToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset);

	/** [20/41] Get count of equipped instances of item */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Query")
	int32 GetAmountOfEquippedItem(UDataAsset* ItemAsset);
	virtual int32 GetAmountOfEquippedItem_Implementation(UDataAsset* ItemAsset);

	// --- Weapon Slot Queries (5) ---

	/** [21/41] Get active weapon slot tag */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Weapons")
	FGameplayTag GetActiveWeaponSlot(bool bRightHand);
	virtual FGameplayTag GetActiveWeaponSlot_Implementation(bool bRightHand);

	/** [22/41] Get active wheel slot for weapon cycling */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Weapons")
	FGameplayTag GetActiveWheelSlotForWeapon(bool bRightHand);
	virtual FGameplayTag GetActiveWheelSlotForWeapon_Implementation(bool bRightHand);

	/** [23/41] Check if both weapon slots are active (dual wield) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Weapons")
	bool AreBothWeaponSlotsActive();
	virtual bool AreBothWeaponSlotsActive_Implementation();

	/** [24/41] Check if dual wield is possible */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Weapons")
	bool IsDualWieldPossible();
	virtual bool IsDualWieldPossible_Implementation();

	/** [25/41] Check if two-hand stance is active */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Weapons")
	bool IsTwoHandStanceActive();
	virtual bool IsTwoHandStanceActive_Implementation();

	// --- Tool Slot Operations (3) ---

	/** [26/41] Get active wheel slot for tool cycling */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Tools")
	FGameplayTag GetActiveWheelSlotForTool();
	virtual FGameplayTag GetActiveWheelSlotForTool_Implementation();

	/** [27/41] Set active tool slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Tools")
	void SetActiveToolSlot(FGameplayTag SlotTag);
	virtual void SetActiveToolSlot_Implementation(FGameplayTag SlotTag);

	/** [28/41] Get active tool slot */
	UFUNCTION(BlueprintPure, Category = "Equipment|Tools")
	FGameplayTag GetActiveToolSlot() const { return ActiveToolSlot; }

	// --- Overlay State (3) ---

	/** [29/41] Update overlay states based on equipment */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Overlay")
	void UpdateOverlayStates();
	virtual void UpdateOverlayStates_Implementation();

	/** [30/41] Override overlay states manually */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Overlay")
	void OverrideOverlayStates(ESLFOverlayState LeftHand, ESLFOverlayState RightHand);
	virtual void OverrideOverlayStates_Implementation(ESLFOverlayState LeftHand, ESLFOverlayState RightHand);

	/** [31/41] Adjust equipment for two-hand stance */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Overlay")
	void AdjustForTwoHandStance(bool bRightHand);
	virtual void AdjustForTwoHandStance_Implementation(bool bRightHand);

	// --- Guard/Block (4) ---

	/** [32/41] Check if equipment supports guarding */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Guard")
	bool DoesEquipmentSupportGuard();
	virtual bool DoesEquipmentSupportGuard_Implementation();

	/** [33/41] Refresh active guard sequence animation */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Guard")
	void RefreshActiveGuardSequence();
	virtual void RefreshActiveGuardSequence_Implementation();

	/** [34/41] Get guard hit reaction montage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Guard")
	UAnimMontage* GetGuardHitMontage();
	virtual UAnimMontage* GetGuardHitMontage_Implementation();

	// --- Stats & Buffs (5) ---

	/** [35/41] Apply stat changes from equipment */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Stats")
	void ApplyStatChanges(UDataAsset* ItemAsset, bool bAdd);
	virtual void ApplyStatChanges_Implementation(UDataAsset* ItemAsset, bool bAdd);

	/** [36/41] Check if player meets required stats for item */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Stats")
	bool CheckRequiredStats(UDataAsset* ItemAsset);
	virtual bool CheckRequiredStats_Implementation(UDataAsset* ItemAsset);

	/** [37/41] Try to grant buffs from equipment */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Buffs")
	void TryGrantBuffs(bool bIsLoading);
	virtual void TryGrantBuffs_Implementation(bool bIsLoading);

	/** [38/41] Try to remove buffs from equipment */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Buffs")
	void TryRemoveBuffs();
	virtual void TryRemoveBuffs_Implementation();

	/** [39/41] Get buff manager component reference */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Buffs")
	UBuffManagerComponent* GetBuffManager();
	virtual UBuffManagerComponent* GetBuffManager_Implementation();

	// --- Serialization (2) ---

	/** [40/41] Serialize equipment data for saving */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Save")
	void SerializeEquipmentData(TArray<FInstancedStruct>& OutData);
	virtual void SerializeEquipmentData_Implementation(TArray<FInstancedStruct>& OutData);

	/** [41/41] Reinitialize movement component with equipment weight */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	void ReinitializeMovementWithWeight();
	virtual void ReinitializeMovementWithWeight_Implementation();

	// ═══════════════════════════════════════════════════════════════════
	// ADDITIONAL EVENTS
	// ═══════════════════════════════════════════════════════════════════

	/** Recursively initialize loaded equipment from save */
	UFUNCTION(BlueprintCallable, Category = "Equipment|Load")
	void EventRecursiveInitializeLoadedEquipment();

	/** Switch weapon based on slot */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SwitchOnSlot(FGameplayTag SlotTag);

	/** Async spawn and equip weapon */
	UFUNCTION(BlueprintCallable, Category = "Equipment|Async")
	void EventAsyncSpawnAndEquipWeapon(UDataAsset* ItemAsset, FGameplayTag SlotTag);

	/** Initialize loaded items from save */
	UFUNCTION(BlueprintCallable, Category = "Equipment|Load")
	void EventInitLoadedItems();

	/** Get two-hand stance state (left or right) */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool GetTwoHandStance(bool& bLeftHand, bool& bRightHand) const;
};
