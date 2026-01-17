// W_Equipment_Item_RequiredStats.cpp
// C++ Widget implementation for W_Equipment_Item_RequiredStats
//
// 20-PASS VALIDATION: 2026-01-16 - Shows ALL primary stats with requirements

#include "Widgets/W_Equipment_Item_RequiredStats.h"
#include "Widgets/W_ItemInfoEntry_RequiredStats.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/AC_StatManager.h"
#include "Components/StatManagerComponent.h"
#include "Blueprints/B_Stat.h"

UW_Equipment_Item_RequiredStats::UW_Equipment_Item_RequiredStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RequiredStatEntriesBox = nullptr;
	RequiredStatEntryWidgetClass = nullptr;

	// Initialize the primary stat category tag
	PrimaryStatCategoryTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary")), false);
}

void UW_Equipment_Item_RequiredStats::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_RequiredStats::NativeConstruct"));
}

void UW_Equipment_Item_RequiredStats::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_RequiredStats::NativeDestruct"));
}

void UW_Equipment_Item_RequiredStats::CacheWidgetReferences()
{
	if (!RequiredStatEntriesBox)
	{
		RequiredStatEntriesBox = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("RequiredStatEntriesBox")));
	}

	// Try to load the entry widget class if not set
	if (!RequiredStatEntryWidgetClass)
	{
		static const FString EntryWidgetPath = TEXT("/Game/SoulslikeFramework/Widgets/Equipment/W_ItemInfoEntry_RequiredStats.W_ItemInfoEntry_RequiredStats_C");
		RequiredStatEntryWidgetClass = LoadClass<UUserWidget>(nullptr, *EntryWidgetPath);
	}
}

FText UW_Equipment_Item_RequiredStats::GetStatDisplayName(const FGameplayTag& StatTag) const
{
	// Extract the last part of the tag and abbreviate for display
	FString TagString = StatTag.ToString();
	int32 LastDotIndex;
	if (TagString.FindLastChar('.', LastDotIndex))
	{
		FString StatName = TagString.RightChop(LastDotIndex + 1);

		// Abbreviate common stat names (matches bp_only layout)
		if (StatName == TEXT("Vigor")) return FText::FromString(TEXT("Vig"));
		if (StatName == TEXT("Mind")) return FText::FromString(TEXT("Min"));
		if (StatName == TEXT("Endurance")) return FText::FromString(TEXT("End"));
		if (StatName == TEXT("Strength")) return FText::FromString(TEXT("Str"));
		if (StatName == TEXT("Dexterity")) return FText::FromString(TEXT("Dex"));
		if (StatName == TEXT("Intelligence")) return FText::FromString(TEXT("Int"));
		if (StatName == TEXT("Faith")) return FText::FromString(TEXT("Fai"));
		if (StatName == TEXT("Arcane")) return FText::FromString(TEXT("Arc"));

		return FText::FromString(StatName);
	}
	return FText::FromString(TagString);
}

FGameplayTagContainer UW_Equipment_Item_RequiredStats::GetPrimaryStatTags() const
{
	FGameplayTagContainer StatTags;

	// Get the player pawn
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RequiredStats] GetPrimaryStatTags - No owning pawn!"));
		return StatTags;
	}

	// Try to get StatManager component (try both types)
	UAC_StatManager* ACStatManager = OwningPawn->FindComponentByClass<UAC_StatManager>();
	if (ACStatManager)
	{
		StatTags = ACStatManager->GetStatsForCategory(PrimaryStatCategoryTag);
		UE_LOG(LogTemp, Log, TEXT("[RequiredStats] Got %d primary stat tags from AC_StatManager"), StatTags.Num());
	}
	else
	{
		UStatManagerComponent* StatManager = OwningPawn->FindComponentByClass<UStatManagerComponent>();
		if (StatManager)
		{
			StatTags = StatManager->GetStatsForCategory(PrimaryStatCategoryTag);
			UE_LOG(LogTemp, Log, TEXT("[RequiredStats] Got %d primary stat tags from StatManagerComponent"), StatTags.Num());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[RequiredStats] No StatManager component found on pawn!"));
		}
	}

	return StatTags;
}

