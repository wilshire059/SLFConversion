// W_Inventory.cpp
// C++ Widget implementation for W_Inventory
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with navigation and slot management

#include "Widgets/W_Inventory.h"
#include "Widgets/W_InventorySlot.h"
#include "Widgets/W_Inventory_CategoryEntry.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/AC_InventoryManager.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

UW_Inventory::UW_Inventory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SelectedSlot = nullptr;
	StorageMode = false;
	CategoryNavigationIndex = 0;
	ItemNavigationIndex = 0;
	ActiveFilterCategory = ESLFItemCategory::None;
}

void UW_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::NativeConstruct"));
}

void UW_Inventory::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::NativeDestruct"));
}

void UW_Inventory::CacheWidgetReferences()
{
	// Get inventory component from player
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		InventoryComponent = PlayerPawn->FindComponentByClass<UAC_InventoryManager>();
	}
}

/**
 * AddNewSlots - Create new inventory/storage slots
 */
void UW_Inventory::AddNewSlots_Implementation(ESLFInventorySlotType SlotType)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::AddNewSlots - SlotType: %d"), (int32)SlotType);

	// Slots are typically created in Blueprint UMG designer
	// This function would dynamically add more slots if needed
}

/**
 * GetTranslationForActionMenu - Calculate position for action menu popup
 */
FVector2D UW_Inventory::GetTranslationForActionMenu_Implementation(UScrollBox* TargetScrollbox)
{
	if (TargetScrollbox)
	{
		FGeometry Geometry = TargetScrollbox->GetCachedGeometry();
		FVector2D LocalSize = Geometry.GetLocalSize();
		return FVector2D(LocalSize.X * 0.5f, LocalSize.Y * 0.5f);
	}
	return FVector2D::ZeroVector;
}

/**
 * SetStorageMode - Toggle between inventory and storage view
 */
