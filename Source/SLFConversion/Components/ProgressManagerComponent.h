// ProgressManagerComponent.h
// C++ base class for AC_ProgressManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_ProgressManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         4/4 migrated (CurrentProgress, LevelUpCosts, DefaultsToProgress, PlayTime)
// Functions:         10/10 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 2/2 migrated
// Graphs:            8 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_ProgressManager
//
// PURPOSE: Tracks game progress (quests, bosses, NPCs) and play time

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveFloat.h"
#include "InstancedStruct.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "ProgressManagerComponent.generated.h"

// Types used from SLFEnums.h/SLFGameTypes.h:
// - ESLFProgress
// - FSLFProgressSaveInfo
// - FSLFSaveGameInfo

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 2/2 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/2] Called when play time is updated */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayTimeUpdated, FTimespan, NewTime);

/** [2/2] Called when save is requested with progress data */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProgressSaveRequested, FGameplayTag, SaveDataTag, const TArray<FInstancedStruct>&, ProgressData);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UProgressManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UProgressManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 4/4 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/4] Current progress state for each tag */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, ESLFProgress> CurrentProgress;

	/** [2/4] Level up cost curve */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UCurveFloat* LevelUpCosts;

	/** [3/4] Default progress values to initialize */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, ESLFProgress> DefaultsToProgress;

	/** [4/4] Total play time */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FTimespan PlayTime;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] Broadcast when play time updates */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayTimeUpdated OnPlayTimeUpdated;

	/** [2/2] Broadcast when save is requested */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnProgressSaveRequested OnSaveRequested;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 10/10 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Access Functions (2) ---

	/** [1/10] Get progress state for a tag
	 * @param ProgressTag - Tag to look up
	 * @param OutState - Current progress state
	 * @param bFound - Whether the tag was found
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Progress Manager|Access")
	void GetProgress(FGameplayTag ProgressTag, ESLFProgress& OutState, bool& bFound);
	virtual void GetProgress_Implementation(FGameplayTag ProgressTag, ESLFProgress& OutState, bool& bFound);

	/** [2/10] Set progress state for a tag
	 * @param ProgressTag - Tag to set
	 * @param State - New progress state
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Progress Manager|Access")
	void SetProgress(FGameplayTag ProgressTag, ESLFProgress State);
	virtual void SetProgress_Implementation(FGameplayTag ProgressTag, ESLFProgress State);

	// --- Query Functions (1) ---

	/** [3/10] Get required currency for a specific level
	 * @param Level - Level to check
	 * @return Currency required
	 */
	UFUNCTION(BlueprintPure, Category = "Progress Manager|Query")
	int32 GetRequiredCurrencyForLevel(int32 Level) const;

	// --- Event Execution (1) ---

	/** [4/10] Execute gameplay events from dialog or other sources
	 * @param Events - Array of dialog gameplay events
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Progress Manager")
	void ExecuteGameplayEvents(const TArray<FInstancedStruct>& Events);
	virtual void ExecuteGameplayEvents_Implementation(const TArray<FInstancedStruct>& Events);

	/** Execute dialog gameplay events (items, progress, etc.)
	 * @param GameplayEvents - Array of dialog gameplay events
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Progress Manager")
	void ExecuteDialogGameplayEvents(const TArray<FSLFDialogGameplayEvent>& GameplayEvents);
	virtual void ExecuteDialogGameplayEvents_Implementation(const TArray<FSLFDialogGameplayEvent>& GameplayEvents);

	// --- Serialization (1) ---

	/** [5/10] Serialize progress data for saving */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Progress Manager|Saving")
	void SerializeProgressData();
	virtual void SerializeProgressData_Implementation();

	// --- Timer/Time Functions (2) ---

	/** [6/10] Internal timer callback for play time tracking */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Progress Manager")
	void PlayTimeTimer();
	virtual void PlayTimeTimer_Implementation();

	/** [7/10] Initialize play time from loaded save
	 * @param LoadedPlayTime - Play time from save
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Progress Manager")
	void InitializeLoadedPlayTime(FTimespan LoadedPlayTime);
	virtual void InitializeLoadedPlayTime_Implementation(FTimespan LoadedPlayTime);

	// --- Load Functions (2) ---

	/** [8/10] Initialize progress from loaded save data
	 * @param LoadedProgress - Array of progress save info
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Progress Manager")
	void InitializeLoadedProgress(const TArray<FSLFProgressSaveInfo>& LoadedProgress);
	virtual void InitializeLoadedProgress_Implementation(const TArray<FSLFProgressSaveInfo>& LoadedProgress);

	/** [9/10] Called when save data is loaded
	 * @param LoadedData - Full save game info
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Progress Manager")
	void OnDataLoaded(const FSLFSaveGameInfo& LoadedData);
	virtual void OnDataLoaded_Implementation(const FSLFSaveGameInfo& LoadedData);

	// --- Lifecycle (1) ---
	// [10/10] ReceiveBeginPlay -> BeginPlay override above
};
