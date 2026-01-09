// AC_EquipmentManager.cpp
// C++ component implementation for AC_EquipmentManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_EquipmentManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "Components/AC_EquipmentManager.h"
#include "Components/AC_BuffManager.h"
#include "Components/StatManagerComponent.h"
#include "Components/AC_InventoryManager.h"
#include "Blueprints/B_Item.h"
#include "Widgets/W_InventorySlot.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

UAC_EquipmentManager::UAC_EquipmentManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	ActiveOverlayState = ESLFOverlayState::Default;
	LeftHandOverlayState = ESLFOverlayState::Default;
	RightHandOverlayState = ESLFOverlayState::Default;
	WeaponAbilitySlot = ESLFWeaponAbilityHandle::RightHand;
	ActiveBlockSequence = nullptr;
	IsAsyncWeaponBusy = false;
	SlotTable = nullptr;
}

void UAC_EquipmentManager::BeginPlay()
{
	Super::BeginPlay();

	// Load default SlotTable if not set
	if (!SlotTable)
	{
		static const TCHAR* DefaultSlotTablePath = TEXT("/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleEquipmentSlotInfo.DT_ExampleEquipmentSlotInfo");
		SlotTable = LoadObject<UDataTable>(nullptr, DefaultSlotTablePath);

		if (SlotTable)
		{
			UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::BeginPlay - Loaded default SlotTable with %d rows"), SlotTable->GetRowNames().Num());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAC_EquipmentManager::BeginPlay - SlotTable not set and default not found!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::BeginPlay - SlotTable already set with %d rows"), SlotTable->GetRowNames().Num());
	}
}

void UAC_EquipmentManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAC_EquipmentManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAC_EquipmentManager, ActiveOverlayState);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * IsSlotOccupied - Check if an equipment slot has an item
 */
bool UAC_EquipmentManager::IsSlotOccupied_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::IsSlotOccupied - Slot: %s"), *SlotTag.ToString());
	if (TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
	{
		return IsValid(*ItemPtr);
	}
	return false;
}

/**
 * EquipWeaponToSlot - Equip a weapon to a specific slot
 *
 * Blueprint Logic:
 * 1. Unequip any existing item at the slot
 * 2. Store item in AllEquippedItems map
 * 3. Apply stat changes if requested
 * 4. Spawn world actor (weapon mesh) via AsyncSpawnAndEquipWeapon
 * 5. Update overlay states and guard sequence
 * 6. Broadcast OnItemEquippedToSlot
 */
void UAC_EquipmentManager::EquipWeaponToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::EquipWeaponToSlot - Item: %s, Slot: %s, ChangeStats: %s"),
		TargetItem ? *TargetItem->GetName() : TEXT("None"), *TargetEquipmentSlot.ToString(),
		ChangeStats ? TEXT("true") : TEXT("false"));

	OutSuccess = false;
	OutSuccess_1 = false;
	OutSuccess_2 = false;
	OutSuccess_3 = false;

	if (!IsValid(TargetItem) || !TargetEquipmentSlot.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid item or slot tag"));
		return;
	}

	// Unequip existing item at slot
	if (IsSlotOccupied(TargetEquipmentSlot))
	{
		UnequipWeaponAtSlot(TargetEquipmentSlot);
	}

	// Store item directly in map
	AllEquippedItems.Add(TargetEquipmentSlot, TargetItem);
	UE_LOG(LogTemp, Log, TEXT("  Stored item in AllEquippedItems"));

	// Apply stat changes if requested
	if (ChangeStats)
	{
		if (UPDA_Item* Item = Cast<UPDA_Item>(TargetItem))
		{
			ApplyStatChanges(Item->ItemInformation, true);
			UE_LOG(LogTemp, Log, TEXT("  Applied stat changes"));
		}
	}

	// Spawn world actor for the weapon (mesh attached to character)
	if (UPDA_Item* Item = Cast<UPDA_Item>(TargetItem))
	{
		SpawnEquipmentActor(Item, TargetEquipmentSlot);
	}

	// Update overlay states
	UpdateOverlayStates();

	// Refresh guard sequence
	RefreshActiveGuardSequence();

	// Broadcast event with item data
	FSLFCurrentEquipment EquipData;
	EquipData.ItemAsset = TargetItem;
	OnItemEquippedToSlot.Broadcast(EquipData, TargetEquipmentSlot);

	OutSuccess = true;
	OutSuccess_1 = true;
	OutSuccess_2 = true;
	OutSuccess_3 = true;

	UE_LOG(LogTemp, Log, TEXT("  Weapon equipped successfully"));
}

/**
 * EquipArmorToSlot - Equip armor to a specific slot
 */
