// AC_StatManager.h
// C++ component for AC_StatManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_StatManager.json
// Variables: 8 | Functions: 12 | Dispatchers: 3

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_StatManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAC_StatManager_OnStatsInitialized);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_StatManager_OnLevelUpdated, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAC_StatManager_OnSaveRequested, FGameplayTag, SaveGameTag, TArray<FInstancedStruct>, DataToSave);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_StatManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_StatManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (8)
	// ═══════════════════════════════════════════════════════════════════════

	// ActiveStats: Runtime map of stat tags to active stat objects
	// Note: JSON shows this as TMap<FGameplayTag, UB_Stat*> (map values are B_Stat objects)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UB_Stat*> ActiveStats;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool IsAiComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* SpeedAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataTable* StatTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, TSubclassOf<UB_Stat>> Stats;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	int32 Level;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UPrimaryDataAsset* SelectedClassAsset;
	// StatOverrides: Map of stat tags to override values (double)
	// Note: JSON shows value type as FGameplayTag but this is likely a float/double for override amounts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, double> StatOverrides;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_StatManager_OnStatsInitialized OnStatsInitialized;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_StatManager_OnLevelUpdated OnLevelUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_StatManager_OnSaveRequested OnSaveRequested;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (12)
	// ═══════════════════════════════════════════════════════════════════════

	// GetStat: Look up a stat by its tag
	// Returns the stat object and its info struct
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void GetStat(const FGameplayTag& StatTag, UB_Stat*& OutFoundStat, FStatInfo& OutStatInfo);
	virtual void GetStat_Implementation(const FGameplayTag& StatTag, UB_Stat*& OutFoundStat, FStatInfo& OutStatInfo);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void GetAllStats(TArray<UB_Stat*>& OutStatObjects, TMap<FGameplayTag, TSubclassOf<UB_Stat>>& OutStatClassesAndCategories);
	virtual void GetAllStats_Implementation(TArray<UB_Stat*>& OutStatObjects, TMap<FGameplayTag, TSubclassOf<UB_Stat>>& OutStatClassesAndCategories);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void ResetStat(const FGameplayTag& StatTag);
	virtual void ResetStat_Implementation(const FGameplayTag& StatTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void AdjustStat(const FGameplayTag& StatTag, ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen);
	virtual void AdjustStat_Implementation(const FGameplayTag& StatTag, ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void AdjustAffected(const FGameplayTag& StatTag, ESLFValueType ValueType, double Change);
	virtual void AdjustAffected_Implementation(const FGameplayTag& StatTag, ESLFValueType ValueType, double Change);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void AdjustAffectedStats(UB_Stat* Stat, double Change, ESLFValueType ValueType);
	virtual void AdjustAffectedStats_Implementation(UB_Stat* Stat, double Change, ESLFValueType ValueType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void SerializeStatsData();
	virtual void SerializeStatsData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void InitializeLoadedStats(const TArray<FStatInfo>& LoadedStats);
	virtual void InitializeLoadedStats_Implementation(const TArray<FStatInfo>& LoadedStats);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	FGameplayTagContainer GetStatsForCategory(const FGameplayTag& StatCategory);
	virtual FGameplayTagContainer GetStatsForCategory_Implementation(const FGameplayTag& StatCategory);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void AdjustLevel(int32 Delta);
	virtual void AdjustLevel_Implementation(int32 Delta);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	bool IsStatMoreThan(const FGameplayTag& StatTag, double Threshold);
	virtual bool IsStatMoreThan_Implementation(const FGameplayTag& StatTag, double Threshold);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatManager")
	void ToggleRegenForStat(const FGameplayTag& StatTag, bool Stop);
	virtual void ToggleRegenForStat_Implementation(const FGameplayTag& StatTag, bool Stop);
};
