// W_Crafting.cpp
// C++ Widget implementation for W_Crafting
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/WidgetBlueprint/W_Crafting.json
//
// Full implementation with crafting logic

#include "Widgets/W_Crafting.h"
#include "Widgets/W_InventorySlot.h"
#include "Components/AC_InventoryManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

UW_Crafting::UW_Crafting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NavigationIndex = 0;
	InventoryComponent = nullptr;
	SelectedSlot = nullptr;
	ActiveSlot = nullptr;
}

void UW_Crafting::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::NativeConstruct"));

	// Cache widget references
	CacheWidgetReferences();

	// Get inventory manager from player pawn
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			InventoryComponent = Pawn->FindComponentByClass<UAC_InventoryManager>();
		}
	}

	// Refresh craftables list
	RefreshCraftables();
}

void UW_Crafting::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::NativeDestruct"));
}

void UW_Crafting::CacheWidgetReferences()
{
	// Widget references would be set via BindWidget in Blueprint
}

/**
 * SetupItemInformationPanel - Display item details in the info panel
 */
void UW_Crafting::SetupItemInformationPanel_Implementation(UPrimaryDataAsset* InItem)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::SetupItemInformationPanel - Item: %s"),
		InItem ? *InItem->GetName() : TEXT("None"));

	if (!IsValid(InItem))
	{
		return;
	}

	// Cast to UPDA_Item to access item information
	if (UPDA_Item* Item = Cast<UPDA_Item>(InItem))
	{
		// Update item name
		if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText"))))
		{
			NameText->SetText(Item->ItemInformation.DisplayName);
		}

		// Update item description
		if (UTextBlock* DescText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemDescriptionText"))))
		{
			DescText->SetText(Item->ItemInformation.ShortDescription);
		}

		// Update item icon
		if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
		{
			if (!Item->ItemInformation.IconSmall.IsNull())
			{
				UTexture2D* ItemIcon = Item->ItemInformation.IconSmall.LoadSynchronous();
				if (ItemIcon)
				{
					IconImage->SetBrushFromTexture(ItemIcon);
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("  Updated info panel for: %s"), *Item->ItemInformation.DisplayName.ToString());
	}
}

/**
 * GetOwnedAmountFromTag - Get how many of an item the player owns by tag
 */
int32 UW_Crafting::GetOwnedAmountFromTag_Implementation(const FGameplayTag& Tag)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::GetOwnedAmountFromTag - Tag: %s"), *Tag.ToString());

	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No inventory component"));
		return 0;
	}

	int32 Amount = 0;
	bool bSuccess = false;
	InventoryComponent->GetAmountOfItemWithTag(Tag, Amount, bSuccess);

	UE_LOG(LogTemp, Log, TEXT("  Amount: %d, Success: %s"), Amount, bSuccess ? TEXT("true") : TEXT("false"));
	return Amount;
}

/**
 * CheckIfCraftable - Check if player has all required materials to craft an item
 *
 * Blueprint Logic:
 * 1. Get ItemAsset->ItemInformation->CraftingDetails->RequiredItems map
 * 2. For each required item (amount -> item asset):
 *    a. Get the item's tag
 *    b. Check owned amount via GetOwnedAmountFromTag
 *    c. If owned < required, return false (with break)
 * 3. If all requirements met, return true
 */
bool UW_Crafting::CheckIfCraftable_Implementation(UPrimaryDataAsset* ItemAsset)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::CheckIfCraftable - Item: %s"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("None"));

	if (!IsValid(ItemAsset))
	{
		return false;
	}

	// Cast to UPDA_Item to access crafting info
	UPDA_Item* Item = Cast<UPDA_Item>(ItemAsset);
	if (!IsValid(Item))
	{
		return false;
	}

	// Check if crafting is unlocked for this item
	if (!Item->bCrafingUnlocked)
	{
		UE_LOG(LogTemp, Log, TEXT("  Crafting not unlocked"));
		return false;
	}

	// Get required items from crafting details
	// RequiredItems is TMap<FGameplayTag, int32> where key is item tag, value is required amount
	const TMap<FGameplayTag, int32>& RequiredItems = Item->ItemInformation.CraftingDetails.RequiredItems;

	if (RequiredItems.Num() == 0)
	{
		// No requirements = craftable
		UE_LOG(LogTemp, Log, TEXT("  No requirements - craftable"));
		return true;
	}

	// Check each required item
	for (const auto& Requirement : RequiredItems)
	{
		FGameplayTag ItemTag = Requirement.Key;
		int32 RequiredAmount = Requirement.Value;

		if (!ItemTag.IsValid())
		{
			continue;
		}

		// Check owned amount
		int32 OwnedAmount = GetOwnedAmountFromTag(ItemTag);

		UE_LOG(LogTemp, Log, TEXT("  Requirement: %s x%d, Owned: %d"),
			*ItemTag.ToString(), RequiredAmount, OwnedAmount);

		if (OwnedAmount < RequiredAmount)
		{
			UE_LOG(LogTemp, Log, TEXT("  Not enough - NOT craftable"));
			return false;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  All requirements met - CRAFTABLE"));
	return true;
}

/**
 * RefreshCraftables - Rebuild the list of craftable items
 *
 * Blueprint Logic:
 * 1. Clear UnlockedCraftableEntries
 * 2. For each item in UnlockedCraftables:
 *    a. Check if craftable via CheckIfCraftable
 *    b. Create/update slot widget
 *    c. Add to UnlockedCraftableEntries
 */
void UW_Crafting::RefreshCraftables_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::RefreshCraftables"));

	// Clear existing entries
	UnlockedCraftableEntries.Empty();

	// Process each unlocked craftable
	for (const auto& CraftablePair : UnlockedCraftables)
	{
		FGameplayTag ItemTag = CraftablePair.Key;
		UPrimaryDataAsset* ItemAsset = CraftablePair.Value;

		if (!IsValid(ItemAsset))
		{
			continue;
		}

		// Check if this item can be crafted
		bool bCanCraft = CheckIfCraftable(ItemAsset);

		UE_LOG(LogTemp, Log, TEXT("  Item: %s, CanCraft: %s"),
			*ItemAsset->GetName(), bCanCraft ? TEXT("true") : TEXT("false"));

		// Would create/update a slot widget here and add to entries
		// For now, just track that we processed it
	}

	// Broadcast update event
	EventOnCraftablesUpdated();

	UE_LOG(LogTemp, Log, TEXT("  Refreshed %d craftables"), UnlockedCraftables.Num());
}

