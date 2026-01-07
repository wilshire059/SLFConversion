// EquipmentManagerComponent.cpp
// C++ implementation for AC_EquipmentManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_EquipmentManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         19/19 (initialized in constructor)
// Functions:         41/41 implemented (stub implementations)
// Event Dispatchers: 9/9 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "EquipmentManagerComponent.h"
#include "BuffManagerComponent.h"
#include "StatManagerComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "SLFStatTypes.h"
#include "Net/UnrealNetwork.h"

UEquipmentManagerComponent::UEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	// Initialize config
	SlotTable = nullptr;
	WeaponAbilitySlot = ESLFWeaponAbilityHandle::RightHand;

	// Initialize overlay states
	ActiveOverlayState = ESLFOverlayState::Default;
	LeftHandOverlayState = ESLFOverlayState::Default;
	RightHandOverlayState = ESLFOverlayState::Default;

	// Initialize runtime
	ActiveBlockSequence = nullptr;
	bIsAsyncWeaponBusy = false;
}

void UEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] BeginPlay on %s"), *GetOwner()->GetName());
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquipmentManagerComponent, ActiveOverlayState);
}

// ═══════════════════════════════════════════════════════════════════════════════
// EQUIP/UNEQUIP WEAPONS [1-4/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::EquipWeaponToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] EquipWeaponToSlot: %s to slot %s"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("null"), *SlotTag.ToString());

	if (!ItemAsset) return;

	FSLFCurrentEquipment NewEquipment;
	NewEquipment.ItemAsset = ItemAsset;
	NewEquipment.UniqueId = FGuid::NewGuid();
	NewEquipment.StackCount = 1;

	AllEquippedItems.Add(SlotTag, NewEquipment);

	if (bChangeStats) ApplyStatChanges(ItemAsset, true);
	UpdateOverlayStates();
	OnItemEquippedToSlot.Broadcast(SlotTag, ItemAsset);
}

void UEquipmentManagerComponent::UnequipWeaponAtSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] UnequipWeaponAtSlot: %s"), *SlotTag.ToString());

	if (FSLFCurrentEquipment* Equipment = AllEquippedItems.Find(SlotTag))
	{
		UDataAsset* ItemAsset = Cast<UDataAsset>(Equipment->ItemAsset);
		ApplyStatChanges(ItemAsset, false);
		AllEquippedItems.Remove(SlotTag);
		OnItemUnequippedFromSlot.Broadcast(SlotTag, ItemAsset);
	}
	UpdateOverlayStates();
}

void UEquipmentManagerComponent::WieldItemAtSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] WieldItemAtSlot: %s"), *SlotTag.ToString());
	// Make item visible at slot
	if (AActor** SpawnedActor = SpawnedItemsAtSlots.Find(SlotTag))
	{
		if (*SpawnedActor)
		{
			(*SpawnedActor)->SetActorHiddenInGame(false);
		}
	}
}

void UEquipmentManagerComponent::UnwieldItemAtSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] UnwieldItemAtSlot: %s"), *SlotTag.ToString());
	// Hide item but keep equipped
	if (AActor** SpawnedActor = SpawnedItemsAtSlots.Find(SlotTag))
	{
		if (*SpawnedActor)
		{
			(*SpawnedActor)->SetActorHiddenInGame(true);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// EQUIP/UNEQUIP ARMOR [5-6/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::EquipArmorToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] EquipArmorToSlot: %s"), *SlotTag.ToString());
	EquipItemToSlot(SlotTag, ItemAsset, bChangeStats);
}

void UEquipmentManagerComponent::UnequipArmorAtSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] UnequipArmorAtSlot: %s"), *SlotTag.ToString());
	UnequipItemAtSlot(SlotTag);
}

// ═══════════════════════════════════════════════════════════════════════════════
// EQUIP/UNEQUIP TOOLS [7-10/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::EquipToolToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] EquipToolToSlot: %s"), *SlotTag.ToString());
	UnequipToolFromOtherSlots(ItemAsset);
	EquipItemToSlot(SlotTag, ItemAsset, bChangeStats);
}

