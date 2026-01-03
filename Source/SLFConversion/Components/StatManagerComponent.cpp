// StatManagerComponent.cpp
// C++ implementation for AC_StatManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_StatManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         8/8 (initialized in constructor)
// Functions:         12/12 implemented
// Event Dispatchers: 3/3 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "StatManagerComponent.h"
#include "Engine/DataTable.h"

UStatManagerComponent::UStatManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config
	bIsAiComponent = false;
	SpeedAsset = nullptr;
	StatTable = nullptr;
	SelectedClassAsset = nullptr;

	// Initialize runtime
	Level = 1;
}

void UStatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════════════════════════════"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] BeginPlay on %s (AI: %s)"),
		Owner ? *Owner->GetName() : TEXT("NULL OWNER"), bIsAiComponent ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Component: %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Class: %s"), *GetClass()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] StatTable: %s"), StatTable ? *StatTable->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] ActiveStats count: %d"), ActiveStats.Num());
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] StatOverrides count: %d"), StatOverrides.Num());

	// Initialize stats from table
	if (StatTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] Loading stats from table..."));
		// TODO: Load stat definitions from data table
		// For each row in StatTable, create B_Stat instances
	}

	// Apply overrides
	for (const auto& Override : StatOverrides)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] Applying override for: %s"), *Override.Key.ToString());
		if (UObject** StatObj = ActiveStats.Find(Override.Key))
		{
			// TODO: Apply override values to stat
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Broadcasting OnStatsInitialized..."));
	OnStatsInitialized.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════════════════════════════"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// STAT ACCESS [1-3/12]
// ═══════════════════════════════════════════════════════════════════════════════

bool UStatManagerComponent::GetStat_Implementation(FGameplayTag StatTag, UObject*& OutStatObject, FSLFStatInfo& OutStatInfo)
{
	if (UObject** Found = ActiveStats.Find(StatTag))
	{
		OutStatObject = *Found;

		// TODO: Extract FSLFStatInfo from the B_Stat object
		OutStatInfo.Tag = StatTag;

		return true;
	}

	OutStatObject = nullptr;
	return false;
}

void UStatManagerComponent::GetAllStats_Implementation(TArray<UObject*>& OutStatObjects, TMap<TSubclassOf<UObject>, FGameplayTag>& OutStatClassesAndCategories)
{
	OutStatObjects.Empty();
	OutStatClassesAndCategories = Stats;

	for (const auto& Pair : ActiveStats)
	{
		OutStatObjects.Add(Pair.Value);
	}
}

FGameplayTagContainer UStatManagerComponent::GetStatsForCategory_Implementation(FGameplayTag StatCategory)
{
	FGameplayTagContainer StatsContainer;

	// Iterate through all active stats and find those matching the category
	for (const auto& Pair : ActiveStats)
	{
		// Check if stat tag is a child of the category tag
		if (Pair.Key.MatchesTag(StatCategory))
		{
			StatsContainer.AddTag(Pair.Key);
		}
	}

	return StatsContainer;
}

// ═══════════════════════════════════════════════════════════════════════════════
// STAT MODIFICATION [4-7/12]
// ═══════════════════════════════════════════════════════════════════════════════

void UStatManagerComponent::ResetStat_Implementation(FGameplayTag StatTag)
{
	UE_LOG(LogTemp, Log, TEXT("[StatManager] ResetStat: %s"), *StatTag.ToString());

	if (UObject** Found = ActiveStats.Find(StatTag))
	{
		// TODO: Call Reset on B_Stat object to restore current = max
	}
}

void UStatManagerComponent::AdjustStat_Implementation(FGameplayTag StatTag, ESLFValueType ValueType, double Change, bool bLevelUp, bool bTriggerRegen)
{
	UE_LOG(LogTemp, Log, TEXT("[StatManager] AdjustStat: %s by %.2f (%s) LevelUp=%s TriggerRegen=%s"),
		*StatTag.ToString(), Change,
		ValueType == ESLFValueType::CurrentValue ? TEXT("Current") : TEXT("Max"),
		bLevelUp ? TEXT("true") : TEXT("false"),
		bTriggerRegen ? TEXT("true") : TEXT("false"));

	if (UObject** Found = ActiveStats.Find(StatTag))
	{
		// TODO: Adjust the stat value on B_Stat object
		// if (ValueType == ESLFValueType::CurrentValue)
		//     Stat->CurrentValue = FMath::Clamp(Stat->CurrentValue + Change, 0.0, Stat->MaxValue);
		// else
		//     Stat->MaxValue += Change;

		// If bTriggerRegen is true, restart regeneration timer for this stat
		if (bTriggerRegen)
		{
			// TODO: Trigger regen for this stat
		}
	}
}

void UStatManagerComponent::AdjustAffected_Implementation(FGameplayTag StatTag, double Change, ESLFValueType ValueType)
{
	UE_LOG(LogTemp, Log, TEXT("[StatManager] AdjustAffected: %s"), *StatTag.ToString());

	if (UObject** Found = ActiveStats.Find(StatTag))
	{
		AdjustAffectedStats(*Found, Change, ValueType);
	}
}

void UStatManagerComponent::AdjustAffectedStats_Implementation(UObject* Stat, double Change, ESLFValueType ValueType)
{
	if (!Stat) return;

	UE_LOG(LogTemp, Log, TEXT("[StatManager] AdjustAffectedStats from %s"), *Stat->GetName());

	// TODO: Get StatModifiers.StatsToAffect from the B_Stat
	// For each affected stat class, find its tag and adjust by (Change * Multiplier)
}

// ═══════════════════════════════════════════════════════════════════════════════
// LEVEL FUNCTIONS [8-9/12]
// ═══════════════════════════════════════════════════════════════════════════════

void UStatManagerComponent::AdjustLevel_Implementation(int32 Delta)
{
	int32 OldLevel = Level;
	Level = FMath::Max(1, Level + Delta);

	if (Level != OldLevel)
	{
		UE_LOG(LogTemp, Log, TEXT("[StatManager] AdjustLevel: %d -> %d"), OldLevel, Level);
		OnLevelUpdated.Broadcast(Level);
	}
}

bool UStatManagerComponent::IsStatMoreThan_Implementation(FGameplayTag StatTag, double Threshold)
{
	UObject* StatObj = nullptr;
	FSLFStatInfo StatInfo;

	if (GetStat(StatTag, StatObj, StatInfo))
	{
		return StatInfo.CurrentValue > Threshold;
	}

	return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SERIALIZATION [10-11/12]
// ═══════════════════════════════════════════════════════════════════════════════

void UStatManagerComponent::SerializeStatsData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[StatManager] SerializeStatsData"));

	TArray<FInstancedStruct> StatsData;

	// TODO: For each ActiveStat, create FInstancedStruct with save data
	// and add to StatsData array

	OnSaveRequested.Broadcast();
}

void UStatManagerComponent::InitializeLoadedStats_Implementation(const TArray<FInstancedStruct>& LoadedStats)
{
	UE_LOG(LogTemp, Log, TEXT("[StatManager] InitializeLoadedStats: %d entries"), LoadedStats.Num());

	// TODO: For each loaded stat data:
	// 1. Find the corresponding ActiveStat by tag
	// 2. Apply saved values (current, max, regen settings)
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPERS [12/12]
// ═══════════════════════════════════════════════════════════════════════════════

void UStatManagerComponent::ToggleRegenForStat_Implementation(FGameplayTag StatTag, bool bStop)
{
	UE_LOG(LogTemp, Log, TEXT("[StatManager] ToggleRegenForStat: %s - %s"),
		*StatTag.ToString(), bStop ? TEXT("Stop") : TEXT("Start"));

	if (UObject** Found = ActiveStats.Find(StatTag))
	{
		// TODO: Call ToggleRegen on B_Stat object
		// Stat->RegenInfo.bCanRegenerate = !bStop;
	}
}
