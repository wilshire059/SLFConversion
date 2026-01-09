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
#include "UObject/ConstructorHelpers.h"
#include "Engine/DataTable.h"

UAC_StatManager::UAC_StatManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	Level = 1;
	IsAiComponent = false;

	// Load default StatTable - critical for stats to work
	static ConstructorHelpers::FObjectFinder<UDataTable> DefaultStatTableFinder(
		TEXT("/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable.DT_ExampleStatTable"));
	if (DefaultStatTableFinder.Succeeded())
	{
		StatTable = DefaultStatTableFinder.Object;
	}
	else
	{
		StatTable = nullptr;
	}
}

void UAC_StatManager::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] BeginPlay on %s (AI: %s)"),
		Owner ? *Owner->GetName() : TEXT("NULL OWNER"), IsAiComponent ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Component: %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Class: %s"), *GetClass()->GetName());

	// If StatTable is not set, load it at runtime
	// This handles Blueprint components that don't inherit C++ constructor defaults
	if (!StatTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] StatTable is NULL, loading at runtime..."));
		UObject* LoadedObject = StaticLoadObject(
			UDataTable::StaticClass(), nullptr,
			TEXT("/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable.DT_ExampleStatTable"));

		if (LoadedObject)
		{
			StatTable = Cast<UDataTable>(LoadedObject);
			UE_LOG(LogTemp, Warning, TEXT("[StatManager] Runtime loaded StatTable: %s"), StatTable ? *StatTable->GetName() : TEXT("CAST FAILED"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[StatManager] FAILED to load StatTable at runtime!"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[StatManager] StatTable: %s"), StatTable ? *StatTable->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] ActiveStats count: %d"), ActiveStats.Num());

	// Initialize stats from DataTable - this populates Stats and ActiveStats maps
	// Then broadcasts OnStatsInitialized
	EventInitializeStats();
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
void UAC_StatManager::GetAllStats_Implementation(TArray<UB_Stat*>& OutStatObjects, TMap<TSubclassOf<UB_Stat>, FGameplayTag>& OutStatClassesAndCategories)
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
	for (const auto& StatEntry : ActiveStats)
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

/**
 * EventInitializeStats - Initialize stats from DataTable
 *
 * Blueprint Logic (from JSON):
 * 1. Clear Stats map
 * 2. GetDataTableRowNames(StatTable) -> get all row names
 * 3. ForEachLoop over row names:
 *    - GetDataTableRow<FSLFStatEntry>(StatTable, RowName)
 *    - Add to Stats map: Key=StatObject (class), Value=ParentCategory (tag)
 * 4. Clear ActiveStats map
 * 5. ForEachLoop over Stats.Keys() (stat classes):
 *    - ConstructObject from Class (create UB_Stat instance)
 *    - Get StatInfo.Tag from the new stat
 *    - Add to ActiveStats: Key=Tag, Value=StatInstance
 * 6. Broadcast OnStatsInitialized
 */
void UAC_StatManager::EventInitializeStats_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] EventInitializeStats called"));

	// Step 1: Clear Stats map (will be populated from DataTable)
	Stats.Empty();

	// Step 2-3: Populate Stats map from DataTable
	if (StatTable)
	{
		TArray<FName> RowNames = StatTable->GetRowNames();
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] Found %d rows in StatTable"), RowNames.Num());

		for (const FName& RowName : RowNames)
		{
			// Get the row as FSLFStatEntry
			FSLFStatEntry* RowData = StatTable->FindRow<FSLFStatEntry>(RowName, TEXT("EventInitializeStats"));
			if (RowData)
			{
				// Add to Stats map: Key=StatObject (class), Value=ParentCategory (tag)
				if (RowData->StatObject)
				{
					TSubclassOf<UB_Stat> StatClass = TSubclassOf<UB_Stat>(RowData->StatObject);
					if (StatClass)
					{
						Stats.Add(StatClass, RowData->ParentCategory);
						UE_LOG(LogTemp, Log, TEXT("[StatManager] Added stat class: %s with category: %s"),
							*RowData->StatObject->GetName(), *RowData->ParentCategory.ToString());
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[StatManager] Row %s has NULL StatObject"), *RowName.ToString());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[StatManager] Failed to find row: %s"), *RowName.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[StatManager] StatTable is NULL - cannot initialize stats!"));
	}

	// Step 4: Clear ActiveStats map
	ActiveStats.Empty();

	// Step 5: Create stat instances from Stats map classes
	AActor* Owner = GetOwner();
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Creating %d stat instances"), Stats.Num());

	for (const auto& StatEntry : Stats)
	{
		TSubclassOf<UB_Stat> StatClass = StatEntry.Key;
		const FGameplayTag& ParentCategory = StatEntry.Value;

		if (StatClass)
		{
			// Construct the stat object with Owner as outer
			UB_Stat* NewStat = NewObject<UB_Stat>(Owner, StatClass);
			if (NewStat)
			{
				// FIX: Blueprint CDO overrides C++ constructor defaults
				// After reparenting, Blueprint CDO has serialized struct values that override
				// C++ constructor defaults. We MUST copy from C++ parent for Blueprint classes.
				// Check if this is a Blueprint class (compiled from Blueprint)
				bool bIsBlueprintClass = StatClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint);
				
				if (bIsBlueprintClass)
				{
					// Walk up the class hierarchy to find native C++ parent with valid defaults
					UClass* ParentClass = StatClass->GetSuperClass();
					while (ParentClass)
					{
						// Check if this is a native C++ class (not compiled from Blueprint)
						if (!ParentClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
						{
							UB_Stat* ParentCDO = Cast<UB_Stat>(ParentClass->GetDefaultObject());
							if (ParentCDO && ParentCDO->StatInfo.Tag.IsValid())
							{
								// Copy FULL struct from C++ parent (preserves all constructor defaults)
								NewStat->StatInfo = ParentCDO->StatInfo;
								UE_LOG(LogTemp, Warning, TEXT("[StatManager] Blueprint class %s: copied StatInfo from C++ parent %s (Curr: %f, Max: %f)"),
									*StatClass->GetName(), *ParentClass->GetName(), 
									NewStat->StatInfo.CurrentValue, NewStat->StatInfo.MaxValue);
								break;
							}
						}
						ParentClass = ParentClass->GetSuperClass();
					}
				}

				// Get the stat's tag from its StatInfo
				FGameplayTag StatTag = NewStat->StatInfo.Tag;

				if (StatTag.IsValid())
				{
					// Add to ActiveStats: Key=Tag, Value=StatInstance
					ActiveStats.Add(StatTag, NewStat);
					UE_LOG(LogTemp, Log, TEXT("[StatManager] Created stat instance: %s (Tag: %s, Curr: %f, Max: %f)"),
						*NewStat->GetName(), *StatTag.ToString(), NewStat->StatInfo.CurrentValue, NewStat->StatInfo.MaxValue);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[StatManager] Stat %s has invalid tag even after parent lookup"), *NewStat->GetName());
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Initialized %d active stats"), ActiveStats.Num());

	// Step 6: Broadcast OnStatsInitialized
	OnStatsInitialized.Broadcast();
}
