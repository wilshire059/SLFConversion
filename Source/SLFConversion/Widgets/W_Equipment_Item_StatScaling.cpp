// W_Equipment_Item_StatScaling.cpp
// C++ Widget implementation for W_Equipment_Item_StatScaling
//
// 20-PASS VALIDATION: 2026-01-16 - Implemented actual scaling display

#include "Widgets/W_Equipment_Item_StatScaling.h"
#include "Widgets/W_ItemInfoEntry_StatScaling.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/TextBlock.h"
#include "Components/AC_StatManager.h"
#include "Components/StatManagerComponent.h"

UW_Equipment_Item_StatScaling::UW_Equipment_Item_StatScaling(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatScalingEntriesBox = nullptr;
	ScalingEntryWidgetClass = nullptr;

	// Initialize the primary stat category tag
	PrimaryStatCategoryTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary")), false);
}

void UW_Equipment_Item_StatScaling::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_StatScaling::NativeConstruct"));
}

void UW_Equipment_Item_StatScaling::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_StatScaling::NativeDestruct"));
}

void UW_Equipment_Item_StatScaling::CacheWidgetReferences()
{
	if (!StatScalingEntriesBox)
	{
		StatScalingEntriesBox = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("StatScalingEntriesBox")));
	}

	// Try to load the entry widget class if not set
	if (!ScalingEntryWidgetClass)
	{
		static const FString EntryWidgetPath = TEXT("/Game/SoulslikeFramework/Widgets/Equipment/W_ItemInfoEntry_StatScaling.W_ItemInfoEntry_StatScaling_C");
		ScalingEntryWidgetClass = LoadClass<UUserWidget>(nullptr, *EntryWidgetPath);
	}
}

FText UW_Equipment_Item_StatScaling::GetStatDisplayName(const FGameplayTag& StatTag) const
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

FGameplayTagContainer UW_Equipment_Item_StatScaling::GetPrimaryStatTags() const
{
	FGameplayTagContainer StatTags;

	// Get the player pawn
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatScaling] GetPrimaryStatTags - No owning pawn!"));
		return StatTags;
	}

	// Try to get StatManager component (try both types)
	UAC_StatManager* ACStatManager = OwningPawn->FindComponentByClass<UAC_StatManager>();
	if (ACStatManager)
	{
		StatTags = ACStatManager->GetStatsForCategory(PrimaryStatCategoryTag);
		UE_LOG(LogTemp, Log, TEXT("[StatScaling] Got %d primary stat tags from AC_StatManager"), StatTags.Num());
	}
	else
	{
		UStatManagerComponent* StatManager = OwningPawn->FindComponentByClass<UStatManagerComponent>();
		if (StatManager)
		{
			StatTags = StatManager->GetStatsForCategory(PrimaryStatCategoryTag);
			UE_LOG(LogTemp, Log, TEXT("[StatScaling] Got %d primary stat tags from StatManagerComponent"), StatTags.Num());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[StatScaling] No StatManager component found on pawn!"));
		}
	}

	return StatTags;
}

FText UW_Equipment_Item_StatScaling::GetScalingGradeText(ESLFStatScaling ScalingGrade) const
{
	switch (ScalingGrade)
	{
	case ESLFStatScaling::S: return FText::FromString(TEXT("S"));
	case ESLFStatScaling::A: return FText::FromString(TEXT("A"));
	case ESLFStatScaling::B: return FText::FromString(TEXT("B"));
	case ESLFStatScaling::C: return FText::FromString(TEXT("C"));
	case ESLFStatScaling::D: return FText::FromString(TEXT("D"));
	case ESLFStatScaling::E: return FText::FromString(TEXT("E"));
	default: return FText::FromString(TEXT("-"));
	}
}

void UW_Equipment_Item_StatScaling::EventSetupStatScaling_Implementation(const TMap<FGameplayTag, ESLFStatScaling>& ScalingInfo)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_StatScaling::EventSetupStatScaling_Implementation - Scaling entries: %d"), ScalingInfo.Num());

	// Clear existing entries
	if (StatScalingEntriesBox)
	{
		StatScalingEntriesBox->ClearChildren();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatScaling] StatScalingEntriesBox is null!"));
		return;
	}

	// Need a valid entry widget class
	if (!ScalingEntryWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatScaling] ScalingEntryWidgetClass is null - cannot create entries"));
		return;
	}

	// Get ALL primary stat tags from StatManager (like bp_only does)
	FGameplayTagContainer AllPrimaryStats = GetPrimaryStatTags();

	// If we couldn't get stats from StatManager, fall back to hardcoded list
	if (AllPrimaryStats.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatScaling] No primary stats from StatManager, using hardcoded list"));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Vigor")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Mind")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Endurance")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Strength")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Dexterity")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Intelligence")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Faith")), false));
		AllPrimaryStats.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Primary.Arcane")), false));
	}

	UE_LOG(LogTemp, Log, TEXT("[StatScaling] Iterating over %d primary stats"), AllPrimaryStats.Num());

	// Create an entry widget for EACH primary stat (like bp_only does)
	int32 Column = 0;
	int32 Row = 0;
	for (const FGameplayTag& StatTag : AllPrimaryStats)
	{
		// Check if this stat has a scaling value (Map_Find equivalent)
		FText ScalingGradeText = FText::FromString(TEXT("-")); // Default: no scaling
		if (const ESLFStatScaling* FoundScaling = ScalingInfo.Find(StatTag))
		{
			ScalingGradeText = GetScalingGradeText(*FoundScaling);
		}

		// Create the entry widget
		UUserWidget* EntryWidget = CreateWidget<UUserWidget>(this, ScalingEntryWidgetClass);
		if (EntryWidget)
		{
			// Try to cast to the specific type to set properties
			UW_ItemInfoEntry_StatScaling* ScalingEntry = Cast<UW_ItemInfoEntry_StatScaling>(EntryWidget);
			if (ScalingEntry)
			{
				ScalingEntry->EntryNameText = GetStatDisplayName(StatTag);
				ScalingEntry->Value = ScalingGradeText;
			}

			// Add to the uniform grid panel at (Row, Column)
			UUniformGridSlot* GridSlot = StatScalingEntriesBox->AddChildToUniformGrid(EntryWidget, Row, Column);

			UE_LOG(LogTemp, Log, TEXT("[StatScaling] Created entry: %s = %s at (%d, %d)"),
				*StatTag.ToString(), *ScalingGradeText.ToString(), Row, Column);

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