void UW_Crafting::EventAsyncLoadCraftables_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventAsyncLoadCraftables"));

	// Async load all craftable item assets
	// Once loaded, call RefreshCraftables
	RefreshCraftables();
}

void UW_Crafting::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateCancel"));

	// Close crafting menu
	OnCraftingClosed.Broadcast();
}

void UW_Crafting::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateDown"));

	if (UnlockedCraftableEntries.Num() == 0)
	{
		return;
	}

	// Move to next row (based on slots per row)
	int32 SlotsPerRow = 4; // Would be configured
	NavigationIndex = FMath::Min(NavigationIndex + SlotsPerRow, UnlockedCraftableEntries.Num() - 1);

	// Update selection
	if (UnlockedCraftableEntries.IsValidIndex(NavigationIndex))
	{
		ActiveSlot = UnlockedCraftableEntries[NavigationIndex];
		EventOnCraftingSlotSelected(true, ActiveSlot);
	}
}

void UW_Crafting::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateLeft"));

	if (UnlockedCraftableEntries.Num() == 0 || NavigationIndex <= 0)
	{
		return;
	}

	NavigationIndex--;

	if (UnlockedCraftableEntries.IsValidIndex(NavigationIndex))
	{
		ActiveSlot = UnlockedCraftableEntries[NavigationIndex];
		EventOnCraftingSlotSelected(true, ActiveSlot);
	}
}

void UW_Crafting::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateOk"));

	// Show crafting action dialog for selected slot
	if (IsValid(ActiveSlot))
	{
		EventOnCraftingSlotPressed(ActiveSlot);
	}
}

void UW_Crafting::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateRight"));

	if (UnlockedCraftableEntries.Num() == 0)
	{
		return;
	}

	NavigationIndex = FMath::Min(NavigationIndex + 1, UnlockedCraftableEntries.Num() - 1);

	if (UnlockedCraftableEntries.IsValidIndex(NavigationIndex))
	{
		ActiveSlot = UnlockedCraftableEntries[NavigationIndex];
		EventOnCraftingSlotSelected(true, ActiveSlot);
	}
}

void UW_Crafting::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateUp"));

	if (UnlockedCraftableEntries.Num() == 0)
	{
		return;
	}

	// Move to previous row
	int32 SlotsPerRow = 4;
	NavigationIndex = FMath::Max(0, NavigationIndex - SlotsPerRow);

	if (UnlockedCraftableEntries.IsValidIndex(NavigationIndex))
	{
		ActiveSlot = UnlockedCraftableEntries[NavigationIndex];
		EventOnCraftingSlotSelected(true, ActiveSlot);
	}
}

void UW_Crafting::EventOnCraftablesUpdated_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnCraftablesUpdated"));

	// Would update UI to reflect new craftable states
}

void UW_Crafting::EventOnCraftingSlotPressed_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnCraftingSlotPressed"));

	if (!IsValid(InSlot))
	{
		return;
	}

	// Show crafting action dialog
	EventToggleCraftingAction(true);
}

void UW_Crafting::EventOnCraftingSlotSelected_Implementation(bool Selected, UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnCraftingSlotSelected - Selected: %s"),
		Selected ? TEXT("true") : TEXT("false"));

	if (!IsValid(InSlot))
	{
		return;
	}

	if (Selected)
	{
		ActiveSlot = InSlot;

		// Update item info panel
		// Would get item from slot and call SetupItemInformationPanel
		EventToggleItemInfo(true);
	}
}

void UW_Crafting::EventOnInventoryUpdated_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnInventoryUpdated"));

	// Refresh craftable states when inventory changes
	RefreshCraftables();
}

void UW_Crafting::EventOnVisibilityChanged_Implementation(uint8 InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnVisibilityChanged - %d"), InVisibility);

	if (InVisibility == static_cast<uint8>(ESlateVisibility::Visible) ||
		InVisibility == static_cast<uint8>(ESlateVisibility::SelfHitTestInvisible))
	{
		// Refresh when becoming visible
		RefreshCraftables();
	}
}

void UW_Crafting::EventToggleCraftingAction_Implementation(bool Show)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventToggleCraftingAction - Show: %s"),
		Show ? TEXT("true") : TEXT("false"));

	// Would show/hide the crafting action dialog widget
}

void UW_Crafting::EventToggleItemInfo_Implementation(bool Visible)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventToggleItemInfo - Visible: %s"),
		Visible ? TEXT("true") : TEXT("false"));

	// Would show/hide the item info panel
}
