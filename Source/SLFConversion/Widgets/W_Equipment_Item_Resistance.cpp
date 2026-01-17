// W_Equipment_Item_Resistance.cpp
// C++ Widget implementation for W_Equipment_Item_Resistance
//
// AAA DESIGN: Resistance stats are ITEM bonuses, not character stats.
// The widget displays what resistance values an armor piece provides.
// All 4 resistance types are shown (Immunity, Focus, Robustness, Vitality)
// with values from the item's StatChanges, defaulting to 0 if not present.

#include "Widgets/W_Equipment_Item_Resistance.h"
#include "Widgets/W_ItemInfoEntry.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetTree.h"

// ════════════════════════════════════════════════════════════════════════════════
// RESISTANCE STAT TYPES - All 4 resistance categories in Soulslike games
// ════════════════════════════════════════════════════════════════════════════════
namespace SLFResistanceStats
{
	// The 4 standard resistance types in order
	static const FName StatTypes[] = {
		FName("SoulslikeFramework.Stat.Resistance.Immunity"),
		FName("SoulslikeFramework.Stat.Resistance.Focus"),
		FName("SoulslikeFramework.Stat.Resistance.Robustness"),
		FName("SoulslikeFramework.Stat.Resistance.Vitality")
	};
	static const int32 NumStats = 4;
}

UW_Equipment_Item_Resistance::UW_Equipment_Item_Resistance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ResistanceEntriesBox(nullptr)
	, StatsContainer(nullptr)
{
}

void UW_Equipment_Item_Resistance::NativeConstruct()
{
	Super::NativeConstruct();
	CacheWidgetReferences();
}

void UW_Equipment_Item_Resistance::NativeDestruct()
{
	Super::NativeDestruct();
	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_Resistance::NativeDestruct"));
}

void UW_Equipment_Item_Resistance::CacheWidgetReferences()
{
	// Use BindWidget property if bound, otherwise find by name
	if (ResistanceEntriesBox)
	{
		StatsContainer = ResistanceEntriesBox;
	}
	else if (!StatsContainer)
	{
		StatsContainer = Cast<UVerticalBox>(GetWidgetFromName(TEXT("ResistanceEntriesBox")));
		if (!StatsContainer)
		{
			StatsContainer = Cast<UVerticalBox>(GetWidgetFromName(TEXT("VerticalBox_7")));
		}
	}

	// Load the W_ItemInfoEntry widget class if not set
	if (!ItemInfoEntryClass)
	{
		ItemInfoEntryClass = LoadClass<UW_ItemInfoEntry>(nullptr,
			TEXT("/Game/SoulslikeFramework/Widgets/Equipment/W_ItemInfoEntry.W_ItemInfoEntry_C"));
	}
}

FText UW_Equipment_Item_Resistance::GetDisplayNameFromTag(const FGameplayTag& Tag) const
{
	// Extract the last part of the tag as display name
	// e.g., "SoulslikeFramework.Stat.Resistance.Immunity" -> "Immunity"
	FString TagString = Tag.ToString();
	int32 LastDotIndex;
	if (TagString.FindLastChar('.', LastDotIndex))
	{
		FString DisplayName = TagString.RightChop(LastDotIndex + 1);
		return FText::FromString(DisplayName);
	}
	return FText::FromString(TagString);
}

void UW_Equipment_Item_Resistance::EventSetupResistanceEntries_Implementation(
	const TMap<FGameplayTag, int32>& TargetResistanceStats,
	const TMap<FGameplayTag, int32>& CurrentResistanceStats,
	bool CanCompare)
{
	UE_LOG(LogTemp, Log, TEXT("[Resistance] EventSetupResistanceEntries - Target: %d stats, Current: %d stats"),
		TargetResistanceStats.Num(), CurrentResistanceStats.Num());

	CacheWidgetReferences();

	if (!StatsContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Resistance] StatsContainer not found!"));
		return;
	}

	if (!ItemInfoEntryClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Resistance] ItemInfoEntryClass not set!"));
		return;
	}

	// Clear existing children
	StatsContainer->ClearChildren();

	// ════════════════════════════════════════════════════════════════════════════════
	// AAA PATTERN: Iterate over ALL defined resistance types
	// Look up values from item's StatChanges, default to 0 if not present
	// This ensures all 4 types are always shown in consistent order
	// ════════════════════════════════════════════════════════════════════════════════

	for (int32 i = 0; i < SLFResistanceStats::NumStats; ++i)
	{
		FGameplayTag StatTag = FGameplayTag::RequestGameplayTag(SLFResistanceStats::StatTypes[i]);

		// Look up target value from item's stat map (default to 0)
		int32 TargetValue = 0;
		if (const int32* TargetPtr = TargetResistanceStats.Find(StatTag))
		{
			TargetValue = *TargetPtr;
		}

		// Look up current value for comparison (default to 0)
		int32 CurrentValue = 0;
		if (const int32* CurrentPtr = CurrentResistanceStats.Find(StatTag))
		{
			CurrentValue = *CurrentPtr;
		}

		// Create the entry widget
		UW_ItemInfoEntry* EntryWidget = CreateWidget<UW_ItemInfoEntry>(this, ItemInfoEntryClass);
		if (!EntryWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Resistance] Failed to create entry widget for %s"), *StatTag.ToString());
			continue;
		}

		// Set the entry properties
		EntryWidget->EntryNameText = GetDisplayNameFromTag(StatTag);
		EntryWidget->Value = TargetValue;
		EntryWidget->CurrentStatValue = static_cast<double>(CurrentValue);
		EntryWidget->CanCompare = CanCompare;

		// Add to container
		UVerticalBoxSlot* EntrySlot = StatsContainer->AddChildToVerticalBox(EntryWidget);
		if (EntrySlot)
		{
			EntrySlot->SetPadding(FMargin(0, 2, 0, 2));
		}

		UE_LOG(LogTemp, Log, TEXT("[Resistance] Created entry: %s = %d"),
			*EntryWidget->EntryNameText.ToString(), TargetValue);
	}

	UE_LOG(LogTemp, Log, TEXT("[Resistance] Populated %d stat entries"), SLFResistanceStats::NumStats);
}

void UW_Equipment_Item_Resistance::CreateEntriesFromMap(
	const TMap<FGameplayTag, int32>& TargetStats,
	const TMap<FGameplayTag, int32>& CurrentStats,
	bool CanCompare)
{
	// Legacy fallback - no longer used but kept for compatibility
	EventSetupResistanceEntries_Implementation(TargetStats, CurrentStats, CanCompare);
}
