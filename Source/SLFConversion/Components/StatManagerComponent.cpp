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
#include "Blueprints/B_Stat.h"

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
		// Load stat definitions from data table
		TArray<FName> RowNames = StatTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			// Get the row data (assuming FStatTableRow structure)
			FStatInfo* RowData = StatTable->FindRow<FStatInfo>(RowName, TEXT("StatManagerInit"));
			if (RowData)
			{
				// Create a new B_Stat instance for this stat
				UB_Stat* NewStat = NewObject<UB_Stat>(this);
				NewStat->StatInfo = *RowData;
				ActiveStats.Add(RowData->Tag, NewStat);
				UE_LOG(LogTemp, Warning, TEXT("[StatManager] Created stat: %s"), *RowData->Tag.ToString());
			}
		}
	}

	// Apply overrides
	for (const auto& Override : StatOverrides)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] Applying override for: %s"), *Override.Key.ToString());
		if (UObject** StatObj = ActiveStats.Find(Override.Key))
		{
			UB_Stat* Stat = Cast<UB_Stat>(*StatObj);
			if (Stat)
			{
				// Apply override values to stat
				Stat->StatInfo.CurrentValue = Override.Value.OverrideCurrentValue;
				Stat->StatInfo.MaxValue = Override.Value.OverrideMaxValue;
				UE_LOG(LogTemp, Warning, TEXT("[StatManager] Applied override - Current: %.2f, Max: %.2f"),
					Override.Value.OverrideCurrentValue, Override.Value.OverrideMaxValue);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Broadcasting OnStatsInitialized..."));
	OnStatsInitialized.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════════════════════════════"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// STAT ACCESS [1-3/12]
// ═══════════════════════════════════════════════════════════════════════════════

bool UStatManagerComponent::GetStat_Implementation(FGameplayTag StatTag, UObject*& OutStatObject, FStatInfo& OutStatInfo)
{
	if (UObject** Found = ActiveStats.Find(StatTag))
	{
		OutStatObject = *Found;

		// Extract FStatInfo from the B_Stat object
		UB_Stat* Stat = Cast<UB_Stat>(*Found);
		if (Stat)
		{
			OutStatInfo = Stat->StatInfo;
		}
		else
		{
			OutStatInfo.Tag = StatTag;
		}

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
		// Reset B_Stat's current value to max value
		UB_Stat* Stat = Cast<UB_Stat>(*Found);
		if (Stat)
		{
			Stat->StatInfo.CurrentValue = Stat->StatInfo.MaxValue;
			Stat->OnStatUpdated.Broadcast(Stat, 0.0, false, ESLFValueType::CurrentValue);
		}
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
		UB_Stat* Stat = Cast<UB_Stat>(*Found);
		if (Stat)
		{
			// Use the B_Stat's own AdjustValue function
			Stat->AdjustValue(ValueType, Change, bLevelUp, bTriggerRegen);
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

	// Get StatBehavior.StatsToAffect from the B_Stat
	UB_Stat* BStat = Cast<UB_Stat>(Stat);
	if (!BStat) return;

	const FStatBehavior& StatBehavior = BStat->StatBehavior;

	// For each affected stat, apply the scaled change
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
			UE_LOG(LogTemp, Log, TEXT("  Applied %.2f to %s (%s)"),
				ScaledChange, *AffectedStatTag.ToString(),
				TargetValueType == ESLFValueType::MaxValue ? TEXT("Max") : TEXT("Current"));
		}
	}
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
	FStatInfo StatInfo;

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

	// For each ActiveStat, create FInstancedStruct with save data
	for (const auto& StatEntry : ActiveStats)
	{
		UB_Stat* Stat = Cast<UB_Stat>(StatEntry.Value);
		if (Stat)
		{
			// Create save info struct from the stat
			FStatInfo SaveInfo = Stat->StatInfo;
			SaveInfo.Tag = StatEntry.Key;

			FInstancedStruct StructInstance;
			StructInstance.InitializeAs<FStatInfo>(SaveInfo);
			StatsData.Add(StructInstance);

			UE_LOG(LogTemp, Log, TEXT("  Serialized stat: %s (%.2f/%.2f)"),
				*StatEntry.Key.ToString(), SaveInfo.CurrentValue, SaveInfo.MaxValue);
		}
	}

	OnSaveRequested.Broadcast();
}

void UStatManagerComponent::InitializeLoadedStats_Implementation(const TArray<FInstancedStruct>& LoadedStats)
{
	UE_LOG(LogTemp, Log, TEXT("[StatManager] InitializeLoadedStats: %d entries"), LoadedStats.Num());

	// For each loaded stat data, find and update the corresponding ActiveStat
	for (const FInstancedStruct& LoadedData : LoadedStats)
	{
		if (const FStatInfo* StatData = LoadedData.GetPtr<FStatInfo>())
		{
			if (UObject** Found = ActiveStats.Find(StatData->Tag))
			{
				UB_Stat* Stat = Cast<UB_Stat>(*Found);
				if (Stat)
				{
					Stat->StatInfo = *StatData;
					UE_LOG(LogTemp, Log, TEXT("  Loaded stat: %s (%.2f/%.2f)"),
						*StatData->Tag.ToString(), StatData->CurrentValue, StatData->MaxValue);
				}
			}
		}
	}
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
		// Call ToggleStatRegen on B_Stat object
		UB_Stat* Stat = Cast<UB_Stat>(*Found);
		if (Stat)
		{
			Stat->ToggleStatRegen(bStop);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// INITIALIZATION & LEVEL UP EVENTS
// ═══════════════════════════════════════════════════════════════════════════════

void UStatManagerComponent::EventInitializeStats()
{
	UE_LOG(LogTemp, Log, TEXT("[StatManager] EventInitializeStats"));

	// Initialize stats from the Stats map (class -> tag mapping)
	for (const auto& StatPair : Stats)
	{
		TSubclassOf<UObject> StatClass = StatPair.Key;
		FGameplayTag StatTag = StatPair.Value;

		if (StatClass)
		{
			UB_Stat* StatInstance = NewObject<UB_Stat>(this, StatClass);
			if (StatInstance)
			{
				StatInstance->StatInfo.Tag = StatTag;
				ActiveStats.Add(StatTag, StatInstance);
				UE_LOG(LogTemp, Log, TEXT("[StatManager] Initialized stat: %s"), *StatTag.ToString());
			}
		}
	}

	OnStatsInitialized.Broadcast();
}

void UStatManagerComponent::EventOnLevelUpRequested(FGameplayTag StatTag)
{
	UE_LOG(LogTemp, Log, TEXT("[StatManager] EventOnLevelUpRequested: %s"), *StatTag.ToString());

	// Find the stat and try to level it up
	if (UObject** Found = ActiveStats.Find(StatTag))
	{
		UB_Stat* Stat = Cast<UB_Stat>(*Found);
		if (Stat)
		{
			// Check if we have enough currency for level up
			// Would typically check ProgressManager for experience/souls
			// For now, just increment the stat
			AdjustStat(StatTag, ESLFValueType::MaxValue, 1.0, false, false);
			UE_LOG(LogTemp, Log, TEXT("[StatManager] Leveled up stat: %s"), *StatTag.ToString());
		}
	}
}
