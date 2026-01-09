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

	// Set initial background icon if configured, otherwise hide to avoid white square
	if (UImage* BackgroundImage = Cast<UImage>(GetWidgetFromName(TEXT("BackgroundImage"))))
	{
		if (!BackgroundIcon.IsNull())
		{
			UTexture2D* BgTexture = BackgroundIcon.LoadSynchronous();
			if (BgTexture)
			{
				BackgroundImage->SetBrushFromTexture(BgTexture);
				BackgroundImage->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				// No valid texture loaded - hide to prevent white square
				BackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else
		{
			// No background icon set - hide to prevent white square
			BackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
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

	// Show background icon (empty slot indicator) only if we have a valid texture
	if (UImage* BackgroundImage = Cast<UImage>(GetWidgetFromName(TEXT("BackgroundImage"))))
	{
		if (!BackgroundIcon.IsNull())
		{
			UTexture2D* BgTexture = BackgroundIcon.LoadSynchronous();
			if (BgTexture)
			{
				BackgroundImage->SetBrushFromTexture(BgTexture);
				BackgroundImage->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				BackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else
		{
			// No background texture - hide to prevent white square
			BackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Hide highlight border
	if (UBorder* HighlightBorder = Cast<UBorder>(GetWidgetFromName(TEXT("HightlightBorder"))))
	{
		HighlightBorder->SetVisibility(ESlateVisibility::Collapsed);
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
 * 1. Toggle highlight border visibility
 * 2. Broadcast OnSelected event
 */
void UW_EquipmentSlot::EventOnSelected_Implementation(bool Selected)
{
	UE_LOG(LogTemp, Log, TEXT("UW_EquipmentSlot::EventOnSelected - Selected: %s"), Selected ? TEXT("true") : TEXT("false"));

	// Toggle highlight border visibility (Blueprint pattern - note typo in widget name)
	if (UBorder* HighlightBorder = Cast<UBorder>(GetWidgetFromName(TEXT("HightlightBorder"))))
	{
		HighlightBorder->SetVisibility(Selected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// Broadcast event
	OnSelected.Broadcast(Selected, this);
}

/**
 * EventSetHighlighted - Set highlighted state (hover effect)
 *
 * Blueprint Logic:
 * 1. Toggle highlight border visibility for hover
 * 2. Does not broadcast (visual feedback only)
 */
void UW_EquipmentSlot::EventSetHighlighted_Implementation(bool Highlighted)
{
	UE_LOG(LogTemp, Log, TEXT("UW_EquipmentSlot::EventSetHighlighted - Highlighted: %s"), Highlighted ? TEXT("true") : TEXT("false"));

	// Toggle highlight border visibility for hover
	if (UBorder* HighlightBorder = Cast<UBorder>(GetWidgetFromName(TEXT("HightlightBorder"))))
	{
		HighlightBorder->SetVisibility(Highlighted ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
