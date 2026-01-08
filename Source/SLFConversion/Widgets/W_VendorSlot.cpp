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
}

void UW_VendorSlot::NativeConstruct()
{
	Super::NativeConstruct();

	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::NativeConstruct"));
}

void UW_VendorSlot::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_VendorSlot::NativeDestruct"));
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

	// Update border/background color based on selection
	if (UBorder* SlotBorder = Cast<UBorder>(GetWidgetFromName(TEXT("SlotBorder"))))
	{
		if (Selected)
		{
			SlotBorder->SetBrushColor(FLinearColor(1.0f, 0.8f, 0.2f, 1.0f)); // Gold highlight
		}
		else
		{
			SlotBorder->SetBrushColor(SlotColor);
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