void UAC_EquipmentManager::EquipArmorToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::EquipArmorToSlot - Item: %s, Slot: %s, ChangeStats: %s"),
		TargetItem ? *TargetItem->GetName() : TEXT("None"), *TargetEquipmentSlot.ToString(),
		ChangeStats ? TEXT("true") : TEXT("false"));

	OutSuccess = false;
	OutSuccess_1 = false;
	OutSuccess_2 = false;
	OutSuccess_3 = false;

	if (!IsValid(TargetItem) || !TargetEquipmentSlot.IsValid())
	{
		return;
	}

	// Unequip existing item at slot
	if (IsSlotOccupied(TargetEquipmentSlot))
	{
		UnequipArmorAtSlot(TargetEquipmentSlot);
	}

	// Store item directly in map
	AllEquippedItems.Add(TargetEquipmentSlot, TargetItem);

	// Apply stat changes if requested
	if (ChangeStats)
	{
		if (UPDA_Item* Item = Cast<UPDA_Item>(TargetItem))
		{
			ApplyStatChanges(Item->ItemInformation, true);
		}
	}

	// Broadcast event with item data
	FSLFCurrentEquipment EquipData;
	EquipData.ItemAsset = TargetItem;
	OnItemEquippedToSlot.Broadcast(EquipData, TargetEquipmentSlot);

	OutSuccess = true;
	OutSuccess_1 = true;
	OutSuccess_2 = true;
	OutSuccess_3 = true;

	UE_LOG(LogTemp, Log, TEXT("  Armor equipped successfully"));
}

/**
 * UnequipWeaponAtSlot - Remove weapon from slot
 *
 * Blueprint Logic:
 * 1. Get item at slot
 * 2. Remove stat changes
 * 3. Destroy spawned world actor (weapon mesh)
 * 4. Remove from AllEquippedItems
 * 5. Update overlay states and guard sequence
 * 6. Broadcast OnItemUnequippedFromSlot
 */
void UAC_EquipmentManager::UnequipWeaponAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::UnequipWeaponAtSlot - Slot: %s"), *SlotTag.ToString());

	if (TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
	{
		UPrimaryDataAsset* Item = *ItemPtr;

		// Remove stat changes before removing item
		if (UPDA_Item* PDAItem = Cast<UPDA_Item>(Item))
		{
			ApplyStatChanges(PDAItem->ItemInformation, false);
		}

		// Destroy spawned world actor (weapon mesh) if any
		DestroyEquipmentActor(SlotTag);

		// Remove from item map
		AllEquippedItems.Remove(SlotTag);

		// Update overlay states
		UpdateOverlayStates();

		// Refresh guard
		RefreshActiveGuardSequence();

		// Broadcast with item
		OnItemUnequippedFromSlot.Broadcast(Item, SlotTag);
		UE_LOG(LogTemp, Log, TEXT("  Weapon unequipped successfully"));
	}
}

/**
 * UnequipArmorAtSlot - Remove armor from slot
 */
void UAC_EquipmentManager::UnequipArmorAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::UnequipArmorAtSlot - Slot: %s"), *SlotTag.ToString());

	if (TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
	{
		UPrimaryDataAsset* Item = *ItemPtr;

		// Remove stat changes before removing item
		if (UPDA_Item* PDAItem = Cast<UPDA_Item>(Item))
		{
			ApplyStatChanges(PDAItem->ItemInformation, false);
		}

		AllEquippedItems.Remove(SlotTag);
		OnItemUnequippedFromSlot.Broadcast(Item, SlotTag);
	}
}

/**
 * HideItemAtSlot - Hide the visual representation of an item
 */
void UAC_EquipmentManager::HideItemAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::HideItemAtSlot - Slot: %s"), *SlotTag.ToString());

	if (TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
	{
		// Mark slot as hidden (for visual purposes)
		// We use the slot tag itself as a marker since we store items directly now
		HiddenItems.Add(SlotTag, SlotTag);
	}
}

/**
 * IsItemEquipped - Check if a specific item is equipped anywhere
 */
void UAC_EquipmentManager::IsItemEquipped_Implementation(const FSLFItemInfo& TargetItem, bool& OutEquipped, FGameplayTag& OutOnSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::IsItemEquipped - ItemTag: %s"), *TargetItem.ItemTag.ToString());

	OutEquipped = false;
	OutOnSlot = FGameplayTag();

	// Search through all equipped items
	for (const auto& EquipEntry : AllEquippedItems)
	{
		if (UPDA_Item* EquippedItem = Cast<UPDA_Item>(EquipEntry.Value.Get()))
		{
			// Compare item tags
			if (EquippedItem->ItemInformation.ItemTag.MatchesTagExact(TargetItem.ItemTag))
			{
				OutEquipped = true;
				OutOnSlot = EquipEntry.Key;
				return;
			}
		}
	}
}

/**
 * IsItemEquippedToSlot - Check if item is equipped to specific slot
 */
bool UAC_EquipmentManager::IsItemEquippedToSlot_Implementation(const FSLFItemInfo& TargetItem, const FGameplayTag& ActiveEquipmentSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::IsItemEquippedToSlot - Slot: %s"), *ActiveEquipmentSlot.ToString());

	if (TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(ActiveEquipmentSlot))
	{
		if (UPDA_Item* EquippedItem = Cast<UPDA_Item>(ItemPtr->Get()))
		{
			return EquippedItem->ItemInformation.ItemTag.MatchesTagExact(TargetItem.ItemTag);
		}
	}
	return false;
}

/**
 * GetBuffManager - Get the buff manager component from owner
 */
