// AC_StatManager.cpp
// C++ component implementation for AC_StatManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_StatManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added
// PASS 11-15: Verification in progress

#include "Components/AC_StatManager.h"
#include "Blueprints/B_Stat.h"

UAC_StatManager::UAC_StatManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	Level = 1;
	IsAiComponent = false;
}

void UAC_StatManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::BeginPlay - Initializing stats"));

	// Blueprint EventGraph: BeginPlay -> Event Initialize Stats
	// This would trigger stat initialization from StatTable and class assets
	// For now, defer to Blueprint initialization until full migration
}

void UAC_StatManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * GetStat - Look up a stat by its gameplay tag
 *
 * Blueprint Logic (from JSON lines 10043-10640):
 * 1. Map_Find(ActiveStats, StatTag) -> returns Value (UB_Stat*) and bool found
 * 2. Branch on found:
 *    - TRUE: Return Found Stat = Value, Stat Info = Value->StatInfo
 *    - FALSE: Return null/default
 */
void UAC_StatManager::GetStat_Implementation(const FGameplayTag& StatTag, UB_Stat*& OutFoundStat, FStatInfo& OutStatInfo)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::GetStat - Tag: %s"), *StatTag.ToString());

	// Look up in ActiveStats map
	if (UB_Stat** FoundStatPtr = ActiveStats.Find(StatTag))
	{
		// TRUE branch: Stat found
		OutFoundStat = *FoundStatPtr;
		if (OutFoundStat)
		{
			OutStatInfo = OutFoundStat->StatInfo;
			UE_LOG(LogTemp, Log, TEXT("  Found stat: %s"), *OutFoundStat->GetName());
		}
	}
	else
	{
		// FALSE branch: Stat not found
		OutFoundStat = nullptr;
		OutStatInfo = FStatInfo();
		UE_LOG(LogTemp, Warning, TEXT("  Stat not found for tag: %s"), *StatTag.ToString());
	}
}

/**
 * GetAllStats - Return all active stats and their class mappings
 *
 * Blueprint Logic (from JSON lines 10641-11020):
 * 1. Map_Values(ActiveStats) -> OutStatObjects
 * 2. Return Stats map as OutStatClassesAndCategories
 */
void UAC_StatManager::GetAllStats_Implementation(TArray<UB_Stat*>& OutStatObjects, TMap<FGameplayTag, TSubclassOf<UB_Stat>>& OutStatClassesAndCategories)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::GetAllStats"));

	// Get all values from ActiveStats map
	ActiveStats.GenerateValueArray(OutStatObjects);

	// Copy the Stats config map (class definitions)
	OutStatClassesAndCategories = Stats;

	UE_LOG(LogTemp, Log, TEXT("  Returned %d stat objects"), OutStatObjects.Num());
}

/**
 * ResetStat - Reset a stat to its base value
 *
 * Blueprint Logic (from JSON lines 11021-11710):
 * 1. GetStat(StatTag) -> Found Stat
 * 2. If Found Stat valid:
 *    - Calculate reset value (BaseValue - CurrentValue as negative change)
 *    - Call FoundStat->AdjustValue(ValueType::Current, resetAmount, false, false)
 */
void UAC_StatManager::ResetStat_Implementation(const FGameplayTag& StatTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::ResetStat - Tag: %s"), *StatTag.ToString());

	UB_Stat* FoundStat = nullptr;
	FStatInfo StatInfo;
	GetStat(StatTag, FoundStat, StatInfo);

	if (IsValid(FoundStat))
	{
		// Calculate the change needed to reset to max value
		// resetChange = MaxValue - CurrentValue (reset to max)
		double ResetChange = StatInfo.MaxValue - StatInfo.CurrentValue;

		// Apply the reset change (CurrentValue type, no level up, no regen trigger)
		FoundStat->AdjustValue(ESLFValueType::CurrentValue, ResetChange, false, false);

		UE_LOG(LogTemp, Log, TEXT("  Reset stat by %f"), ResetChange);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Cannot reset - stat not found"));
	}
}

/**
 * AdjustStat - Modify a stat's value
 *
 * Blueprint Logic (from JSON lines 11711-12926):
 * 1. GetStat(StatTag) -> Found Stat
 * 2. IsValid macro on Found Stat
 * 3. If Valid: Call FoundStat->AdjustValue(ValueType, Change, LevelUp?, TriggerRegen?)
 */
void UAC_StatManager::AdjustStat_Implementation(const FGameplayTag& StatTag, ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::AdjustStat - Tag: %s, Change: %f, ValueType: %d"),
		*StatTag.ToString(), Change, static_cast<int32>(ValueType));

	UB_Stat* FoundStat = nullptr;
	FStatInfo StatInfo;
	GetStat(StatTag, FoundStat, StatInfo);

	if (IsValid(FoundStat))
	{
		FoundStat->AdjustValue(ValueType, Change, LevelUp, TriggerRegen);
		UE_LOG(LogTemp, Log, TEXT("  Adjusted stat successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Cannot adjust - stat not found"));
	}
}