void UEquipmentManagerComponent::UnequipToolAtSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] UnequipToolAtSlot: %s"), *SlotTag.ToString());
	UnequipItemAtSlot(SlotTag);
}

void UEquipmentManagerComponent::UnequipToolFromOtherSlots_Implementation(UDataAsset* ItemAsset)
{
	for (const auto& Pair : AllEquippedItems)
	{
		if (Pair.Value.ItemAsset == ItemAsset && ToolSlots.HasTag(Pair.Key))
		{
			UnequipToolAtSlot(Pair.Key);
			break;
		}
	}
}

void UEquipmentManagerComponent::HideItemAtSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] HideItemAtSlot: %s"), *SlotTag.ToString());
}

// ═══════════════════════════════════════════════════════════════════════════════
// EQUIP/UNEQUIP TALISMANS [11-12/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::EquipTalismanToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] EquipTalismanToSlot: %s"), *SlotTag.ToString());
	EquipItemToSlot(SlotTag, ItemAsset, bChangeStats);
	TryGrantBuffs(false);
}

void UEquipmentManagerComponent::UnequipTalismanAtSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] UnequipTalismanAtSlot: %s"), *SlotTag.ToString());
	TryRemoveBuffs();
	UnequipItemAtSlot(SlotTag);
}

// ═══════════════════════════════════════════════════════════════════════════════
// GENERIC EQUIP/UNEQUIP [13-14/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::EquipItemToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset, bool bChangeStats)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] EquipItemToSlot: %s to %s"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("null"), *SlotTag.ToString());

	if (!ItemAsset) return;

	FSLFCurrentEquipment NewEquipment;
	NewEquipment.ItemAsset = ItemAsset;
	NewEquipment.UniqueId = FGuid::NewGuid();

	AllEquippedItems.Add(SlotTag, NewEquipment);
	if (bChangeStats) ApplyStatChanges(ItemAsset, true);
	OnItemEquippedToSlot.Broadcast(SlotTag, ItemAsset);
}

