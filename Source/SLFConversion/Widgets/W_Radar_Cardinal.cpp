// W_Radar_Cardinal.cpp
// C++ Widget implementation for W_Radar_Cardinal
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Updated: 2026-01-20 - Added text display logic

#include "Widgets/W_Radar_Cardinal.h"
#include "Components/TextBlock.h"

UW_Radar_Cardinal::UW_Radar_Cardinal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MaxAllowedNameLength(1)
	, CardinalText(nullptr)
{
}

void UW_Radar_Cardinal::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Radar_Cardinal::NativeConstruct - Text: %s, Angle: %.0f"),
		*CardinalData.UIDisplayText, CardinalData.Value);
}

void UW_Radar_Cardinal::NativeDestruct()
{
	Super::NativeDestruct();
}

void UW_Radar_Cardinal::CacheWidgetReferences()
{
	if (!CardinalText)
	{
		CardinalText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CardinalText")));
	}
}

void UW_Radar_Cardinal::UpdateCardinalDisplay()
{
	// Ensure we have the text widget reference
	if (!CardinalText)
	{
		CardinalText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CardinalText")));
	}

	if (CardinalText)
	{
		// Get the display text from CardinalData
		FString DisplayText = CardinalData.UIDisplayText;

		// Truncate to MaxAllowedNameLength if needed
		if (MaxAllowedNameLength > 0 && DisplayText.Len() > MaxAllowedNameLength)
		{
			DisplayText = DisplayText.Left(MaxAllowedNameLength);
		}

		// Convert to uppercase
		DisplayText = DisplayText.ToUpper();

		// Set the text
		CardinalText->SetText(FText::FromString(DisplayText));

		UE_LOG(LogTemp, Log, TEXT("UW_Radar_Cardinal::UpdateCardinalDisplay - Set text to: %s (Angle: %.0f)"),
			*DisplayText, CardinalData.Value);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_Radar_Cardinal::UpdateCardinalDisplay - CardinalText widget not found!"));
	}
}

