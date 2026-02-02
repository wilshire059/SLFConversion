// W_VendorSlot.cpp
// C++ Widget implementation for W_VendorSlot
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with vendor slot state management and event broadcasting

#include "Widgets/W_VendorSlot.h"
#include "SLFPrimaryDataAssets.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"

UW_VendorSlot::UW_VendorSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssignedItem = nullptr;
	AssignedItemPrice = 0;
	IsOccupied = false;
	Count = 0;
	CraftingSlotEnabled = true;
	SlotColor = FLinearColor::White;
	Currency = 0;
	Infinite = false;
	Type = ESLFVendorType::Buy;
	CachedSlotButton = nullptr;
}

void UW_VendorSlot::NativeConstruct()
{
	Super::NativeConstruct();

	CacheWidgetReferences();

	// CRITICAL: Bind to SlotButton if it exists in the Blueprint
	BindSlotButtonEvents();

	// CRITICAL: Enable mouse input for this widget
	// This allows NativeOnMouseButtonDown/Enter/Leave to be called as fallback
	SetVisibility(ESlateVisibility::Visible);

	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::NativeConstruct - SlotButton: %s"),
		CachedSlotButton ? TEXT("Bound") : TEXT("NULL (using native input)"));
}

void UW_VendorSlot::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::NativeDestruct"));
}

FReply UW_VendorSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_VendorSlot] Left mouse button pressed - calling EventSlotPressed"));
		EventSlotPressed();
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UW_VendorSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	// Show hover highlight
	EventSetHighlighted(true);
}

void UW_VendorSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	// Remove hover highlight
	EventSetHighlighted(false);
}

void UW_VendorSlot::CacheWidgetReferences()
{
	// Widget references are bound via BindWidget in Blueprint UMG
}

/**
 * AdjustAmountAndIconOpacity - Adjust the visual opacity of vendor slot contents
 */
void UW_VendorSlot::AdjustAmountAndIconOpacity_Implementation(double Alpha)
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::AdjustAmountAndIconOpacity - Alpha: %f"), Alpha);

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

	// Find and adjust price text opacity
	if (UTextBlock* PriceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PriceText"))))
	{
		PriceText->SetRenderOpacity(Alpha);
	}
}

/**
 * EventChangeAmount - Update the item count in vendor slot
 */
void UW_VendorSlot::EventChangeAmount_Implementation(int32 NewCount)
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::EventChangeAmount - OldCount: %d, NewCount: %d"), Count, NewCount);

	Count = NewCount;

	// Update amount text display
	if (UTextBlock* AmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("AmountText"))))
	{
		if (Infinite)
		{
			// Show infinity symbol for infinite stock
			AmountText->SetText(FText::FromString(TEXT("\u221E"))); // Unicode infinity
			AmountText->SetVisibility(ESlateVisibility::Visible);
		}
		else if (Count > 1)
		{
			AmountText->SetText(FText::AsNumber(Count));
			AmountText->SetVisibility(ESlateVisibility::Visible);
		}
		else if (Count == 1)
		{
			AmountText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			// Out of stock
			EventClearSlot(true);
		}
	}
}

/**
 * EventClearSlot - Clear the vendor slot
 */