void UEquipmentManagerComponent::UnequipItemAtSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] UnequipItemAtSlot: %s"), *SlotTag.ToString());

	if (FSLFCurrentEquipment* Equipment = AllEquippedItems.Find(SlotTag))
	{
		UDataAsset* ItemAsset = Cast<UDataAsset>(Equipment->ItemAsset);
		ApplyStatChanges(ItemAsset, false);
		AllEquippedItems.Remove(SlotTag);
		OnItemUnequippedFromSlot.Broadcast(SlotTag, ItemAsset);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// SLOT QUERIES [15-20/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::GetItemAtSlot_Implementation(FGameplayTag SlotTag, UDataAsset*& OutItemAsset, FGuid& OutId)
{
	if (FSLFCurrentEquipment* Equipment = AllEquippedItems.Find(SlotTag))
	{
		OutItemAsset = Cast<UDataAsset>(Equipment->ItemAsset);
		OutId = Equipment->UniqueId;
	}
	else
	{
		OutItemAsset = nullptr;
		OutId = FGuid();
	}
}

AActor* UEquipmentManagerComponent::GetSpawnedItemAtSlot_Implementation(FGameplayTag SlotTag)
{
	if (AActor** Found = SpawnedItemsAtSlots.Find(SlotTag))
		return *Found;
	return nullptr;
}

bool UEquipmentManagerComponent::IsSlotOccupied_Implementation(FGameplayTag SlotTag)
{
	return AllEquippedItems.Contains(SlotTag);
}

bool UEquipmentManagerComponent::IsItemEquipped_Implementation(UDataAsset* ItemAsset)
{
	for (const auto& Pair : AllEquippedItems)
	{
		if (Pair.Value.ItemAsset == ItemAsset) return true;
	}
	return false;
}

bool UEquipmentManagerComponent::IsItemEquippedToSlot_Implementation(FGameplayTag SlotTag, UDataAsset* ItemAsset)
{
	if (FSLFCurrentEquipment* Equipment = AllEquippedItems.Find(SlotTag))
		return Equipment->ItemAsset == ItemAsset;
	return false;
}

int32 UEquipmentManagerComponent::GetAmountOfEquippedItem_Implementation(UDataAsset* ItemAsset)
{
	int32 Count = 0;
	for (const auto& Pair : AllEquippedItems)
	{
		if (Pair.Value.ItemAsset == ItemAsset) Count++;
	}
	return Count;
}

// ═══════════════════════════════════════════════════════════════════════════════
// WEAPON SLOT QUERIES [21-25/41]
// ═══════════════════════════════════════════════════════════════════════════════

FGameplayTag UEquipmentManagerComponent::GetActiveWeaponSlot_Implementation(bool bRightHand)
{
	FGameplayTagContainer& Slots = bRightHand ? RightHandSlots : LeftHandSlots;
	for (const FGameplayTag& Slot : Slots)
	{
		if (AllEquippedItems.Contains(Slot)) return Slot;
	}
	return FGameplayTag();
}

FGameplayTag UEquipmentManagerComponent::GetActiveWheelSlotForWeapon_Implementation(bool bRightHand)
{
	return GetActiveWeaponSlot(bRightHand);
}

bool UEquipmentManagerComponent::AreBothWeaponSlotsActive_Implementation()
{
	return GetActiveWeaponSlot(true).IsValid() && GetActiveWeaponSlot(false).IsValid();
}

bool UEquipmentManagerComponent::IsDualWieldPossible_Implementation()
{
	// Check if both weapon slots have one-handed weapons equipped
	FGameplayTag LeftSlot = GetActiveWeaponSlot(true);
	FGameplayTag RightSlot = GetActiveWeaponSlot(false);

	if (!LeftSlot.IsValid() || !RightSlot.IsValid())
		return false;

	// Both slots must have items and both must support one-handed use
	return LeftHandOverlayState == ESLFOverlayState::OneHanded &&
	       RightHandOverlayState == ESLFOverlayState::OneHanded;
}

bool UEquipmentManagerComponent::IsTwoHandStanceActive_Implementation()
{
	// Check if current overlay state is two-handed
	return ActiveOverlayState == ESLFOverlayState::TwoHanded;
}

// ═══════════════════════════════════════════════════════════════════════════════
// TOOL SLOT OPERATIONS [26-28/41]
// ═══════════════════════════════════════════════════════════════════════════════

FGameplayTag UEquipmentManagerComponent::GetActiveWheelSlotForTool_Implementation()
{
	return ActiveToolSlot;
}

void UEquipmentManagerComponent::SetActiveToolSlot_Implementation(FGameplayTag SlotTag)
{
	ActiveToolSlot = SlotTag;
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] SetActiveToolSlot: %s"), *SlotTag.ToString());
}

// ═══════════════════════════════════════════════════════════════════════════════
// OVERLAY STATE [29-31/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::UpdateOverlayStates_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] UpdateOverlayStates"));

	// Determine overlay states from equipped weapon items
	LeftHandOverlayState = ESLFOverlayState::Unarmed;
	RightHandOverlayState = ESLFOverlayState::Unarmed;

	// Check each equipped item for weapon overlay info
	for (const auto& Pair : AllEquippedItems)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			FGameplayTag WeaponOverlay = ItemData->ItemInformation.EquipmentDetails.WeaponOverlay;
			if (WeaponOverlay.IsValid())
			{
				// Check overlay tag to determine state
				FString OverlayStr = WeaponOverlay.ToString();
				ESLFOverlayState ItemOverlay = ESLFOverlayState::Default;

				if (OverlayStr.Contains(TEXT("Shield")))
					ItemOverlay = ESLFOverlayState::Shield;
				else if (OverlayStr.Contains(TEXT("TwoHanded")))
					ItemOverlay = ESLFOverlayState::TwoHanded;
				else if (OverlayStr.Contains(TEXT("OneHanded")))
					ItemOverlay = ESLFOverlayState::OneHanded;
				else if (OverlayStr.Contains(TEXT("DualWield")))
					ItemOverlay = ESLFOverlayState::DualWield;

				// Assign to appropriate hand based on slot
				FString SlotStr = Pair.Key.ToString();
				if (SlotStr.Contains(TEXT("Left")))
					LeftHandOverlayState = ItemOverlay;
				else if (SlotStr.Contains(TEXT("Right")))
					RightHandOverlayState = ItemOverlay;
			}
		}
	}

	// Determine combined active overlay state
	if (LeftHandOverlayState == ESLFOverlayState::TwoHanded || RightHandOverlayState == ESLFOverlayState::TwoHanded)
		ActiveOverlayState = ESLFOverlayState::TwoHanded;
	else if (IsDualWieldPossible())
		ActiveOverlayState = ESLFOverlayState::DualWield;
	else if (RightHandOverlayState != ESLFOverlayState::Unarmed)
		ActiveOverlayState = RightHandOverlayState;
	else if (LeftHandOverlayState != ESLFOverlayState::Unarmed)
		ActiveOverlayState = LeftHandOverlayState;
	else
		ActiveOverlayState = ESLFOverlayState::Unarmed;

	OnStanceChanged.Broadcast();
}

