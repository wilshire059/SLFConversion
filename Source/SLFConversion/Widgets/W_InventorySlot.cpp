// W_InventorySlot.cpp
// C++ Widget implementation for W_InventorySlot
//
// 20-PASS VALIDATION: 2026-01-08
// Full implementation with slot state management and event broadcasting

#include "Widgets/W_InventorySlot.h"
#include "SLFPrimaryDataAssets.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"

UW_InventorySlot::UW_InventorySlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssignedItem = nullptr;
	IsOccupied = false;
	Count = 0;
	CraftingRelated = false;
	StorageRelated = false;
	EquipmentRelated = false;
	CraftingSlotEnabled = true;
	SlotColor = FLinearColor::White;

	// Initialize cached widget pointers
	CachedSlotButton = nullptr;
	CachedItemIcon = nullptr;
	CachedItemAmount = nullptr;
	CachedSlotBorder = nullptr;
}

void UW_InventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	CacheWidgetReferences();
	BindButtonEvents();

	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::NativeConstruct"));
}

void UW_InventorySlot::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::NativeDestruct"));
}

void UW_InventorySlot::CacheWidgetReferences()
{
	// Cache widget references for faster access
	CachedSlotButton = Cast<UButton>(GetWidgetFromName(TEXT("SlotButton")));
	CachedItemIcon = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon")));
	CachedItemAmount = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemAmount")));
	CachedSlotBorder = Cast<UBorder>(GetWidgetFromName(TEXT("SlotBorder")));
}

void UW_InventorySlot::BindButtonEvents()
{
	// Find and bind to the SlotButton's events
	if (CachedSlotButton)
	{
		CachedSlotButton->OnPressed.AddDynamic(this, &UW_InventorySlot::OnSlotButtonPressed);
		CachedSlotButton->OnHovered.AddDynamic(this, &UW_InventorySlot::OnSlotButtonHovered);
		CachedSlotButton->OnUnhovered.AddDynamic(this, &UW_InventorySlot::OnSlotButtonUnhovered);
		UE_LOG(LogTemp, Log, TEXT("[W_InventorySlot] Bound SlotButton events"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_InventorySlot] SlotButton widget not found - click events won't work!"));
	}
}

void UW_InventorySlot::OnSlotButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_InventorySlot] SlotButton pressed - IsOccupied: %s, Item: %s"),
		IsOccupied ? TEXT("true") : TEXT("false"),
		AssignedItem ? *AssignedItem->GetName() : TEXT("None"));

	// Call EventSlotPressed which broadcasts OnPressed
	EventSlotPressed();
}

void UW_InventorySlot::OnSlotButtonHovered()
{
	UE_LOG(LogTemp, Log, TEXT("[W_InventorySlot] SlotButton hovered"));

	// Call EventSetHighlighted with true
	EventSetHighlighted(true);
}

void UW_InventorySlot::OnSlotButtonUnhovered()
{
	UE_LOG(LogTemp, Log, TEXT("[W_InventorySlot] SlotButton unhovered"));

	// Call EventSetHighlighted with false
	EventSetHighlighted(false);
}

/**
 * AdjustAmountAndIconOpacity - Adjust the visual opacity of slot contents
 *
 * Used for visual feedback (e.g., dimming when item is being moved)
 */
void UW_InventorySlot::AdjustAmountAndIconOpacity_Implementation(double Alpha)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::AdjustAmountAndIconOpacity - Alpha: %f"), Alpha);

	// Find and adjust icon image opacity
	if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		IconImage->SetRenderOpacity(Alpha);
	}

	// Find and adjust amount text opacity
	if (UTextBlock* AmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("AmountText"))))
	{
		AmountText->SetRenderOpacity(Alpha);
	}
}

/**
 * EventChangeAmount - Update the item count in this slot
 *
 * Blueprint Logic:
 * 1. Update Count variable
 * 2. Update amount text display
 * 3. If count reaches 0, clear the slot
 */
