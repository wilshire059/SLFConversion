// W_Equipment_Item_StatsGranted.cpp
// C++ Widget implementation for W_Equipment_Item_StatsGranted
//
// 20-PASS VALIDATION: 2026-01-16 - Implemented actual granted stats display

#include "Widgets/W_Equipment_Item_StatsGranted.h"
#include "Widgets/W_ItemInfoEntry.h"
#include "Components/VerticalBox.h"

UW_Equipment_Item_StatsGranted::UW_Equipment_Item_StatsGranted(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatScalingEntriesBox = nullptr;
	GrantedStatEntryWidgetClass = nullptr;
}

void UW_Equipment_Item_StatsGranted::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_StatsGranted::NativeConstruct"));
}

void UW_Equipment_Item_StatsGranted::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_StatsGranted::NativeDestruct"));
}

void UW_Equipment_Item_StatsGranted::CacheWidgetReferences()
{
	if (!StatScalingEntriesBox)
	{
		StatScalingEntriesBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatScalingEntriesBox")));
	}

	// Try to load the entry widget class if not set
	if (!GrantedStatEntryWidgetClass)
	{
		static const FString EntryWidgetPath = TEXT("/Game/SoulslikeFramework/Widgets/Equipment/W_ItemInfoEntry.W_ItemInfoEntry_C");
		GrantedStatEntryWidgetClass = LoadClass<UW_ItemInfoEntry>(nullptr, *EntryWidgetPath);
	}
}

FText UW_Equipment_Item_StatsGranted::GetStatDisplayName(const FGameplayTag& StatTag) const
{
	// Extract the last part of the tag for display
	FString TagString = StatTag.ToString();
	int32 LastDotIndex;
	if (TagString.FindLastChar('.', LastDotIndex))
	{
		return FText::FromString(TagString.RightChop(LastDotIndex + 1));
	}
	return FText::FromString(TagString);
}

void UW_Equipment_Item_StatsGranted::EventSetupGrantedStats_Implementation(const TMap<FGameplayTag, double>& TargetGrantedStats, const TMap<FGameplayTag, double>& CurrentGrantedStats)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_StatsGranted::EventSetupGrantedStats_Implementation - Target stats: %d, Current stats: %d"),
		TargetGrantedStats.Num(), CurrentGrantedStats.Num());

	// Clear existing entries
	if (StatScalingEntriesBox)
	{
		StatScalingEntriesBox->ClearChildren();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[GrantedStats] StatScalingEntriesBox is null!"));
		return;
	}

	// Need a valid entry widget class
	if (!GrantedStatEntryWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GrantedStats] GrantedStatEntryWidgetClass is null - cannot create entries"));
		return;
	}

	// Create an entry widget for each granted stat
	for (const auto& StatPair : TargetGrantedStats)
	{
		const FGameplayTag& StatTag = StatPair.Key;
		double TargetValue = StatPair.Value;

		// Get current value for comparison (0 if not present)
		double CurrentValue = 0.0;
		if (const double* FoundCurrent = CurrentGrantedStats.Find(StatTag))
		{
			CurrentValue = *FoundCurrent;
		}

		// Create the entry widget
		UW_ItemInfoEntry* EntryWidget = CreateWidget<UW_ItemInfoEntry>(this, GrantedStatEntryWidgetClass);
		if (EntryWidget)
		{
			// Set the properties before adding to parent
			EntryWidget->EntryNameText = GetStatDisplayName(StatTag);
			EntryWidget->Value = static_cast<int32>(TargetValue);
			EntryWidget->CurrentStatValue = CurrentValue;
			EntryWidget->CanCompare = CurrentGrantedStats.Num() > 0;

			// Add to the vertical box
			StatScalingEntriesBox->AddChild(EntryWidget);

			UE_LOG(LogTemp, Log, TEXT("[GrantedStats] Created entry: %s = %.1f (current: %.1f)"),
				*StatTag.ToString(), TargetValue, CurrentValue);
		}
	}
}
