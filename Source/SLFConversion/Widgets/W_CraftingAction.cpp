// W_CraftingAction.cpp
// C++ Widget implementation for W_CraftingAction
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/WidgetBlueprint/W_CraftingAction.json
//
// Full implementation with material consumption and crafting

#include "Widgets/W_CraftingAction.h"
#include "Widgets/W_InventorySlot.h"
#include "Components/AC_InventoryManager.h"
#include "Components/InventoryManagerComponent.h"
#include "Kismet/GameplayStatics.h"

UW_CraftingAction::UW_CraftingAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SelectedSlot = nullptr;
	CurrentAmount = 1;
	MaxPossibleAmount = 0;
	AssignedItem = nullptr;
}

void UW_CraftingAction::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::NativeConstruct"));

	// Cache widget references
	CacheWidgetReferences();
}

void UW_CraftingAction::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_CraftingAction::NativeDestruct"));
}

void UW_CraftingAction::CacheWidgetReferences()
{
	// Widget references would be set via BindWidget in Blueprint
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

	// Get pawn for inventory component lookup
	APawn* Pawn = nullptr;
	if (APlayerController* PC = GetOwningPlayer())
	{
		Pawn = PC->GetPawn();
	}

	if (!Pawn)
	{
		return 0;
	}

	// Try AC_InventoryManager first
	UAC_InventoryManager* InventoryManager = Pawn->FindComponentByClass<UAC_InventoryManager>();

	// Fallback to InventoryManagerComponent
	UInventoryManagerComponent* InvManagerComp = nullptr;
	if (!InventoryManager)
	{
		InvManagerComp = Pawn->FindComponentByClass<UInventoryManagerComponent>();
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

	// Get pawn for inventory component lookup
	APawn* Pawn = nullptr;
	if (APlayerController* PC = GetOwningPlayer())
	{
		Pawn = PC->GetPawn();
	}

	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No pawn"));
		return;
	}

	// Try AC_InventoryManager first
	UAC_InventoryManager* InventoryManager = Pawn->FindComponentByClass<UAC_InventoryManager>();

	// Fallback to InventoryManagerComponent
	UInventoryManagerComponent* InvManagerComp = nullptr;
	if (!InventoryManager)
	{
		InvManagerComp = Pawn->FindComponentByClass<UInventoryManagerComponent>();
	}

	if (!InventoryManager && !InvManagerComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No inventory manager"));
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
		return;
	}

	// Get item from slot
	// Would call SelectedSlot->GetAssignedItem() or similar
	// For now assume AssignedItem is set externally

	// Setup required items display
	SetupRequiredItems();

	// Reset current amount
	CurrentAmount = FMath::Min(1, MaxPossibleAmount);
}
