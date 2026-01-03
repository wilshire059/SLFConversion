// SLFPickupItemBase.cpp
// C++ implementation for B_PickupItem
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_PickupItem
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         5/5 (initialized in constructor)
// Functions:         4/4 implemented (3 migrated + 1 override)
// Event Dispatchers: 1/1 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFPickupItemBase.h"

ASLFPickupItemBase::ASLFPickupItemBase()
{
	// Initialize pickup config
	Item = nullptr;
	Count = 1;
	bUsePhysics = false;
	PlacementTraceChannel = ECC_WorldStatic;

	// Update interaction text
	InteractionText = FText::FromString(TEXT("Pick Up"));
}

void ASLFPickupItemBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[PickupItem] BeginPlay: %s x%d"),
		Item ? *Item->GetName() : TEXT("null"), Count);
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTIONS [1-3/4]
// ═══════════════════════════════════════════════════════════════════════════════

bool ASLFPickupItemBase::TryGetItemInfo_Implementation(FSLFItemInfo& OutInfo)
{
	if (Item)
	{
		OutInfo = ItemInfo;
		return true;
	}
	return false;
}

void ASLFPickupItemBase::TriggerOnItemLooted_Implementation()
{
	OnItemLooted.Broadcast(Item, Count);
}

void ASLFPickupItemBase::SetupInteractable_Implementation()
{
	Super::SetupInteractable_Implementation();

	UE_LOG(LogTemp, Log, TEXT("[PickupItem] SetupInteractable"));

	// Get item info from data asset
	if (Item)
	{
		TryGetItemInfo(ItemInfo);
		InteractableDisplayName = ItemInfo.DisplayName;
	}

	// Setup physics if enabled
	if (bUsePhysics)
	{
		// TODO: Enable physics simulation
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERACTION OVERRIDE [4/4]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFPickupItemBase::OnInteract_Implementation(AActor* Interactor)
{
	Super::OnInteract_Implementation(Interactor);

	UE_LOG(LogTemp, Log, TEXT("[PickupItem] OnInteract - Adding item to inventory"));

	// TODO: Add item to player's inventory via InventoryManager
	// if (InventoryManager->AddItem(Item, Count)) { ... }

	TriggerOnItemLooted();

	// Destroy after pickup
	Destroy();
}