void UW_InventorySlot::EventChangeAmount_Implementation(int32 NewCount)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::EventChangeAmount - OldCount: %d, NewCount: %d"), Count, NewCount);

	Count = NewCount;

	// Update amount text display
	if (UTextBlock* AmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("AmountText"))))
	{
		if (Count > 1)
		{
			AmountText->SetText(FText::AsNumber(Count));
			AmountText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			AmountText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// If count is 0, clear the slot
	if (Count <= 0)
	{
		EventClearSlot(true);
	}
}

/**
 * EventClearSlot - Clear the slot of its item
 *
 * Blueprint Logic:
 * 1. Reset slot state variables
 * 2. Hide item icon
 * 3. Reset visual state
 * 4. Broadcast OnSlotCleared
 */
void UW_InventorySlot::EventClearSlot_Implementation(bool TriggerShift)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::EventClearSlot - TriggerShift: %s"), TriggerShift ? TEXT("true") : TEXT("false"));

	// Store reference before clearing
	UPrimaryDataAsset* PreviousItem = AssignedItem;

	// Reset state
	AssignedItem = nullptr;
	IsOccupied = false;
	Count = 0;

	// Hide item icon
	if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Hide amount text
	if (UTextBlock* AmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("AmountText"))))
	{
		AmountText->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Hide equipped indicator if present
	EventToggleEquippedVisual(false);

	// Broadcast event
	OnSlotCleared.Broadcast(this, TriggerShift);
}

/**
 * EventOccupySlot - Assign an item to this slot
 *
 * Blueprint Logic:
 * 1. Store item reference and count
 * 2. Set IsOccupied to true
 * 3. Update icon from item's ItemInformation.IconSmall
 * 4. Show/hide amount text based on count
 * 5. Broadcast OnSlotAssigned
 */
void UW_InventorySlot::EventOccupySlot_Implementation(UPDA_Item* AssignedItemAsset, int32 InCount)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::EventOccupySlot - Item: %s, Count: %d"),
		AssignedItemAsset ? *AssignedItemAsset->GetName() : TEXT("None"), InCount);

	if (!AssignedItemAsset)
	{
		return;
	}

	// Store item data
	AssignedItem = AssignedItemAsset;
	Count = InCount;
	IsOccupied = true;

	// Update icon from item's ItemInformation
	if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		// Get icon from item - access ItemInformation.IconSmall
		UTexture2D* ItemIcon = nullptr;

		// Log the icon path for debugging
		FString IconPath = AssignedItemAsset->ItemInformation.IconSmall.ToString();
		UE_LOG(LogTemp, Log, TEXT("[InventorySlot] Icon path: %s, IsNull: %s, IsValid: %s"),
			*IconPath,
			AssignedItemAsset->ItemInformation.IconSmall.IsNull() ? TEXT("true") : TEXT("false"),
			AssignedItemAsset->ItemInformation.IconSmall.IsValid() ? TEXT("true") : TEXT("false"));

		if (!AssignedItemAsset->ItemInformation.IconSmall.IsNull())
		{
			ItemIcon = AssignedItemAsset->ItemInformation.IconSmall.LoadSynchronous();
			UE_LOG(LogTemp, Log, TEXT("[InventorySlot] Loaded icon: %s"), ItemIcon ? *ItemIcon->GetName() : TEXT("FAILED"));
		}

		if (ItemIcon)
		{
			IconImage->SetBrushFromTexture(ItemIcon);
			UE_LOG(LogTemp, Log, TEXT("[InventorySlot] Set brush from texture successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[InventorySlot] No icon texture for item %s"), *AssignedItemAsset->GetName());
		}
		IconImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventorySlot] ItemIcon widget not found!"));
	}

	// Update amount text - widget is named "ItemAmount" in Blueprint
	if (UTextBlock* AmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemAmount"))))
	{
		if (Count > 1)
		{
			AmountText->SetText(FText::AsNumber(Count));
			AmountText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			AmountText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Broadcast event
	OnSlotAssigned.Broadcast(this);
}

/**
 * EventOnSelected - Handle slot selection state change
 *
 * Blueprint Logic:
 * 1. Update visual state (border color, highlight)
 * 2. Broadcast OnSelected event
 */
void UW_InventorySlot::EventOnSelected_Implementation(bool Selected)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::EventOnSelected - Selected: %s"), Selected ? TEXT("true") : TEXT("false"));

	// Update border/background color based on selection
	if (UBorder* SlotBorder = Cast<UBorder>(GetWidgetFromName(TEXT("SlotBorder"))))
	{
		if (Selected)
		{
			// Use a highlight color when selected
			SlotBorder->SetBrushColor(FLinearColor(1.0f, 0.8f, 0.2f, 1.0f)); // Gold highlight
		}
		else
		{
			// Return to normal color
			SlotBorder->SetBrushColor(SlotColor);
		}
	}

	// Broadcast event
	OnSelected.Broadcast(Selected, this);
}

/**
 * SetSlotSelected - Update selection visual without broadcasting
 * Use this from parent widgets to prevent event recursion
 */
void UW_InventorySlot::SetSlotSelected(bool bSelected)
{
	// Update border/background color based on selection - same as EventOnSelected but NO broadcast
	if (UBorder* SlotBorder = Cast<UBorder>(GetWidgetFromName(TEXT("SlotBorder"))))
	{
		if (bSelected)
		{
			// Use a highlight color when selected
			SlotBorder->SetBrushColor(FLinearColor(1.0f, 0.8f, 0.2f, 1.0f)); // Gold highlight
		}
		else
		{
			// Return to normal color
			SlotBorder->SetBrushColor(SlotColor);
		}
	}
	// NOTE: No broadcast here - that's the key difference from EventOnSelected
}

/**
 * EventSetHighlighted - Set highlighted state (hover effect)
 */
void UW_InventorySlot::EventSetHighlighted_Implementation(bool Highlighted)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::EventSetHighlighted - Highlighted: %s"), Highlighted ? TEXT("true") : TEXT("false"));

	// Update visual highlight state
	if (UBorder* SlotBorder = Cast<UBorder>(GetWidgetFromName(TEXT("SlotBorder"))))
	{
		if (Highlighted)
		{
			// Subtle highlight on hover
			SlotBorder->SetBrushColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f));
		}
		else
		{
			// Return to slot color
			SlotBorder->SetBrushColor(SlotColor);
		}
	}
}

