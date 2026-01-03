// SLFStatTypes.h
// C++ types for StatManager component migration
//
// These types replace Blueprint UserDefinedEnums/Structs.
// CoreRedirects in DefaultEngine.ini redirect Blueprint paths to these C++ types.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveFloat.h"
#include "SLFStatTypes.generated.h"

//////////////////////////////////////////////////////////////////////////
// ESLFValueType Enum
// Replaces: /Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType
//
// Blueprint values:
//   NewEnumerator0 = "Current Value" (0)
//   NewEnumerator1 = "Max Value" (1)
//////////////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ESLFValueType : uint8
{
	CurrentValue = 0	UMETA(DisplayName = "Current Value"),
	MaxValue = 1		UMETA(DisplayName = "Max Value")
};

//////////////////////////////////////////////////////////////////////////
// FRegen Struct
// Replaces: /Game/SoulslikeFramework/Structures/Stats/FRegen.FRegen
//
// Blueprint members:
//   CanRegenerate? (bool)
//   RegenPercent (float)
//   RegenInterval (float)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FRegen
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen", meta = (DisplayName = "Can Regenerate?"))
	bool bCanRegenerate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen", meta = (DisplayName = "Regen Percent"))
	float RegenPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen", meta = (DisplayName = "Regen Interval"))
	float RegenInterval = 0.0f;

	FRegen() : bCanRegenerate(false), RegenPercent(0.0f), RegenInterval(0.0f) {}
};

//////////////////////////////////////////////////////////////////////////
// FAffectedStat Struct
// Replaces: /Game/SoulslikeFramework/Structures/Stats/FAffectedStat.FAffectedStat
//
// Blueprint members:
//   FromLevel (int32)
//   UntilLevel (int32)
//   AffectMaxValue? (bool)
//   Modifier (double)
//   Calculation (UObject*)
//   ChangeByCurveOptional (UCurveFloat*)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FAffectedStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AffectedStat", meta = (DisplayName = "From Level"))
	int32 FromLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AffectedStat", meta = (DisplayName = "Until Level"))
	int32 UntilLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AffectedStat", meta = (DisplayName = "Affect Max Value?"))
	bool bAffectMaxValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AffectedStat", meta = (DisplayName = "Modifier"))
	double Modifier = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AffectedStat", meta = (DisplayName = "Calculation"))
	UObject* Calculation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AffectedStat", meta = (DisplayName = "Change by Curve Optional"))
	UCurveFloat* ChangeByCurve = nullptr;

	FAffectedStat() : FromLevel(0), UntilLevel(0), bAffectMaxValue(false), Modifier(0.0), Calculation(nullptr), ChangeByCurve(nullptr) {}
};

//////////////////////////////////////////////////////////////////////////
// FAffectedStats Struct (Container)
// Replaces: /Game/SoulslikeFramework/Structures/Stats/FAffectedStats.FAffectedStats
//
// Blueprint members:
//   SoftcapData (TArray<FAffectedStat>)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FAffectedStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AffectedStats", meta = (DisplayName = "Softcap Data"))
	TArray<FAffectedStat> SoftcapData;

	FAffectedStats() {}
};

//////////////////////////////////////////////////////////////////////////
// FStatBehavior Struct
// Replaces: /Game/SoulslikeFramework/Structures/Stats/FStatBehavior.FStatBehavior
//
// Blueprint members:
//   StatsToAffect (TMap<FGameplayTag, FAffectedStats>)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FStatBehavior
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatBehavior", meta = (DisplayName = "Stats to Affect"))
	TMap<FGameplayTag, FAffectedStats> StatsToAffect;

	FStatBehavior() {}
};

//////////////////////////////////////////////////////////////////////////
// FStatInfo Struct
// Replaces: /Game/SoulslikeFramework/Structures/Stats/FStatInfo.FStatInfo
//
// Blueprint members:
//   Tag (FGameplayTag)
//   DisplayName (FText)
//   Description (FText)
//   DisplayAsPercent? (bool)
//   CurrentValue (double)
//   MaxValue (double)
//   ShowMaxValue? (bool)
//   RegenInfo (FRegen)
//   StatModifiers (FStatBehavior)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FStatInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatInfo", meta = (DisplayName = "Tag"))
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatInfo", meta = (DisplayName = "Display Name"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatInfo", meta = (DisplayName = "Description"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatInfo", meta = (DisplayName = "Display as Percent?"))
	bool bDisplayAsPercent = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatInfo", meta = (DisplayName = "Current Value"))
	double CurrentValue = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatInfo", meta = (DisplayName = "Max Value"))
	double MaxValue = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatInfo", meta = (DisplayName = "Show Max Value?"))
	bool bShowMaxValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatInfo", meta = (DisplayName = "Regen Info"))
	FRegen RegenInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatInfo", meta = (DisplayName = "Stat Modifiers"))
	FStatBehavior StatModifiers;

	FStatInfo() : bDisplayAsPercent(false), CurrentValue(0.0), MaxValue(0.0), bShowMaxValue(false) {}
};

