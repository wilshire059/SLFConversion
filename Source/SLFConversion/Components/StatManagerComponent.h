// StatManagerComponent.h
// C++ base class for AC_StatManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_StatManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         8/8 migrated (+ 3 dispatchers counted separately)
// Functions:         12/12 migrated (excluding ReceiveBeginPlay, ExecuteUbergraph)
// Event Dispatchers: 3/3 migrated
// Graphs:            16 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_StatManager
//
// PURPOSE: Manages character stats (health, stamina, etc.), stat overrides, and leveling

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "InstancedStruct.h"
#include "SLFStatTypes.h"
#include "Blueprints/SLFStatBase.h"
#include "StatManagerComponent.generated.h"

// Forward declarations
class UDataAsset;

// Types used from SLFStatTypes.h:
// - ESLFValueType (has SLF prefix)
// - FRegen, FStatBehavior, FStatInfo, FStatOverride (no SLF prefix)

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 3/3 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/3] Called when stats are fully initialized */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsInitialized);

/** [2/3] Called when level is updated */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUpdated, int32, NewLevel);

/** [3/3] Called when save is requested */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveRequested);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UStatManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 8/8 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Config Variables (5) ---

	/** [1/8] Whether this is an AI stat component (affects initialization) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bIsAiComponent;

	/** [2/8] Movement speed data asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataAsset* SpeedAsset;

	/** [3/8] Data table containing stat definitions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataTable* StatTable;

	/** [4/8] Map of stat classes (B_Stat subclasses) to their tags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<TSubclassOf<UObject>, FGameplayTag> Stats;

	/** [5/8] Stat value overrides by tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, FStatOverride> StatOverrides;

	// --- Runtime Variables (3) ---

	/** [6/8] Active stat instances (B_Stat objects) by tag */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UObject*> ActiveStats;

	/** [7/8] Current character level */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	int32 Level;

	/** [8/8] Selected character class asset */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UDataAsset* SelectedClassAsset;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 3/3 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/3] Broadcast when all stats are initialized */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatsInitialized OnStatsInitialized;

	/** [2/3] Broadcast when level changes */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLevelUpdated OnLevelUpdated;

	/** [3/3] Broadcast when save is requested */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSaveRequested OnSaveRequested;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 12/12 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Stat Access (3 functions) ---

	/** [1/12] Get a stat by tag
	 * @param StatTag - Tag of the stat to find
	 * @param OutStatObject - Found stat object (B_Stat)
	 * @param OutStatInfo - Stat info struct
	 * @return True if stat was found
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Access")
	bool GetStat(FGameplayTag StatTag, UObject*& OutStatObject, FStatInfo& OutStatInfo);
	virtual bool GetStat_Implementation(FGameplayTag StatTag, UObject*& OutStatObject, FStatInfo& OutStatInfo);

	/** [2/12] Get all stats
	 * @param OutStatObjects - Array of all stat objects
	 * @param OutStatClassesAndCategories - Map of classes to tags
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Access")
	void GetAllStats(TArray<UObject*>& OutStatObjects, TMap<TSubclassOf<UObject>, FGameplayTag>& OutStatClassesAndCategories);
	virtual void GetAllStats_Implementation(TArray<UObject*>& OutStatObjects, TMap<TSubclassOf<UObject>, FGameplayTag>& OutStatClassesAndCategories);

	/** [3/12] Get stats for a specific category
	 * Returns a container of stat tags that fall under the specified category
	 * @param StatCategory - Category tag to filter by
	 * @return Stats - Container of matching stat tags
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Access")
	FGameplayTagContainer GetStatsForCategory(FGameplayTag StatCategory);
	virtual FGameplayTagContainer GetStatsForCategory_Implementation(FGameplayTag StatCategory);

	// --- Stat Modification (4 functions) ---

	/** [4/12] Reset a stat to its default/max value
	 * @param StatTag - Tag of the stat to reset
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Modify")
	void ResetStat(FGameplayTag StatTag);
	virtual void ResetStat_Implementation(FGameplayTag StatTag);

	/** [5/12] Adjust a stat's value
	 * @param StatTag - Tag of the stat to adjust
	 * @param ValueType - Which value to adjust (current or max)
	 * @param Change - Amount to add (negative to subtract)
	 * @param bLevelUp - Whether this adjustment is from leveling up
	 * @param bTriggerRegen - Whether to trigger regeneration after adjustment
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Modify")
	void AdjustStat(FGameplayTag StatTag, ESLFValueType ValueType, double Change, bool bLevelUp, bool bTriggerRegen);
	virtual void AdjustStat_Implementation(FGameplayTag StatTag, ESLFValueType ValueType, double Change, bool bLevelUp, bool bTriggerRegen);

	/** [6/12] Adjust affected stats based on stat behavior
	 * @param StatTag - Tag of the source stat
	 * @param Change - Amount of change
	 * @param ValueType - Which value type
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Modify")
	void AdjustAffected(FGameplayTag StatTag, double Change, ESLFValueType ValueType);
	virtual void AdjustAffected_Implementation(FGameplayTag StatTag, double Change, ESLFValueType ValueType);

	/** [7/12] Adjust affected stats from a stat object
	 * @param Stat - The stat object
	 * @param Change - Amount of change
	 * @param ValueType - Which value type
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Modify")
	void AdjustAffectedStats(UObject* Stat, double Change, ESLFValueType ValueType);
	virtual void AdjustAffectedStats_Implementation(UObject* Stat, double Change, ESLFValueType ValueType);

	// --- Level Functions (2 functions) ---

	/** [8/12] Adjust the character level
	 * @param Delta - Amount to add (negative to subtract)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Level")
	void AdjustLevel(int32 Delta);
	virtual void AdjustLevel_Implementation(int32 Delta);

	/** [9/12] Check if stat is more than a threshold
	 * @param StatTag - Tag of the stat to check
	 * @param Threshold - Value to compare against
	 * @return True if stat's current value > threshold
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Level")
	bool IsStatMoreThan(FGameplayTag StatTag, double Threshold);
	virtual bool IsStatMoreThan_Implementation(FGameplayTag StatTag, double Threshold);

	// --- Serialization (2 functions) ---

	/** [10/12] Serialize stats data for saving
	 * Creates instanced structs and sends to controller
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Save")
	void SerializeStatsData();
	virtual void SerializeStatsData_Implementation();

	/** [11/12] Initialize stats from loaded save data
	 * @param LoadedStats - Array of loaded stat data
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Save")
	void InitializeLoadedStats(const TArray<FInstancedStruct>& LoadedStats);
	virtual void InitializeLoadedStats_Implementation(const TArray<FInstancedStruct>& LoadedStats);

	// --- Helpers (1 function) ---

	/** [12/12] Toggle regeneration for a stat
	 * @param StatTag - Tag of the stat
	 * @param bStop - True to stop regen, false to start
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager|Helpers")
	void ToggleRegenForStat(FGameplayTag StatTag, bool bStop);
	virtual void ToggleRegenForStat_Implementation(FGameplayTag StatTag, bool bStop);

	// ═══════════════════════════════════════════════════════════════════
	// INITIALIZATION & LEVEL UP EVENTS
	// ═══════════════════════════════════════════════════════════════════

	/** Initialize all stats from default data */
	UFUNCTION(BlueprintCallable, Category = "Stat Manager|Init")
	void EventInitializeStats();

	/** Handle level up request */
	UFUNCTION(BlueprintCallable, Category = "Stat Manager|Level")
	void EventOnLevelUpRequested(FGameplayTag StatTag);
};