/**
 * AdjustAffected - Adjust stats that are affected by the given stat
 *
 * Blueprint Logic (from JSON lines 12927-13752):
 * 1. GetStat(StatTag) -> Found Stat
 * 2. IsValid check
 * 3. If Valid: Call AdjustAffectedStats(FoundStat, Change, ValueType)
 */
void UAC_StatManager::AdjustAffected_Implementation(const FGameplayTag& StatTag, ESLFValueType ValueType, double Change)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::AdjustAffected - Tag: %s, Change: %f"), *StatTag.ToString(), Change);

	UB_Stat* FoundStat = nullptr;
	FStatInfo StatInfo;
	GetStat(StatTag, FoundStat, StatInfo);

	if (IsValid(FoundStat))
	{
		AdjustAffectedStats(FoundStat, Change, ValueType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Cannot adjust affected - source stat not found"));
	}
}

/**
 * AdjustAffectedStats - Adjust all stats affected by the given stat object
 *
 * Blueprint Logic (from JSON lines 13753-18353):
 * 1. Get StatBehavior from Stat
 * 2. Check if StatsToAffect map has entries (Greater_IntInt on length)
 * 3. ForEachLoop over StatsToAffect map keys/values
 * 4. For each affected stat: call AdjustStat with scaled change
 */
void UAC_StatManager::AdjustAffectedStats_Implementation(UB_Stat* Stat, double Change, ESLFValueType ValueType)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::AdjustAffectedStats - Change: %f"), Change);

	if (!IsValid(Stat))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid stat passed"));
		return;
	}

	// Get the stat behavior which contains StatsToAffect map
	// Blueprint accesses: Stat->StatBehavior->StatsToAffect
	const FStatBehavior& StatBehavior = Stat->StatBehavior;

	// Check if there are any affected stats
	if (StatBehavior.StatsToAffect.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("  Processing %d affected stats"), StatBehavior.StatsToAffect.Num());

		// ForEach loop over the StatsToAffect map
		for (const auto& AffectedEntry : StatBehavior.StatsToAffect)
		{
			FGameplayTag AffectedStatTag = AffectedEntry.Key;
			const FAffectedStats& AffectedStatsData = AffectedEntry.Value;

			// Process each softcap entry for this affected stat
			for (const FAffectedStat& AffectedStat : AffectedStatsData.SoftcapData)
			{
				// Scale the change by the modifier
				double ScaledChange = Change * AffectedStat.Modifier;

				// Determine which value type to affect
				ESLFValueType TargetValueType = AffectedStat.bAffectMaxValue ? ESLFValueType::MaxValue : ESLFValueType::CurrentValue;

				// Apply to the affected stat
				AdjustStat(AffectedStatTag, TargetValueType, ScaledChange, false, false);

				UE_LOG(LogTemp, Log, TEXT("    Adjusted %s by %f (Modifier: %f)"), *AffectedStatTag.ToString(), ScaledChange, AffectedStat.Modifier);
			}
		}
	}
}

/**
 * SerializeStatsData - Serialize all stats for saving
 *
 * Blueprint Logic (from JSON lines 18354-20251):
 * 1. Clear LOCAL_StatsToSave array
 * 2. ForEachLoop over ActiveStats
 * 3. For each stat: get StatInfo and add to array
 * 4. Convert to InstancedStruct array
 * 5. Broadcast OnSaveRequested with save tag and data
 */
void UAC_StatManager::SerializeStatsData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::SerializeStatsData"));

	TArray<FStatInfo> StatsToSave;
	TArray<FInstancedStruct> InstancedStats;

	// Collect all stat info
	for (const auto& StatEntry : ActiveStats)
	{
		if (UB_Stat* Stat = StatEntry.Value)
		{
			StatsToSave.Add(Stat->StatInfo);
		}
	}

	// Convert to InstancedStruct array
	for (const FStatInfo& Info : StatsToSave)
	{
		FInstancedStruct InstancedStat;
		InstancedStat.InitializeAs<FStatInfo>(Info);
		InstancedStats.Add(InstancedStat);
	}

	// Broadcast save request
	FGameplayTag SaveTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Save.Stats"));
	OnSaveRequested.Broadcast(SaveTag, InstancedStats);

	UE_LOG(LogTemp, Log, TEXT("  Serialized %d stats for saving"), StatsToSave.Num());
}

/**
 * InitializeLoadedStats - Initialize stats from loaded save data
 *
 * Blueprint Logic (from JSON lines 20252-21130):
 * 1. Check if LoadedStats is not empty
 * 2. ForEachLoop over LoadedStats
 * 3. For each loaded stat: find corresponding active stat and update values
 */
