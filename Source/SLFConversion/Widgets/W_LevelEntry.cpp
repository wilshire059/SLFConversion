// W_LevelEntry.cpp
// C++ Widget implementation for W_LevelEntry

#include "Widgets/W_LevelEntry.h"
#include "Widgets/W_StatEntry_StatName.h"
#include "Components/StatManagerComponent.h"
#include "Components/TextBlock.h"

UW_LevelEntry::UW_LevelEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LocalPlayerLevel = 0;
	ValueTextBlock = nullptr;
	NameWidget = nullptr;
	CachedStatManager = nullptr;
}

void UW_LevelEntry::NativeConstruct()
{
	Super::NativeConstruct();

	CacheWidgetReferences();

	// Set the display name to "Level"
	if (NameWidget)
	{
		NameWidget->Text = FText::FromString(TEXT("Level"));
		// The StatName TextBlock is the root widget of W_StatEntry_StatName
		// We need to find it and set its text directly since PreConstruct already ran
		if (UTextBlock* StatNameText = Cast<UTextBlock>(NameWidget->GetWidgetFromName(TEXT("StatName"))))
		{
			StatNameText->SetText(NameWidget->Text);
		}
	}

	// Get StatManagerComponent from owning player's pawn
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (OwningPawn)
	{
		CachedStatManager = OwningPawn->FindComponentByClass<UStatManagerComponent>();
		if (IsValid(CachedStatManager))
		{
			CachedStatManager->OnLevelUpdated.AddDynamic(this, &UW_LevelEntry::EventOnLevelUpdated);
			LocalPlayerLevel = CachedStatManager->Level;
			UpdateLevelDisplay();
		}
	}
}

void UW_LevelEntry::NativeDestruct()
{
	if (IsValid(CachedStatManager))
	{
		CachedStatManager->OnLevelUpdated.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UW_LevelEntry::CacheWidgetReferences()
{
	ValueTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("Value")));
	NameWidget = Cast<UW_StatEntry_StatName>(GetWidgetFromName(TEXT("Name")));
}

void UW_LevelEntry::UpdateLevelDisplay()
{
	if (ValueTextBlock)
	{
		ValueTextBlock->SetText(FText::AsNumber(LocalPlayerLevel));
	}
}

void UW_LevelEntry::EventOnLevelUpdated_Implementation(int32 NewLevel)
{
	LocalPlayerLevel = NewLevel;
	UpdateLevelDisplay();
}