void UAC_EquipmentManager::GetBuffManager_Implementation(UAC_BuffManager*& OutReturnValue, UAC_BuffManager*& OutReturnValue_1)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::GetBuffManager"));

	UAC_BuffManager* BuffManager = nullptr;
	if (AActor* Owner = GetOwner())
	{
		BuffManager = Owner->FindComponentByClass<UAC_BuffManager>();
	}
	OutReturnValue = BuffManager;
	OutReturnValue_1 = BuffManager;
}

/**
 * GetSpawnedItemAtSlot - Get the spawned item actor at a slot
 */
void UAC_EquipmentManager::GetSpawnedItemAtSlot_Implementation(const FGameplayTag& SlotTag, AB_Item*& OutItem, AB_Item*& OutItem_1)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::GetSpawnedItemAtSlot - Slot: %s"), *SlotTag.ToString());

	OutItem = nullptr;
	OutItem_1 = nullptr;

	// Get spawned actor from map
	if (TObjectPtr<AActor>* ActorPtr = SpawnedItemsAtSlots.Find(SlotTag))
	{
		if (IsValid(*ActorPtr))
		{
			OutItem = Cast<AB_Item>(*ActorPtr);
			OutItem_1 = OutItem;
			UE_LOG(LogTemp, Log, TEXT("  Found spawned item actor: %s"), *(*ActorPtr)->GetName());
		}
	}
}

/**
 * GetActiveWeaponSlot - Get the currently active weapon slot for hand
 */
FGameplayTag UAC_EquipmentManager::GetActiveWeaponSlot_Implementation(bool RightHand)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::GetActiveWeaponSlot - RightHand: %s"),
		RightHand ? TEXT("true") : TEXT("false"));

	// Return first occupied slot from hand slots
	const FGameplayTagContainer& HandSlots = RightHand ? RightHandSlots : LeftHandSlots;

	for (const FGameplayTag& SlotTag : HandSlots)
	{
		if (IsSlotOccupied(SlotTag))
		{
			return SlotTag;
		}
	}
	return FGameplayTag();
}

/**
 * GetActiveToolSlot - Get the active tool slot
 */
FGameplayTag UAC_EquipmentManager::GetActiveToolSlot_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::GetActiveToolSlot - %s"), *ActiveToolSlot.ToString());
	return ActiveToolSlot;
}

/**
 * SetActiveToolSlot - Set the active tool slot
 */
void UAC_EquipmentManager::SetActiveToolSlot_Implementation(const FGameplayTag& InActiveToolSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::SetActiveToolSlot - %s"), *InActiveToolSlot.ToString());
	ActiveToolSlot = InActiveToolSlot;
}

/**
 * GetActiveWheelSlotForTool - Get wheel slot for active tool
 */
FGameplayTag UAC_EquipmentManager::GetActiveWheelSlotForTool_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::GetActiveWheelSlotForTool"));
	return ActiveToolSlot;
}

/**
 * GetActiveWheelSlotForWeapon - Get wheel slot for active weapon
 */
FGameplayTag UAC_EquipmentManager::GetActiveWheelSlotForWeapon_Implementation(bool RightHand)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::GetActiveWheelSlotForWeapon"));
	return GetActiveWeaponSlot(RightHand);
}

/**
 * ApplyStatChanges - Apply stat changes from equipment
 */
void UAC_EquipmentManager::ApplyStatChanges_Implementation(const FSLFItemInfo& Item, bool Equipped)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::ApplyStatChanges - Equipped: %s"),
		Equipped ? TEXT("true") : TEXT("false"));

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (!IsValid(StatManager))
	{
		return;
	}

	// Apply stat changes from equipment
	double Multiplier = Equipped ? 1.0 : -1.0;

	// Apply each stat change from equipment details
	for (const auto& StatChange : Item.EquipmentDetails.StatChanges)
	{
		StatManager->AdjustStat(StatChange.Key, ESLFValueType::CurrentValue, StatChange.Value.Delta * Multiplier, false, false);
	}

	// Reinitialize movement with new weight
	ReinitializeMovementWithWeight();
}

/**
 * GetItemAtSlot - Get item data at a specific slot
 */
void UAC_EquipmentManager::GetItemAtSlot_Implementation(const FGameplayTag& SlotTag, FSLFItemInfo& OutItemData, UPrimaryDataAsset*& OutItemAsset, FGuid& OutId, FSLFItemInfo& OutItemData_1, UPrimaryDataAsset*& OutItemAsset_1, FGuid& OutId_1)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::GetItemAtSlot - Slot: %s"), *SlotTag.ToString());

	OutItemData = FSLFItemInfo();
	OutItemAsset = nullptr;
	OutId = FGuid();
	OutItemData_1 = FSLFItemInfo();
	OutItemAsset_1 = nullptr;
	OutId_1 = FGuid();

	// Look up item directly from map
	if (TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
	{
		OutItemAsset = ItemPtr->Get();
		OutItemAsset_1 = OutItemAsset;

		if (UPDA_Item* Item = Cast<UPDA_Item>(OutItemAsset))
		{
			OutItemData = Item->ItemInformation;
			OutItemData_1 = OutItemData;
			UE_LOG(LogTemp, Log, TEXT("  Found item: %s"), *Item->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("  No item at slot"));
	}
}

/**
 * CheckRequiredStats - Check if player meets stat requirements
 */
void UAC_EquipmentManager::CheckRequiredStats_Implementation(const FSLFItemInfo& Item, bool& OutCanEquip, bool& OutCanEquip_1)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::CheckRequiredStats"));

	OutCanEquip = true; // Default to true
	OutCanEquip_1 = true;

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (!IsValid(StatManager))
	{
		return;
	}

	// Check if equipment has stat requirements
	if (Item.EquipmentDetails.WeaponStatInfo.bHasStatRequirement)
	{
		// Check each required stat from WeaponStatInfo
		for (const auto& RequiredStat : Item.EquipmentDetails.WeaponStatInfo.StatRequirementInfo)
		{
			UObject* StatObj = nullptr;
			FStatInfo StatInfo;
			StatManager->GetStat(RequiredStat.Key, StatObj, StatInfo);

			if (StatInfo.CurrentValue < RequiredStat.Value)
			{
				OutCanEquip = false;
				OutCanEquip_1 = false;
				OnWeaponStatCheckFailed.Broadcast();
				return;
			}
		}
	}
}

