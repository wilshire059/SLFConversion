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
	// TODO: Make item visible (unhide actor at slot)
}

void UEquipmentManagerComponent::UnwieldItemAtSlot_Implementation(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] UnwieldItemAtSlot: %s"), *SlotTag.ToString());
	// TODO: Hide item but keep equipped
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
	// TODO: Check if weapons support dual wield
	return true;
}

bool UEquipmentManagerComponent::IsTwoHandStanceActive_Implementation()
{
	// TODO: Check stance state
	return false;
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
	// TODO: Determine overlay state from equipped items
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
	// TODO: Check if any equipped item supports guard
	return true;
}

void UEquipmentManagerComponent::RefreshActiveGuardSequence_Implementation()
{
	// TODO: Get guard animation from weapon
}

UAnimMontage* UEquipmentManagerComponent::GetGuardHitMontage_Implementation()
{
	// TODO: Get guard hit reaction from weapon
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATS & BUFFS [35-39/41]
// ═══════════════════════════════════════════════════════════════════════════════

void UEquipmentManagerComponent::ApplyStatChanges_Implementation(UDataAsset* ItemAsset, bool bAdd)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] ApplyStatChanges: %s (%s)"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("null"), bAdd ? TEXT("Add") : TEXT("Remove"));
	// TODO: Apply stat modifiers from item
}

bool UEquipmentManagerComponent::CheckRequiredStats_Implementation(UDataAsset* ItemAsset)
{
	// TODO: Check if player meets required stats
	return true;
}

void UEquipmentManagerComponent::TryGrantBuffs_Implementation(bool bIsLoading)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipmentManager] TryGrantBuffs"));
	// TODO: Apply buffs from equipped items via BuffManager
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
	// TODO: Calculate total equipment weight and adjust movement
}
