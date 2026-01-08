// W_EquipmentSlot.cpp
// C++ Widget implementation for W_EquipmentSlot
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with equipment slot state management and event broadcasting

#include "Widgets/W_EquipmentSlot.h"
#include "SLFPrimaryDataAssets.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

UW_EquipmentSlot::UW_EquipmentSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssignedItem = nullptr;
	IsOccupied = false;
	ID = FGuid::NewGuid();
}

void UW_EquipmentSlot::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Set initial background icon if configured
	if (UImage* BackgroundImage = Cast<UImage>(GetWidgetFromName(TEXT("BackgroundImage"))))
	{
		if (!BackgroundIcon.IsNull())
		{
			UTexture2D* BgTexture = BackgroundIcon.LoadSynchronous();
			if (BgTexture)
			{
				BackgroundImage->SetBrushFromTexture(BgTexture);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UW_EquipmentSlot::NativeConstruct - Slot: %s"), *EquipmentSlot.ToString());
}

void UW_EquipmentSlot::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_EquipmentSlot::NativeDestruct"));
}

void UW_EquipmentSlot::CacheWidgetReferences()
{
	// Widget references are bound via BindWidget in Blueprint UMG
}

/**
 * EventClearEquipmentSlot - Clear the equipment slot
 *
 * Blueprint Logic:
 * 1. Reset state variables (AssignedItem = null, IsOccupied = false)
 * 2. Hide item icon
 * 3. Show background icon (empty slot indicator)
 */
void UW_EquipmentSlot::EventClearEquipmentSlot_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_EquipmentSlot::EventClearEquipmentSlot - Slot: %s"), *EquipmentSlot.ToString());

	// Reset state
	AssignedItem = nullptr;
	IsOccupied = false;

	// Hide item icon
	if (UImage* ItemIcon = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Show background icon (empty slot indicator)
	if (UImage* BackgroundImage = Cast<UImage>(GetWidgetFromName(TEXT("BackgroundImage"))))
	{
		if (!BackgroundIcon.IsNull())
		{
			UTexture2D* BgTexture = BackgroundIcon.LoadSynchronous();
			if (BgTexture)
			{
				BackgroundImage->SetBrushFromTexture(BgTexture);
			}
		}
		BackgroundImage->SetVisibility(ESlateVisibility::Visible);
	}

	// Reset border color
	if (UBorder* SlotBorder = Cast<UBorder>(GetWidgetFromName(TEXT("SlotBorder"))))
	{
		SlotBorder->SetBrushColor(FLinearColor::White);
	}
}

/**
 * EventEquipmentPressed - Handle equipment slot being pressed
 *
 * Blueprint Logic:
 * 1. Broadcast OnPressed event
 */
void UW_EquipmentSlot::EventEquipmentPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_EquipmentSlot::EventEquipmentPressed - IsOccupied: %s"), IsOccupied ? TEXT("true") : TEXT("false"));

	// Broadcast press event
	OnPressed.Broadcast(this);
}

/**
 * EventOccupyEquipmentSlot - Assign an item to this equipment slot
 *
 * Blueprint Logic:
 * 1. Store item reference
 * 2. Set IsOccupied to true
 * 3. Update icon from item's ItemInformation.IconSmall
 * 4. Hide background icon (slot is now occupied)
 */
void UW_EquipmentSlot::EventOccupyEquipmentSlot_Implementation(UPDA_Item* InAssignedItem)
{
	UE_LOG(LogTemp, Log, TEXT("UW_EquipmentSlot::EventOccupyEquipmentSlot - Item: %s, Slot: %s"),
		InAssignedItem ? *InAssignedItem->GetName() : TEXT("None"),
		*EquipmentSlot.ToString());

	if (!InAssignedItem)
	{
		return;
	}

	// Store item data
	AssignedItem = InAssignedItem;
	IsOccupied = true;

	// Update icon from item's ItemInformation
	if (UImage* ItemIcon = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		UTexture2D* ItemTexture = nullptr;
		if (!InAssignedItem->ItemInformation.IconSmall.IsNull())
		{
			ItemTexture = InAssignedItem->ItemInformation.IconSmall.LoadSynchronous();
		}

		if (ItemTexture)
		{
			ItemIcon->SetBrushFromTexture(ItemTexture);
		}
		ItemIcon->SetVisibility(ESlateVisibility::Visible);
	}

	// Hide background icon when slot is occupied
	if (UImage* BackgroundImage = Cast<UImage>(GetWidgetFromName(TEXT("BackgroundImage"))))
	{
		BackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

/**
 * EventOnSelected - Handle equipment slot selection state change
 *
 * Blueprint Logic:
 * 1. Update visual state (border color, highlight)
 * 2. Broadcast OnSelected event
 */
void UW_EquipmentSlot::EventOnSelected_Implementation(bool Selected)
{
	UE_LOG(LogTemp, Log, TEXT("UW_EquipmentSlot::EventOnSelected - Selected: %s"), Selected ? TEXT("true") : TEXT("false"));

	// Update border/background color based on selection
	if (UBorder* SlotBorder = Cast<UBorder>(GetWidgetFromName(TEXT("SlotBorder"))))
	{
		if (Selected)
		{
			// Use a highlight color when selected (gold)
			SlotBorder->SetBrushColor(FLinearColor(1.0f, 0.8f, 0.2f, 1.0f));
		}
		else
		{
			// Return to normal color
			SlotBorder->SetBrushColor(FLinearColor::White);
		}
	}

	// Broadcast event
	OnSelected.Broadcast(Selected, this);
}

/**
 * EventSetHighlighted - Set highlighted state (hover effect)
 *
 * Blueprint Logic:
 * 1. Update visual state to show hover
 * 2. Does not broadcast (visual feedback only)
 */
void UW_EquipmentSlot::EventSetHighlighted_Implementation(bool Highlighted)
{
	UE_LOG(LogTemp, Log, TEXT("UW_EquipmentSlot::EventSetHighlighted - Highlighted: %s"), Highlighted ? TEXT("true") : TEXT("false"));

	// Update visual highlight state
	if (UBorder* SlotBorder = Cast<UBorder>(GetWidgetFromName(TEXT("SlotBorder"))))
	{
		if (Highlighted)
		{
			// Subtle highlight on hover (light gray)
			SlotBorder->SetBrushColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f));
		}
		else
		{
			// Return to normal white
			SlotBorder->SetBrushColor(FLinearColor::White);
		}
	}
}