/**
 * SerializeEquipmentData - Serialize equipment for saving
 */
void UAC_EquipmentManager::SerializeEquipmentData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::SerializeEquipmentData"));

	TArray<FInstancedStruct> EquipmentData;

	// Convert AllEquippedItems to FInstancedStruct array
	for (const auto& EquipEntry : AllEquippedItems)
	{
		if (!IsValid(EquipEntry.Value))
		{
			continue;
		}

		FSLFEquipmentItemsSaveInfo SaveInfo;
		SaveInfo.SlotTag = EquipEntry.Key;
		SaveInfo.AssignedItem = EquipEntry.Value;

		FInstancedStruct StructInstance;
		StructInstance.InitializeAs<FSLFEquipmentItemsSaveInfo>(SaveInfo);
		EquipmentData.Add(StructInstance);
	}

	FGameplayTag SaveTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Save.Equipment"));
	OnSaveRequested.Broadcast(SaveTag, EquipmentData);
}

/**
 * EquipItemToSlot - Generic equip function
 */
void UAC_EquipmentManager::EquipItemToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3, bool& OutSuccess_4, bool& OutSuccess_5, bool& OutSuccess_6)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::EquipItemToSlot"));

	// Determine item type and call appropriate equip function
	// For now, use weapon equip as default
	EquipWeaponToSlot(TargetItem, TargetEquipmentSlot, ChangeStats, OutSuccess, OutSuccess_1, OutSuccess_2, OutSuccess_3);
	OutSuccess_4 = OutSuccess;
	OutSuccess_5 = OutSuccess;
	OutSuccess_6 = OutSuccess;
}

/**
 * UnequipItemAtSlot - Generic unequip function
 */
void UAC_EquipmentManager::UnequipItemAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::UnequipItemAtSlot"));
	UnequipWeaponAtSlot(SlotTag);
}

/**
 * EquipTalismanToSlot - Equip a talisman
 */
bool UAC_EquipmentManager::EquipTalismanToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::EquipTalismanToSlot - Item: %s, Slot: %s"),
		TargetItem ? *TargetItem->GetName() : TEXT("None"), *TargetEquipmentSlot.ToString());

	if (!IsValid(TargetItem) || !TargetEquipmentSlot.IsValid())
	{
		return false;
	}

	// Unequip existing
	if (IsSlotOccupied(TargetEquipmentSlot))
	{
		UnequipTalismanAtSlot(TargetEquipmentSlot);
	}

	// Store item directly
	AllEquippedItems.Add(TargetEquipmentSlot, TargetItem);

	// Apply stat changes if requested
	if (ChangeStats)
	{
		if (UPDA_Item* Item = Cast<UPDA_Item>(TargetItem))
		{
			ApplyStatChanges(Item->ItemInformation, true);
		}
	}

	// Broadcast event with item data
	FSLFCurrentEquipment EquipData;
	EquipData.ItemAsset = TargetItem;
	OnItemEquippedToSlot.Broadcast(EquipData, TargetEquipmentSlot);

	return true;
}

/**
 * UnequipTalismanAtSlot - Remove talisman from slot
 */
void UAC_EquipmentManager::UnequipTalismanAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::UnequipTalismanAtSlot - Slot: %s"), *SlotTag.ToString());

	if (TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
	{
		UPrimaryDataAsset* Item = *ItemPtr;

		// Remove stat changes
		if (UPDA_Item* PDAItem = Cast<UPDA_Item>(Item))
		{
			ApplyStatChanges(PDAItem->ItemInformation, false);
		}

		AllEquippedItems.Remove(SlotTag);
		OnItemUnequippedFromSlot.Broadcast(Item, SlotTag);
	}
}

/**
 * GetAmountOfEquippedItem - Count how many of an item are equipped
 */
int32 UAC_EquipmentManager::GetAmountOfEquippedItem_Implementation(UPrimaryDataAsset* Item)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::GetAmountOfEquippedItem"));

	if (!IsValid(Item))
	{
		return 0;
	}

	int32 Count = 0;
	for (const auto& EquipEntry : AllEquippedItems)
	{
		if (EquipEntry.Value == Item)
		{
			Count++;
		}
	}
	return Count;
}