//////////////////////////////////////////////////////////////////////////
// FStatOverride Struct
// Replaces: /Game/SoulslikeFramework/Structures/Stats/FStatOverride.FStatOverride
//
// Blueprint members:
//   OverrideCurrentValue (double)
//   OverrideMaxValue (double)
//   OverrideRegen? (bool)
//   OverrideRegenInfo (FRegen)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FStatOverride
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatOverride", meta = (DisplayName = "Override Current Value"))
	double OverrideCurrentValue = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatOverride", meta = (DisplayName = "Override Max Value"))
	double OverrideMaxValue = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatOverride", meta = (DisplayName = "Override Regen?"))
	bool bOverrideRegen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatOverride", meta = (DisplayName = "Override Regen Info"))
	FRegen OverrideRegenInfo;

	FStatOverride() : OverrideCurrentValue(0.0), OverrideMaxValue(0.0), bOverrideRegen(false) {}
};

//////////////////////////////////////////////////////////////////////////
// FStatChange Struct
// Replaces: /Game/SoulslikeFramework/Structures/Stats/FStatChange.FStatChange
//
// Blueprint members:
//   StatTag (FGameplayTag)
//   ValueType (E_ValueType)
//   MinAmount (double)
//   MaxAmount (double)
//   TryActivateRegen? (bool)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FStatChange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatChange", meta = (DisplayName = "Stat Tag"))
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatChange", meta = (DisplayName = "Value Type"))
	ESLFValueType ValueType = ESLFValueType::CurrentValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatChange", meta = (DisplayName = "Min Amount"))
	double MinAmount = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatChange", meta = (DisplayName = "Max Amount"))
	double MaxAmount = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatChange", meta = (DisplayName = "Try Activate Regen?"))
	bool bTryActivateRegen = false;

	FStatChange() : ValueType(ESLFValueType::CurrentValue), MinAmount(0.0), MaxAmount(0.0), bTryActivateRegen(false) {}
};

//////////////////////////////////////////////////////////////////////////
// FStatChangePercent Struct
// Replaces: /Game/SoulslikeFramework/Structures/Stats/FStatChangePercent.FStatChangePercent
//
// Blueprint members:
//   StatTag (FGameplayTag)
//   ValueType (E_ValueType)
//   PercentChange (double)
//   TryActivateRegen? (bool)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FStatChangePercent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatChangePercent", meta = (DisplayName = "Stat Tag"))
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatChangePercent", meta = (DisplayName = "Value Type"))
	ESLFValueType ValueType = ESLFValueType::CurrentValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatChangePercent", meta = (DisplayName = "Percent Change"))
	double PercentChange = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatChangePercent", meta = (DisplayName = "Try Activate Regen?"))
	bool bTryActivateRegen = false;

	FStatChangePercent() : ValueType(ESLFValueType::CurrentValue), PercentChange(0.0), bTryActivateRegen(false) {}
};

//////////////////////////////////////////////////////////////////////////
// FLevelChangeData Struct
// Replaces: /Game/SoulslikeFramework/Structures/Stats/FLevelChangeData.FLevelChangeData
//
// Blueprint members:
//   Change (double)
//   ValueType (E_ValueType)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FLevelChangeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelChangeData", meta = (DisplayName = "Change"))
	double Change = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelChangeData", meta = (DisplayName = "Value Type"))
	ESLFValueType ValueType = ESLFValueType::CurrentValue;

	FLevelChangeData() : Change(0.0), ValueType(ESLFValueType::CurrentValue) {}
};

//////////////////////////////////////////////////////////////////////////
// FStatusEffectStatChanges Struct
// Replaces: /Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectStatChanges.FStatusEffectStatChanges
//
// Blueprint members:
//   StatChanges (TArray<FStatChange>)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FStatusEffectStatChanges
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect", meta = (DisplayName = "Stat Changes"))
	TArray<FStatChange> StatChanges;

	FStatusEffectStatChanges() {}
};

//////////////////////////////////////////////////////////////////////////
// FStatusEffectTick Struct
// Replaces: /Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectTick.FStatusEffectTick
//
// Blueprint members:
//   Duration (double)
//   Interval (double)
//   TickingStatAdjustment (TArray<FStatChange>)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FStatusEffectTick
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect", meta = (DisplayName = "Duration"))
	double Duration = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect", meta = (DisplayName = "Interval"))
	double Interval = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect", meta = (DisplayName = "Ticking Stat Adjustment"))
	TArray<FStatChange> TickingStatAdjustment;

	FStatusEffectTick() : Duration(0.0), Interval(0.0) {}
};

//////////////////////////////////////////////////////////////////////////
// FStatusEffectOneShotAndTick Struct
// Replaces: /Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectOneShotAndTick.FStatusEffectOneShotAndTick
//
// Blueprint members:
//   Duration (double)
//   Interval (double)
//   InstantStatAdjustment (TArray<FStatChange>)
//   TickingStatAdjustment (TArray<FStatChange>)
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FStatusEffectOneShotAndTick
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect", meta = (DisplayName = "Duration"))
	double Duration = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect", meta = (DisplayName = "Interval"))
	double Interval = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect", meta = (DisplayName = "Instant Stat Adjustment"))
	TArray<FStatChange> InstantStatAdjustment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect", meta = (DisplayName = "Ticking Stat Adjustment"))
	TArray<FStatChange> TickingStatAdjustment;

	FStatusEffectOneShotAndTick() : Duration(0.0), Interval(0.0) {}
};

// Type alias for Blueprint compatibility
using E_ValueType = ESLFValueType;
