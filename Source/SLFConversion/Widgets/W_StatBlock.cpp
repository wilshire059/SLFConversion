// W_StatBlock.cpp
// C++ Widget implementation for W_StatBlock
//
// 20-PASS VALIDATION: 2026-01-08 - Full implementation with StatManager binding

#include "Widgets/W_StatBlock.h"
#include "Widgets/W_StatEntry.h"
#include "Components/StatManagerComponent.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprints/B_Stat.h"

UW_StatBlock::UW_StatBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Category(FGameplayTag::EmptyTag)
	, ShowUpgradeButtons(false)
	, StatManagerComponent(nullptr)
{
}

void UW_StatBlock::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] NativeConstruct - Category: %s"), *Category.ToString());

	// Get StatManagerComponent - check Pawn FIRST since that's where it's defined (on Character)
	APawn* Pawn = GetOwningPlayerPawn();
	if (Pawn)
	{
		StatManagerComponent = Pawn->FindComponentByClass<UStatManagerComponent>();
		if (StatManagerComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] Found StatManagerComponent on Pawn: %s"), *Pawn->GetName());

			// Check if stats are already initialized
			if (StatManagerComponent->ActiveStats.Num() > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] Stats already initialized (%d stats), setting up now"),
					StatManagerComponent->ActiveStats.Num());
				SetupFromStatManager();
			}
			else
			{
				// Bind to OnStatsInitialized for when stats become available
				StatManagerComponent->OnStatsInitialized.AddDynamic(this, &UW_StatBlock::OnStatsInitialized);
				UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] Bound to OnStatsInitialized"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_StatBlock] StatManagerComponent NOT FOUND on Pawn: %s"), *Pawn->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatBlock] No owning pawn found!"));
	}

	// Fallback: try PlayerController (in case component is there instead)
	if (!StatManagerComponent)
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			StatManagerComponent = PC->FindComponentByClass<UStatManagerComponent>();
			if (StatManagerComponent)
			{
				UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] Found StatManagerComponent on PlayerController (fallback)"));

				if (StatManagerComponent->ActiveStats.Num() > 0)
				{
					SetupFromStatManager();
				}
				else
				{
					StatManagerComponent->OnStatsInitialized.AddDynamic(this, &UW_StatBlock::OnStatsInitialized);
				}
			}
		}
	}
}

void UW_StatBlock::NativeDestruct()
{
	// Unbind from StatManager
	if (StatManagerComponent)
	{
		StatManagerComponent->OnStatsInitialized.RemoveAll(this);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_StatBlock::NativeDestruct"));
}

void UW_StatBlock::CacheWidgetReferences()
{
	// Get the StatsBox vertical box for adding entries
	StatsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatsBox")));
	if (!StatsBox)
	{
		// Try alternative name
		StatsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatEntriesBox")));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] CacheWidgetReferences - StatsBox: %s"),
		StatsBox ? TEXT("FOUND") : TEXT("NOT FOUND"));
}

void UW_StatBlock::OnStatsInitialized()
{
	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] OnStatsInitialized - Setting up stats for category: %s"), *Category.ToString());
	SetupFromStatManager();
}

void UW_StatBlock::SetupFromStatManager()
{
	if (!StatManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatBlock] SetupFromStatManager - No StatManagerComponent!"));
		return;
	}

	// Get all stats from the manager
	TArray<UObject*> AllStatObjects;
	TMap<TSubclassOf<UObject>, FGameplayTag> StatClassesAndCategories;
	StatManagerComponent->GetAllStats(AllStatObjects, StatClassesAndCategories);

	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] SetupFromStatManager - Got %d stats from manager"), AllStatObjects.Num());

	// Filter stats by category
	TArray<UB_Stat*> FilteredStats;
	for (UObject* StatObj : AllStatObjects)
	{
		UB_Stat* Stat = Cast<UB_Stat>(StatObj);
		if (Stat)
		{
			// Check if this stat matches our category
			// If category is empty, show all stats; otherwise filter by category
			if (!Category.IsValid() || Stat->StatInfo.Tag.MatchesTag(Category))
			{
				FilteredStats.Add(Stat);
				UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] Including stat: %s (Category match: %s)"),
					*Stat->StatInfo.Tag.ToString(), *Category.ToString());
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] Filtered to %d stats for category: %s"),
		FilteredStats.Num(), *Category.ToString());

	// Convert to the format SetupCurrentStats expects
	TMap<FGameplayTag, TSubclassOf<UB_Stat>> StatMap;
	SetupCurrentStats(FilteredStats, StatMap);

	// Notify that initialization is complete
	EventStatInitializationComplete();
}

void UW_StatBlock::SetupCurrentStats_Implementation(const TArray<UB_Stat*>& StatObjects, const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& StatClassesAndCategories)
{
	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] SetupCurrentStats - %d stats"), StatObjects.Num());

	// Clear existing entries
	StatEntries.Empty();

	// Make sure we have a container for entries
	CacheWidgetReferences();

	if (!StatsBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatBlock] SetupCurrentStats - No StatsBox container found!"));
		return;
	}

	// Clear existing children
	StatsBox->ClearChildren();

	// Get the W_StatEntry widget class
	// First try to load the Blueprint class
	UClass* StatEntryClass = LoadClass<UW_StatEntry>(nullptr,
		TEXT("/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry.W_StatEntry_C"));

	if (!StatEntryClass)
	{
		// Fall back to C++ class
		StatEntryClass = UW_StatEntry::StaticClass();
		UE_LOG(LogTemp, Warning, TEXT("[W_StatBlock] Using C++ UW_StatEntry class (Blueprint not found)"));
	}

	// Create a W_StatEntry widget for each stat
	for (UB_Stat* Stat : StatObjects)
	{
		if (!Stat) continue;

		UW_StatEntry* StatEntry = CreateWidget<UW_StatEntry>(this, StatEntryClass);
		if (StatEntry)
		{
			// Set the stat on the entry
			StatEntry->Stat = Stat;
			StatEntry->ShowAdjustButtons = ShowUpgradeButtons;

			// Add to container
			StatsBox->AddChild(StatEntry);
			StatEntries.Add(StatEntry);

			// Initialize the entry
			StatEntry->InitStatEntry();

			UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] Created StatEntry for: %s"), *Stat->StatInfo.Tag.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_StatBlock] Failed to create StatEntry widget"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] SetupCurrentStats complete - %d entries created"), StatEntries.Num());
}

TArray<UW_StatEntry*> UW_StatBlock::GetAllStatsInBlock_Implementation()
{
	return StatEntries;
}

void UW_StatBlock::EventStatInitializationComplete_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock] EventStatInitializationComplete - %d entries"), StatEntries.Num());
}
