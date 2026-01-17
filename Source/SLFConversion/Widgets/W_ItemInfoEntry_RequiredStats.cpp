// W_ItemInfoEntry_RequiredStats.cpp
// C++ Widget implementation for W_ItemInfoEntry_RequiredStats
//
// 20-PASS VALIDATION: 2026-01-16 - Fixed to update TextBlock widgets

#include "Widgets/W_ItemInfoEntry_RequiredStats.h"
#include "Components/TextBlock.h"

UW_ItemInfoEntry_RequiredStats::UW_ItemInfoEntry_RequiredStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, RequiredValue(0)
	, CurrentValue(0.0)
	, EntryText(nullptr)
	, ValueText(nullptr)
{
}

void UW_ItemInfoEntry_RequiredStats::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Update the TextBlock widgets with the current data
	if (EntryText)
	{
		EntryText->SetText(EntryNameText);
	}

	if (ValueText)
	{
		// If there's no requirement (RequiredValue == 0), show empty/dash
		// If there IS a requirement, show the value with coloring
		if (RequiredValue <= 0)
		{
			// No requirement for this stat - show nothing (like bp_only)
			ValueText->SetText(FText::GetEmpty());
		}
		else
		{
			// Display the required value as text
			ValueText->SetText(FText::AsNumber(RequiredValue));

			// Set color based on whether current value meets requirement
			// Green if current >= required, red otherwise
			if (CurrentValue >= static_cast<double>(RequiredValue))
			{
				ValueText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.8f, 0.2f, 1.0f))); // Green
			}
			else
			{
				ValueText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.2f, 0.2f, 1.0f))); // Red
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UW_ItemInfoEntry_RequiredStats::NativeConstruct - Name: %s, Required: %d, Current: %.1f"),
		*EntryNameText.ToString(), RequiredValue, CurrentValue);
}

void UW_ItemInfoEntry_RequiredStats::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_ItemInfoEntry_RequiredStats::NativeDestruct"));
}

void UW_ItemInfoEntry_RequiredStats::CacheWidgetReferences()
{
	if (!EntryText)
	{
		EntryText = Cast<UTextBlock>(GetWidgetFromName(TEXT("EntryText")));
	}
	if (!ValueText)
	{
		ValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ValueText")));
	}
}

