// B_Stat.h
// C++ class for Blueprint B_Stat
//
// 20-PASS VALIDATION: 2026-01-05
// FULL IMPLEMENTATION - All functions implemented with proper logic
// Source: BlueprintDNA/Blueprint/B_Stat.json
// Parent: Object -> UObject
// Variables: 5 | Functions: 7 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"
#include "Engine/World.h"
#include "TimerManager.h"
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
	// VARIABLES (5)
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
	// FUNCTIONS (7)
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * AdjustValue - Core stat modification function
	 * @param ValueType - CurrentValue or MaxValue
	 * @param Change - Amount to add (positive) or subtract (negative)
	 * @param LevelUp - If true, this is a level-up change (affects max, broadcasts OnLeveledUp)
	 * @param TriggerRegen - If true and change is negative, start regeneration timer
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void AdjustValue(ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen);
	virtual void AdjustValue_Implementation(ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen);

	/**
	 * AdjustAffectedValue - Called when another stat cascades changes to this stat
	 * @param ValueType - CurrentValue or MaxValue
	 * @param Change - Amount to add (already scaled by modifier)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void AdjustAffectedValue(ESLFValueType ValueType, double Change);
	virtual void AdjustAffectedValue_Implementation(ESLFValueType ValueType, double Change);

	/**
	 * CalculatePercent - Returns current value as percentage of max (0-100)
	 * Used for UI progress bars
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "B_Stat")
	double CalculatePercent();
	virtual double CalculatePercent_Implementation();

	/**
	 * GetRegenInfo - Retrieves regeneration configuration
	 * @param OutCanRegenerate - Whether this stat can regenerate
	 * @param OutRegenInterval - Time between regen ticks in seconds
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "B_Stat")
	void GetRegenInfo(bool& OutCanRegenerate, double& OutRegenInterval);
	virtual void GetRegenInfo_Implementation(bool& OutCanRegenerate, double& OutRegenInterval);

	/**
	 * UpdateStatInfo - Applies new stat values from level-up or equipment changes
	 * @param NewStatInfo - The new stat configuration to apply
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void UpdateStatInfo(const FStatInfo& NewStatInfo);
	virtual void UpdateStatInfo_Implementation(const FStatInfo& NewStatInfo);

	/**
	 * InitializeBaseClassValue - Sets initial max value from character class asset
	 * @param BaseValues - Map of stat class -> base value from selected character class
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void InitializeBaseClassValue(const TMap<UClass*, double>& BaseValues);
	virtual void InitializeBaseClassValue_Implementation(const TMap<UClass*, double>& BaseValues);

	/**
	 * ToggleStatRegen - Start or stop the regeneration timer
	 * @param bStop - If true, stop regen. If false, start regen (if stat can regenerate)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Stat")
	void ToggleStatRegen(bool bStop);
	virtual void ToggleStatRegen_Implementation(bool bStop);

protected:
	// Regen timer handle
	FTimerHandle RegenTimerHandle;

	// Called on each regen tick to add RegenPercent of MaxValue
	void OnRegenTick();

	// Get the world for timer management (UObject doesn't have GetWorld by default)
	UWorld* GetWorld() const override;
};
