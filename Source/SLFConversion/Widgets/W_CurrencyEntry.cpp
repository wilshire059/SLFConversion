// W_CurrencyEntry.cpp
// C++ Widget implementation for W_CurrencyEntry

#include "Widgets/W_CurrencyEntry.h"
#include "Widgets/W_StatEntry_StatName.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/TextBlock.h"

UW_CurrencyEntry::UW_CurrencyEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LocalCurrency = 0;
	ValueTextBlock = nullptr;
	NameWidget = nullptr;
	CachedInventoryManager = nullptr;
}

void UW_CurrencyEntry::NativeConstruct()
{
	Super::NativeConstruct();

	CacheWidgetReferences();

	// Set the display name to "Currency"  
	if (NameWidget)
	{
		NameWidget->Text = FText::FromString(TEXT("Currency"));
		// The StatName TextBlock is the root widget of W_StatEntry_StatName
		// We need to find it and set its text directly since PreConstruct already ran
		if (UTextBlock* StatNameText = Cast<UTextBlock>(NameWidget->GetWidgetFromName(TEXT("StatName"))))
		{
			StatNameText->SetText(NameWidget->Text);
		}
	}

	// Get InventoryManagerComponent - check PlayerController first, then Pawn
	APlayerController* PC = GetOwningPlayer();
	APawn* Pawn = GetOwningPlayerPawn();
	
	if (PC)
	{
		CachedInventoryManager = PC->FindComponentByClass<UInventoryManagerComponent>();
	}
	
	if (!CachedInventoryManager && Pawn)
	{
		CachedInventoryManager = Pawn->FindComponentByClass<UInventoryManagerComponent>();
	}
	
	if (IsValid(CachedInventoryManager))
	{
		CachedInventoryManager->OnCurrencyUpdated.AddDynamic(this, &UW_CurrencyEntry::EventOnCurrencyUpdated);
		LocalCurrency = CachedInventoryManager->Currency;
		UpdateCurrencyDisplay();
	}
}

void UW_CurrencyEntry::NativeDestruct()
{
	if (IsValid(CachedInventoryManager))
	{
		CachedInventoryManager->OnCurrencyUpdated.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UW_CurrencyEntry::CacheWidgetReferences()
{
	ValueTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("Value")));
	NameWidget = Cast<UW_StatEntry_StatName>(GetWidgetFromName(TEXT("Name")));
}

void UW_CurrencyEntry::UpdateCurrencyDisplay()
{
	if (ValueTextBlock)
	{
		ValueTextBlock->SetText(FText::AsNumber(LocalCurrency));
	}
}

void UW_CurrencyEntry::EventOnCurrencyUpdated_Implementation(int32 NewCurrency)
{
	LocalCurrency = NewCurrency;
	UpdateCurrencyDisplay();
}