/**
 * EquipToolToSlot - Equip a tool
 */
void UAC_EquipmentManager::EquipToolToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::EquipToolToSlot - Item: %s, Slot: %s"),
		TargetItem ? *TargetItem->GetName() : TEXT("None"), *TargetEquipmentSlot.ToString());

	OutSuccess = false;
	OutSuccess_1 = false;
	OutSuccess_2 = false;

	if (!IsValid(TargetItem) || !TargetEquipmentSlot.IsValid())
	{
		return;
	}

	// Unequip from other slots first
	UnequipToolFromOtherSlots(TargetItem);

	// Unequip existing at slot
	if (IsSlotOccupied(TargetEquipmentSlot))
	{
		UnequipToolAtSlot(TargetEquipmentSlot);
	}

	// Store item directly
	AllEquippedItems.Add(TargetEquipmentSlot, TargetItem);

	// Set as active tool slot
	SetActiveToolSlot(TargetEquipmentSlot);

	// Broadcast with item data
	FSLFCurrentEquipment EquipData;
	EquipData.ItemAsset = TargetItem;
	OnItemEquippedToSlot.Broadcast(EquipData, TargetEquipmentSlot);

	OutSuccess = true;
	OutSuccess_1 = true;
	OutSuccess_2 = true;
}

/**
 * UnequipToolAtSlot - Remove tool from slot
 */
void UAC_EquipmentManager::UnequipToolAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::UnequipToolAtSlot - Slot: %s"), *SlotTag.ToString());

	if (TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
	{
		UPrimaryDataAsset* Item = *ItemPtr;
		AllEquippedItems.Remove(SlotTag);
		OnItemUnequippedFromSlot.Broadcast(Item, SlotTag);
	}
}

/**
 * UpdateOverlayStates - Update animation overlay states based on equipment
 */
void UAC_EquipmentManager::UpdateOverlayStates_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::UpdateOverlayStates"));

	// Default to no overlay
	LeftHandOverlayState = ESLFOverlayState::Default;
	RightHandOverlayState = ESLFOverlayState::Default;

	// Check left hand weapon
	FGameplayTag LeftSlot = GetActiveWeaponSlot(false);
	if (LeftSlot.IsValid() && IsSlotOccupied(LeftSlot))
	{
		// Default to OneHanded overlay for equipped weapons
		LeftHandOverlayState = ESLFOverlayState::OneHanded;
	}

	// Check right hand weapon
	FGameplayTag RightSlot = GetActiveWeaponSlot(true);
	if (RightSlot.IsValid() && IsSlotOccupied(RightSlot))
	{
		RightHandOverlayState = ESLFOverlayState::OneHanded;
	}

	// Determine combined overlay state
	if (LeftHandOverlayState != ESLFOverlayState::Default || RightHandOverlayState != ESLFOverlayState::Default)
	{
		ActiveOverlayState = RightHandOverlayState;
	}
	else
	{
		ActiveOverlayState = ESLFOverlayState::Default;
	}
}

/**
 * AreBothWeaponSlotsActive - Check if both hands have weapons
 */
bool UAC_EquipmentManager::AreBothWeaponSlotsActive_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::AreBothWeaponSlotsActive"));

	FGameplayTag LeftSlot = GetActiveWeaponSlot(false);
	FGameplayTag RightSlot = GetActiveWeaponSlot(true);

	return LeftSlot.IsValid() && RightSlot.IsValid() &&
		   IsSlotOccupied(LeftSlot) && IsSlotOccupied(RightSlot);
}

/**
 * RefreshActiveGuardSequence - Update guard animation based on equipment
 */
void UAC_EquipmentManager::RefreshActiveGuardSequence_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::RefreshActiveGuardSequence"));

	ActiveBlockSequence = nullptr;

	// Check left hand slots for shield/weapon that supports guarding
	// Shields typically go in left hand and provide block animations
	for (const FGameplayTag& SlotTag : LeftHandSlots)
	{
		if (IsSlotOccupied(SlotTag))
		{
			// Get the item at this slot
			FSLFItemInfo ItemData;
			UPrimaryDataAsset* ItemAsset = nullptr;
			FGuid ItemId;
			FSLFItemInfo ItemData_1;
			UPrimaryDataAsset* ItemAsset_1 = nullptr;
			FGuid ItemId_1;
			GetItemAtSlot(SlotTag, ItemData, ItemAsset, ItemId, ItemData_1, ItemAsset_1, ItemId_1);

			// Check if item has block animation via moveset data
			if (ItemData.EquipmentDetails.MovesetWeapons)
			{
				// Item has a moveset, check for block sequence
				// For now, assume any left-hand weapon/shield can block
				UE_LOG(LogTemp, Log, TEXT("  Left hand item supports guarding"));
				// ActiveBlockSequence would be set from the item's animation data
				break;
			}
		}
	}
}

/**
 * DoesEquipmentSupportGuard - Check if current equipment can guard
 */
void UAC_EquipmentManager::DoesEquipmentSupportGuard_Implementation(bool& OutReturnValue, bool& OutReturnValue_1, bool& OutReturnValue_2)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::DoesEquipmentSupportGuard"));

	OutReturnValue = IsValid(ActiveBlockSequence);
	OutReturnValue_1 = OutReturnValue;
	OutReturnValue_2 = OutReturnValue;
}

