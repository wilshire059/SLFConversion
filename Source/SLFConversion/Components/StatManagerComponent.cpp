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
#include "Engine/GameInstance.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFPrimaryDataAssets.h"
#include "Interfaces/BPI_GameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h"  // For TFieldIterator, FProperty, FClassProperty

UStatManagerComponent::UStatManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config
	bIsAiComponent = false;
	SpeedAsset = nullptr;
	SelectedClassAsset = nullptr;

	// Load default StatTable if not set by Blueprint
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

	// Initialize runtime - Level starts at 1 (matches bp_only default)
	Level = 1;
}

void UStatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Error, TEXT("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	UE_LOG(LogTemp, Error, TEXT("!!!!! UStatManagerComponent::BeginPlay() CALLED !!!!!"));
	UE_LOG(LogTemp, Error, TEXT("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] >>> UStatManagerComponent::BeginPlay() called"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] >>> StatTable before load: %s"), StatTable ? *StatTable->GetName() : TEXT("NULL"));

	// If StatTable is not set, load it at runtime
	// This handles Blueprint components that don't inherit C++ constructor defaults
	if (!StatTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] >>> Attempting runtime load of StatTable..."));
		UObject* LoadedObject = StaticLoadObject(
			UDataTable::StaticClass(), nullptr,
			TEXT("/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable.DT_ExampleStatTable"));
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] >>> LoadedObject: %s"), LoadedObject ? *LoadedObject->GetName() : TEXT("NULL"));

		if (LoadedObject)
		{
			StatTable = Cast<UDataTable>(LoadedObject);
			UE_LOG(LogTemp, Warning, TEXT("[StatManager] >>> Cast to UDataTable: %s"), StatTable ? *StatTable->GetName() : TEXT("NULL/CAST FAILED"));
		}
	}

	AActor* Owner = GetOwner();
	UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════════════════════════════"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] BeginPlay on %s (AI: %s)"),
		Owner ? *Owner->GetName() : TEXT("NULL OWNER"), bIsAiComponent ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Component: %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Class: %s"), *GetClass()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] StatTable: %s"), StatTable ? *StatTable->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] ActiveStats count: %d"), ActiveStats.Num());
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] StatOverrides count: %d"), StatOverrides.Num());

	// Initialize stats from table using generic row access
	// (DataTable uses Blueprint struct, so we use reflection to read StatObject property)
	if (StatTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] Loading stats from table using reflection..."));

		const UScriptStruct* RowStruct = StatTable->GetRowStruct();
		if (!RowStruct)
		{
			UE_LOG(LogTemp, Error, TEXT("[StatManager] DataTable has no RowStruct!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[StatManager] RowStruct: %s"), *RowStruct->GetName());

			// Find StatObject property in the row struct
			FProperty* StatObjectProp = RowStruct->FindPropertyByName(TEXT("StatObject"));
			if (!StatObjectProp)
			{
				// Try with Blueprint naming convention (may have GUID suffix)
				for (TFieldIterator<FProperty> It(RowStruct); It; ++It)
				{
					if (It->GetName().StartsWith(TEXT("StatObject")))
					{
						StatObjectProp = *It;
						UE_LOG(LogTemp, Warning, TEXT("[StatManager] Found property: %s"), *It->GetName());
						break;
					}
				}
			}

			if (StatObjectProp)
			{
				// Iterate over all rows using raw row map
				const TMap<FName, uint8*>& RowMap = StatTable->GetRowMap();
				for (const auto& Pair : RowMap)
				{
					const FName& RowName = Pair.Key;
					uint8* RowData = Pair.Value;

					// Get StatObject value using property accessor
					UClass* StatClass = nullptr;
					if (FClassProperty* ClassProp = CastField<FClassProperty>(StatObjectProp))
					{
						StatClass = Cast<UClass>(ClassProp->GetObjectPropertyValue(StatObjectProp->ContainerPtrToValuePtr<void>(RowData)));
					}
					else if (FSoftClassProperty* SoftClassProp = CastField<FSoftClassProperty>(StatObjectProp))
					{
						FSoftObjectPtr SoftPtr = SoftClassProp->GetPropertyValue(StatObjectProp->ContainerPtrToValuePtr<void>(RowData));
						StatClass = Cast<UClass>(SoftPtr.LoadSynchronous());
					}
					else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(StatObjectProp))
					{
						StatClass = Cast<UClass>(ObjProp->GetObjectPropertyValue(StatObjectProp->ContainerPtrToValuePtr<void>(RowData)));
					}

					if (StatClass)
					{
						UE_LOG(LogTemp, Warning, TEXT("[StatManager] Row %s: Got StatClass %s"), *RowName.ToString(), *StatClass->GetName());

						// Validate that StatClass is a USLFStatBase subclass
						if (!StatClass->IsChildOf(USLFStatBase::StaticClass()))
						{
							UE_LOG(LogTemp, Warning, TEXT("[StatManager] Row %s: StatClass %s is not a USLFStatBase subclass!"),
								*RowName.ToString(), *StatClass->GetName());
							continue;
						}

						// Create an instance of the stat class
						USLFStatBase* NewStat = NewObject<USLFStatBase>(this, StatClass);
						if (NewStat)
						{
							// Get stat tag from its default StatInfo
							FGameplayTag StatTag = NewStat->StatInfo.Tag;

							// ALWAYS copy from C++ parent when StatClass is a Blueprint class
							// Blueprint CDOs may have serialized incorrect values from before migration
							// The C++ parent classes (USLFStatHP, USLFStatVigor, etc.) have the authoritative defaults
							bool bIsBlueprintClass = StatClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint);

							if (bIsBlueprintClass)
							{
								UClass* ParentClass = StatClass->GetSuperClass();
								while (ParentClass)
								{
									// Check if this is a native C++ class (not Blueprint)
									if (!ParentClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
									{
										if (USLFStatBase* ParentCDO = Cast<USLFStatBase>(ParentClass->GetDefaultObject()))
										{
											if (ParentCDO->StatInfo.Tag.IsValid())
											{
												// Copy the FULL StatInfo from C++ parent (not just tag)
												NewStat->StatInfo = ParentCDO->StatInfo;
												NewStat->MinValue = ParentCDO->MinValue;
												NewStat->bOnlyMaxValueRelevant = ParentCDO->bOnlyMaxValueRelevant;
												NewStat->bShowMaxValueOnLevelUp = ParentCDO->bShowMaxValueOnLevelUp;
												NewStat->StatBehavior = ParentCDO->StatBehavior;
												StatTag = NewStat->StatInfo.Tag;
												UE_LOG(LogTemp, Warning, TEXT("[StatManager] Got StatInfo from C++ parent %s (Tag: %s, Value: %.0f/%.0f)"),
													*ParentClass->GetName(), *StatTag.ToString(),
													NewStat->StatInfo.CurrentValue, NewStat->StatInfo.MaxValue);
												break;
											}
										}
									}
									ParentClass = ParentClass->GetSuperClass();
								}
							}

							if (StatTag.IsValid())
							{
								ActiveStats.Add(StatTag, NewStat);
								UE_LOG(LogTemp, Warning, TEXT("[StatManager] Created stat: %s (Tag: %s, Value: %.0f/%.0f)"),
									*NewStat->GetName(), *StatTag.ToString(),
									NewStat->StatInfo.CurrentValue, NewStat->StatInfo.MaxValue);
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("[StatManager] Stat %s has invalid tag (no C++ parent tag found)"), *NewStat->GetName());
							}
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[StatManager] Row %s: StatObject is NULL or wrong type"), *RowName.ToString());
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[StatManager] Could not find StatObject property in RowStruct!"));
			}
		}
	}

	// Apply overrides
	for (const auto& Override : StatOverrides)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] Applying override for: %s"), *Override.Key.ToString());
		if (UObject** StatObj = ActiveStats.Find(Override.Key))
		{
			USLFStatBase* Stat = Cast<USLFStatBase>(*StatObj);
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

	// Apply base stats from character class (e.g., Vigor=10 for Warrior class)
	// This must happen after stats are created but before they're used
	ApplyBaseStatsFromCharacterClass();

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
		USLFStatBase* Stat = Cast<USLFStatBase>(*Found);
		if (Stat)
		{
			OutStatInfo = Stat->StatInfo;
			UE_LOG(LogTemp, Log, TEXT("[StatManager] GetStat %s: Stat=%p, CurrentValue=%.2f, MaxValue=%.2f"),
				*StatTag.ToString(), Stat, Stat->StatInfo.CurrentValue, Stat->StatInfo.MaxValue);
		}
		else
		{
			OutStatInfo.Tag = StatTag;
			UE_LOG(LogTemp, Warning, TEXT("[StatManager] GetStat %s: Found object but NOT USLFStatBase (class=%s)"),
				*StatTag.ToString(), *(*Found)->GetClass()->GetName());
		}

		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("[StatManager] GetStat %s: NOT FOUND in ActiveStats"), *StatTag.ToString());
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
		USLFStatBase* Stat = Cast<USLFStatBase>(*Found);
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
		USLFStatBase* Stat = Cast<USLFStatBase>(*Found);
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

	// CRITICAL: Get StatsToAffect from StatInfo.StatModifiers (not the separate StatBehavior member)
	// The stat classes set StatInfo.StatModifiers.StatsToAffect in their constructors
	// This matches bp_only which uses FStatInfo.StatModifiers.StatsToAffect
	USLFStatBase* BStat = Cast<USLFStatBase>(Stat);
	if (!BStat) return;

	// Use StatInfo.StatModifiers (the correct location matching bp_only and stat class constructors)
	const FStatBehavior& StatModifiers = BStat->StatInfo.StatModifiers;

	UE_LOG(LogTemp, Log, TEXT("[StatManager]   StatModifiers.StatsToAffect has %d entries"), StatModifiers.StatsToAffect.Num());

	// For each affected stat, apply the scaled change
	for (const auto& AffectedEntry : StatModifiers.StatsToAffect)
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
		USLFStatBase* Stat = Cast<USLFStatBase>(StatEntry.Value);
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
				USLFStatBase* Stat = Cast<USLFStatBase>(*Found);
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
		USLFStatBase* Stat = Cast<USLFStatBase>(*Found);
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
			USLFStatBase* StatInstance = NewObject<USLFStatBase>(this, StatClass);
			if (StatInstance)
			{
				StatInstance->StatInfo.Tag = StatTag;

				// CRITICAL: Initialize CurrentValue to MaxValue so stats start at full
				// If MaxValue is 0, check the CDO for default values
				if (StatInstance->StatInfo.MaxValue <= 0.0)
				{
					// Try to get MaxValue from the C++ parent class CDO
					if (USLFStatBase* CDO = StatClass->GetDefaultObject<USLFStatBase>())
					{
						StatInstance->StatInfo.MaxValue = CDO->StatInfo.MaxValue;
					}
				}
				// Set CurrentValue to MaxValue (stat starts at full)
				if (StatInstance->StatInfo.CurrentValue <= 0.0 && StatInstance->StatInfo.MaxValue > 0.0)
				{
					StatInstance->StatInfo.CurrentValue = StatInstance->StatInfo.MaxValue;
				}

				ActiveStats.Add(StatTag, StatInstance);
				UE_LOG(LogTemp, Log, TEXT("[StatManager] Initialized stat: %s (Current: %.0f, Max: %.0f)"),
					*StatTag.ToString(), StatInstance->StatInfo.CurrentValue, StatInstance->StatInfo.MaxValue);
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
		USLFStatBase* Stat = Cast<USLFStatBase>(*Found);
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

void UStatManagerComponent::ApplyBaseStatsFromCharacterClass()
{
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] ApplyBaseStatsFromCharacterClass - SelectedClassAsset: %s"),
		SelectedClassAsset ? *SelectedClassAsset->GetName() : TEXT("NULL"));

	// If SelectedClassAsset is not set, try to get it from GameInstance
	if (!SelectedClassAsset)
	{
		UGameInstance* GI = GetOwner() ? GetOwner()->GetGameInstance() : nullptr;
		if (GI && GI->Implements<UBPI_GameInstance>())
		{
			UPrimaryDataAsset* SelectedClass = nullptr;
			IBPI_GameInstance::Execute_GetSelectedClass(GI, SelectedClass);
			if (SelectedClass)
			{
				SelectedClassAsset = SelectedClass;
				UE_LOG(LogTemp, Warning, TEXT("[StatManager] Got SelectedClassAsset from GameInstance: %s"), *SelectedClass->GetName());
			}
		}
	}

	// Check if we have a selected character class
	if (!SelectedClassAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] No SelectedClassAsset set - cannot apply base stats"));
		return;
	}

	// Cast to UPDA_BaseCharacterInfo to access BaseStats
	UPDA_BaseCharacterInfo* CharacterInfo = Cast<UPDA_BaseCharacterInfo>(SelectedClassAsset);
	if (!CharacterInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] SelectedClassAsset is not UPDA_BaseCharacterInfo (class: %s)"),
			*SelectedClassAsset->GetClass()->GetName());
		return;
	}

	// Check if BaseStats map has any entries
	if (CharacterInfo->BaseStats.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager] BaseStats map is empty for %s"), *CharacterInfo->GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Applying %d base stats from character class %s"),
		CharacterInfo->BaseStats.Num(), *CharacterInfo->GetName());

	// Debug: Log what's in BaseStats
	for (const auto& BasePair : CharacterInfo->BaseStats)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatManager]   BaseStats key: %s -> %.2f"),
			BasePair.Key ? *BasePair.Key->GetName() : TEXT("NULL"), BasePair.Value);
	}

	// Apply base stats to each active stat using InitializeBaseClassValue
	// Store the changes so we can apply affected stats afterward
	TMap<USLFStatBase*, double> StatChanges;

	for (const auto& StatPair : ActiveStats)
	{
		USLFStatBase* Stat = Cast<USLFStatBase>(StatPair.Value);
		if (Stat)
		{
			double OldValue = Stat->StatInfo.CurrentValue;
			UE_LOG(LogTemp, Warning, TEXT("[StatManager] Applying BaseStats to stat %s (class: %s)"),
				*Stat->StatInfo.DisplayName.ToString(), *Stat->GetClass()->GetName());
			// Call InitializeBaseClassValue on the stat
			Stat->InitializeBaseClassValue(CharacterInfo->BaseStats);
			double NewValue = Stat->StatInfo.CurrentValue;
			double Change = NewValue - OldValue;
			if (!FMath::IsNearlyZero(Change))
			{
				StatChanges.Add(Stat, Change);
			}
		}
	}

	// Now trigger affected stats for all primary stats that changed
	// This calculates derived stats like HP from Vigor, FP from Mind, etc.
	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Triggering affected stats for %d changed stats"), StatChanges.Num());
	for (const auto& ChangePair : StatChanges)
	{
		USLFStatBase* Stat = ChangePair.Key;
		double Change = ChangePair.Value;

		// CRITICAL: Read from StatInfo.StatModifiers (the correct location matching bp_only)
		if (Stat->StatInfo.StatModifiers.StatsToAffect.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[StatManager] Calling AdjustAffectedStats for %s (change: %.2f, affects: %d stats)"),
				*Stat->StatInfo.DisplayName.ToString(), Change, Stat->StatInfo.StatModifiers.StatsToAffect.Num());
			AdjustAffectedStats(Stat, Change, ESLFValueType::MaxValue);
		}
	}

	// After applying all base stats and derived stats, reset Current to Max for secondary stats
	// This ensures HP/FP/Stamina start at full after character creation
	for (const auto& StatPair : ActiveStats)
	{
		USLFStatBase* Stat = Cast<USLFStatBase>(StatPair.Value);
		if (Stat)
		{
			// Check if this is a secondary stat (HP, FP, Stamina) that should be reset to max
			if (Stat->StatInfo.Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"))) ||
				Stat->StatInfo.Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.FP"))) ||
				Stat->StatInfo.Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"))))
			{
				Stat->StatInfo.CurrentValue = Stat->StatInfo.MaxValue;
				UE_LOG(LogTemp, Warning, TEXT("[StatManager] Reset %s to max: %.0f/%.0f"),
					*Stat->StatInfo.DisplayName.ToString(), Stat->StatInfo.CurrentValue, Stat->StatInfo.MaxValue);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[StatManager] Base stats applied successfully"));
}
