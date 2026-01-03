// B_Stat.h
// C++ class for Blueprint B_Stat
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Stat.json
// Parent: Object -> UObject
// Variables: 4 | Functions: 6 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "SkeletalMergingLibrary.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Field/FieldSystemObjects.h"
#include "B_Stat.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FB_Stat_OnStatUpdated, UB_Stat*, UpdatedStat, double, Change, bool, UpdateAffectedStats, ESLFValueType, ValueType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FB_Stat_OnLeveledUp, int32, Delta);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UB_Stat : public UObject
{
	GENERATED_BODY()

public:
	UB_Stat();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	double MinValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	bool OnlyMaxValueRelevant;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	bool ShowMaxValueOnLevelUp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	FStatInfo StatInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	FStatBehavior StatBehavior;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_Stat_OnStatUpdated OnStatUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_Stat_OnLeveledUp OnLeveledUp;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (6)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void AdjustValue(ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen);
	virtual void AdjustValue_Implementation(ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void AdjustAffectedValue(ESLFValueType ValueType, double Change);
	virtual void AdjustAffectedValue_Implementation(ESLFValueType ValueType, double Change);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	double CalculatePercent();
	virtual double CalculatePercent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void GetRegenInfo(bool& OutCanRegenerate, double& OutRegenInterval);
	virtual void GetRegenInfo_Implementation(bool& OutCanRegenerate, double& OutRegenInterval);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void UpdateStatInfo(const FStatInfo& NewStatInfo);
	virtual void UpdateStatInfo_Implementation(const FStatInfo& NewStatInfo);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void InitializeBaseClassValue(const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& BaseValues);
	virtual void InitializeBaseClassValue_Implementation(const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& BaseValues);

	// ToggleStatRegen - Custom Event from Blueprint EventGraph
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void ToggleStatRegen(bool bStop);
	virtual void ToggleStatRegen_Implementation(bool bStop);
};