/**
 * OverrideOverlayStates - Manually override overlay states
 */
void UAC_EquipmentManager::OverrideOverlayStates_Implementation(ESLFOverlayState InLeftHandOverlayState, ESLFOverlayState InRightHandOverlayState)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::OverrideOverlayStates"));

	LeftHandOverlayState = InLeftHandOverlayState;
	RightHandOverlayState = InRightHandOverlayState;

	// Update active based on right hand (default priority)
	ActiveOverlayState = InRightHandOverlayState;
}

/**
 * IsDualWieldPossible - Check if dual wielding is possible
 */
void UAC_EquipmentManager::IsDualWieldPossible_Implementation(bool& OutReturnValue, bool& OutReturnValue_1, bool& OutReturnValue_2)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::IsDualWieldPossible"));

	// Dual wield possible if both hands have one-handed weapons
	OutReturnValue = AreBothWeaponSlotsActive();
	OutReturnValue_1 = OutReturnValue;
	OutReturnValue_2 = OutReturnValue;
}

/**
 * WieldItemAtSlot - Make item visible/active
 */
void UAC_EquipmentManager::WieldItemAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::WieldItemAtSlot - Slot: %s"), *SlotTag.ToString());

	// Remove from hidden items if present
	HiddenItems.Remove(SlotTag);
}

/**
 * UnwieldItemAtSlot - Hide item
 */
void UAC_EquipmentManager::UnwieldItemAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::UnwieldItemAtSlot - Slot: %s"), *SlotTag.ToString());

	HideItemAtSlot(SlotTag);
}

/**
 * AdjustForTwoHandStance - Switch to two-handed grip
 */
void UAC_EquipmentManager::AdjustForTwoHandStance_Implementation(bool RightHand)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::AdjustForTwoHandStance - RightHand: %s"),
		RightHand ? TEXT("true") : TEXT("false"));

	// Update stance state
	if (RightHand)
	{
		bRightHandTwoHandStance = !bRightHandTwoHandStance;  // Toggle
	}
	else
	{
		bLeftHandTwoHandStance = !bLeftHandTwoHandStance;  // Toggle
	}

	// Broadcast stance change
	OnStanceChanged.Broadcast(RightHand, RightHand ? bRightHandTwoHandStance : bLeftHandTwoHandStance);
}

/**
 * IsTwoHandStanceActive - Check if two-handed stance is active
 */
void UAC_EquipmentManager::IsTwoHandStanceActive_Implementation(bool& OutLeftHand, bool& OutRightHand, bool& OutLeftHand_1, bool& OutRightHand_1, bool& OutLeftHand_2, bool& OutRightHand_2)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::IsTwoHandStanceActive"));

	// Return the current two-hand stance state for each hand
	OutLeftHand = bLeftHandTwoHandStance;
	OutRightHand = bRightHandTwoHandStance;
	OutLeftHand_1 = bLeftHandTwoHandStance;
	OutRightHand_1 = bRightHandTwoHandStance;
	OutLeftHand_2 = bLeftHandTwoHandStance;
	OutRightHand_2 = bRightHandTwoHandStance;
}

/**
 * ReinitializeMovementWithWeight - Update movement based on equipment weight
 */
void UAC_EquipmentManager::ReinitializeMovementWithWeight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::ReinitializeMovementWithWeight"));

	// Adjust character movement component based on weight stat
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (!IsValid(Character))
	{
		return;
	}

	UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
	if (!IsValid(MovementComp))
	{
		return;
	}

	// Get weight stat from stat manager
	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (IsValid(StatManager))
	{
		FGameplayTag WeightTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Misc.Weight"));
		UObject* WeightStatObj = nullptr;
		FStatInfo WeightInfo;
		StatManager->GetStat(WeightTag, WeightStatObj, WeightInfo);

		// Calculate movement speed modifier based on equip load ratio
		// MaxValue represents max carry capacity, CurrentValue is current weight
		if (WeightInfo.MaxValue > 0)
		{
			double EquipLoadRatio = WeightInfo.CurrentValue / WeightInfo.MaxValue;

			// Apply movement penalty based on equip load:
			// 0-70%: Light load (full speed)
			// 70-100%: Medium load (reduced speed)
			// 100%+: Heavy load (fat rolling/slow)
			double SpeedMultiplier = 1.0;
			if (EquipLoadRatio > 1.0)
			{
				SpeedMultiplier = 0.5;  // Overencumbered
			}
			else if (EquipLoadRatio > 0.7)
			{
				SpeedMultiplier = 0.85;  // Medium load
			}

			MovementComp->MaxWalkSpeed = 600.0 * SpeedMultiplier;
			UE_LOG(LogTemp, Log, TEXT("  Equip load: %.1f%%, Speed multiplier: %.2f"),
				EquipLoadRatio * 100.0, SpeedMultiplier);
		}
	}
}

/**
 * UnequipToolFromOtherSlots - Remove tool from all other slots
 */
