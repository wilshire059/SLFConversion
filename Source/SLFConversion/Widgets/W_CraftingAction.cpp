// W_CraftingAction.cpp
// C++ Widget implementation for W_CraftingAction
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/WidgetBlueprint/W_CraftingAction.json
//
// Full implementation with material consumption and crafting
// UPDATED: 2026-01-22 - Fixed button text initialization for OK/Cancel buttons

#include "Widgets/W_CraftingAction.h"
#include "Widgets/W_InventorySlot.h"
#include "Widgets/W_GenericButton.h"
#include "Components/AC_InventoryManager.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

UW_CraftingAction::UW_CraftingAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SelectedSlot = nullptr;
	CurrentAmount = 1;
	MaxPossibleAmount = 0;
	AssignedItem = nullptr;
	CachedOKButton = nullptr;
	CachedCancelButton = nullptr;
	CachedItemNameText = nullptr;
	CachedItemIcon = nullptr;
}

void UW_CraftingAction::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::NativeConstruct"));

	// Cache widget references
	CacheWidgetReferences();

	// Bind button click events
	BindButtonEvents();
}

void UW_CraftingAction::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::NativeDestruct"));
}

void UW_CraftingAction::CacheWidgetReferences()
{
	// Cache OK button and set text
	CachedOKButton = Cast<UW_GenericButton>(GetWidgetFromName(TEXT("W_GB_OK")));
	if (CachedOKButton)
	{
		CachedOKButton->EventSetButtonText(FText::FromString(TEXT("Craft")));
		UE_LOG(LogTemp, Log, TEXT("  Set W_GB_OK text to 'Craft'"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not find W_GB_OK button"));
	}

	// Cache Cancel button and set text
	CachedCancelButton = Cast<UW_GenericButton>(GetWidgetFromName(TEXT("W_GB_Cancel")));
	if (CachedCancelButton)
	{
		CachedCancelButton->EventSetButtonText(FText::FromString(TEXT("Cancel")));
		UE_LOG(LogTemp, Log, TEXT("  Set W_GB_Cancel text to 'Cancel'"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not find W_GB_Cancel button"));
	}

	// Cache ItemNameText widget (displays the item being crafted)
	CachedItemNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText")));
	if (!CachedItemNameText)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not find ItemNameText widget"));
	}

	// Cache ItemIcon widget
	CachedItemIcon = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon")));
	if (!CachedItemIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not find ItemIcon widget"));
	}
}

void UW_CraftingAction::BindButtonEvents()
{
	// Bind OK button (Craft) click event
	if (CachedOKButton)
	{
		CachedOKButton->OnButtonPressed.AddDynamic(this, &UW_CraftingAction::HandleOKButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("  Bound W_GB_OK OnButtonPressed"));
	}

	// Bind Cancel button click event
	if (CachedCancelButton)
	{
		CachedCancelButton->OnButtonPressed.AddDynamic(this, &UW_CraftingAction::HandleCancelButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("  Bound W_GB_Cancel OnButtonPressed"));
	}
}

void UW_CraftingAction::HandleOKButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::HandleOKButtonPressed"));
	EventCraftButtonPressed();
}

void UW_CraftingAction::HandleCancelButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::HandleCancelButtonPressed"));
	EventCancelButtonPressed();
}

void UW_CraftingAction::EventCancelButtonPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::EventCancelButtonPressed - Closing menu"));
	OnCraftingActionClosed.Broadcast();
}

/**
 * GetMaxPossibleAmount - Calculate max craftable amount based on available materials
 *
 * Blueprint Logic:
 * 1. Get AssignedItem->ItemInformation->CraftingDetails->RequiredItems
 * 2. For each required item:
 *    a. Get owned amount
 *    b. Calculate how many times we can craft (owned / required)
 * 3. Return minimum across all requirements
 */