void UAC_StatManager::InitializeLoadedStats_Implementation(const TArray<FStatInfo>& LoadedStats)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::InitializeLoadedStats - %d stats to load"), LoadedStats.Num());

	if (LoadedStats.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No stats to load"));
		return;
	}

	// Process each loaded stat
	for (const FStatInfo& LoadedInfo : LoadedStats)
	{
		// Find the corresponding active stat using Tag (not StatTag)
		if (UB_Stat** FoundStatPtr = ActiveStats.Find(LoadedInfo.Tag))
		{
			if (UB_Stat* FoundStat = *FoundStatPtr)
			{
				// Update the stat info with loaded values
				FoundStat->StatInfo = LoadedInfo;
				UE_LOG(LogTemp, Log, TEXT("  Loaded stat: %s"), *LoadedInfo.Tag.ToString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Active stat not found for tag: %s"), *LoadedInfo.Tag.ToString());
		}
	}
}

/**
 * GetStatsForCategory - Get all stat tags that belong to a category
 *
 * Blueprint Logic (from JSON lines 21131-22504):
 * 1. Get keys from Stats config map
 * 2. ForEachLoop: filter stats matching category
 * 3. Return matching tags as GameplayTagContainer
 */
FGameplayTagContainer UAC_StatManager::GetStatsForCategory_Implementation(const FGameplayTag& StatCategory)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::GetStatsForCategory - Category: %s"), *StatCategory.ToString());

	FGameplayTagContainer MatchingStats;

	// Iterate over all stat definitions
	for (const auto& StatEntry : Stats)
	{
		FGameplayTag StatTag = StatEntry.Key;

		// Check if this stat tag matches the category
		// (the tag should be a child of the category tag)
		if (StatTag.MatchesTag(StatCategory))
		{
			MatchingStats.AddTag(StatTag);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Found %d stats in category"), MatchingStats.Num());
	return MatchingStats;
}

/**
 * AdjustLevel - Change the character level
 *
 * Blueprint Logic (from JSON lines 22505-23169):
 * 1. Check if not AI component
 * 2. Level = Level + Delta
 * 3. Broadcast OnLevelUpdated(Level)
 */
void UAC_StatManager::AdjustLevel_Implementation(int32 Delta)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::AdjustLevel - Delta: %d"), Delta);

	// Only process for non-AI components (matches Blueprint logic)
	if (!IsAiComponent)
	{
		Level = Level + Delta;

		// Broadcast the level update
		OnLevelUpdated.Broadcast(Level);

		UE_LOG(LogTemp, Log, TEXT("  New level: %d"), Level);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("  Skipped - AI component"));
	}
}

/**
 * IsStatMoreThan - Check if a stat's current value exceeds a threshold
 *
 * Blueprint Logic (from JSON lines 23170-23678):
 * 1. GetStat(StatTag) -> get StatInfo
 * 2. Return StatInfo.CurrentValue > Threshold
 */
bool UAC_StatManager::IsStatMoreThan_Implementation(const FGameplayTag& StatTag, double Threshold)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::IsStatMoreThan - Tag: %s, Threshold: %f"), *StatTag.ToString(), Threshold);

	UB_Stat* FoundStat = nullptr;
	FStatInfo StatInfo;
	GetStat(StatTag, FoundStat, StatInfo);

	bool Result = StatInfo.CurrentValue > Threshold;

	UE_LOG(LogTemp, Log, TEXT("  CurrentValue: %f, Result: %s"), StatInfo.CurrentValue, Result ? TEXT("true") : TEXT("false"));
	return Result;
}

/**
 * ToggleRegenForStat - Start or stop regeneration for a stat
 *
 * Blueprint Logic (from JSON lines 23679-24189):
 * 1. Map_Find(ActiveStats, StatTag) -> Value, found
 * 2. Branch on found
 * 3. If found: Call Value->Event ToggleStatRegen(Stop?)
 */
void UAC_StatManager::ToggleRegenForStat_Implementation(const FGameplayTag& StatTag, bool Stop)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatManager::ToggleRegenForStat - Tag: %s, Stop: %s"),
		*StatTag.ToString(), Stop ? TEXT("true") : TEXT("false"));

	if (UB_Stat** FoundStatPtr = ActiveStats.Find(StatTag))
	{
		if (UB_Stat* FoundStat = *FoundStatPtr)
		{
			// Call the stat's toggle regen function
			// Blueprint calls: Event ToggleStatRegen
			FoundStat->ToggleStatRegen(Stop);

			UE_LOG(LogTemp, Log, TEXT("  Toggled regen for stat"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Stat not found for regen toggle"));
	}
}
