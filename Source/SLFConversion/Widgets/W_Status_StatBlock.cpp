// W_Status_StatBlock.cpp
// C++ Widget implementation for W_Status_StatBlock
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Status_StatBlock.h"
#include "Widgets/W_StatEntry_Status.h"
#include "Blueprints/SLFStatBase.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

UW_Status_StatBlock::UW_Status_StatBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, StatBox(nullptr)
{
}

void UW_Status_StatBlock::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Status_StatBlock::NativeConstruct"));
}

void UW_Status_StatBlock::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Status_StatBlock::NativeDestruct"));
}

void UW_Status_StatBlock::CacheWidgetReferences()
{
	// TODO: Cache any widget references needed for logic
}

void UW_Status_StatBlock::SetupCurrentStats_Implementation(const TArray<USLFStatBase*>& StatObjects, const TMap<FGameplayTag, TSubclassOf<USLFStatBase>>& StatClassesAndCategories)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Status_StatBlock] SetupCurrentStats - %d stats, Block Category: %s"),
		StatObjects.Num(), *Category.ToString());

	// Get VerticalBox to add children to - try BindWidget first, then fallback to GetWidgetFromName
	UVerticalBox* StatsBox = StatBox;
	if (!StatsBox)
	{
		StatsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatBox")));
	}

	if (!StatsBox)
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Status_StatBlock] Could not find StatBox widget!"));
		return;
	}

	// Clear existing children
	StatsBox->ClearChildren();
	StatEntries.Empty();

	// Load the W_StatEntry_Status Blueprint class
	UClass* StatEntryClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status.W_StatEntry_Status_C"));

	if (!StatEntryClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Status_StatBlock] Could not load W_StatEntry_Status class!"));
		return;
	}

	int32 CreatedCount = 0;

	// For each stat, check category and create widget
	for (USLFStatBase* StatObj : StatObjects)
	{
		if (!StatObj) continue;

		// Get the stat's tag (e.g., "SoulslikeFramework.Stat.Primary.Vigor")
		const FGameplayTag& StatTag = StatObj->StatInfo.Tag;

		// Check if this stat belongs in this block by category
		// MatchesTag() returns true if StatTag is a child of Category
		// e.g., "SoulslikeFramework.Stat.Primary.Vigor".MatchesTag("SoulslikeFramework.Stat.Primary") = true
		// If Category is not valid (empty), show all stats
		bool bShouldShow = !Category.IsValid() || StatTag.MatchesTag(Category);

		UE_LOG(LogTemp, Verbose, TEXT("[W_Status_StatBlock] Stat: %s, Tag: %s, MatchesCategory: %s"),
			*StatObj->StatInfo.DisplayName.ToString(),
			*StatTag.ToString(),
			bShouldShow ? TEXT("YES") : TEXT("NO"));

		if (bShouldShow)
		{
			// Create W_StatEntry_Status widget
			UW_StatEntry_Status* NewEntry = CreateWidget<UW_StatEntry_Status>(GetOwningPlayer(), StatEntryClass);
			if (NewEntry)
			{
				// Set the StatBase property BEFORE adding to parent (NativeConstruct reads it)
				NewEntry->StatBase = StatObj;

				// Add to vertical box
				UVerticalBoxSlot* BoxSlot = StatsBox->AddChildToVerticalBox(NewEntry);
				if (BoxSlot)
				{
					// Optional: Set slot properties for padding/alignment
					BoxSlot->SetPadding(FMargin(0.f, 2.f, 0.f, 2.f));
				}

				// Track in array
				StatEntries.Add(NewEntry);
				CreatedCount++;

				UE_LOG(LogTemp, Log, TEXT("[W_Status_StatBlock] Created entry for stat: %s (Tag: %s, Current: %.0f, Max: %.0f)"),
					*StatObj->StatInfo.DisplayName.ToString(),
					*StatTag.ToString(),
					StatObj->StatInfo.CurrentValue,
					StatObj->StatInfo.MaxValue);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Status_StatBlock] Created %d stat entries for category %s"), CreatedCount, *Category.ToString());
}

TArray<UW_StatEntry_Status*> UW_Status_StatBlock::GetAllStatsInBlock_Implementation()
{
	// Return all status stat entry widgets in this block
	return StatEntries;
}
void UW_Status_StatBlock::EventStatInitializationComplete_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Status_StatBlock::EventStatInitializationComplete_Implementation"));
}