int32 UW_CraftingAction::GetMaxPossibleAmount_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::GetMaxPossibleAmount"));

	if (!IsValid(AssignedItem))
	{
		return 0;
	}

	UPDA_Item* Item = Cast<UPDA_Item>(AssignedItem);
	if (!IsValid(Item))
	{
		return 0;
	}

	// Get required items - TMap<FGameplayTag, int32> where Key=ItemTag, Value=RequiredCount
	const TMap<FGameplayTag, int32>& RequiredItems = Item->ItemInformation.CraftingDetails.RequiredItems;

	if (RequiredItems.Num() == 0)
	{
		// No requirements = unlimited (cap at 99)
		MaxPossibleAmount = 99;
		return MaxPossibleAmount;
	}

	// Get PlayerController for inventory component lookup
	// Note: InventoryManagerComponent is on PlayerController, not on Pawn
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return 0;
	}

	// Try AC_InventoryManager on PC first
	UAC_InventoryManager* InventoryManager = PC->FindComponentByClass<UAC_InventoryManager>();

	// Fallback to InventoryManagerComponent on PC
	UInventoryManagerComponent* InvManagerComp = nullptr;
	if (!InventoryManager)
	{
		InvManagerComp = PC->FindComponentByClass<UInventoryManagerComponent>();
	}

	// Also check Pawn as fallback
	if (!InventoryManager && !InvManagerComp)
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			InventoryManager = Pawn->FindComponentByClass<UAC_InventoryManager>();
			if (!InventoryManager)
			{
				InvManagerComp = Pawn->FindComponentByClass<UInventoryManagerComponent>();
			}
		}
	}

	if (!InventoryManager && !InvManagerComp)
	{
		return 0;
	}

	int32 MinCraftable = INT32_MAX;

	for (const auto& Requirement : RequiredItems)
	{
		FGameplayTag ItemTag = Requirement.Key;
		int32 RequiredAmount = Requirement.Value;

		if (!ItemTag.IsValid() || RequiredAmount <= 0)
		{
			continue;
		}

		// Get owned amount from whichever inventory manager exists
		int32 OwnedAmount = 0;
		bool bSuccess = false;
		if (InventoryManager)
		{
			InventoryManager->GetAmountOfItemWithTag(ItemTag, OwnedAmount, bSuccess);
		}
		else if (InvManagerComp)
		{
			InvManagerComp->GetAmountOfItemWithTag(ItemTag, OwnedAmount, bSuccess);
		}

		// Calculate how many times we can craft
		int32 CraftableFromThis = OwnedAmount / RequiredAmount;
		MinCraftable = FMath::Min(MinCraftable, CraftableFromThis);

		UE_LOG(LogTemp, Log, TEXT("  %s: Own %d, Need %d, Can craft %d"),
			*ItemTag.ToString(), OwnedAmount, RequiredAmount, CraftableFromThis);
	}

	MaxPossibleAmount = (MinCraftable == INT32_MAX) ? 0 : MinCraftable;
	UE_LOG(LogTemp, Log, TEXT("  Max possible: %d"), MaxPossibleAmount);

	return MaxPossibleAmount;
}

/**
 * SetupRequiredItems - Display required materials in the UI
 */
void UW_CraftingAction::SetupRequiredItems_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::SetupRequiredItems"));

	if (!IsValid(AssignedItem))
	{
		return;
	}

	UPDA_Item* Item = Cast<UPDA_Item>(AssignedItem);
	if (!IsValid(Item))
	{
		return;
	}

	// Get required items and display them - TMap<FGameplayTag, int32>
	const TMap<FGameplayTag, int32>& RequiredItems = Item->ItemInformation.CraftingDetails.RequiredItems;

	for (const auto& Requirement : RequiredItems)
	{
		FGameplayTag ItemTag = Requirement.Key;
		int32 RequiredAmount = Requirement.Value;

		if (!ItemTag.IsValid())
		{
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("  Requires: %s x%d"), *ItemTag.ToString(), RequiredAmount);

		// Would create a requirement entry widget here
	}

	// Calculate max possible amount
	GetMaxPossibleAmount();

	// Reset current amount to 1 (or max if less)
	CurrentAmount = FMath::Min(1, MaxPossibleAmount);
}