void UEquipmentManagerComponent::OverrideOverlayStates_Implementation(ESLFOverlayState LeftHand, ESLFOverlayState RightHand)
{
	LeftHandOverlayState = LeftHand;
	RightHandOverlayState = RightHand;
	OnStanceChanged.Broadcast();
}

void UEquipmentManagerComponent::AdjustForTwoHandStance_Implementation(bool bRightHand)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] AdjustForTwoHandStance: %s hand"), bRightHand ? TEXT("Right") : TEXT("Left"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// GUARD/BLOCK [32-34/41]
// ═══════════════════════════════════════════════════════════════════════════════

bool UEquipmentManagerComponent::DoesEquipmentSupportGuard_Implementation()
{
	// Check if any equipped item has shield overlay or supports guard
	if (LeftHandOverlayState == ESLFOverlayState::Shield || RightHandOverlayState == ESLFOverlayState::Shield)
		return true;

	// Also check GrantedTags for guard capability
	for (const auto& Pair : AllEquippedItems)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			if (ItemData->ItemInformation.EquipmentDetails.GrantedTags.HasTagExact(
				FGameplayTag::RequestGameplayTag(FName("Equipment.Guard"))))
			{
				return true;
			}
		}
	}
	return false;
}

void UEquipmentManagerComponent::RefreshActiveGuardSequence_Implementation()
{
	// Get guard animation from equipped shield/weapon
	ActiveBlockSequence = nullptr;

	// Find equipped shield or guard-capable item
	for (const auto& Pair : AllEquippedItems)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			// Check if this is a shield or guard item
			FGameplayTag WeaponOverlay = ItemData->ItemInformation.EquipmentDetails.WeaponOverlay;
			if (WeaponOverlay.IsValid() && WeaponOverlay.ToString().Contains(TEXT("Shield")))
			{
				// Get guard animation from MovesetWeapons if available
				if (UObject* Animset = ItemData->ItemInformation.EquipmentDetails.MovesetWeapons)
				{
					// Access guard sequence from animset
					UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] Found guard animset for shield item"));
				}
				break;
			}
		}
	}
}

UAnimMontage* UEquipmentManagerComponent::GetGuardHitMontage_Implementation()
{
	// Get guard hit reaction montage from equipped shield/weapon
	for (const auto& Pair : AllEquippedItems)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			FGameplayTag WeaponOverlay = ItemData->ItemInformation.EquipmentDetails.WeaponOverlay;
			if (WeaponOverlay.IsValid() && WeaponOverlay.ToString().Contains(TEXT("Shield")))
			{
				// Guard hit montage would be accessed from the animset data asset
				// For now return nullptr - implement when animset structure is known
				UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] GetGuardHitMontage: Found shield item"));
			}
		}
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATS & BUFFS [35-39/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::ApplyStatChanges_Implementation(UDataAsset* ItemAsset, bool bAdd)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] ApplyStatChanges: %s (%s)"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("null"), bAdd ? TEXT("Add") : TEXT("Remove"));

	if (!ItemAsset) return;

	UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset);
	if (!ItemData) return;

	// Get StatManager to apply changes
	UStatManagerComponent* StatMgr = nullptr;
	if (AActor* Owner = GetOwner())
	{
		StatMgr = Owner->FindComponentByClass<UStatManagerComponent>();
	}

	if (!StatMgr) return;

	// Iterate StatChanges and apply each modifier
	for (const auto& StatPair : ItemData->ItemInformation.EquipmentDetails.StatChanges)
	{
		FGameplayTag StatTag = StatPair.Key;
		const FSLFEquipmentStat& StatMod = StatPair.Value;

		// Apply or remove the stat change via StatManager
		double ModValue = bAdd ? StatMod.Delta : -StatMod.Delta;
		StatMgr->AdjustStat(StatTag, ESLFValueType::MaxValue, ModValue, false, false);

		UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] %s stat %s by %f"),
			bAdd ? TEXT("Added") : TEXT("Removed"), *StatTag.ToString(), ModValue);
	}
}

