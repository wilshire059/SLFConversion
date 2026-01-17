// W_Equipment_Item_DamageNegation.cpp
// C++ Widget implementation for W_Equipment_Item_DamageNegation
//
// AAA DESIGN: DamageNegation stats are ITEM bonuses, not character stats.
// The widget displays what defense values an armor piece provides.
// All 6 damage types are shown (Physical, Magic, Fire, Frost, Lightning, Holy)
// with values from the item's StatChanges, defaulting to 0 if not present.

#include "Widgets/W_Equipment_Item_DamageNegation.h"
#include "Widgets/W_ItemInfoEntry.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetTree.h"

// ════════════════════════════════════════════════════════════════════════════════
// DAMAGE NEGATION STAT TYPES - All 6 defense categories in Soulslike games
// ════════════════════════════════════════════════════════════════════════════════
namespace SLFDamageNegationStats
{
	// The 6 standard damage negation types in order
	static const FName StatTypes[] = {
		FName("SoulslikeFramework.Stat.Defense.Negation.Physical"),
		FName("SoulslikeFramework.Stat.Defense.Negation.Magic"),
		FName("SoulslikeFramework.Stat.Defense.Negation.Fire"),
		FName("SoulslikeFramework.Stat.Defense.Negation.Frost"),
		FName("SoulslikeFramework.Stat.Defense.Negation.Lightning"),
		FName("SoulslikeFramework.Stat.Defense.Negation.Holy")
	};
	static const int32 NumStats = 6;
}

UW_Equipment_Item_DamageNegation::UW_Equipment_Item_DamageNegation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DamageNegationEntriesBox(nullptr)
	, StatsContainer(nullptr)
{
}

void UW_Equipment_Item_DamageNegation::NativeConstruct()
{
	Super::NativeConstruct();
	CacheWidgetReferences();
}

void UW_Equipment_Item_DamageNegation::NativeDestruct()
{
	Super::NativeDestruct();
	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_DamageNegation::NativeDestruct"));
}

void UW_Equipment_Item_DamageNegation::CacheWidgetReferences()
{
	// Use BindWidget property if bound, otherwise find by name
	if (DamageNegationEntriesBox)
	{
		StatsContainer = DamageNegationEntriesBox;
	}
	else if (!StatsContainer)
	{
		StatsContainer = Cast<UVerticalBox>(GetWidgetFromName(TEXT("DamageNegationEntriesBox")));
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

FText UW_Equipment_Item_DamageNegation::GetDisplayNameFromTag(const FGameplayTag& Tag) const
{
	// Extract the last part of the tag as display name
	// e.g., "SoulslikeFramework.Stat.Defense.Negation.Physical" -> "Physical"
	FString TagString = Tag.ToString();
	int32 LastDotIndex;
	if (TagString.FindLastChar('.', LastDotIndex))
	{
		FString DisplayName = TagString.RightChop(LastDotIndex + 1);
		return FText::FromString(DisplayName);
	}
	return FText::FromString(TagString);
}

void UW_Equipment_Item_DamageNegation::EventSetupDamageNegationStats_Implementation(
	const TMap<FGameplayTag, int32>& TargetDamageNegationStats,
	const TMap<FGameplayTag, int32>& CurrentDamageNegationStats,
	bool CanCompare)
{
	UE_LOG(LogTemp, Log, TEXT("[DamageNegation] EventSetupDamageNegationStats - Target: %d stats, Current: %d stats"),
		TargetDamageNegationStats.Num(), CurrentDamageNegationStats.Num());

	CacheWidgetReferences();

	if (!StatsContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageNegation] StatsContainer not found!"));
		return;
	}

	if (!ItemInfoEntryClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageNegation] ItemInfoEntryClass not set!"));
		return;
	}

	// Clear existing children
	StatsContainer->ClearChildren();

	// ════════════════════════════════════════════════════════════════════════════════
	// AAA PATTERN: Iterate over ALL defined damage negation types
	// Look up values from item's StatChanges, default to 0 if not present
	// This ensures all 6 types are always shown in consistent order
	// ════════════════════════════════════════════════════════════════════════════════

	for (int32 i = 0; i < SLFDamageNegationStats::NumStats; ++i)
	{
		FGameplayTag StatTag = FGameplayTag::RequestGameplayTag(SLFDamageNegationStats::StatTypes[i]);

		// Look up target value from item's stat map (default to 0)
		int32 TargetValue = 0;
		if (const int32* TargetPtr = TargetDamageNegationStats.Find(StatTag))
		{
			TargetValue = *TargetPtr;
		}

		// Look up current value for comparison (default to 0)
		int32 CurrentValue = 0;
		if (const int32* CurrentPtr = CurrentDamageNegationStats.Find(StatTag))
		{
			CurrentValue = *CurrentPtr;
		}

		// Create the entry widget
		UW_ItemInfoEntry* EntryWidget = CreateWidget<UW_ItemInfoEntry>(this, ItemInfoEntryClass);
		if (!EntryWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("[DamageNegation] Failed to create entry widget for %s"), *StatTag.ToString());
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

		UE_LOG(LogTemp, Log, TEXT("[DamageNegation] Created entry: %s = %d"),
			*EntryWidget->EntryNameText.ToString(), TargetValue);
	}

	UE_LOG(LogTemp, Log, TEXT("[DamageNegation] Populated %d stat entries"), SLFDamageNegationStats::NumStats);
}

void UW_Equipment_Item_DamageNegation::CreateEntriesFromMap(
	const TMap<FGameplayTag, int32>& TargetStats,
	const TMap<FGameplayTag, int32>& CurrentStats,
	bool CanCompare)
{
	// Legacy fallback - no longer used but kept for compatibility
	EventSetupDamageNegationStats_Implementation(TargetStats, CurrentStats, CanCompare);
}
