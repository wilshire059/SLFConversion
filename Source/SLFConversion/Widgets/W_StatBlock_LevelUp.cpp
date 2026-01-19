// W_StatBlock_LevelUp.cpp
// C++ Widget implementation for W_StatBlock_LevelUp
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_StatBlock_LevelUp.h"
#include "Widgets/W_StatEntry_LevelUp.h"
#include "Blueprints/SLFStatBase.h"
#include "Components/StatManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UW_StatBlock_LevelUp::UW_StatBlock_LevelUp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ShowUpgradeButtons = true;
	CachedStatBox = nullptr;
	CachedStatManager = nullptr;
}

void UW_StatBlock_LevelUp::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Load stat entry widget class (Blueprint class)
	StatEntryWidgetClass = LoadClass<UW_StatEntry_LevelUp>(
		nullptr,
		TEXT("/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_StatEntry_LevelUp.W_StatEntry_LevelUp_C")
	);

	// Find and cache the StatManager component
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (IsValid(PlayerPawn))
	{
		// Try via BPI_GenericCharacter interface first (matches Blueprint behavior)
		if (PlayerPawn->Implements<UBPI_GenericCharacter>())
		{
			UActorComponent* StatManagerComponent = nullptr;
			IBPI_GenericCharacter::Execute_GetStatManager(PlayerPawn, StatManagerComponent);
			CachedStatManager = Cast<UStatManagerComponent>(StatManagerComponent);
		}

		// Fallback to FindComponentByClass
		if (!IsValid(CachedStatManager))
		{
			CachedStatManager = PlayerPawn->FindComponentByClass<UStatManagerComponent>();
		}
	}

	if (IsValid(CachedStatManager))
	{
		// Check if stats are already initialized
		if (CachedStatManager->ActiveStats.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_StatBlock_LevelUp] Stats already initialized (%d), setting up now"),
				CachedStatManager->ActiveStats.Num());
			InitializeFromStatManager();
		}
		else
		{
			// Bind to OnStatsInitialized for when stats become available
			CachedStatManager->OnStatsInitialized.AddDynamic(this, &UW_StatBlock_LevelUp::OnStatsInitialized);
			UE_LOG(LogTemp, Log, TEXT("[W_StatBlock_LevelUp] Bound to OnStatsInitialized, waiting for stats..."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatBlock_LevelUp] No StatManager component found"));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_StatBlock_LevelUp::NativeConstruct - Category: %s"), *Category.ToString());
}

void UW_StatBlock_LevelUp::NativeDestruct()
{
	// Unbind from StatManager events
	if (CachedStatManager)
	{
		CachedStatManager->OnStatsInitialized.RemoveAll(this);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_StatBlock_LevelUp::NativeDestruct"));
}

void UW_StatBlock_LevelUp::CacheWidgetReferences()
{
	// Get CachedStatBox (VerticalBox that holds stat entries)
	CachedStatBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatBox")));

	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock_LevelUp] CacheWidgetReferences - CachedStatBox: %s"),
		CachedStatBox ? TEXT("found") : TEXT("null"));
}

void UW_StatBlock_LevelUp::OnStatsInitialized()
{
	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock_LevelUp] OnStatsInitialized - Setting up stats for category: %s"), *Category.ToString());
	InitializeFromStatManager();
}

void UW_StatBlock_LevelUp::InitializeFromStatManager()
{
	// Use cached StatManager
	if (!IsValid(CachedStatManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatBlock_LevelUp] InitializeFromStatManager - No cached StatManager"));
		return;
	}

	// Get all stats from StatManager (returns UObject* because stats can be various classes)
	TArray<UObject*> AllStatObjects;
	TMap<TSubclassOf<UObject>, FGameplayTag> StatClassesAndCategories;
	CachedStatManager->GetAllStats(AllStatObjects, StatClassesAndCategories);

	UE_LOG(LogTemp, Log, TEXT("[W_StatBlock_LevelUp] Got %d stats from StatManager"), AllStatObjects.Num());

	// Convert to USLFStatBase* array for SetupStatsByCategory
	TArray<USLFStatBase*> StatArray;
	for (UObject* Obj : AllStatObjects)
	{
		if (USLFStatBase* Stat = Cast<USLFStatBase>(Obj))
		{
			StatArray.Add(Stat);
		}
	}

	// Convert the map to the format expected by SetupStatsByCategory
	// Blueprint uses TMap<FGameplayTag, TSubclassOf<USLFStatBase>> but we have the inverse
	TMap<FGameplayTag, TSubclassOf<USLFStatBase>> CategoryToClass;
	for (const auto& Entry : StatClassesAndCategories)
	{
		if (Entry.Key->IsChildOf(USLFStatBase::StaticClass()))
		{
			CategoryToClass.Add(Entry.Value, *Entry.Key);
		}
	}

	// Call SetupStatsByCategory
	SetupStatsByCategory(StatArray, CategoryToClass);
}