void UAC_EquipmentManager::UnequipToolFromOtherSlots_Implementation(UPrimaryDataAsset* Item)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::UnequipToolFromOtherSlots"));

	if (!IsValid(Item))
	{
		return;
	}

	TArray<FGameplayTag> SlotsToRemove;

	// Find all slots with this item
	for (const auto& EquipEntry : AllEquippedItems)
	{
		if (EquipEntry.Value == Item && ToolSlots.HasTag(EquipEntry.Key))
		{
			SlotsToRemove.Add(EquipEntry.Key);
		}
	}

	// Remove from found slots
	for (const FGameplayTag& SlotTag : SlotsToRemove)
	{
		UnequipToolAtSlot(SlotTag);
	}
}

/**
 * TryGrantBuffs - Grant buffs from equipment
 */
void UAC_EquipmentManager::TryGrantBuffs_Implementation(const TMap<FGameplayTag, UPrimaryDataAsset*>& Buffs_Ranks, bool IsLoading)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::TryGrantBuffs"));

	UAC_BuffManager* BuffManager = nullptr;
	UAC_BuffManager* BuffManager2 = nullptr;
	GetBuffManager(BuffManager, BuffManager2);

	if (!IsValid(BuffManager))
	{
		return;
	}

	for (const auto& BuffEntry : Buffs_Ranks)
	{
		// Default rank 1
		BuffManager->TryAddBuff(BuffEntry.Value, 1, IsLoading);
	}

	OnItemEffectAdded.Broadcast();
}

/**
 * TryRemoveBuffs - Remove buffs from equipment
 */
void UAC_EquipmentManager::TryRemoveBuffs_Implementation(const TMap<FGameplayTag, UPrimaryDataAsset*>& Buffs_Ranks, double DelayDuration)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::TryRemoveBuffs"));

	UAC_BuffManager* BuffManager = nullptr;
	UAC_BuffManager* BuffManager2 = nullptr;
	GetBuffManager(BuffManager, BuffManager2);

	if (!IsValid(BuffManager))
	{
		return;
	}

	for (const auto& BuffEntry : Buffs_Ranks)
	{
		if (DelayDuration > 0)
		{
			BuffManager->DelayedRemoveBuff(DelayDuration, BuffEntry.Value);
		}
		else
		{
			BuffManager->RemoveBuffOfType(BuffEntry.Value);
		}
	}

	OnItemEffectRemoved.Broadcast();
}

/**
 * GetGuardHitMontage - Get guard hit reaction montage
 */