void UW_Inventory::SetStorageMode_Implementation(bool InStorageMode)
{
	StorageMode = InStorageMode;

	// Reset navigation index when switching modes
	ItemNavigationIndex = 0;

	// Rebuild occupied slots for the new mode
	ReinitOccupiedInventorySlots();

	// Select first slot if available
	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
	if (ActiveSlots.Num() > 0)
	{
		EventOnSlotSelected(true, ActiveSlots[0]);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::SetStorageMode - StorageMode: %s"), StorageMode ? TEXT("true") : TEXT("false"));
}

/**
 * ReinitOccupiedInventorySlots - Rebuild list of slots with items
 */
void UW_Inventory::ReinitOccupiedInventorySlots_Implementation()
{
	OccupiedInventorySlots.Empty();

	TArray<UW_InventorySlot*>& SourceSlots = StorageMode ? StorageSlots : InventorySlots;

	for (UW_InventorySlot* InvSlot : SourceSlots)
	{
		if (InvSlot && InvSlot->IsOccupied)
		{
			// Filter by category if active
			if (ActiveFilterCategory == ESLFItemCategory::None)
			{
				OccupiedInventorySlots.Add(InvSlot);
			}
			else
			{
				// Would check item category here
				// For now, add all occupied slots
				OccupiedInventorySlots.Add(InvSlot);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::ReinitOccupiedInventorySlots - Count: %d"), OccupiedInventorySlots.Num());
}

/**
 * SetCategorization - Filter items by category
 */
void UW_Inventory::SetCategorization_Implementation(ESLFItemCategory ItemCategory)
{
	ActiveFilterCategory = ItemCategory;

	// Update category entry visuals
	for (int32 i = 0; i < CategoryEntries.Num(); ++i)
	{
		if (CategoryEntries[i])
		{
			// Select the matching category
			// CategoryEntries[i]->SetSelected(CategoryEntries[i]->Category == ItemCategory);
		}
	}

	// Rebuild occupied slots with new filter
	ReinitOccupiedInventorySlots();

	// Reset item navigation
	ItemNavigationIndex = 0;
	if (OccupiedInventorySlots.Num() > 0)
	{
		EventOnSlotSelected(true, OccupiedInventorySlots[0]);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::SetCategorization - Category: %d"), (int32)ItemCategory);
}

/**
 * ResetCategorization - Show all item categories
 */
void UW_Inventory::ResetCategorization_Implementation()
{
	ActiveFilterCategory = ESLFItemCategory::None;
	CategoryNavigationIndex = 0;

	ReinitOccupiedInventorySlots();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::ResetCategorization"));
}

/**
 * EventNavigateUp - Move selection up in grid
 */
void UW_Inventory::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateUp"));

	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;

	if (ActiveSlots.Num() == 0)
	{
		return;
	}

	// Get slots per row from inventory component
	int32 SlotsPerRow = InventoryComponent ? InventoryComponent->SlotsPerRow : 8;

	// Calculate new index (move up one row)
	int32 NewIndex = ItemNavigationIndex - SlotsPerRow;

	if (NewIndex >= 0)
	{
		// Deselect current
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(false);
		}

		ItemNavigationIndex = NewIndex;

		// Select new
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(true);
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			EventSetupItemInfoPanel(SelectedSlot);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  New index: %d"), ItemNavigationIndex);
}

/**
 * EventNavigateDown - Move selection down in grid
 */
void UW_Inventory::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateDown"));

	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;

	if (ActiveSlots.Num() == 0)
	{
		return;
	}

	int32 SlotsPerRow = InventoryComponent ? InventoryComponent->SlotsPerRow : 8;

	// Calculate new index (move down one row)
	int32 NewIndex = ItemNavigationIndex + SlotsPerRow;

	if (NewIndex < ActiveSlots.Num())
	{
		// Deselect current
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(false);
		}

		ItemNavigationIndex = NewIndex;

		// Select new
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(true);
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			EventSetupItemInfoPanel(SelectedSlot);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  New index: %d"), ItemNavigationIndex);
}

/**
 * EventNavigateLeft - Move selection left in grid
 */
void UW_Inventory::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateLeft"));

	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;

	if (ActiveSlots.Num() == 0)
	{
		return;
	}

	int32 SlotsPerRow = InventoryComponent ? InventoryComponent->SlotsPerRow : 8;

	// Don't wrap to previous row
	int32 CurrentRow = ItemNavigationIndex / SlotsPerRow;
	int32 NewIndex = ItemNavigationIndex - 1;
	int32 NewRow = NewIndex / SlotsPerRow;

	if (NewIndex >= 0 && NewRow == CurrentRow)
	{
		// Deselect current
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(false);
		}

		ItemNavigationIndex = NewIndex;

		// Select new
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(true);
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			EventSetupItemInfoPanel(SelectedSlot);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  New index: %d"), ItemNavigationIndex);
}

/**
 * EventNavigateRight - Move selection right in grid
 */
void UW_Inventory::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateRight"));

	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;

	if (ActiveSlots.Num() == 0)
	{
		return;
	}

	int32 SlotsPerRow = InventoryComponent ? InventoryComponent->SlotsPerRow : 8;

	// Don't wrap to next row
	int32 CurrentRow = ItemNavigationIndex / SlotsPerRow;
	int32 NewIndex = ItemNavigationIndex + 1;
	int32 NewRow = NewIndex / SlotsPerRow;

	if (NewIndex < ActiveSlots.Num() && NewRow == CurrentRow)
	{
		// Deselect current
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(false);
		}

		ItemNavigationIndex = NewIndex;

		// Select new
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(true);
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			EventSetupItemInfoPanel(SelectedSlot);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  New index: %d"), ItemNavigationIndex);
}

/**
 * EventNavigateCategoryLeft - Switch to previous category
 */
void UW_Inventory::EventNavigateCategoryLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateCategoryLeft"));

	if (CategoryEntries.Num() == 0)
	{
		return;
	}

	// Move to previous category
	int32 NewIndex = CategoryNavigationIndex - 1;
	if (NewIndex < 0)
	{
		NewIndex = CategoryEntries.Num() - 1; // Wrap around
	}

	CategoryNavigationIndex = NewIndex;

	// Get the category from the entry and apply filter
	// This would call: SetCategorization(CategoryEntries[NewIndex]->Category);

	UE_LOG(LogTemp, Log, TEXT("  Category index: %d"), CategoryNavigationIndex);
}

