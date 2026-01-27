// W_LevelCurrencyBlock_LevelUp.cpp
// C++ Widget implementation for W_LevelCurrencyBlock_LevelUp
//
// 20-PASS VALIDATION: 2026-01-17

#include "Widgets/W_LevelCurrencyBlock_LevelUp.h"
#include "Components/TextBlock.h"
#include "Components/ProgressManagerComponent.h"
#include "GameFramework/PlayerController.h"
#include "Styling/SlateColor.h"

UW_LevelCurrencyBlock_LevelUp::UW_LevelCurrencyBlock_LevelUp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, LocalPlayerLevel(1)
	, LocalCurrency(0)
{
}

void UW_LevelCurrencyBlock_LevelUp::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_LevelCurrencyBlock_LevelUp::NativeConstruct"));
}

void UW_LevelCurrencyBlock_LevelUp::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_LevelCurrencyBlock_LevelUp::NativeDestruct"));
}

void UW_LevelCurrencyBlock_LevelUp::CacheWidgetReferences()
{
	// Widget references are found by name in the event functions
}

void UW_LevelCurrencyBlock_LevelUp::EventInitializeLevelUpCost_Implementation(int32 NewLevel, int32 CurrentCurrency)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelCurrencyBlock_LevelUp::EventInitializeLevelUpCost - Level: %d, Currency: %d"),
		NewLevel, CurrentCurrency);

	// Store local values
	LocalPlayerLevel = NewLevel;
	LocalCurrency = CurrentCurrency;

	// Update Level_Current text
	if (UTextBlock* LevelCurrentText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Level_Current"))))
	{
		LevelCurrentText->SetText(FText::AsNumber(LocalPlayerLevel));
		UE_LOG(LogTemp, Log, TEXT("  Set Level_Current to: %d"), LocalPlayerLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Level_Current widget not found!"));
	}

	// Update Level_New text (next level)
	if (UTextBlock* LevelNewText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Level_New"))))
	{
		LevelNewText->SetText(FText::AsNumber(LocalPlayerLevel + 1));
		UE_LOG(LogTemp, Log, TEXT("  Set Level_New to: %d"), LocalPlayerLevel + 1);
	}

	// Update CurrencyCurrent text
	if (UTextBlock* CurrencyCurrentText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CurrencyCurrent"))))
	{
		CurrencyCurrentText->SetText(FText::AsNumber(LocalCurrency));
		UE_LOG(LogTemp, Log, TEXT("  Set CurrencyCurrent to: %d"), LocalCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  CurrencyCurrent widget not found!"));
	}

	// Get required currency for next level from ProgressManager
	// bp_only uses GetOwningPlayer() (PlayerController), NOT GetOwningPlayerPawn() (Pawn)
	// ProgressManager is on the PlayerController, not the Pawn
	// NOTE: The actual class is UProgressManagerComponent, NOT UAC_ProgressManager
	int32 RequiredCurrency = 0;
	APlayerController* PC = GetOwningPlayer();
	UE_LOG(LogTemp, Log, TEXT("  GetOwningPlayer: %s"), PC ? *PC->GetName() : TEXT("NULL"));
	if (IsValid(PC))
	{
		UProgressManagerComponent* ProgressManager = PC->FindComponentByClass<UProgressManagerComponent>();
		UE_LOG(LogTemp, Log, TEXT("  ProgressManager: %s"), ProgressManager ? TEXT("FOUND") : TEXT("NULL"));
		if (ProgressManager)
		{
			RequiredCurrency = ProgressManager->GetRequiredCurrencyForLevel(LocalPlayerLevel + 1);
			UE_LOG(LogTemp, Log, TEXT("  Required currency for level %d: %d"), LocalPlayerLevel + 1, RequiredCurrency);
		}
	}

	// Update RequiredCurrency text
	if (UTextBlock* RequiredText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RequiredCurrency"))))
	{
		RequiredText->SetText(FText::AsNumber(RequiredCurrency));
	}

	// Determine if player can afford the level up
	bool bCanLevelUp = (LocalCurrency > RequiredCurrency);
	UE_LOG(LogTemp, Log, TEXT("  CanLevelUp: %s (Have: %d, Need: %d)"),
		bCanLevelUp ? TEXT("YES") : TEXT("NO"), LocalCurrency, RequiredCurrency);

	// Update CurrencyNew text with the same currency value
	// Color: RED if can't afford (currency <= required), BEIGE if can afford (currency > required)
	// bp_only colors: RED = (R=0.359375, G=0.005616, B=0.005616), BEIGE = (R=0.858824, G=0.847059, B=0.831373)
	if (UTextBlock* CurrencyNewText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CurrencyNew"))))
	{
		CurrencyNewText->SetText(FText::AsNumber(LocalCurrency));

		// Set color based on affordability
		FLinearColor TextColor = bCanLevelUp
			? FLinearColor(0.858824f, 0.847059f, 0.831373f, 1.0f)  // Beige - can afford
			: FLinearColor(0.359375f, 0.005616f, 0.005616f, 1.0f); // Red - can't afford
		CurrencyNewText->SetColorAndOpacity(FSlateColor(TextColor));

		UE_LOG(LogTemp, Log, TEXT("  Set CurrencyNew to: %d (Color: %s)"),
			LocalCurrency, bCanLevelUp ? TEXT("Beige") : TEXT("Red"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  CurrencyNew widget not found!"));
	}

	// Broadcast OnLevelUpCostChanged so W_LevelUp can enable/disable confirm button
	OnLevelUpCostChanged.Broadcast(bCanLevelUp);
}

void UW_LevelCurrencyBlock_LevelUp::EventOnCurrencyUpdated_Implementation(int32 NewCurrency)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelCurrencyBlock_LevelUp::EventOnCurrencyUpdated - Currency: %d"), NewCurrency);

	LocalCurrency = NewCurrency;

	// Update CurrencyCurrent text
	if (UTextBlock* CurrencyCurrentText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CurrencyCurrent"))))
	{
		CurrencyCurrentText->SetText(FText::AsNumber(LocalCurrency));
	}

	// Recalculate if player can afford level up
	// bp_only uses GetOwningPlayer() (PlayerController) - ProgressManager is on PC, not Pawn
	// NOTE: The actual class is UProgressManagerComponent, NOT UAC_ProgressManager
	int32 RequiredCurrency = 0;
	APlayerController* PC = GetOwningPlayer();
	UE_LOG(LogTemp, Log, TEXT("  [OnCurrencyUpdated] GetOwningPlayer: %s"), PC ? *PC->GetName() : TEXT("NULL"));
	if (IsValid(PC))
	{
		UProgressManagerComponent* ProgressManager = PC->FindComponentByClass<UProgressManagerComponent>();
		UE_LOG(LogTemp, Log, TEXT("  [OnCurrencyUpdated] ProgressManager: %s"), ProgressManager ? TEXT("FOUND") : TEXT("NULL"));
		if (ProgressManager)
		{
			RequiredCurrency = ProgressManager->GetRequiredCurrencyForLevel(LocalPlayerLevel + 1);
			UE_LOG(LogTemp, Log, TEXT("  [OnCurrencyUpdated] Required currency for level %d: %d"), LocalPlayerLevel + 1, RequiredCurrency);
		}
	}

	bool bCanLevelUp = (LocalCurrency > RequiredCurrency);
	UE_LOG(LogTemp, Log, TEXT("  [OnCurrencyUpdated] CanLevelUp: %s (Have: %d, Need: %d)"),
		bCanLevelUp ? TEXT("YES") : TEXT("NO"), LocalCurrency, RequiredCurrency);

	// Update CurrencyNew text with the same currency value and proper color
	if (UTextBlock* CurrencyNewText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CurrencyNew"))))
	{
		CurrencyNewText->SetText(FText::AsNumber(LocalCurrency));

		// Set color based on affordability
		// bp_only colors: RED = (R=0.359375, G=0.005616, B=0.005616), BEIGE = (R=0.858824, G=0.847059, B=0.831373)
		FLinearColor TextColor = bCanLevelUp
			? FLinearColor(0.858824f, 0.847059f, 0.831373f, 1.0f)  // Beige - can afford
			: FLinearColor(0.359375f, 0.005616f, 0.005616f, 1.0f); // Red - can't afford
		CurrencyNewText->SetColorAndOpacity(FSlateColor(TextColor));
	}

	OnLevelUpCostChanged.Broadcast(bCanLevelUp);
}

void UW_LevelCurrencyBlock_LevelUp::EventOnLevelUpdated_Implementation(int32 NewLevel)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelCurrencyBlock_LevelUp::EventOnLevelUpdated - Level: %d"), NewLevel);

	LocalPlayerLevel = NewLevel;

	// Update Level_Current text
	if (UTextBlock* LevelCurrentText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Level_Current"))))
	{
		LevelCurrentText->SetText(FText::AsNumber(LocalPlayerLevel));
	}

	// Update Level_New text (next level)
	if (UTextBlock* LevelNewText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Level_New"))))
	{
		LevelNewText->SetText(FText::AsNumber(LocalPlayerLevel + 1));
	}

	// Recalculate required currency for new level
	// bp_only uses GetOwningPlayer() (PlayerController) - ProgressManager is on PC
	// NOTE: The actual class is UProgressManagerComponent, NOT UAC_ProgressManager
	int32 RequiredCurrency = 0;
	APlayerController* PC = GetOwningPlayer();
	if (IsValid(PC))
	{
		if (UProgressManagerComponent* ProgressManager = PC->FindComponentByClass<UProgressManagerComponent>())
		{
			RequiredCurrency = ProgressManager->GetRequiredCurrencyForLevel(LocalPlayerLevel + 1);
		}
	}

	// Update RequiredCurrency text
	if (UTextBlock* RequiredText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RequiredCurrency"))))
	{
		RequiredText->SetText(FText::AsNumber(RequiredCurrency));
	}

	bool bCanLevelUp = (LocalCurrency > RequiredCurrency);

	// Update CurrencyNew color based on new affordability (currency stays the same, but required changed)
	if (UTextBlock* CurrencyNewText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CurrencyNew"))))
	{
		// Set color based on affordability
		// bp_only colors: RED = (R=0.359375, G=0.005616, B=0.005616), BEIGE = (R=0.858824, G=0.847059, B=0.831373)
		FLinearColor TextColor = bCanLevelUp
			? FLinearColor(0.858824f, 0.847059f, 0.831373f, 1.0f)  // Beige - can afford
			: FLinearColor(0.359375f, 0.005616f, 0.005616f, 1.0f); // Red - can't afford
		CurrencyNewText->SetColorAndOpacity(FSlateColor(TextColor));
	}

	OnLevelUpCostChanged.Broadcast(bCanLevelUp);
}

void UW_LevelCurrencyBlock_LevelUp::EventSetCurrentLevel_Implementation(int32 Level)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelCurrencyBlock_LevelUp::EventSetCurrentLevel - Level: %d"), Level);

	LocalPlayerLevel = Level;

	// Update Level_Current text
	if (UTextBlock* LevelCurrentText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Level_Current"))))
	{
		LevelCurrentText->SetText(FText::AsNumber(LocalPlayerLevel));
	}
}