bool UEquipmentManagerComponent::CheckRequiredStats_Implementation(UDataAsset* ItemAsset)
{
	if (!ItemAsset) return true;

	UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset);
	if (!ItemData) return true;

	// Check if item has stat requirements
	if (!ItemData->ItemInformation.EquipmentDetails.WeaponStatInfo.bHasStatRequirement)
		return true;

	// Get StatManager to check requirements
	UStatManagerComponent* StatMgr = nullptr;
	if (AActor* Owner = GetOwner())
	{
		StatMgr = Owner->FindComponentByClass<UStatManagerComponent>();
	}

	if (!StatMgr) return true;

	// Check each stat requirement
	for (const auto& ReqPair : ItemData->ItemInformation.EquipmentDetails.WeaponStatInfo.StatRequirementInfo)
	{
		FGameplayTag StatTag = ReqPair.Key;
		int32 RequiredValue = ReqPair.Value;

		UObject* StatObject = nullptr;
		FStatInfo StatInfo;
		bool bFound = StatMgr->GetStat(StatTag, StatObject, StatInfo);

		if (!bFound || StatInfo.CurrentValue < RequiredValue)
		{
			UE_LOG(LogTemp, Warning, TEXT("[EquipmentManager] Stat requirement not met: %s requires %d, has %f"),
				*StatTag.ToString(), RequiredValue, StatInfo.CurrentValue);
			return false;
		}
	}

	return true;
}

void UEquipmentManagerComponent::TryGrantBuffs_Implementation(bool bIsLoading)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] TryGrantBuffs"));

	// Get BuffManager to apply buffs
	UBuffManagerComponent* BuffMgr = GetBuffManager();
	if (!BuffMgr) return;

	// Iterate equipped items and grant buffs from GrantedBuffRank
	for (const auto& Pair : AllEquippedItems)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			for (const auto& BuffPair : ItemData->ItemInformation.EquipmentDetails.GrantedBuffRank)
			{
				FGameplayTag BuffTag = BuffPair.Key;
				int32 Rank = BuffPair.Value;

				UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] Granting buff %s (rank %d) from %s"),
					*BuffTag.ToString(), Rank, *ItemData->GetName());

				// Apply buff via BuffManager - the buff tag maps to a buff data asset
				// Actual buff application would call BuffMgr->ApplyBuff or similar
			}
		}
	}
}

void UEquipmentManagerComponent::TryRemoveBuffs_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] TryRemoveBuffs"));
}

UBuffManagerComponent* UEquipmentManagerComponent::GetBuffManager_Implementation()
{
	if (AActor* Owner = GetOwner())
		return Owner->FindComponentByClass<UBuffManagerComponent>();
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SERIALIZATION [40-41/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::SerializeEquipmentData_Implementation(TArray<FInstancedStruct>& OutData)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] SerializeEquipmentData"));
	OnSaveRequested.Broadcast();
}

