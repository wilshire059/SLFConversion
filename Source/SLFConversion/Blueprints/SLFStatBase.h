// SLFStatBase.h
// C++ base class for B_Stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Stat
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         4/4 migrated (excluding 2 dispatchers counted separately)
// Functions:         8/8 migrated (excluding EventGraph)
// Event Dispatchers: 2/2 migrated
// Graphs:            9 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Data/Stats/B_Stat
//
// PURPOSE: Base stat class - individual stat with value, regen, scaling
// CHILDREN: B_HP, B_Stamina, B_FP, B_Poise, B_Vigor, B_Endurance, etc.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SLFStatBase.generated.h"

/**
 * Stat regen info - matches FRegen Blueprint struct
 */
USTRUCT(BlueprintType)
struct FSLFRegen
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	bool bCanRegenerate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenInterval = 0.1f;
};

/**
 * Stat info struct - matches FStatInfo Blueprint struct
 */
USTRUCT(BlueprintType)
struct FSLFStatInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	double CurrentValue = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	double MaxValue = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FSLFRegen RegenInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FGameplayTag CategoryTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	bool bAffectedBySoftcap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Display")
	bool bShowMaxValue = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Display")
	bool bDisplayAsPercent = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 2/2 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/2] Called when stat value is updated */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatValueUpdated, FGameplayTag, StatTag, double, CurrentValue, double, MaxValue);

/** [2/2] Called when stat is leveled up */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatLeveledUp, FGameplayTag, StatTag, int32, NewLevel);

/**
 * Base stat object - individual stat instance with value, regen, and scaling
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatBase : public UObject
{
	GENERATED_BODY()

public:
	USLFStatBase();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 4/4 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/4] Minimum allowed value for this stat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double MinValue;

	/** [2/4] Whether only max value is relevant (no current value tracking) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bOnlyMaxValueRelevant;

	/** [3/4] Whether to show max value on level up UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bShowMaxValueOnLevelUp;

	/** [4/4] Full stat info struct */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FSLFStatInfo StatInfo;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] Broadcast when stat value changes */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatValueUpdated OnStatUpdated;

	/** [2/2] Broadcast when stat is leveled up */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatLeveledUp OnLeveledUp;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 8/8 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/8] Adjust stat current value by amount
	 * @param Amount - Value to add (can be negative)
	 * @param bClamp - Whether to clamp result to min/max
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void AdjustValue(double Amount, bool bClamp = true);
	virtual void AdjustValue_Implementation(double Amount, bool bClamp = true);

	/** [2/8] Adjust affected value (for stats that affect other stats)
	 * @param AffectingStatTag - Tag of stat causing the adjustment
	 * @param Amount - Value to add
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void AdjustAffectedValue(FGameplayTag AffectingStatTag, double Amount);
	virtual void AdjustAffectedValue_Implementation(FGameplayTag AffectingStatTag, double Amount);

	/** [3/8] Calculate current value as percent of max
	 * @return Percentage (0.0 - 1.0)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	double CalculatePercent();
	virtual double CalculatePercent_Implementation();

	/** [4/8] Get regen info for this stat
	 * @return Regen info struct
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	FSLFRegen GetRegenInfo();
	virtual FSLFRegen GetRegenInfo_Implementation();

	/** [5/8] Update stat info struct (after external modification)
	 * @param NewInfo - Updated stat info
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void UpdateStatInfo(const FSLFStatInfo& NewInfo);
	virtual void UpdateStatInfo_Implementation(const FSLFStatInfo& NewInfo);

	/** [6/8] Initialize base class value from data table row
	 * @param BaseValue - Initial max value
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void InitializeBaseClassValue(double BaseValue);
	virtual void InitializeBaseClassValue_Implementation(double BaseValue);

	// --- Dispatcher Trigger Functions (called to fire events) ---

	/** [7/8] Trigger OnStatUpdated dispatcher */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat|Events")
	void TriggerOnStatUpdated();
	virtual void TriggerOnStatUpdated_Implementation();

	/** [8/8] Trigger OnLeveledUp dispatcher */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat|Events")
	void TriggerOnLeveledUp(int32 NewLevel);
	virtual void TriggerOnLeveledUp_Implementation(int32 NewLevel);

	// --- Getters ---

	UFUNCTION(BlueprintPure, Category = "Stat|Getters")
	FGameplayTag GetStatTag() const { return StatInfo.Tag; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getters")
	double GetCurrentValue() const { return StatInfo.CurrentValue; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getters")
	double GetMaxValue() const { return StatInfo.MaxValue; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getters")
	FSLFStatInfo GetStatInfo() const { return StatInfo; }
};