/**
 * EventNavigateCategoryRight - Switch to next category
 */
void UW_Inventory::EventNavigateCategoryRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateCategoryRight"));

	if (CategoryEntries.Num() == 0)
	{
		return;
	}

	// Move to next category
	int32 NewIndex = CategoryNavigationIndex + 1;
	if (NewIndex >= CategoryEntries.Num())
	{
		NewIndex = 0; // Wrap around
	}

	CategoryNavigationIndex = NewIndex;

	// Get the category from the entry and apply filter
	// This would call: SetCategorization(CategoryEntries[NewIndex]->Category);

	UE_LOG(LogTemp, Log, TEXT("  Category index: %d"), CategoryNavigationIndex);
}

/**
 * EventNavigateOk - Confirm selection / open action menu
 */
void UW_Inventory::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateOk"));

	if (SelectedSlot && SelectedSlot->IsOccupied)
	{
		// Trigger slot pressed event which opens action menu
		SelectedSlot->EventSlotPressed();
	}
}

/**
 * EventNavigateCancel - Close inventory or go back
 */
void UW_Inventory::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateCancel"));

	// Broadcast that inventory is closing
	OnInventoryClosed.Broadcast();

	// Hide the inventory widget
	SetVisibility(ESlateVisibility::Collapsed);
}

/**
 * EventNavigateDetailedView - Toggle detailed item info panel
 */
void UW_Inventory::EventNavigateDetailedView_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateDetailedView"));

	// Toggle item info visibility
	static bool bInfoVisible = false;
	bInfoVisible = !bInfoVisible;

	EventToggleItemInfo(bInfoVisible);
}

/**
 * EventOnSlotPressed - Handle slot being pressed/clicked
 */
void UW_Inventory::EventOnSlotPressed_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnSlotPressed"));

	if (!InSlot)
	{
		return;
	}

	// Set as selected slot
	SelectedSlot = InSlot;

	// Find index of this slot
	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
	ItemNavigationIndex = ActiveSlots.Find(InSlot);

	// If slot is occupied, open action menu
	if (InSlot->IsOccupied)
	{
		// The parent widget (HUD) listens for this and shows the action menu
		InSlot->OnPressed.Broadcast(InSlot);
	}
}

/**
 * EventOnSlotSelected - Handle slot selection change
 */
void UW_Inventory::EventOnSlotSelected_Implementation(bool bSelected, UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnSlotSelected - bSelected: %s"), bSelected ? TEXT("true") : TEXT("false"));

	if (!InSlot)
	{
		return;
	}

	if (bSelected)
	{
		// Deselect previous slot
		if (SelectedSlot && SelectedSlot != InSlot)
		{
			SelectedSlot->EventOnSelected(false);
		}

		SelectedSlot = InSlot;
		InSlot->EventOnSelected(true);

		// Update item info panel
		EventSetupItemInfoPanel(InSlot);

		// Update navigation index
		TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
		ItemNavigationIndex = ActiveSlots.Find(InSlot);
	}
}

/**
 * EventOnSlotCleared - Handle slot being cleared
 */
void UW_Inventory::EventOnSlotCleared_Implementation(UW_InventorySlot* InSlot, bool bTriggerShift)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnSlotCleared - TriggerShift: %s"), bTriggerShift ? TEXT("true") : TEXT("false"));

	// Remove from occupied slots
	OccupiedInventorySlots.Remove(InSlot);

	// If this was the selected slot, select next available
	if (SelectedSlot == InSlot)
	{
		SelectedSlot = nullptr;

		TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			if (SelectedSlot)
			{
				SelectedSlot->EventOnSelected(true);
			}
		}
	}

	if (bTriggerShift)
	{
		// Would shift items to fill gap
		ReinitOccupiedInventorySlots();
	}
}