void UW_VendorSlot::EventClearSlot_Implementation(bool TriggerShift)
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::EventClearSlot - TriggerShift: %s"), TriggerShift ? TEXT("true") : TEXT("false"));

	// Reset state
	AssignedItem = nullptr;
	AssignedItemPrice = 0;
	IsOccupied = false;
	Count = 0;
	Infinite = false;

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

	// Hide price text
	if (UTextBlock* PriceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PriceText"))))
	{
		PriceText->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Broadcast event
	OnSlotCleared.Broadcast(this, TriggerShift);
}

/**
 * EventOccupySlot - Assign an item to vendor slot
 *
 * Blueprint Logic:
 * 1. Store item reference, count, price, and vendor type
 * 2. Set IsOccupied to true
 * 3. Update icon from item's ItemInformation.IconSmall
 * 4. Show price and amount
 * 5. Broadcast OnSlotAssigned
 */
void UW_VendorSlot::EventOccupySlot_Implementation(UPDA_Item* AssignedItemAsset, int32 InCount, int32 Price, bool InfiniteStock, uint8 InType)
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::EventOccupySlot - Item: %s, Count: %d, Price: %d, Infinite: %s"),
		AssignedItemAsset ? *AssignedItemAsset->GetName() : TEXT("None"),
		InCount, Price, InfiniteStock ? TEXT("true") : TEXT("false"));

	if (!AssignedItemAsset)
	{
		return;
	}

	// Store data
	AssignedItem = AssignedItemAsset;
	Count = InCount;
	AssignedItemPrice = Price;
	Infinite = InfiniteStock;
	Type = static_cast<ESLFVendorType>(InType);
	IsOccupied = true;

	// Update icon from item's ItemInformation
	if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		UTexture2D* ItemIcon = nullptr;
		if (!AssignedItemAsset->ItemInformation.IconSmall.IsNull())
		{
			ItemIcon = AssignedItemAsset->ItemInformation.IconSmall.LoadSynchronous();
		}

		if (ItemIcon)
		{
			IconImage->SetBrushFromTexture(ItemIcon);
		}
		IconImage->SetVisibility(ESlateVisibility::Visible);
	}

	// Update amount text
	if (UTextBlock* AmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("AmountText"))))
	{
		if (Infinite)
		{
			AmountText->SetText(FText::FromString(TEXT("\u221E")));
			AmountText->SetVisibility(ESlateVisibility::Visible);
		}
		else if (Count > 1)
		{
			AmountText->SetText(FText::AsNumber(Count));
			AmountText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			AmountText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Update price text
	if (UTextBlock* PriceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PriceText"))))
	{
		PriceText->SetText(FText::AsNumber(Price));
		PriceText->SetVisibility(ESlateVisibility::Visible);
	}

	// Broadcast event
	OnSlotAssigned.Broadcast(this);
}

/**
 * EventOnSelected - Handle vendor slot selection state change
 */
void UW_VendorSlot::EventOnSelected_Implementation(bool Selected)
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::EventOnSelected - Selected: %s"), Selected ? TEXT("true") : TEXT("false"));

	// Blueprint uses "HightlightBorder" (note typo) - toggle visibility like other slot widgets
	if (UBorder* HighlightBorder = Cast<UBorder>(GetWidgetFromName(TEXT("HightlightBorder"))))
	{
		HighlightBorder->SetVisibility(Selected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("  HightlightBorder visibility: %s"), Selected ? TEXT("Visible") : TEXT("Collapsed"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  HightlightBorder NOT FOUND - trying BackgroundBorder fallback"));
		// Fallback: try BackgroundBorder with color change
		if (UBorder* BackgroundBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BackgroundBorder"))))
		{
			if (Selected)
			{
				BackgroundBorder->SetBrushColor(FLinearColor(1.0f, 0.8f, 0.2f, 1.0f)); // Gold highlight
			}
			else
			{
				BackgroundBorder->SetBrushColor(SlotColor);
			}
		}
	}

	// Broadcast event
	OnSelected.Broadcast(Selected, this);
}

/**
 * EventSetHighlighted - Set highlighted state (hover effect)
 */
void UW_VendorSlot::EventSetHighlighted_Implementation(bool Highlighted)
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::EventSetHighlighted - Highlighted: %s"), Highlighted ? TEXT("true") : TEXT("false"));

	// Blueprint uses "HightlightBorder" (note typo) - matches W_InventorySlot and other widgets
	if (UBorder* HighlightBorder = Cast<UBorder>(GetWidgetFromName(TEXT("HightlightBorder"))))
	{
		// Toggle visibility like other slot widgets (inventory, equipment)
		HighlightBorder->SetVisibility(Highlighted ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("  HightlightBorder visibility: %s"), Highlighted ? TEXT("Visible") : TEXT("Collapsed"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  HightlightBorder NOT FOUND - trying SlotBorder fallback"));
		// Fallback: try SlotBorder with color change
		if (UBorder* SlotBorder = Cast<UBorder>(GetWidgetFromName(TEXT("SlotBorder"))))
		{
			if (Highlighted)
			{
				SlotBorder->SetBrushColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f));
			}
			else
			{
				SlotBorder->SetBrushColor(SlotColor);
			}
		}
	}
}

/**
 * EventSlotPressed - Handle vendor slot being pressed
 */
void UW_VendorSlot::EventSlotPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::EventSlotPressed - IsOccupied: %s"), IsOccupied ? TEXT("true") : TEXT("false"));

	// Broadcast press event
	OnPressed.Broadcast(this);
}

/**
 * EventToggleSlot - Enable/disable the vendor slot
 */
void UW_VendorSlot::EventToggleSlot_Implementation(bool IsEnabled)
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::EventToggleSlot - IsEnabled: %s"), IsEnabled ? TEXT("true") : TEXT("false"));

	CraftingSlotEnabled = IsEnabled;

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

void UW_VendorSlot::BindSlotButtonEvents()
{
	// Find the SlotButton widget (Button class in Blueprint)
	CachedSlotButton = Cast<UButton>(GetWidgetFromName(TEXT("SlotButton")));

	if (CachedSlotButton)
	{
		// Bind to button events
		CachedSlotButton->OnClicked.AddDynamic(this, &UW_VendorSlot::HandleSlotButtonClicked);
		CachedSlotButton->OnHovered.AddDynamic(this, &UW_VendorSlot::HandleSlotButtonHovered);
		CachedSlotButton->OnUnhovered.AddDynamic(this, &UW_VendorSlot::HandleSlotButtonUnhovered);

		UE_LOG(LogTemp, Log, TEXT("[W_VendorSlot] Bound to SlotButton events"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[W_VendorSlot] SlotButton not found - using native mouse input"));
	}
}

void UW_VendorSlot::HandleSlotButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_VendorSlot] SlotButton clicked - calling EventSlotPressed"));
	EventSlotPressed();
}

void UW_VendorSlot::HandleSlotButtonHovered()
{
	UE_LOG(LogTemp, Verbose, TEXT("[W_VendorSlot] SlotButton hovered"));
	EventSetHighlighted(true);
}

void UW_VendorSlot::HandleSlotButtonUnhovered()
{
	UE_LOG(LogTemp, Verbose, TEXT("[W_VendorSlot] SlotButton unhovered"));
	EventSetHighlighted(false);
}
