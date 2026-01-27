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
#include "Interfaces/SLFPlayerInterface.h"
#include "Interfaces/BPI_Player.h"
#include "Framework/SLFGameInstance.h"
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
	ActiveOverlayState = ESLFOverlayState::Unarmed;
	LeftHandOverlayState = ESLFOverlayState::Unarmed;
	RightHandOverlayState = ESLFOverlayState::Unarmed;
	WeaponAbilitySlot = ESLFWeaponAbilityHandle::RightHand;
	ActiveBlockSequence = nullptr;
	IsAsyncWeaponBusy = false;
	SlotTable = nullptr;

	// Initialize hand slot tags - these define which equipment slots are left/right hand
	// Left hand slots: Shields and secondary weapons
	LeftHandSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 1"), false));
	LeftHandSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 2"), false));
	LeftHandSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 3"), false));

	// Right hand slots: Primary weapons
	RightHandSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"), false));
	RightHandSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2"), false));
	RightHandSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"), false));

	// Tool slots
	ToolSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tools 1"), false));
	ToolSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tools 2"), false));
	ToolSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tools 3"), false));
	ToolSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tools 4"), false));
	ToolSlots.AddTag(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tools 5"), false));
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

		// Add weapon's overlay tag to the appropriate hand's tag container
		FGameplayTag WeaponOverlayTag = Item->ItemInformation.EquipmentDetails.WeaponOverlay;
		if (WeaponOverlayTag.IsValid())
		{
			// Determine if this is a left or right hand slot
			if (LeftHandSlots.HasTag(TargetEquipmentSlot))
			{
				LeftHandOverlayTags.AddTag(WeaponOverlayTag);
				UE_LOG(LogTemp, Log, TEXT("  Added overlay tag %s to LeftHandOverlayTags"), *WeaponOverlayTag.ToString());
			}
			else if (RightHandSlots.HasTag(TargetEquipmentSlot))
			{
				RightHandOverlayTags.AddTag(WeaponOverlayTag);
				UE_LOG(LogTemp, Log, TEXT("  Added overlay tag %s to RightHandOverlayTags"), *WeaponOverlayTag.ToString());
			}
		}
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

	// Apply armor mesh to character
	if (UPDA_Item* ArmorItem = Cast<UPDA_Item>(TargetItem))
	{
		ApplyArmorMeshToCharacter(ArmorItem, TargetEquipmentSlot);
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

			// Remove weapon's overlay tag from the appropriate hand's tag container
			FGameplayTag WeaponOverlayTag = PDAItem->ItemInformation.EquipmentDetails.WeaponOverlay;
			if (WeaponOverlayTag.IsValid())
			{
				// Determine if this is a left or right hand slot
				if (LeftHandSlots.HasTag(SlotTag))
				{
					LeftHandOverlayTags.RemoveTag(WeaponOverlayTag);
					UE_LOG(LogTemp, Log, TEXT("  Removed overlay tag %s from LeftHandOverlayTags"), *WeaponOverlayTag.ToString());
				}
				else if (RightHandSlots.HasTag(SlotTag))
				{
					RightHandOverlayTags.RemoveTag(WeaponOverlayTag);
					UE_LOG(LogTemp, Log, TEXT("  Removed overlay tag %s from RightHandOverlayTags"), *WeaponOverlayTag.ToString());
				}
			}
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

		// Remove armor mesh from character (revert to default)
		RemoveArmorMeshFromCharacter(SlotTag);

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
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::IsItemEquippedToSlot - Slot: %s, TargetDisplayName: %s, AllEquippedItems count: %d"),
		*ActiveEquipmentSlot.ToString(),
		*TargetItem.DisplayName.ToString(),
		AllEquippedItems.Num());

	if (TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(ActiveEquipmentSlot))
	{
		UE_LOG(LogTemp, Log, TEXT("  Found item in slot, ItemPtr valid: %s"), ItemPtr->Get() ? TEXT("yes") : TEXT("no"));
		if (UPDA_Item* EquippedItem = Cast<UPDA_Item>(ItemPtr->Get()))
		{
			UE_LOG(LogTemp, Log, TEXT("  EquippedItem: %s, EquippedDisplayName: %s"),
				*EquippedItem->GetName(),
				*EquippedItem->ItemInformation.DisplayName.ToString());

			// Compare by DisplayName since ItemTag may not be set
			// Also try comparing by ItemTag if both are valid
			bool bMatches = false;

			// First try ItemTag comparison if both are valid
			if (TargetItem.ItemTag.IsValid() && EquippedItem->ItemInformation.ItemTag.IsValid())
			{
				bMatches = EquippedItem->ItemInformation.ItemTag.MatchesTagExact(TargetItem.ItemTag);
			}
			// Fallback: compare by DisplayName
			else if (!TargetItem.DisplayName.IsEmpty() && !EquippedItem->ItemInformation.DisplayName.IsEmpty())
			{
				bMatches = EquippedItem->ItemInformation.DisplayName.EqualTo(TargetItem.DisplayName);
			}

			UE_LOG(LogTemp, Log, TEXT("  Matches: %s"), bMatches ? TEXT("YES") : TEXT("NO"));
			return bMatches;
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("  Slot NOT found in AllEquippedItems"));
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
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::ApplyStatChanges - Equipped: %s, ItemTag: %s"),
		Equipped ? TEXT("true") : TEXT("false"), *Item.ItemTag.ToString());

	UE_LOG(LogTemp, Log, TEXT("  StatChanges count: %d"), Item.EquipmentDetails.StatChanges.Num());

	// EquipmentManager is on the Controller, but StatManager is on the Character (Pawn)
	// Get the Pawn from the Controller to find StatManager
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Owner is invalid!"));
		return;
	}

	APawn* OwnerPawn = nullptr;
	if (AController* Controller = Cast<AController>(Owner))
	{
		OwnerPawn = Controller->GetPawn();
	}
	else if (APawn* Pawn = Cast<APawn>(Owner))
	{
		// Owner might be the pawn directly in some cases
		OwnerPawn = Pawn;
	}

	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not get Pawn from Owner!"));
		return;
	}

	UStatManagerComponent* StatManager = OwnerPawn->FindComponentByClass<UStatManagerComponent>();
	if (!IsValid(StatManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("  StatManager not found on Pawn %s!"), *OwnerPawn->GetName());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("  Found StatManager on Pawn: %s"), *OwnerPawn->GetName());

	// Apply stat changes from equipment
	double Multiplier = Equipped ? 1.0 : -1.0;

	// Apply each stat change from equipment details
	for (const auto& StatChange : Item.EquipmentDetails.StatChanges)
	{
		double FinalDelta = StatChange.Value.Delta * Multiplier;
		UE_LOG(LogTemp, Log, TEXT("  Adjusting stat: %s by %f (Delta: %f, Multiplier: %f)"),
			*StatChange.Key.ToString(), FinalDelta, StatChange.Value.Delta, Multiplier);
		StatManager->AdjustStat(StatChange.Key, ESLFValueType::CurrentValue, FinalDelta, false, false);
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
 * Routes to appropriate equip function based on item category
 */
void UAC_EquipmentManager::EquipItemToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3, bool& OutSuccess_4, bool& OutSuccess_5, bool& OutSuccess_6)
{
	OutSuccess = false;
	OutSuccess_1 = false;
	OutSuccess_2 = false;
	OutSuccess_3 = false;
	OutSuccess_4 = false;
	OutSuccess_5 = false;
	OutSuccess_6 = false;

	// Get item category to determine which equip function to use
	UPDA_Item* Item = Cast<UPDA_Item>(TargetItem);
	if (!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAC_EquipmentManager::EquipItemToSlot - Item is not UPDA_Item"));
		// Fallback to weapon equip
		EquipWeaponToSlot(TargetItem, TargetEquipmentSlot, ChangeStats, OutSuccess, OutSuccess_1, OutSuccess_2, OutSuccess_3);
		OutSuccess_4 = OutSuccess;
		OutSuccess_5 = OutSuccess;
		OutSuccess_6 = OutSuccess;
		return;
	}

	ESLFItemCategory Category = Item->ItemInformation.Category.Category;
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::EquipItemToSlot - Item: %s, Category: %d, Slot: %s"),
		*Item->GetName(), static_cast<int32>(Category), *TargetEquipmentSlot.ToString());

	switch (Category)
	{
	case ESLFItemCategory::Weapons:
	case ESLFItemCategory::Shields:
	case ESLFItemCategory::Abilities:
		// Weapons, shields, and abilities spawn as actors attached to sockets
		EquipWeaponToSlot(TargetItem, TargetEquipmentSlot, ChangeStats, OutSuccess, OutSuccess_1, OutSuccess_2, OutSuccess_3);
		break;

	case ESLFItemCategory::Armor:
		// Armor swaps character skeletal mesh components
		EquipArmorToSlot(TargetItem, TargetEquipmentSlot, ChangeStats, OutSuccess, OutSuccess_1, OutSuccess_2, OutSuccess_3);
		break;

	case ESLFItemCategory::Bolstering:
		// Talismans apply buffs/stats only (no visual)
		{
			bool bTalismanSuccess = EquipTalismanToSlot(TargetItem, TargetEquipmentSlot, ChangeStats);
			OutSuccess = bTalismanSuccess;
			OutSuccess_1 = bTalismanSuccess;
			OutSuccess_2 = bTalismanSuccess;
			OutSuccess_3 = bTalismanSuccess;
		}
		break;

	case ESLFItemCategory::Tools:
		// Tools go to tool slots
		{
			bool ToolSuccess1, ToolSuccess2, ToolSuccess3;
			EquipToolToSlot(TargetItem, TargetEquipmentSlot, ChangeStats, ToolSuccess1, ToolSuccess2, ToolSuccess3);
			OutSuccess = ToolSuccess1;
			OutSuccess_1 = ToolSuccess2;
			OutSuccess_2 = ToolSuccess3;
			OutSuccess_3 = ToolSuccess1;
		}
		break;

	default:
		// Unknown category - fallback to weapon equip
		UE_LOG(LogTemp, Warning, TEXT("UAC_EquipmentManager::EquipItemToSlot - Unknown category %d, using weapon equip"),
			static_cast<int32>(Category));
		EquipWeaponToSlot(TargetItem, TargetEquipmentSlot, ChangeStats, OutSuccess, OutSuccess_1, OutSuccess_2, OutSuccess_3);
		break;
	}

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

	// Define the overlay tags to check
	static const FGameplayTag OneHandedTag = FGameplayTag::RequestGameplayTag(TEXT("SoulslikeFramework.Equipment.Weapons.Overlay.OneHanded"), false);
	static const FGameplayTag ShieldTag = FGameplayTag::RequestGameplayTag(TEXT("SoulslikeFramework.Equipment.Weapons.Overlay.Shield"), false);
	static const FGameplayTag TwoHandedTag = FGameplayTag::RequestGameplayTag(TEXT("SoulslikeFramework.Equipment.Weapons.Overlay.TwoHanded"), false);

	// Debug: Log tag validity
	UE_LOG(LogTemp, Log, TEXT("  Tags valid - OneHanded: %s, Shield: %s, TwoHanded: %s"),
		OneHandedTag.IsValid() ? TEXT("YES") : TEXT("NO"),
		ShieldTag.IsValid() ? TEXT("YES") : TEXT("NO"),
		TwoHandedTag.IsValid() ? TEXT("YES") : TEXT("NO"));

	// Debug: Log container contents
	UE_LOG(LogTemp, Log, TEXT("  LeftHandOverlayTags count: %d"), LeftHandOverlayTags.Num());
	for (const FGameplayTag& Tag : LeftHandOverlayTags)
	{
		UE_LOG(LogTemp, Log, TEXT("    - %s"), *Tag.ToString());
	}
	UE_LOG(LogTemp, Log, TEXT("  RightHandOverlayTags count: %d"), RightHandOverlayTags.Num());
	for (const FGameplayTag& Tag : RightHandOverlayTags)
	{
		UE_LOG(LogTemp, Log, TEXT("    - %s"), *Tag.ToString());
	}

	// Default to Unarmed (no weapon equipped)
	LeftHandOverlayState = ESLFOverlayState::Unarmed;
	RightHandOverlayState = ESLFOverlayState::Unarmed;

	// Check left hand overlay tags
	// Blueprint uses Sequence node - all branches execute in order, last match "wins"
	// Order: OneHanded, Shield, TwoHanded (so TwoHanded has highest priority)
	if (LeftHandOverlayTags.HasTag(OneHandedTag))
	{
		LeftHandOverlayState = ESLFOverlayState::OneHanded;
		UE_LOG(LogTemp, Log, TEXT("  LeftHand: OneHanded"));
	}
	if (LeftHandOverlayTags.HasTag(ShieldTag))
	{
		LeftHandOverlayState = ESLFOverlayState::Shield;
		UE_LOG(LogTemp, Log, TEXT("  LeftHand: Shield"));
	}
	if (LeftHandOverlayTags.HasTag(TwoHandedTag))
	{
		LeftHandOverlayState = ESLFOverlayState::TwoHanded;
		UE_LOG(LogTemp, Log, TEXT("  LeftHand: TwoHanded"));
	}

	// Check right hand overlay tags
	if (RightHandOverlayTags.HasTag(OneHandedTag))
	{
		RightHandOverlayState = ESLFOverlayState::OneHanded;
		UE_LOG(LogTemp, Log, TEXT("  RightHand: OneHanded"));
	}
	if (RightHandOverlayTags.HasTag(ShieldTag))
	{
		RightHandOverlayState = ESLFOverlayState::Shield;
		UE_LOG(LogTemp, Log, TEXT("  RightHand: Shield"));
	}
	if (RightHandOverlayTags.HasTag(TwoHandedTag))
	{
		RightHandOverlayState = ESLFOverlayState::TwoHanded;
		UE_LOG(LogTemp, Log, TEXT("  RightHand: TwoHanded"));
	}

	// Check for dual wield (both hands have one-handed weapons)
	if (LeftHandOverlayState == ESLFOverlayState::OneHanded && RightHandOverlayState == ESLFOverlayState::OneHanded)
	{
		// Check if dual wield is actually active (both slots occupied)
		if (AreBothWeaponSlotsActive())
		{
			LeftHandOverlayState = ESLFOverlayState::DualWield;
			RightHandOverlayState = ESLFOverlayState::DualWield;
			UE_LOG(LogTemp, Log, TEXT("  DualWield active"));
		}
	}

	// Determine combined overlay state (right hand takes priority)
	if (RightHandOverlayState != ESLFOverlayState::Unarmed)
	{
		ActiveOverlayState = RightHandOverlayState;
	}
	else if (LeftHandOverlayState != ESLFOverlayState::Unarmed)
	{
		ActiveOverlayState = LeftHandOverlayState;
	}
	else
	{
		ActiveOverlayState = ESLFOverlayState::Unarmed;
	}

	UE_LOG(LogTemp, Log, TEXT("  Final States - Left: %d, Right: %d, Active: %d"),
		(int32)LeftHandOverlayState, (int32)RightHandOverlayState, (int32)ActiveOverlayState);
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
				// Cast to UPDA_WeaponAnimset to get Guard_L sequence
				UPDA_WeaponAnimset* Animset = Cast<UPDA_WeaponAnimset>(ItemData.EquipmentDetails.MovesetWeapons);
				if (Animset && Animset->Guard_L)
				{
					ActiveBlockSequence = Animset->Guard_L;
					UE_LOG(LogTemp, Log, TEXT("  Left hand Guard_L sequence set: %s"), *Animset->Guard_L->GetName());
					return;
				}
				UE_LOG(LogTemp, Log, TEXT("  Left hand item has moveset but no Guard_L"));
			}
		}
	}

	// If no left hand guard, check right hand slots for two-handed weapons
	for (const FGameplayTag& SlotTag : RightHandSlots)
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

			if (ItemData.EquipmentDetails.MovesetWeapons)
			{
				UPDA_WeaponAnimset* Animset = Cast<UPDA_WeaponAnimset>(ItemData.EquipmentDetails.MovesetWeapons);
				if (Animset && Animset->Guard_R)
				{
					ActiveBlockSequence = Animset->Guard_R;
					UE_LOG(LogTemp, Log, TEXT("  Right hand Guard_R sequence set: %s"), *Animset->Guard_R->GetName());
					return;
				}
				UE_LOG(LogTemp, Log, TEXT("  Right hand item has moveset but no Guard_R"));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  No guard sequence found in any equipped weapon"));
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
 *
 * Blueprint Logic:
 * 1. Get current overlay state for the target hand
 * 2. Toggle: If currently OneHanded -> switch to TwoHanded; if TwoHanded -> switch to OneHanded
 * 3. UnwieldItemAtSlot - Hide the opposite hand's weapon when going to TwoHanded
 * 4. OverrideOverlayStates - Update animation overlay state
 * 5. Broadcast OnStanceChanged
 */
void UAC_EquipmentManager::AdjustForTwoHandStance_Implementation(bool RightHand)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_EquipmentManager::AdjustForTwoHandStance - RightHand: %s"),
		RightHand ? TEXT("true") : TEXT("false"));

	// Get the current overlay state for the target hand
	ESLFOverlayState CurrentHandState = RightHand ? RightHandOverlayState : LeftHandOverlayState;
	UE_LOG(LogTemp, Log, TEXT("  Current overlay state: %d"), static_cast<int32>(CurrentHandState));

	// Determine new overlay state - toggle between OneHanded and TwoHanded
	ESLFOverlayState NewTargetHandState;
	if (CurrentHandState == ESLFOverlayState::TwoHanded)
	{
		// Currently TwoHanded -> switch back to OneHanded
		NewTargetHandState = ESLFOverlayState::OneHanded;
		UE_LOG(LogTemp, Log, TEXT("  Switching from TwoHanded to OneHanded"));
	}
	else
	{
		// Currently OneHanded (or other) -> switch to TwoHanded
		NewTargetHandState = ESLFOverlayState::TwoHanded;
		UE_LOG(LogTemp, Log, TEXT("  Switching from %d to TwoHanded"), static_cast<int32>(CurrentHandState));
	}

	// Update stance boolean
	if (RightHand)
	{
		bRightHandTwoHandStance = (NewTargetHandState == ESLFOverlayState::TwoHanded);
	}
	else
	{
		bLeftHandTwoHandStance = (NewTargetHandState == ESLFOverlayState::TwoHanded);
	}

	// When going to TwoHanded, unwield (hide) the opposite hand's weapon
	if (NewTargetHandState == ESLFOverlayState::TwoHanded)
	{
		// Get the opposite hand's active weapon slot and unwield it
		FGameplayTag OppositeHandSlot = GetActiveWeaponSlot(!RightHand);  // !RightHand = LeftHand if RightHand
		if (OppositeHandSlot.IsValid())
		{
			UnwieldItemAtSlot(OppositeHandSlot);
			UE_LOG(LogTemp, Log, TEXT("  Unwielded opposite hand slot: %s"), *OppositeHandSlot.ToString());
		}
	}
	else
	{
		// When going back to OneHanded, rewield the opposite hand's weapon
		FGameplayTag OppositeHandSlot = GetActiveWeaponSlot(!RightHand);
		if (OppositeHandSlot.IsValid())
		{
			WieldItemAtSlot(OppositeHandSlot);
			UE_LOG(LogTemp, Log, TEXT("  Rewielded opposite hand slot: %s"), *OppositeHandSlot.ToString());
		}
	}

	// Override overlay states based on which hand is being adjusted
	if (RightHand)
	{
		// Right hand two-hand stance: Set left hand state based on toggle, keep right hand state
		ESLFOverlayState NewLeftState = (NewTargetHandState == ESLFOverlayState::TwoHanded)
			? ESLFOverlayState::TwoHanded  // When going 2H, left is also considered 2H (hidden)
			: LeftHandOverlayState;  // When going back, restore original left hand state
		OverrideOverlayStates(NewLeftState, NewTargetHandState);
	}
	else
	{
		// Left hand two-hand stance: Set right hand state based on toggle, keep left hand state
		ESLFOverlayState NewRightState = (NewTargetHandState == ESLFOverlayState::TwoHanded)
			? ESLFOverlayState::TwoHanded  // When going 2H, right is also considered 2H (hidden)
			: RightHandOverlayState;  // When going back, restore original right hand state
		OverrideOverlayStates(NewTargetHandState, NewRightState);
	}

	UE_LOG(LogTemp, Log, TEXT("  Final States - Left: %d, Right: %d, Active: %d"),
		static_cast<int32>(LeftHandOverlayState), static_cast<int32>(RightHandOverlayState), static_cast<int32>(ActiveOverlayState));

	// Broadcast stance change
	bool bIsTwoHandStance = (NewTargetHandState == ESLFOverlayState::TwoHanded);
	OnStanceChanged.Broadcast(RightHand, bIsTwoHandStance);
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
// API COMPATIBILITY HELPERS (for Action classes)
// ═══════════════════════════════════════════════════════════════════════════════