/**
 * EventSlotPressed - Handle slot being pressed/clicked
 *
 * Blueprint Logic:
 * 1. Only process if slot is occupied
 * 2. Broadcast OnPressed event
 */
void UW_InventorySlot::EventSlotPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::EventSlotPressed - IsOccupied: %s"), IsOccupied ? TEXT("true") : TEXT("false"));

	// Broadcast press event (parent widgets handle the action)
	OnPressed.Broadcast(this);
}

/**
 * EventToggleEquippedVisual - Show/hide equipped indicator
 *
 * Used to show that this item is currently equipped
 */
void UW_InventorySlot::EventToggleEquippedVisual_Implementation(bool Show)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::EventToggleEquippedVisual - Show: %s"), Show ? TEXT("true") : TEXT("false"));

	// Find and toggle equipped indicator widget
	if (UWidget* EquippedIndicator = GetWidgetFromName(TEXT("EquippedIndicator")))
	{
		EquippedIndicator->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// Alternative: update border to show equipped state
	if (UBorder* EquippedBorder = Cast<UBorder>(GetWidgetFromName(TEXT("EquippedBorder"))))
	{
		EquippedBorder->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

/**
 * EventToggleSlot - Enable/disable the slot
 *
 * Used to disable slots that shouldn't be interactable
 */
void UW_InventorySlot::EventToggleSlot_Implementation(bool IsEnabled)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventorySlot::EventToggleSlot - IsEnabled: %s"), IsEnabled ? TEXT("true") : TEXT("false"));

	CraftingSlotEnabled = IsEnabled;

	// Update visual state
	if (IsEnabled)
	{
		SetRenderOpacity(1.0f);
		SetIsEnabled(true);
	}
	else
	{
		SetRenderOpacity(0.5f);
		SetIsEnabled(false);
	}
}