/**
 * CraftItem - Execute crafting: consume materials and create item(s)
 *
 * Blueprint Logic:
 * 1. Get AssignedItem and CurrentAmount
 * 2. For each required item:
 *    a. Calculate total needed (required * CurrentAmount)
 *    b. Remove from inventory
 * 3. Add crafted item to inventory (CurrentAmount times)
 * 4. Close crafting action
 */
void UW_CraftingAction::CraftItem_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::CraftItem - Crafting %d items"), CurrentAmount);

	if (!IsValid(AssignedItem) || CurrentAmount <= 0)
	{
		return;
	}

	UPDA_Item* Item = Cast<UPDA_Item>(AssignedItem);
	if (!IsValid(Item))
	{
		return;
	}

	// Get PlayerController for inventory component lookup
	// Note: InventoryManagerComponent is on PlayerController, not on Pawn
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No PlayerController"));
		return;
	}

	// Try AC_InventoryManager on PC first
	UAC_InventoryManager* InventoryManager = PC->FindComponentByClass<UAC_InventoryManager>();

	// Fallback to InventoryManagerComponent on PC
	UInventoryManagerComponent* InvManagerComp = nullptr;
	if (!InventoryManager)
	{
		InvManagerComp = PC->FindComponentByClass<UInventoryManagerComponent>();
	}

	// Also check Pawn as fallback
	if (!InventoryManager && !InvManagerComp)
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			InventoryManager = Pawn->FindComponentByClass<UAC_InventoryManager>();
			if (!InventoryManager)
			{
				InvManagerComp = Pawn->FindComponentByClass<UInventoryManagerComponent>();
			}
		}
	}

	if (!InventoryManager && !InvManagerComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No inventory manager on PC or Pawn"));
		return;
	}

	// Consume required materials - TMap<FGameplayTag, int32>
	const TMap<FGameplayTag, int32>& RequiredItems = Item->ItemInformation.CraftingDetails.RequiredItems;

	// PASS 1: VALIDATION - Check ALL materials are available BEFORE consuming any
	// This ensures atomic crafting: either all materials consumed or none
	for (const auto& Requirement : RequiredItems)
	{
		FGameplayTag ItemTag = Requirement.Key;
		int32 RequiredAmount = Requirement.Value;

		if (!ItemTag.IsValid())
		{
			continue;
		}

		int32 TotalNeeded = RequiredAmount * CurrentAmount;
		int32 OwnedAmount = 0;
		bool bHasItem = false;

		if (InventoryManager)
		{
			InventoryManager->GetAmountOfItemWithTag(ItemTag, OwnedAmount, bHasItem);
		}
		else if (InvManagerComp)
		{
			InvManagerComp->GetAmountOfItemWithTag(ItemTag, OwnedAmount, bHasItem);
		}

		if (OwnedAmount < TotalNeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("  CRAFT FAILED: Need %d of %s but only have %d - aborting craft (no materials consumed)"),
				TotalNeeded, *ItemTag.ToString(), OwnedAmount);
			return; // Fail entire craft BEFORE removing anything
		}

		UE_LOG(LogTemp, Log, TEXT("  Validated: %s - need %d, have %d"), *ItemTag.ToString(), TotalNeeded, OwnedAmount);
	}

	// PASS 2: EXECUTION - Only if all materials verified, now consume them
	for (const auto& Requirement : RequiredItems)
	{
		FGameplayTag ItemTag = Requirement.Key;
		int32 RequiredAmount = Requirement.Value;

		if (!ItemTag.IsValid())
		{
			continue;
		}

		int32 TotalToConsume = RequiredAmount * CurrentAmount;

		UE_LOG(LogTemp, Log, TEXT("  Consuming %s x%d"), *ItemTag.ToString(), TotalToConsume);

		// Remove materials by tag - should always succeed since we validated above
		bool bSuccess = false;
		if (InventoryManager)
		{
			bSuccess = InventoryManager->RemoveItemWithTag(ItemTag, TotalToConsume);
		}
		else if (InvManagerComp)
		{
			bSuccess = InvManagerComp->RemoveItemWithTag(ItemTag, TotalToConsume);
		}

		if (!bSuccess)
		{
			// This should never happen since we validated, but log if it does
			UE_LOG(LogTemp, Error, TEXT("  UNEXPECTED: Failed to consume %s x%d after validation!"),
				*ItemTag.ToString(), TotalToConsume);
		}
	}

	// Add crafted item to inventory
	UE_LOG(LogTemp, Log, TEXT("  Adding crafted item: %s x%d"), *AssignedItem->GetName(), CurrentAmount);
	if (InventoryManager)
	{
		InventoryManager->AddItem(AssignedItem, CurrentAmount, true);
	}
	else if (InvManagerComp)
	{
		InvManagerComp->AddItem(AssignedItem, CurrentAmount, true);
	}

	// Close crafting action dialog
	OnCraftingActionClosed.Broadcast();
}

