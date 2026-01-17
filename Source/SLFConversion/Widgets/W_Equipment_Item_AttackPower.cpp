// W_Equipment_Item_AttackPower.cpp
// C++ Widget implementation for W_Equipment_Item_AttackPower
//
// 20-PASS VALIDATION: 2026-01-16 - Implemented actual stat display

#include "Widgets/W_Equipment_Item_AttackPower.h"
#include "Widgets/W_ItemInfoEntry.h"
#include "Components/VerticalBox.h"
#include "Components/AC_StatManager.h"
#include "Components/StatManagerComponent.h"
#include "GameplayTagsManager.h"

UW_Equipment_Item_AttackPower::UW_Equipment_Item_AttackPower(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AttackPowerEntriesBox = nullptr;
	EntryWidgetClass = nullptr;

	// Initialize the attack power category tag (used to get all damage type stats from StatManager)
	AttackPowerCategoryTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Secondary.AttackPower")), false);
}

void UW_Equipment_Item_AttackPower::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_AttackPower::NativeConstruct"));
}

void UW_Equipment_Item_AttackPower::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_AttackPower::NativeDestruct"));
}

void UW_Equipment_Item_AttackPower::CacheWidgetReferences()
{
	// AttackPowerEntriesBox is bound via meta=(BindWidget)
	if (!AttackPowerEntriesBox)
	{
		AttackPowerEntriesBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("AttackPowerEntriesBox")));
	}

	// Try to load the entry widget class if not set
	if (!EntryWidgetClass)
	{
		static const FString EntryWidgetPath = TEXT("/Game/SoulslikeFramework/Widgets/Equipment/W_ItemInfoEntry.W_ItemInfoEntry_C");
		EntryWidgetClass = LoadClass<UW_ItemInfoEntry>(nullptr, *EntryWidgetPath);
	}
}

FText UW_Equipment_Item_AttackPower::GetStatDisplayName(const FGameplayTag& StatTag) const
{
	// Extract the last part of the tag for display
	// e.g., "SoulslikeFramework.Damage.Physical" -> "Physical"
	FString TagString = StatTag.ToString();
	int32 LastDotIndex;
	if (TagString.FindLastChar('.', LastDotIndex))
	{
		return FText::FromString(TagString.RightChop(LastDotIndex + 1));
	}
	return FText::FromString(TagString);
}

FGameplayTagContainer UW_Equipment_Item_AttackPower::GetAttackPowerStatTags() const
{
	FGameplayTagContainer StatTags;

	// Get the player pawn
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttackPower] GetAttackPowerStatTags - No owning pawn!"));
		return StatTags;
	}

	// Try to get StatManager component (try both types)
	UAC_StatManager* ACStatManager = OwningPawn->FindComponentByClass<UAC_StatManager>();
	if (ACStatManager)
	{
		StatTags = ACStatManager->GetStatsForCategory(AttackPowerCategoryTag);
		UE_LOG(LogTemp, Log, TEXT("[AttackPower] Got %d stat tags from AC_StatManager"), StatTags.Num());
	}
	else
	{
		UStatManagerComponent* StatManager = OwningPawn->FindComponentByClass<UStatManagerComponent>();
		if (StatManager)
		{
			StatTags = StatManager->GetStatsForCategory(AttackPowerCategoryTag);
			UE_LOG(LogTemp, Log, TEXT("[AttackPower] Got %d stat tags from StatManagerComponent"), StatTags.Num());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AttackPower] No StatManager component found on pawn!"));
		}
	}

	return StatTags;
}

void UW_Equipment_Item_AttackPower::EventSetupAttackPowerStats_Implementation(const TMap<FGameplayTag, int32>& TargetAttackPowerStats, const TMap<FGameplayTag, int32>& CurrentAttackPowerStats, bool CanCompare)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Equipment_Item_AttackPower::EventSetupAttackPowerStats_Implementation - Target stats: %d, Current stats: %d, CanCompare: %s"),
		TargetAttackPowerStats.Num(), CurrentAttackPowerStats.Num(), CanCompare ? TEXT("true") : TEXT("false"));

	// Clear existing entries
	if (AttackPowerEntriesBox)
	{
		AttackPowerEntriesBox->ClearChildren();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttackPower] AttackPowerEntriesBox is null!"));
		return;
	}

	// Need a valid entry widget class
	if (!EntryWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttackPower] EntryWidgetClass is null - cannot create entries"));
		return;
	}

	// Get ALL attack power stat tags from StatManager (not just what's in the passed map)
	// This matches Blueprint behavior: iterate over all damage types, showing 0 for those not present
	FGameplayTagContainer AllAttackPowerTags = GetAttackPowerStatTags();

	// If we couldn't get tags from StatManager, fall back to the passed map keys
	if (AllAttackPowerTags.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttackPower] No attack power tags from StatManager, falling back to passed map keys"));
		for (const auto& StatPair : TargetAttackPowerStats)
		{
			AllAttackPowerTags.AddTag(StatPair.Key);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[AttackPower] Creating entries for %d attack power stat types"), AllAttackPowerTags.Num());

	// Create an entry widget for each stat type
	for (const FGameplayTag& StatTag : AllAttackPowerTags)
	{
		// Get target value (0 if not present in map)
		int32 TargetValue = 0;
		if (const int32* FoundTarget = TargetAttackPowerStats.Find(StatTag))
		{
			TargetValue = *FoundTarget;
		}

		// Get current value for comparison (0 if not present)
		int32 CurrentValue = 0;
		if (const int32* FoundCurrent = CurrentAttackPowerStats.Find(StatTag))
		{
			CurrentValue = *FoundCurrent;
		}

		// Create the entry widget
		UW_ItemInfoEntry* EntryWidget = CreateWidget<UW_ItemInfoEntry>(this, EntryWidgetClass);
		if (EntryWidget)
		{
			// Set the properties before adding to parent
			EntryWidget->EntryNameText = GetStatDisplayName(StatTag);
			EntryWidget->Value = TargetValue;
			EntryWidget->CurrentStatValue = static_cast<double>(CurrentValue);
			EntryWidget->CanCompare = CanCompare;

			// Add to the vertical box
			AttackPowerEntriesBox->AddChild(EntryWidget);

			UE_LOG(LogTemp, Log, TEXT("[AttackPower] Created entry: %s = %d (current: %d)"),
				*StatTag.ToString(), TargetValue, CurrentValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AttackPower] Failed to create entry widget for %s"), *StatTag.ToString());
		}
	}
}
