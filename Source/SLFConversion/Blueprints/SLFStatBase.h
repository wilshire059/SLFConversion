// SLFStatBase.h
// C++ base class for B_Stat - Uses FStatInfo from SLFStatTypes.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SLFStatTypes.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "SLFStatBase.generated.h"

class USLFStatBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnStatValueUpdated, USLFStatBase*, UpdatedStat, double, Change, bool, UpdateAffectedStats, ESLFValueType, ValueType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatLeveledUp, int32, Delta);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatBase : public UObject
{
	GENERATED_BODY()

public:
	USLFStatBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double MinValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bOnlyMaxValueRelevant;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bShowMaxValueOnLevelUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FStatInfo StatInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FStatBehavior StatBehavior;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatValueUpdated OnStatUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatLeveledUp OnLeveledUp;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void AdjustValue(ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen);
	virtual void AdjustValue_Implementation(ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void AdjustAffectedValue(ESLFValueType ValueType, double Change);
	virtual void AdjustAffectedValue_Implementation(ESLFValueType ValueType, double Change);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Stat")
	double CalculatePercent();
	virtual double CalculatePercent_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Stat")
	void GetRegenInfo(bool& OutCanRegenerate, double& OutRegenInterval);
	virtual void GetRegenInfo_Implementation(bool& OutCanRegenerate, double& OutRegenInterval);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void UpdateStatInfo(const FStatInfo& NewStatInfo);
	virtual void UpdateStatInfo_Implementation(const FStatInfo& NewStatInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void InitializeBaseClassValue(const TMap<UClass*, double>& BaseValues);
	virtual void InitializeBaseClassValue_Implementation(const TMap<UClass*, double>& BaseValues);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void ToggleStatRegen(bool bStop);
	virtual void ToggleStatRegen_Implementation(bool bStop);

	UFUNCTION(BlueprintPure, Category = "Stat|Getters")
	FGameplayTag GetStatTag() const { return StatInfo.Tag; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getters")
	double GetCurrentValue() const { return StatInfo.CurrentValue; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getters")
	double GetMaxValue() const { return StatInfo.MaxValue; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getters")
	FStatInfo GetStatInfo() const { return StatInfo; }

protected:
	FTimerHandle RegenTimerHandle;
	void OnRegenTick();
	UWorld* GetWorld() const override;
};