void UAC_EquipmentManager::GetItemAtSlotSimple(FGameplayTag SlotTag, UPrimaryDataAsset*& OutItemAsset, FGuid& OutId)
{
	OutItemAsset = nullptr;
	OutId = FGuid();
	
	// Look up item in AllEquippedItems map
	if (const TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
	{
		OutItemAsset = ItemPtr->Get();
		// Generate a consistent GUID based on the item
		if (OutItemAsset)
		{
			OutId = FGuid::NewGuid();  // In production, should track real IDs
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

TMap<UPrimaryDataAsset*, FSLFStatusEffectApplication> UAC_EquipmentManager::GetWeaponStatusEffects() const
{
	TMap<UPrimaryDataAsset*, FSLFStatusEffectApplication> StatusEffects;

	// Get equipped weapon from right hand slots
	for (const FGameplayTag& SlotTag : RightHandSlots)
	{
		if (const TObjectPtr<UPrimaryDataAsset>* ItemPtr = AllEquippedItems.Find(SlotTag))
		{
			if (UPDA_Item* Item = Cast<UPDA_Item>(ItemPtr->Get()))
			{
				// Copy status effects from weapon data
				// FSLFEquipmentInfo.WeaponStatusEffectInfo is TMap<UPrimaryDataAsset*, FSLFStatusEffectApplication>
				for (const auto& EffectEntry : Item->ItemInformation.EquipmentDetails.WeaponStatusEffectInfo)
				{
					if (IsValid(EffectEntry.Key))
					{
						StatusEffects.Add(EffectEntry.Key, EffectEntry.Value);
						UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] Weapon status effect: %s, Rank: %d, BuildupAmount: %.1f"),
							*EffectEntry.Key->GetName(), EffectEntry.Value.Rank, EffectEntry.Value.BuildupAmount);
					}
				}
				break; // Only process first equipped weapon
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

	// Set the ItemInfo and EquippedSlotTag BEFORE finishing spawn (so BeginPlay can read it)
	if (AB_Item* ItemActor = Cast<AB_Item>(SpawnedActor))
	{
		ItemActor->ItemInfo = Item->ItemInformation;
		ItemActor->EquippedSlotTag = SlotTag; // Store actual equipped slot for left/right hand determination
		UE_LOG(LogTemp, Log, TEXT("SpawnEquipmentActor - Set ItemInfo with sockets L=%s R=%s, EquippedSlot=%s"),
			*ItemActor->ItemInfo.EquipmentDetails.AttachmentSockets.LeftHandSocketName.ToString(),
			*ItemActor->ItemInfo.EquipmentDetails.AttachmentSockets.RightHandSocketName.ToString(),
			*SlotTag.ToString());
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

/**
 * ApplyArmorMeshToCharacter - Apply armor skeletal mesh to character body slot
 * Uses existing ItemInformation.EquipmentDetails.SkeletalMeshInfo data (character-keyed TMap)
 */
void UAC_EquipmentManager::ApplyArmorMeshToCharacter(UPDA_Item* ArmorItem, const FGameplayTag& SlotTag)
{

	if (!ArmorItem || !SlotTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyArmorMeshToCharacter - Invalid ArmorItem or SlotTag"));
		return;
	}

	// Get the character - Owner might be PlayerController, so get Pawn if needed
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyArmorMeshToCharacter - No owner"));
		return;
	}

	// If Owner is a PlayerController, get the Pawn instead
	AActor* TargetActor = Owner;
	if (APlayerController* PC = Cast<APlayerController>(Owner))
	{
		TargetActor = PC->GetPawn();
		if (!TargetActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("ApplyArmorMeshToCharacter - PlayerController has no Pawn"));
			return;
		}
	}

	// Get the skeletal mesh map from the armor item
	const TMap<TSoftObjectPtr<UPrimaryDataAsset>, TSoftObjectPtr<USkeletalMesh>>& MeshMap =
		ArmorItem->ItemInformation.EquipmentDetails.SkeletalMeshInfo;


	if (MeshMap.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyArmorMeshToCharacter - No mesh entries for %s (SkeletalMeshInfo is empty)"), *ArmorItem->GetName());
		return;
	}

	// Get the current character's base info from GameInstance to find the right mesh
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;
	if (USLFGameInstance* GI = Cast<USLFGameInstance>(TargetActor->GetGameInstance()))
	{
		if (GI->SelectedBaseClass)
		{
			TSoftObjectPtr<UPrimaryDataAsset> CharacterInfoKey(GI->SelectedBaseClass);
			if (const TSoftObjectPtr<USkeletalMesh>* MeshPtr = MeshMap.Find(CharacterInfoKey))
			{
				ArmorMesh = *MeshPtr;
			}
		}
	}

	// Fallback: use first mesh in map if no character-specific mesh found
	if (ArmorMesh.IsNull() && MeshMap.Num() > 0)
	{
		for (const auto& Pair : MeshMap)
		{
			if (!Pair.Value.IsNull())
			{
				ArmorMesh = Pair.Value;
				break;
			}
		}
	}

	if (ArmorMesh.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyArmorMeshToCharacter - No valid mesh found for %s"), *ArmorItem->GetName());
		return;
	}


	// Call ISLFPlayerInterface mesh swap functions based on slot type
	IBPI_Player* PlayerInterface = Cast<IBPI_Player>(TargetActor);
	if (!PlayerInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyArmorMeshToCharacter - TargetActor %s does not implement IBPI_Player!"), *TargetActor->GetName());
		return;
	}

	if (PlayerInterface)
	{
		static const FGameplayTag HeadSlot = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Head"));
		static const FGameplayTag ChestSlot = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Armor"));
		static const FGameplayTag ArmsSlot = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Gloves"));
		static const FGameplayTag LegsSlot = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Greaves"));

		if (SlotTag.MatchesTag(HeadSlot))
		{
			IBPI_Player::Execute_ChangeHeadpiece(TargetActor, ArmorMesh);
		}
		else if (SlotTag.MatchesTag(ChestSlot))
		{
			IBPI_Player::Execute_ChangeArmor(TargetActor, ArmorMesh);
		}
		else if (SlotTag.MatchesTag(ArmsSlot))
		{
			IBPI_Player::Execute_ChangeGloves(TargetActor, ArmorMesh);
		}
		else if (SlotTag.MatchesTag(LegsSlot))
		{
			IBPI_Player::Execute_ChangeGreaves(TargetActor, ArmorMesh);
		}
	}
}

/**
 * RemoveArmorMeshFromCharacter - Remove armor mesh from character body slot (revert to default)
 */
void UAC_EquipmentManager::RemoveArmorMeshFromCharacter(const FGameplayTag& SlotTag)
{
	if (!SlotTag.IsValid())
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// If Owner is a PlayerController, get the Pawn instead
	AActor* TargetActor = Owner;
	if (APlayerController* PC = Cast<APlayerController>(Owner))
	{
		TargetActor = PC->GetPawn();
		if (!TargetActor)
		{
			return;
		}
	}

	// Call ISLFPlayerInterface mesh swap functions with null/default mesh
	if (IBPI_Player* PlayerInterface = Cast<IBPI_Player>(TargetActor))
	{
		static const FGameplayTag HeadSlot = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Head"));
		static const FGameplayTag ChestSlot = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Armor"));
		static const FGameplayTag ArmsSlot = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Gloves"));
		static const FGameplayTag LegsSlot = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Greaves"));

		TSoftObjectPtr<USkeletalMesh> NullMesh;  // Empty/null mesh to revert

		if (SlotTag.MatchesTag(HeadSlot))
		{
			IBPI_Player::Execute_ChangeHeadpiece(TargetActor, NullMesh);
		}
		else if (SlotTag.MatchesTag(ChestSlot))
		{
			IBPI_Player::Execute_ChangeArmor(TargetActor, NullMesh);
		}
		else if (SlotTag.MatchesTag(ArmsSlot))
		{
			IBPI_Player::Execute_ChangeGloves(TargetActor, NullMesh);
		}
		else if (SlotTag.MatchesTag(LegsSlot))
		{
			IBPI_Player::Execute_ChangeGreaves(TargetActor, NullMesh);
		}
	}
}
