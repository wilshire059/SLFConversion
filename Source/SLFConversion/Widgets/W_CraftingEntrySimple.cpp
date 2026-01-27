// W_CraftingEntrySimple.cpp
// C++ Widget implementation for W_CraftingEntrySimple
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// UPDATED: 2026-01-22 - Fixed EventSetupEntry to actually set ItemNameText

#include "Widgets/W_CraftingEntrySimple.h"
#include "Components/TextBlock.h"

UW_CraftingEntrySimple::UW_CraftingEntrySimple(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_CraftingEntrySimple::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_CraftingEntrySimple::NativeConstruct"));
}

void UW_CraftingEntrySimple::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_CraftingEntrySimple::NativeDestruct"));
}

void UW_CraftingEntrySimple::CacheWidgetReferences()
{
	// Widget reference caching intentionally empty - dev tool widget
}

int32 UW_CraftingEntrySimple::GetOwnedAmount_Implementation()
{
	// Return the amount of this item currently owned by the player
	// This would query the inventory manager
	return 0;
}
void UW_CraftingEntrySimple::EventRefreshAmounts_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingEntrySimple::EventRefreshAmounts_Implementation"));
}


void UW_CraftingEntrySimple::EventSetupEntry_Implementation(const FText& ItemName, int32 CreatedAmount)
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingEntrySimple::EventSetupEntry: '%s' x%d"), *ItemName.ToString(), CreatedAmount);

	// Set ItemNameText - find widget by name from Blueprint
	if (UTextBlock* ItemNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText"))))
	{
		ItemNameText->SetText(ItemName);
		UE_LOG(LogTemp, Log, TEXT("  Set ItemNameText to '%s'"), *ItemName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not find ItemNameText widget!"));
	}

	// Set AmountRequired text if widget exists
	if (UTextBlock* AmountRequired = Cast<UTextBlock>(GetWidgetFromName(TEXT("AmountRequired"))))
	{
		AmountRequired->SetText(FText::AsNumber(CreatedAmount));
	}
}