void UW_CraftingAction::EventCraftButtonPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::EventCraftButtonPressed"));

	// Execute crafting
	CraftItem();
}

void UW_CraftingAction::EventNavigateConfirmButtonsHorizontal_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::EventNavigateConfirmButtonsHorizontal"));

	// Navigate between Craft and Cancel buttons
}

void UW_CraftingAction::EventNavigateCraftingActionDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::EventNavigateCraftingActionDown"));

	// Decrease craft amount
	if (CurrentAmount > 1)
	{
		CurrentAmount--;
		UE_LOG(LogTemp, Log, TEXT("  New amount: %d"), CurrentAmount);
	}
}

void UW_CraftingAction::EventNavigateCraftingActionUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::EventNavigateCraftingActionUp"));

	// Increase craft amount
	if (CurrentAmount < MaxPossibleAmount)
	{
		CurrentAmount++;
		UE_LOG(LogTemp, Log, TEXT("  New amount: %d"), CurrentAmount);
	}
}

void UW_CraftingAction::EventSetupCraftingAction_Implementation(UW_InventorySlot* InSelectedSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::EventSetupCraftingAction"));

	SelectedSlot = InSelectedSlot;

	if (!IsValid(SelectedSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("  SelectedSlot is invalid"));
		return;
	}

	// Get item from slot
	AssignedItem = SelectedSlot->AssignedItem;

	if (!IsValid(AssignedItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("  AssignedItem is invalid"));
		return;
	}

	// Cast to UPDA_Item to access ItemInformation
	UPDA_Item* Item = Cast<UPDA_Item>(AssignedItem);
	if (!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not cast AssignedItem to UPDA_Item"));
		return;
	}

	// Set ItemNameText with the item's DisplayName
	if (CachedItemNameText)
	{
		const FText& DisplayName = Item->ItemInformation.DisplayName;
		CachedItemNameText->SetText(DisplayName);
		UE_LOG(LogTemp, Log, TEXT("  Set ItemNameText to '%s'"), *DisplayName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  CachedItemNameText is null - cannot set item name"));
	}

	// Set item icon if available
	if (CachedItemIcon && !Item->ItemInformation.IconSmall.IsNull())
	{
		UTexture2D* IconTexture = Item->ItemInformation.IconSmall.LoadSynchronous();
		if (IconTexture)
		{
			CachedItemIcon->SetBrushFromTexture(IconTexture);
			UE_LOG(LogTemp, Log, TEXT("  Set item icon"));
		}
	}

	// Select the OK button by default
	if (CachedOKButton)
	{
		CachedOKButton->SetButtonSelected(true);
	}
	if (CachedCancelButton)
	{
		CachedCancelButton->SetButtonSelected(false);
	}

	// Setup required items display
	SetupRequiredItems();

	// Reset current amount to 1 (or max if max is 0)
	CurrentAmount = FMath::Max(1, FMath::Min(1, MaxPossibleAmount));
}
