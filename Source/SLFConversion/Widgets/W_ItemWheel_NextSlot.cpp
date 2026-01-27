// W_ItemWheel_NextSlot.cpp
// C++ Widget implementation for W_ItemWheel_NextSlot
//
// 20-PASS VALIDATION: 2026-01-26
// Full implementation matching bp_only Blueprint logic

#include "Widgets/W_ItemWheel_NextSlot.h"
#include "Widgets/W_ItemWheelSlot.h"
#include "Components/Image.h"
#include "SLFPrimaryDataAssets.h"

UW_ItemWheel_NextSlot::UW_ItemWheel_NextSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ParentWheelSlot(nullptr)
	, ActiveItem(nullptr)
	, ItemIcon(nullptr)
{
}

void UW_ItemWheel_NextSlot::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheel_NextSlot] NativeConstruct"));
}

void UW_ItemWheel_NextSlot::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheel_NextSlot] NativeDestruct"));
}

void UW_ItemWheel_NextSlot::CacheWidgetReferences()
{
	// Cache UI widget references by name (matching Blueprint widget tree)
	ItemIcon = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon")));

	if (!ItemIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_ItemWheel_NextSlot] ItemIcon widget not found"));
	}
}

void UW_ItemWheel_NextSlot::HandleChildItem_Implementation(UPrimaryDataAsset* InItem)
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheel_NextSlot] HandleChildItem - Item: %s"),
		InItem ? *InItem->GetName() : TEXT("None"));

	// Store active item
	ActiveItem = InItem;

	// Update icon display
	if (ItemIcon)
	{
		UPDA_Item* ItemData = Cast<UPDA_Item>(InItem);
		if (ItemData)
		{
			// Get icon from ItemInformation - use IconSmall (primary icon)
			TSoftObjectPtr<UTexture2D> IconTexture = ItemData->ItemInformation.IconSmall;
			if (!IconTexture.IsNull())
			{
				ItemIcon->SetBrushFromSoftTexture(IconTexture);
				ItemIcon->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Log, TEXT("[W_ItemWheel_NextSlot] Set ItemIcon to: %s"), *IconTexture.ToString());
			}
			else
			{
				// No icon, hide the image
				ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else
		{
			// No valid item - hide the icon
			ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UW_ItemWheel_NextSlot::EventOnItemAmountUpdated_Implementation(UPDA_Item* InItem, int32 NewCount)
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheel_NextSlot] EventOnItemAmountUpdated - Item: %s, Count: %d"),
		InItem ? *InItem->GetName() : TEXT("None"), NewCount);

	// Update if this is our active item
	if (InItem && ActiveItem == InItem)
	{
		// Could display count overlay here if needed
		// For now, just log
	}
}

void UW_ItemWheel_NextSlot::EventPlayStanceFadeAnimation_Implementation(bool Pressed)
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheel_NextSlot] EventPlayStanceFadeAnimation - Pressed: %s"),
		Pressed ? TEXT("true") : TEXT("false"));

	// Handle fade animation for stance changes (two-hand vs one-hand)
	// The Blueprint fades out this slot when in two-hand stance
	if (ItemIcon)
	{
		if (Pressed)
		{
			// Two-hand stance active - fade out / dim the icon
			ItemIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.3f));
		}
		else
		{
			// Normal stance - full opacity
			ItemIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}
}