double UW_Equipment_Item_RequiredStats::GetPlayerCurrentStatValue(const FGameplayTag& StatTag) const
{
	// Get the player pawn
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		return 0.0;
	}

	// Try AC_StatManager first (uses UB_Stat*& and returns void)
	UAC_StatManager* ACStatManager = OwningPawn->FindComponentByClass<UAC_StatManager>();
	if (ACStatManager)
	{
		FStatInfo StatInfo;
		UB_Stat* StatObj = nullptr;
		ACStatManager->GetStat(StatTag, StatObj, StatInfo);
		if (StatObj)
		{
			return StatInfo.CurrentValue;
		}
	}
	else
	{
		// Try StatManagerComponent (uses UObject*& and returns bool)
		UStatManagerComponent* StatManager = OwningPawn->FindComponentByClass<UStatManagerComponent>();
		if (StatManager)
		{
			FStatInfo StatInfo;
			UObject* StatObj = nullptr;
			if (StatManager->GetStat(StatTag, StatObj, StatInfo))
			{
				return StatInfo.CurrentValue;
			}
		}
	}

	return 0.0;
}

void UW_Equipment_Item_RequiredStats::EventSetupRequiredStats_Implementation(const TMap<FGameplayTag, int32>& RequiredStats)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_RequiredStats::EventSetupRequiredStats_Implementation - Required stats: %d"), RequiredStats.Num());

	// Clear existing entries
	if (RequiredStatEntriesBox)
	{
		RequiredStatEntriesBox->ClearChildren();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[RequiredStats] RequiredStatEntriesBox is null!"));
		return;
	}

	// Need a valid entry widget class
	if (!RequiredStatEntryWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RequiredStats] RequiredStatEntryWidgetClass is null - cannot create entries"));
		return;
	}

	// Get ALL primary stat tags from StatManager (like bp_only does)
	FGameplayTagContainer AllPrimaryStats = GetPrimaryStatTags();

	// If we couldn't get stats from StatManager, fall back to hardcoded list
	if (AllPrimaryStats.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RequiredStats] No primary stats from StatManager, using hardcoded list"));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Vigor")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Mind")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Endurance")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Strength")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Dexterity")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Intelligence")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Faith")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Arcane")), false));
	}

	UE_LOG(LogTemp, Log, TEXT("[RequiredStats] Iterating over %d primary stats"), AllPrimaryStats.Num());

	// Create an entry widget for EACH primary stat (like bp_only does)
	int32 Column = 0;
	int32 Row = 0;
	for (const FGameplayTag& StatTag : AllPrimaryStats)
	{
		// Check if this stat has a requirement (Map_Find equivalent)
		int32 RequiredValue = 0;
		if (const int32* FoundValue = RequiredStats.Find(StatTag))
		{
			RequiredValue = *FoundValue;
		}

		// Get player's current stat value for comparison (GetStat equivalent)
		double CurrentStatValue = GetPlayerCurrentStatValue(StatTag);

		// Create the entry widget
		UUserWidget* EntryWidget = CreateWidget<UUserWidget>(this, RequiredStatEntryWidgetClass);
		if (EntryWidget)
		{
			// Try to cast to the specific type to set properties
			UW_ItemInfoEntry_RequiredStats* ReqEntry = Cast<UW_ItemInfoEntry_RequiredStats>(EntryWidget);
			if (ReqEntry)
			{
				ReqEntry->EntryNameText = GetStatDisplayName(StatTag);
				ReqEntry->RequiredValue = RequiredValue;
				ReqEntry->CurrentValue = CurrentStatValue;
			}

			// Add to the uniform grid panel at (Row, Column)
			UUniformGridSlot* GridSlot = RequiredStatEntriesBox->AddChildToUniformGrid(EntryWidget, Row, Column);

			UE_LOG(LogTemp, Log, TEXT("[RequiredStats] Created entry: %s = %d (current: %.1f) at (%d, %d)"),
				*StatTag.ToString(), RequiredValue, CurrentStatValue, Row, Column);

			// Move to next position (2 columns per row)
			Column++;
			if (Column >= 2)
			{
				Column = 0;
				Row++;
			}
		}
	}
}