void UW_StatBlock_LevelUp::SetupStatsByCategory_Implementation(const TArray<USLFStatBase*>& StatObjects, const TMap<FGameplayTag, TSubclassOf<USLFStatBase>>& StatClassesAndCategories)
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatBlock_LevelUp::SetupStatsByCategory - %d stats, Category: %s"),
		StatObjects.Num(), *Category.ToString());

	// Validate requirements
	if (!IsValid(CachedStatBox))
	{
		UE_LOG(LogTemp, Warning, TEXT("  CachedStatBox is null - cannot add stat entries"));
		return;
	}

	if (!StatEntryWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("  StatEntryWidgetClass is null - cannot create entries"));
		return;
	}

	// Clear existing entries
	CachedStatBox->ClearChildren();
	StatEntries.Empty();

	// Handle category tag remapping and exclusion for stats
	// The widget hierarchy has these blocks:
	// - PrimaryAttributes: SoulslikeFramework.Stat.Primary (Vigor, Mind, Strength, etc.)
	// - SecondaryStats: SoulslikeFramework.Stat.Secondary (HP, FP, Stamina - NOT AttackPower)
	// - MiscStats: SoulslikeFramework.Stat.Misc (Equip Load, Discovery)
	// - AttackStats: SoulslikeFramework.Stat.Secondary.AttackPower (Physical, Magic, etc.)
	// - NegationStats: SoulslikeFramework.Stat.DamageNegation (Physical, Magic, etc.)
	// - ResistanceStats: SoulslikeFramework.Stat.Resistance (Immunity, Focus, etc.)

	FGameplayTag EffectiveCategory = Category;
	FString CategoryStr = Category.ToString();

	// Build exclusion list - stats matching these tags should NOT appear in the current block
	TArray<FGameplayTag> ExcludeTags;

	// Check if this is the "Secondary" (Base Stats) block
	// It should show HP, FP, Stamina but NOT AttackPower stats
	bool bIsSecondaryBlock = CategoryStr.Equals(TEXT("SoulslikeFramework.Stat.Secondary")) ||
	                         CategoryStr.EndsWith(TEXT(".Secondary"));

	if (bIsSecondaryBlock)
	{
		// Exclude AttackPower stats from the Secondary block - they go in AttackStats block
		ExcludeTags.Add(FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Secondary.AttackPower"))));
		UE_LOG(LogTemp, Log, TEXT("  Secondary block - will exclude AttackPower stats"));
	}

	// Remap category tags if needed
	// The actual stat tags are:
	// - Attack Power: SoulslikeFramework.Stat.Secondary.AttackPower.*
	// - Damage Negation: SoulslikeFramework.Stat.Defense.Negation.*
	// - Resistances: SoulslikeFramework.Stat.Defense.Resistances.*
	if (CategoryStr.Contains(TEXT("Secondary.AttackPower")) || CategoryStr.Contains(TEXT("AttackPower")))
	{
		// AttackStats block - match stats under Secondary.AttackPower
		EffectiveCategory = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Secondary.AttackPower")));
		UE_LOG(LogTemp, Log, TEXT("  Remapped category: %s -> %s"), *CategoryStr, *EffectiveCategory.ToString());
	}
	else if (CategoryStr.Contains(TEXT("DamageNegation")) || CategoryStr.Contains(TEXT("Negation")))
	{
		// NegationStats block - match stats under Defense.Negation
		EffectiveCategory = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Defense.Negation")));
		UE_LOG(LogTemp, Log, TEXT("  Remapped category: %s -> %s"), *CategoryStr, *EffectiveCategory.ToString());
	}
	else if (CategoryStr.Contains(TEXT("Resistance")))
	{
		// ResistanceStats block - match stats under Defense.Resistances
		EffectiveCategory = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Defense.Resistances")));
		UE_LOG(LogTemp, Log, TEXT("  Remapped category: %s -> %s"), *CategoryStr, *EffectiveCategory.ToString());
	}

	// For each stat, check if it belongs to this block's category
	// Use the same pattern as working W_StatBlock: check if stat's Tag matches our Category
	for (USLFStatBase* StatObj : StatObjects)
	{
		if (!IsValid(StatObj))
		{
			continue;
		}

		// Get stat's tag from StatInfo
		FGameplayTag StatTag = StatObj->StatInfo.Tag;

		// Check if this stat matches our category using hierarchical tag matching
		// e.g., SoulslikeFramework.Stat.Primary.Vigor matches category SoulslikeFramework.Stat.Primary
		if (EffectiveCategory.IsValid() && !StatTag.MatchesTag(EffectiveCategory))
		{
			// Skip this stat - doesn't match our category
			continue;
		}

		// Check exclusion list - skip stats that match any excluded tag
		bool bExcluded = false;
		for (const FGameplayTag& ExcludeTag : ExcludeTags)
		{
			if (StatTag.MatchesTag(ExcludeTag))
			{
				UE_LOG(LogTemp, Log, TEXT("  Excluding stat %s (matches exclusion %s)"),
					*StatTag.ToString(), *ExcludeTag.ToString());
				bExcluded = true;
				break;
			}
		}
		if (bExcluded)
		{
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("  Including stat: %s (matches category: %s)"),
			*StatTag.ToString(), *Category.ToString());

		// Create stat entry widget
		UW_StatEntry_LevelUp* NewEntry = CreateWidget<UW_StatEntry_LevelUp>(GetOwningPlayer(), StatEntryWidgetClass);
		if (!IsValid(NewEntry))
		{
			UE_LOG(LogTemp, Warning, TEXT("  Failed to create stat entry widget for %s"), *StatTag.ToString());
			continue;
		}

		// Set properties on the entry (these are "ExposeOnSpawn" in Blueprint)
		NewEntry->Stat = StatObj;
		NewEntry->StatCategory = Category;
		NewEntry->ShowAdjustButtons = ShowUpgradeButtons;
		NewEntry->Color = Color;
		NewEntry->SelectedColor = SelectedColor;

		// Add to CachedStatBox (VerticalBox)
		UVerticalBoxSlot* EntrySlot = CachedStatBox->AddChildToVerticalBox(NewEntry);
		if (EntrySlot)
		{
			// Configure slot properties if needed
			EntrySlot->SetPadding(FMargin(0, 2, 0, 2));
		}

		// Add to StatEntries array
		StatEntries.Add(NewEntry);

		// Set the initial/old value BEFORE calling InitStatEntry
		// This is what the left column displays (the value before any level-up changes)
		double CurrentValue = StatObj->StatInfo.bShowMaxValue ? StatObj->StatInfo.MaxValue : StatObj->StatInfo.CurrentValue;
		NewEntry->EventSetInitialValue(CurrentValue);

		// Initialize the stat entry to display stat info
		NewEntry->InitStatEntry();

		UE_LOG(LogTemp, Log, TEXT("  Created entry for stat: %s (OldValue: %.0f)"), *StatTag.ToString(), CurrentValue);
	}

	UE_LOG(LogTemp, Log, TEXT("  Created %d stat entries for category %s"), StatEntries.Num(), *Category.ToString());

	// Signal completion
	EventStatInitializationComplete();
}

TArray<UW_StatEntry_LevelUp*> UW_StatBlock_LevelUp::GetAllStatsInBlock_Implementation()
{
	return StatEntries;
}

void UW_StatBlock_LevelUp::EventStatInitializationComplete_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatBlock_LevelUp::EventStatInitializationComplete - %d entries"), StatEntries.Num());
}
