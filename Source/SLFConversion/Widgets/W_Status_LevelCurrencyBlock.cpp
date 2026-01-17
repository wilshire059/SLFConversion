// W_Status_LevelCurrencyBlock.cpp
// C++ Widget implementation for W_Status_LevelCurrencyBlock
//
// 20-PASS VALIDATION: 2026-01-16

#include "Widgets/W_Status_LevelCurrencyBlock.h"
#include "Components/TextBlock.h"
#include "Curves/CurveFloat.h"

UW_Status_LevelCurrencyBlock::UW_Status_LevelCurrencyBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, LocalPlayerLevel(1)
	, LocalCurrency(0)
	, LevelRequirementCurve(nullptr)
	, LevelText(nullptr)
	, CurrencyText(nullptr)
	, RequiredCurrencyText(nullptr)
{
}

void UW_Status_LevelCurrencyBlock::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Status_LevelCurrencyBlock::NativeConstruct"));
}

void UW_Status_LevelCurrencyBlock::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Status_LevelCurrencyBlock::NativeDestruct"));
}

void UW_Status_LevelCurrencyBlock::CacheWidgetReferences()
{
	// Widget references are bound via BindWidgetOptional in UPROPERTY
}

void UW_Status_LevelCurrencyBlock::EventInitializeLevelAndCurrency_Implementation(int32 CurrentLevel, int32 CurrentCurrency)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Status_LevelCurrencyBlock] EventInitializeLevelAndCurrency - Level: %d, Currency: %d"), CurrentLevel, CurrentCurrency);

	// Store the values locally
	LocalPlayerLevel = CurrentLevel;
	LocalCurrency = CurrentCurrency;

	// Set LevelText to current level
	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(CurrentLevel));
		UE_LOG(LogTemp, Log, TEXT("[W_Status_LevelCurrencyBlock] Set LevelText to: %d"), CurrentLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Status_LevelCurrencyBlock] LevelText widget is NULL"));
	}

	// Set CurrencyText to current currency
	if (CurrencyText)
	{
		CurrencyText->SetText(FText::AsNumber(CurrentCurrency));
		UE_LOG(LogTemp, Log, TEXT("[W_Status_LevelCurrencyBlock] Set CurrencyText to: %d"), CurrentCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Status_LevelCurrencyBlock] CurrencyText widget is NULL"));
	}

	// Calculate required currency for next level from curve
	int32 RequiredCurrency = 0;
	if (LevelRequirementCurve)
	{
		// Get float value for next level (CurrentLevel + 1)
		float RequiredFloat = LevelRequirementCurve->GetFloatValue(static_cast<float>(CurrentLevel + 1));
		RequiredCurrency = FMath::TruncToInt(RequiredFloat);
		UE_LOG(LogTemp, Log, TEXT("[W_Status_LevelCurrencyBlock] Required currency for level %d: %d (from curve)"), CurrentLevel + 1, RequiredCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Status_LevelCurrencyBlock] LevelRequirementCurve is NULL"));
	}

	// Set RequiredCurrencyText
	if (RequiredCurrencyText)
	{
		RequiredCurrencyText->SetText(FText::AsNumber(RequiredCurrency));

		// Set color based on whether player has enough currency
		// Green-ish if can afford, red-ish if cannot
		if (CurrentCurrency >= RequiredCurrency)
		{
			// Player has enough - use green/success color
			FSlateColor AffordColor = FSlateColor(FLinearColor(0.2f, 0.8f, 0.2f, 1.0f));
			RequiredCurrencyText->SetColorAndOpacity(AffordColor);
			UE_LOG(LogTemp, Log, TEXT("[W_Status_LevelCurrencyBlock] Currency sufficient - green"));
		}
		else
		{
			// Player doesn't have enough - use red/warning color
			FSlateColor CannotAffordColor = FSlateColor(FLinearColor(0.8f, 0.2f, 0.2f, 1.0f));
			RequiredCurrencyText->SetColorAndOpacity(CannotAffordColor);
			UE_LOG(LogTemp, Log, TEXT("[W_Status_LevelCurrencyBlock] Currency insufficient - red"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Status_LevelCurrencyBlock] RequiredCurrencyText widget is NULL"));
	}
}