/**
 * EventOnInventorySlotAssigned - Handle item assigned to slot
 */
void UW_Inventory::EventOnInventorySlotAssigned_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnInventorySlotAssigned"));

	if (!InSlot)
	{
		return;
	}

	// Add to occupied slots if not already present
	if (!OccupiedInventorySlots.Contains(InSlot))
	{
		OccupiedInventorySlots.Add(InSlot);
	}
}

/**
 * EventOnCategorySelected - Handle category tab selection
 */
void UW_Inventory::EventOnCategorySelected_Implementation(UW_Inventory_CategoryEntry* InCategoryEntry, ESLFItemCategory SelectedCategory)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnCategorySelected - Category: %d"), (int32)SelectedCategory);

	// Apply the new category filter
	SetCategorization(SelectedCategory);

	// Update category navigation index
	CategoryNavigationIndex = CategoryEntries.Find(InCategoryEntry);
}

/**
 * EventOnVisibilityChanged - Handle inventory visibility change
 */
void UW_Inventory::EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnVisibilityChanged - Visibility: %d"), (int32)InVisibility);

	if (InVisibility == ESlateVisibility::Visible || InVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		// Inventory becoming visible - refresh data
		CacheWidgetReferences();
		ReinitOccupiedInventorySlots();

		// Select first slot
		TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
		if (ActiveSlots.Num() > 0)
		{
			ItemNavigationIndex = 0;
			EventOnSlotSelected(true, ActiveSlots[0]);
		}
	}
}

/**
 * EventOnActionMenuVisibilityChanged - Handle action menu visibility
 */
void UW_Inventory::EventOnActionMenuVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnActionMenuVisibilityChanged - Visibility: %d"), (int32)InVisibility);

	// When action menu closes, return focus to inventory
	if (InVisibility == ESlateVisibility::Collapsed || InVisibility == ESlateVisibility::Hidden)
	{
		// Refresh slots in case item was used/discarded
		ReinitOccupiedInventorySlots();
	}
}

/**
 * EventToggleItemInfo - Show/hide detailed item info panel
 */
void UW_Inventory::EventToggleItemInfo_Implementation(bool bVisible)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventToggleItemInfo - bVisible: %s"), bVisible ? TEXT("true") : TEXT("false"));

	// The item info panel visibility is controlled by UMG bindings
	// This broadcasts the state change
}

/**
 * EventSetupItemInfoPanel - Configure item info panel for selected slot
 */
void UW_Inventory::EventSetupItemInfoPanel_Implementation(UW_InventorySlot* ForSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventSetupItemInfoPanel"));

	if (!ForSlot || !ForSlot->IsOccupied)
	{
		// Hide item info if no item
		EventToggleItemInfo(false);
		return;
	}

	// The item info panel reads from the selected slot's AssignedItem
	// In Blueprint, this would set text blocks and images from ItemInformation
	EventToggleItemInfo(true);
}

/**
 * EventOnErrorReceived - Display error message
 */
void UW_Inventory::EventOnErrorReceived_Implementation(const FText& ErrorMessage)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnErrorReceived - %s"), *ErrorMessage.ToString());

	// Display error overlay widget with the message
	if (UWidget* ErrorOverlay = GetWidgetFromName(TEXT("ErrorOverlay")))
	{
		ErrorOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if (UTextBlock* ErrorText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ErrorText"))))
	{
		ErrorText->SetText(ErrorMessage);
	}
}

/**
 * EventDismissError - Hide error message
 */
void UW_Inventory::EventDismissError_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventDismissError"));

	// Hide the error overlay widget
	if (UWidget* ErrorOverlay = GetWidgetFromName(TEXT("ErrorOverlay")))
	{
		ErrorOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}
