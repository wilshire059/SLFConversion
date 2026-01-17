// W_ItemInfoEntry.cpp
// C++ Widget implementation for W_ItemInfoEntry
//
// 20-PASS VALIDATION: 2026-01-16 - Fixed to update TextBlock widgets

#include "Widgets/W_ItemInfoEntry.h"
#include "Components/TextBlock.h"

UW_ItemInfoEntry::UW_ItemInfoEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Value(0)
	, CurrentStatValue(0.0)
	, CanCompare(false)
	, EntryText(nullptr)
	, ValueText(nullptr)
{
}

void UW_ItemInfoEntry::NativeConstruct()
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
		// Display the value as text
		ValueText->SetText(FText::AsNumber(Value));

		// If comparison is enabled, set color based on Value vs CurrentStatValue
		if (CanCompare)
		{
			double TargetValue = static_cast<double>(Value);
			if (TargetValue > CurrentStatValue)
			{
				// Better stat - green
				ValueText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.8f, 0.2f, 1.0f)));
			}
			else if (TargetValue < CurrentStatValue)
			{
				// Worse stat - red
				ValueText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.2f, 0.2f, 1.0f)));
			}
			// If equal, keep default color
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UW_ItemInfoEntry::NativeConstruct - Name: %s, Value: %d, Current: %.1f, Compare: %s"),
		*EntryNameText.ToString(), Value, CurrentStatValue, CanCompare ? TEXT("true") : TEXT("false"));
}

void UW_ItemInfoEntry::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_ItemInfoEntry::NativeDestruct"));
}

void UW_ItemInfoEntry::CacheWidgetReferences()
{
	if (!EntryText)
	{
		EntryText = Cast<UTextBlock>(GetWidgetFromName(TEXT("EntryText")));
	}
	if (!ValueText)
	{
		ValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ValueText")));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_ItemInfoEntry] CacheWidgetReferences - EntryText: %s, ValueText: %s"),
		EntryText ? TEXT("FOUND") : TEXT("NULL"),
		ValueText ? TEXT("FOUND") : TEXT("NULL"));
}