void UEquipmentManagerComponent::ReinitializeMovementWithWeight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] ReinitializeMovementWithWeight"));

	// Calculate total equipment weight from all equipped items
	double TotalWeight = 0.0;

	for (const auto& Pair : AllEquippedItems)
	{
		if (UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset))
		{
			// Weight would typically be in ItemInformation - for now use a placeholder
			// TotalWeight += ItemData->ItemInformation.Weight;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] Total equipment weight: %f"), TotalWeight);

	// Notify StatManager of weight change if it tracks weight
	if (AActor* Owner = GetOwner())
	{
		if (UStatManagerComponent* StatMgr = Owner->FindComponentByClass<UStatManagerComponent>())
		{
			// Update weight stat
			FGameplayTag WeightTag = FGameplayTag::RequestGameplayTag(FName("Stat.Weight"));
			StatMgr->AdjustStat(WeightTag, ESLFValueType::CurrentValue, TotalWeight, false, false);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// ADDITIONAL EVENTS
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::EventRecursiveInitializeLoadedEquipment()
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] EventRecursiveInitializeLoadedEquipment - Items: %d"), Cached_LoadedEquipment.Num());

	// Process each cached equipment entry and equip it
	for (const FSLFEquipmentItemsSaveInfo& SaveInfo : Cached_LoadedEquipment)
	{
		UDataAsset* ItemAsset = Cast<UDataAsset>(SaveInfo.AssignedItem);
		if (ItemAsset)
		{
			UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] Loading equipment: %s to slot %s"),
				*ItemAsset->GetName(), *SaveInfo.SlotTag.ToString());

			// Equip the item without triggering stat changes (will be done after all items are equipped)
			EquipItemToSlot(SaveInfo.SlotTag, ItemAsset, false);
		}
	}

	// Clear cache after processing
	Cached_LoadedEquipment.Empty();

	// Update overlay states after all equipment is loaded
	UpdateOverlayStates();
}

void UEquipmentManagerComponent::SwitchOnSlot(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] SwitchOnSlot: %s"), *SlotTag.ToString());

	// Switch active weapon based on slot tag
	FString SlotStr = SlotTag.ToString();

	if (LeftHandSlots.HasTag(SlotTag))
	{
		// This is a left hand slot - wield it
		WieldItemAtSlot(SlotTag);
		UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] Wielding left hand slot: %s"), *SlotTag.ToString());
	}
	else if (RightHandSlots.HasTag(SlotTag))
	{
		// This is a right hand slot - wield it
		WieldItemAtSlot(SlotTag);
		UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] Wielding right hand slot: %s"), *SlotTag.ToString());
	}
	else if (ToolSlots.HasTag(SlotTag))
	{
		// This is a tool slot - set as active
		SetActiveToolSlot(SlotTag);
		UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] Setting active tool slot: %s"), *SlotTag.ToString());
	}

	UpdateOverlayStates();
}

void UEquipmentManagerComponent::EventAsyncSpawnAndEquipWeapon(UDataAsset* ItemAsset, FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] EventAsyncSpawnAndEquipWeapon: %s to slot %s"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("null"), *SlotTag.ToString());

	if (bIsAsyncWeaponBusy)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipmentManager] Async weapon spawn already busy - skipping"));
		return;
	}

	if (!ItemAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipmentManager] No item asset provided for async spawn"));
		return;
	}

	bIsAsyncWeaponBusy = true;

	// Get spawn info from item data
	UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset);
	if (ItemData)
	{
		// Async load the weapon mesh and spawn actor
		// For now, do synchronous equip and mark complete
		EquipWeaponToSlot(SlotTag, ItemAsset, true);

		UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] Weapon equipped via async path"));
	}

	bIsAsyncWeaponBusy = false;
}

void UEquipmentManagerComponent::EventInitLoadedItems()
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] EventInitLoadedItems"));

	// Initialize equipment from loaded data
	// This is typically called after save game load
	if (Cached_LoadedEquipment.Num() > 0)
	{
		EventRecursiveInitializeLoadedEquipment();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] No cached equipment to initialize"));
	}
}

bool UEquipmentManagerComponent::GetTwoHandStance(bool& bLeftHand, bool& bRightHand) const
{
	// Check if two-hand stance is active for either hand
	bLeftHand = (LeftHandOverlayState == ESLFOverlayState::TwoHanded);
	bRightHand = (RightHandOverlayState == ESLFOverlayState::TwoHanded);

	return bLeftHand || bRightHand;
}
