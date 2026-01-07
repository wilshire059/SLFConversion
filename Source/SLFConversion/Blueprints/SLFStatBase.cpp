// SLFStatBase.cpp
// C++ implementation for B_Stat - Full implementation with regen timer support

#include "SLFStatBase.h"
#include "Engine/World.h"
#include "TimerManager.h"

USLFStatBase::USLFStatBase()
	: MinValue(0.0)
	, bOnlyMaxValueRelevant(false)
	, bShowMaxValueOnLevelUp(false)
{
}

UWorld* USLFStatBase::GetWorld() const
{
	if (UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}
	return nullptr;
}

void USLFStatBase::AdjustValue_Implementation(ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] AdjustValue - Tag: %s, ValueType: %s, Change: %.2f"),
		*StatInfo.Tag.ToString(),
		ValueType == ESLFValueType::CurrentValue ? TEXT("Current") : TEXT("Max"),
		Change);

	double OldCurrentValue = StatInfo.CurrentValue;
	double OldMaxValue = StatInfo.MaxValue;

	if (ValueType == ESLFValueType::CurrentValue)
	{
		if (bOnlyMaxValueRelevant)
		{
			StatInfo.CurrentValue = StatInfo.MaxValue;
		}
		else
		{
			StatInfo.CurrentValue = FMath::Clamp(
				StatInfo.CurrentValue + Change,
				MinValue,
				StatInfo.MaxValue
			);
		}

		if (Change < 0.0 && TriggerRegen)
		{
			ToggleStatRegen(false);
		}
	}
	else
	{
		StatInfo.MaxValue = FMath::Max(StatInfo.MaxValue + Change, MinValue);

		if (bOnlyMaxValueRelevant)
		{
			StatInfo.CurrentValue = StatInfo.MaxValue;
		}
		else
		{
			StatInfo.CurrentValue = FMath::Min(StatInfo.CurrentValue, StatInfo.MaxValue);
		}
	}

	OnStatUpdated.Broadcast(this, Change, true, ValueType);

	if (LevelUp)
	{
		int32 LevelDelta = static_cast<int32>(Change);
		OnLeveledUp.Broadcast(LevelDelta);
	}
}

void USLFStatBase::AdjustAffectedValue_Implementation(ESLFValueType ValueType, double Change)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] AdjustAffectedValue - Tag: %s, Change: %.2f"),
		*StatInfo.Tag.ToString(), Change);

	if (ValueType == ESLFValueType::CurrentValue)
	{
		if (bOnlyMaxValueRelevant)
		{
			StatInfo.CurrentValue = StatInfo.MaxValue;
		}
		else
		{
			StatInfo.CurrentValue = FMath::Clamp(
				StatInfo.CurrentValue + Change,
				MinValue,
				StatInfo.MaxValue
			);
		}
	}
	else
	{
		StatInfo.MaxValue = FMath::Max(StatInfo.MaxValue + Change, MinValue);

		if (bOnlyMaxValueRelevant)
		{
			StatInfo.CurrentValue = StatInfo.MaxValue;
		}
		else
		{
			StatInfo.CurrentValue = FMath::Min(StatInfo.CurrentValue, StatInfo.MaxValue);
		}
	}

	OnStatUpdated.Broadcast(this, Change, false, ValueType);
}

double USLFStatBase::CalculatePercent_Implementation()
{
	if (StatInfo.MaxValue <= 0.0)
	{
		return 0.0;
	}
	double Percent = (StatInfo.CurrentValue / StatInfo.MaxValue) * 100.0;
	return FMath::Clamp(Percent, 0.0, 100.0);
}

void USLFStatBase::GetRegenInfo_Implementation(bool& OutCanRegenerate, double& OutRegenInterval)
{
	OutCanRegenerate = StatInfo.RegenInfo.bCanRegenerate;
	OutRegenInterval = static_cast<double>(StatInfo.RegenInfo.RegenInterval);
}

void USLFStatBase::UpdateStatInfo_Implementation(const FStatInfo& NewStatInfo)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] UpdateStatInfo - Tag: %s"), *StatInfo.Tag.ToString());

	StatInfo.Tag = NewStatInfo.Tag;
	StatInfo.DisplayName = NewStatInfo.DisplayName;
	StatInfo.Description = NewStatInfo.Description;
	StatInfo.bDisplayAsPercent = NewStatInfo.bDisplayAsPercent;
	StatInfo.CurrentValue = NewStatInfo.CurrentValue;
	StatInfo.MaxValue = NewStatInfo.MaxValue;
	StatInfo.bShowMaxValue = NewStatInfo.bShowMaxValue;
	StatInfo.RegenInfo = NewStatInfo.RegenInfo;
	StatInfo.StatModifiers = NewStatInfo.StatModifiers;

	OnStatUpdated.Broadcast(this, 0.0, false, ESLFValueType::CurrentValue);
}

void USLFStatBase::InitializeBaseClassValue_Implementation(const TMap<UClass*, double>& BaseValues)
{
	UClass* MyClass = GetClass();

	for (const auto& Pair : BaseValues)
	{
		if (Pair.Key && MyClass->IsChildOf(Pair.Key))
		{
			double BaseValue = Pair.Value;
			double Change = BaseValue - StatInfo.MaxValue;

			UE_LOG(LogTemp, Log, TEXT("[B_Stat] Found base value %.2f for class %s"),
				BaseValue, *Pair.Key->GetName());

			if (!FMath::IsNearlyZero(Change))
			{
				AdjustValue(ESLFValueType::MaxValue, Change, false, false);
			}
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[B_Stat] No base value found for stat class %s"), *MyClass->GetName());
}

void USLFStatBase::ToggleStatRegen_Implementation(bool bStop)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] ToggleStatRegen - Tag: %s, Stop: %s"),
		*StatInfo.Tag.ToString(),
		bStop ? TEXT("true") : TEXT("false"));

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FTimerManager& TimerManager = World->GetTimerManager();

	if (bStop)
	{
		if (TimerManager.IsTimerActive(RegenTimerHandle))
		{
			TimerManager.ClearTimer(RegenTimerHandle);
		}
	}
	else
	{
		if (!StatInfo.RegenInfo.bCanRegenerate)
		{
			return;
		}

		if (StatInfo.CurrentValue >= StatInfo.MaxValue)
		{
			return;
		}

		if (TimerManager.IsTimerActive(RegenTimerHandle))
		{
			TimerManager.ClearTimer(RegenTimerHandle);
		}

		float Interval = StatInfo.RegenInfo.RegenInterval;
		if (Interval > 0.0f)
		{
			TimerManager.SetTimer(
				RegenTimerHandle,
				this,
				&USLFStatBase::OnRegenTick,
				Interval,
				true
			);
		}
	}
}

void USLFStatBase::OnRegenTick()
{
	if (StatInfo.CurrentValue >= StatInfo.MaxValue)
	{
		ToggleStatRegen(true);
		return;
	}

	double RegenAmount = StatInfo.MaxValue * static_cast<double>(StatInfo.RegenInfo.RegenPercent);
	double OldValue = StatInfo.CurrentValue;
	StatInfo.CurrentValue = FMath::Min(StatInfo.CurrentValue + RegenAmount, StatInfo.MaxValue);
	double ActualChange = StatInfo.CurrentValue - OldValue;

	if (!FMath::IsNearlyZero(ActualChange))
	{
		OnStatUpdated.Broadcast(this, ActualChange, false, ESLFValueType::CurrentValue);
	}
}