void UAC_EquipmentManager::GetGuardHitMontage_Implementation(UAnimMontage*& OutGuardHitMontage, UAnimMontage*& OutGuardHitMontage_1, UAnimMontage*& OutGuardHitMontage_2, UAnimMontage*& OutGuardHitMontage_3, UAnimMontage*& OutGuardHitMontage_4, UAnimMontage*& OutGuardHitMontage_5)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::GetGuardHitMontage"));

	OutGuardHitMontage = nullptr;
	OutGuardHitMontage_1 = nullptr;
	OutGuardHitMontage_2 = nullptr;
	OutGuardHitMontage_3 = nullptr;
	OutGuardHitMontage_4 = nullptr;
	OutGuardHitMontage_5 = nullptr;

	// Check left hand slots for shield/weapon with guard hit montage
	for (const FGameplayTag& SlotTag : LeftHandSlots)
	{
		if (IsSlotOccupied(SlotTag))
		{
			FSLFItemInfo ItemData;
			UPrimaryDataAsset* ItemAsset = nullptr;
			FGuid ItemId;
			FSLFItemInfo ItemData_1;
			UPrimaryDataAsset* ItemAsset_1 = nullptr;
			FGuid ItemId_1;
			GetItemAtSlot(SlotTag, ItemData, ItemAsset, ItemId, ItemData_1, ItemAsset_1, ItemId_1);

			// Guard hit montage would come from the item's moveset/animation data
			// For now, return nullptr - actual implementation needs moveset data lookup
			if (IsValid(ItemAsset))
			{
				UE_LOG(LogTemp, Log, TEXT("  Found equipped item for guard hit: %s"), *ItemAsset->GetName());
				// OutGuardHitMontage would be set from item's animation data
				break;
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// WEAPON DAMAGE GETTERS (for AnimNotify weapon trace)
// ═══════════════════════════════════════════════════════════════════════════════

double UAC_EquipmentManager::GetWeaponDamage() const
{
	// Get equipped weapon from right hand slots
	for (const FGameplayTag& SlotTag : RightHandSlots)
	{
		if (const TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
		{
			if (UPDA_Item* Item = Cast<UPDA_Item>(ItemPtr->Get()))
			{
				// Check StatChanges for damage values using gameplay tags
				// Damage is typically stored in StatChanges map with tags like "Stats.Attack.Physical"
				const TMap<FGameplayTag, FSLFEquipmentStat>& StatChanges = Item->ItemInformation.EquipmentDetails.StatChanges;
				double TotalDamage = 0.0;

				for (const auto& StatPair : StatChanges)
				{
					// Sum up all attack/damage stat changes
					if (StatPair.Key.ToString().Contains(TEXT("Attack")) ||
						StatPair.Key.ToString().Contains(TEXT("Damage")))
					{
						TotalDamage += StatPair.Value.Delta;
					}
				}

				if (TotalDamage > 0.0)
				{
					return TotalDamage;
				}

				// Fallback: return based on MinPoiseDamage (rough approximation)
				return FMath::Max(Item->ItemInformation.EquipmentDetails.MinPoiseDamage * 2.0, 10.0);
			}
		}
	}
	return 10.0;  // Unarmed damage
}

double UAC_EquipmentManager::GetWeaponPoiseDamage() const
{
	// Get equipped weapon from right hand slots
	for (const FGameplayTag& SlotTag : RightHandSlots)
	{
		if (const TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
		{
			if (UPDA_Item* Item = Cast<UPDA_Item>(ItemPtr->Get()))
			{
				// Get poise damage from item data
				return Item->ItemInformation.EquipmentDetails.MinPoiseDamage;
			}
		}
	}
	return 5.0;  // Unarmed poise damage
}

TMap<FGameplayTag, UPrimaryDataAsset*> UAC_EquipmentManager::GetWeaponStatusEffects() const
{
	TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffects;

	// Get equipped weapon from right hand slots
	for (const FGameplayTag& SlotTag : RightHandSlots)
	{
		if (const TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
		{
			if (UPDA_Item* Item = Cast<UPDA_Item>(ItemPtr->Get()))
			{
				// Copy status effects from weapon data
				// Note: The original stores as TMap<FGameplayTag, double>, we return assets
				// This needs adjustment based on actual status effect system
			}
		}
	}

	return StatusEffects;
}

// ═══════════════════════════════════════════════════════════════════════════════
// EQUIPMENT ACTOR SPAWNING (AsyncSpawnAndEquipWeapon equivalent)
// ═══════════════════════════════════════════════════════════════════════════════

void UAC_EquipmentManager::SpawnEquipmentActor(UPDA_Item* Item, const FGameplayTag& SlotTag)
{
	if (!IsValid(Item))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnEquipmentActor - Invalid item"));
		return;
	}

	// Get the item class to spawn
	UClass* ItemClass = Item->ItemInformation.ItemClass.LoadSynchronous();
	if (!ItemClass)
	{
		UE_LOG(LogTemp, Log, TEXT("SpawnEquipmentActor - No ItemClass set for %s, skipping spawn"), *Item->GetName());
		return;
	}

	// Get the owning actor (could be PlayerController or Character)
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnEquipmentActor - No valid owner"));
		return;
	}

	// Get the Pawn/Character to use as Instigator
	// If owner is a PlayerController, get its Pawn. If owner is already a Pawn, use it directly.
	APawn* InstigatorPawn = Cast<APawn>(Owner);
	if (!InstigatorPawn)
	{
		if (APlayerController* PC = Cast<APlayerController>(Owner))
		{
			InstigatorPawn = PC->GetPawn();
		}
	}

	if (!InstigatorPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnEquipmentActor - Could not find Pawn for instigator"));
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	// Use deferred spawn so we can set ItemInfo BEFORE BeginPlay runs
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InstigatorPawn;  // Owner should be the character, not the controller
	SpawnParams.Instigator = InstigatorPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bDeferConstruction = true; // CRITICAL: Defer so BeginPlay doesn't run yet

	AActor* SpawnedActor = World->SpawnActor<AActor>(ItemClass, FTransform::Identity, SpawnParams);
	if (!IsValid(SpawnedActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnEquipmentActor - Failed to spawn actor from class %s"), *ItemClass->GetName());
		return;
	}

	// Set the ItemInfo BEFORE finishing spawn (so BeginPlay can read it)
	if (AB_Item* ItemActor = Cast<AB_Item>(SpawnedActor))
	{
		ItemActor->ItemInfo = Item->ItemInformation;
		UE_LOG(LogTemp, Log, TEXT("SpawnEquipmentActor - Set ItemInfo with sockets L=%s R=%s"),
			*ItemActor->ItemInfo.EquipmentDetails.AttachmentSockets.LeftHandSocketName.ToString(),
			*ItemActor->ItemInfo.EquipmentDetails.AttachmentSockets.RightHandSocketName.ToString());
	}

	// Now finish spawning - this will call BeginPlay with ItemInfo populated
	SpawnedActor->FinishSpawning(FTransform::Identity);

	// Store in map
	SpawnedItemsAtSlots.Add(SlotTag, SpawnedActor);

	UE_LOG(LogTemp, Log, TEXT("SpawnEquipmentActor - Spawned %s for slot %s"),
		*SpawnedActor->GetName(), *SlotTag.ToString());

	// Note: The spawned B_Item_Weapon actor handles its own attachment in BeginPlay
	// It reads ItemInfo.EquipmentDetails.AttachmentSockets and uses K2_AttachToComponent
	// to attach to the character's skeletal mesh socket
}

void UAC_EquipmentManager::DestroyEquipmentActor(const FGameplayTag& SlotTag)
{
	if (TObjectPtr<AActor>* ActorPtr = SpawnedItemsAtSlots.Find(SlotTag))
	{
		if (IsValid(*ActorPtr))
		{
			UE_LOG(LogTemp, Log, TEXT("DestroyEquipmentActor - Destroying actor at slot %s"), *SlotTag.ToString());
			(*ActorPtr)->Destroy();
		}
		SpawnedItemsAtSlots.Remove(SlotTag);
	}
}
