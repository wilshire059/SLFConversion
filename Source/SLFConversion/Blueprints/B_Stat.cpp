// B_Stat.cpp
// C++ implementation for Blueprint B_Stat
//
// 20-PASS VALIDATION: 2026-01-05
// FULL IMPLEMENTATION - All functions with complete logic
// NO REFLECTION - Direct property access
// Source: BlueprintDNA/Blueprint/B_Stat.json

#include "Blueprints/B_Stat.h"
#include "Engine/World.h"
#include "TimerManager.h"

UB_Stat::UB_Stat()
	: MinValue(0.0)
	, OnlyMaxValueRelevant(false)
	, ShowMaxValueOnLevelUp(false)
{
}

UWorld* UB_Stat::GetWorld() const
{
	// UObjects need to find the world through their outer
	if (UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}
	return nullptr;
}

void UB_Stat::AdjustValue_Implementation(ESLFValueType ValueType, double Change, bool LevelUp, bool TriggerRegen)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] AdjustValue - Tag: %s, ValueType: %s, Change: %.2f, LevelUp: %s, TriggerRegen: %s"),
		*StatInfo.Tag.ToString(),
		ValueType == ESLFValueType::CurrentValue ? TEXT("Current") : TEXT("Max"),
		Change,
		LevelUp ? TEXT("true") : TEXT("false"),
		TriggerRegen ? TEXT("true") : TEXT("false"));

	// Store old values for logging
	double OldCurrentValue = StatInfo.CurrentValue;
	double OldMaxValue = StatInfo.MaxValue;

	if (ValueType == ESLFValueType::CurrentValue)
	{
		// Modify CurrentValue
		// If OnlyMaxValueRelevant, current always equals max
		if (OnlyMaxValueRelevant)
		{
			// For stats like Poise, current tracks max
			StatInfo.CurrentValue = StatInfo.MaxValue;
		}
		else
		{
			// Apply the change and clamp between MinValue and MaxValue
			StatInfo.CurrentValue = FMath::Clamp(
				StatInfo.CurrentValue + Change,
				MinValue,
				StatInfo.MaxValue
			);
		}

		// If damage (negative change) and TriggerRegen is true, start regeneration
		if (Change < 0.0 && TriggerRegen)
		{
			ToggleStatRegen(false); // false = start regen
		}
	}
	else // ESLFValueType::MaxValue
	{
		// Modify MaxValue
		// Clamp to minimum of MinValue (can't have max below min)
		StatInfo.MaxValue = FMath::Max(StatInfo.MaxValue + Change, MinValue);

		// If OnlyMaxValueRelevant, sync current to max
		if (OnlyMaxValueRelevant)
		{
			StatInfo.CurrentValue = StatInfo.MaxValue;
		}
		else
		{
			// Clamp current to new max if it exceeds
			StatInfo.CurrentValue = FMath::Min(StatInfo.CurrentValue, StatInfo.MaxValue);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Stat] After adjust - Current: %.2f -> %.2f, Max: %.2f -> %.2f"),
		OldCurrentValue, StatInfo.CurrentValue, OldMaxValue, StatInfo.MaxValue);

	// Broadcast OnStatUpdated - listeners (UI, AC_StatManager) will handle cascade
	// UpdateAffectedStats = true unless this is from an affected stat cascade
	OnStatUpdated.Broadcast(this, Change, true, ValueType);

	// If this is a level-up change, also broadcast OnLeveledUp
	if (LevelUp)
	{
		int32 LevelDelta = static_cast<int32>(Change);
		OnLeveledUp.Broadcast(LevelDelta);
		UE_LOG(LogTemp, Log, TEXT("[B_Stat] Broadcast OnLeveledUp with delta: %d"), LevelDelta);
	}
}

void UB_Stat::AdjustAffectedValue_Implementation(ESLFValueType ValueType, double Change)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] AdjustAffectedValue - Tag: %s, ValueType: %s, Change: %.2f"),
		*StatInfo.Tag.ToString(),
		ValueType == ESLFValueType::CurrentValue ? TEXT("Current") : TEXT("Max"),
		Change);

	// This is called when another stat affects this one
	// We apply the change but DON'T trigger further cascades (to avoid infinite loops)
	// Also don't trigger regen since this is a derived change

	if (ValueType == ESLFValueType::CurrentValue)
	{
		if (OnlyMaxValueRelevant)
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
	else // ESLFValueType::MaxValue
	{
		StatInfo.MaxValue = FMath::Max(StatInfo.MaxValue + Change, MinValue);

		if (OnlyMaxValueRelevant)
		{
			StatInfo.CurrentValue = StatInfo.MaxValue;
		}
		else
		{
			StatInfo.CurrentValue = FMath::Min(StatInfo.CurrentValue, StatInfo.MaxValue);
		}
	}

	// Broadcast but with UpdateAffectedStats = false to prevent cascade loops
	OnStatUpdated.Broadcast(this, Change, false, ValueType);
}

double UB_Stat::CalculatePercent_Implementation()
{
	// Return current as percentage of max (0-100)
	if (StatInfo.MaxValue <= 0.0)
	{
		return 0.0;
	}

	double Percent = (StatInfo.CurrentValue / StatInfo.MaxValue) * 100.0;

	// Clamp to 0-100 range
	return FMath::Clamp(Percent, 0.0, 100.0);
}

void UB_Stat::GetRegenInfo_Implementation(bool& OutCanRegenerate, double& OutRegenInterval)
{
	// Extract regen configuration from StatInfo.RegenInfo
	OutCanRegenerate = StatInfo.RegenInfo.bCanRegenerate;
	OutRegenInterval = static_cast<double>(StatInfo.RegenInfo.RegenInterval);

	UE_LOG(LogTemp, Verbose, TEXT("[B_Stat] GetRegenInfo - Tag: %s, CanRegen: %s, Interval: %.2f"),
		*StatInfo.Tag.ToString(),
		OutCanRegenerate ? TEXT("true") : TEXT("false"),
		OutRegenInterval);
}

void UB_Stat::UpdateStatInfo_Implementation(const FStatInfo& NewStatInfo)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] UpdateStatInfo - Updating stat %s"), *StatInfo.Tag.ToString());
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] Old values - Current: %.2f, Max: %.2f"),
		StatInfo.CurrentValue, StatInfo.MaxValue);
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] New values - Current: %.2f, Max: %.2f"),
		NewStatInfo.CurrentValue, NewStatInfo.MaxValue);

	// Apply all fields from the new stat info
	StatInfo.Tag = NewStatInfo.Tag;
	StatInfo.DisplayName = NewStatInfo.DisplayName;
	StatInfo.Description = NewStatInfo.Description;
	StatInfo.bDisplayAsPercent = NewStatInfo.bDisplayAsPercent;
	StatInfo.CurrentValue = NewStatInfo.CurrentValue;
	StatInfo.MaxValue = NewStatInfo.MaxValue;
	StatInfo.bShowMaxValue = NewStatInfo.bShowMaxValue;
	StatInfo.RegenInfo = NewStatInfo.RegenInfo;
	StatInfo.StatModifiers = NewStatInfo.StatModifiers;

	// Broadcast that stat was updated (full update, affects nothing further)
	OnStatUpdated.Broadcast(this, 0.0, false, ESLFValueType::CurrentValue);
}

void UB_Stat::InitializeBaseClassValue_Implementation(const TMap<UClass*, double>& BaseValues)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] InitializeBaseClassValue - Searching for class value"));

	// Find this stat's class in the base values map
	UClass* MyClass = GetClass();

	// Iterate through the map looking for our class or a parent class
	for (const auto& Pair : BaseValues)
	{
		if (Pair.Key && MyClass->IsChildOf(Pair.Key))
		{
			double BaseValue = Pair.Value;
			double Change = BaseValue - StatInfo.MaxValue;

			UE_LOG(LogTemp, Log, TEXT("[B_Stat] Found base value %.2f for class %s, applying change %.2f"),
				BaseValue, *Pair.Key->GetName(), Change);

			// Apply the difference to reach the base value
			if (!FMath::IsNearlyZero(Change))
			{
				AdjustValue(ESLFValueType::MaxValue, Change, false, false);
			}
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[B_Stat] No base value found for stat class %s"), *MyClass->GetName());
}

void UB_Stat::ToggleStatRegen_Implementation(bool bStop)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Stat] ToggleStatRegen - Tag: %s, Stop: %s"),
		*StatInfo.Tag.ToString(),
		bStop ? TEXT("true") : TEXT("false"));

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_Stat] ToggleStatRegen - No world available for timer"));
		return;
	}

	FTimerManager& TimerManager = World->GetTimerManager();

	if (bStop)
	{
		// Stop the regen timer
		if (TimerManager.IsTimerActive(RegenTimerHandle))
		{
			TimerManager.ClearTimer(RegenTimerHandle);
			UE_LOG(LogTemp, Log, TEXT("[B_Stat] Regen timer stopped"));
		}
	}
	else
	{
		// Start the regen timer (if stat can regenerate and isn't at max)
		if (!StatInfo.RegenInfo.bCanRegenerate)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[B_Stat] Stat cannot regenerate, ignoring"));
			return;
		}

		// Don't start if already at max
		if (StatInfo.CurrentValue >= StatInfo.MaxValue)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[B_Stat] Already at max, no regen needed"));
			return;
		}

		// Clear any existing timer first
		if (TimerManager.IsTimerActive(RegenTimerHandle))
		{
			TimerManager.ClearTimer(RegenTimerHandle);
		}

		// Start looping timer
		float Interval = StatInfo.RegenInfo.RegenInterval;
		if (Interval > 0.0f)
		{
			TimerManager.SetTimer(
				RegenTimerHandle,
				this,
				&UB_Stat::OnRegenTick,
				Interval,
				true // Looping
			);
			UE_LOG(LogTemp, Log, TEXT("[B_Stat] Regen timer started with interval %.2f"), Interval);
		}
	}
}

void UB_Stat::OnRegenTick()
{
	// Check if we're already at max
	if (StatInfo.CurrentValue >= StatInfo.MaxValue)
	{
		// Stop the timer, we're done regenerating
		ToggleStatRegen(true); // true = stop
		UE_LOG(LogTemp, Log, TEXT("[B_Stat] Regen complete - at max value"));
		return;
	}

	// Calculate regen amount: RegenPercent of MaxValue
	double RegenAmount = StatInfo.MaxValue * static_cast<double>(StatInfo.RegenInfo.RegenPercent);

	// Apply regen (but don't trigger another regen cycle)
	double OldValue = StatInfo.CurrentValue;
	StatInfo.CurrentValue = FMath::Min(StatInfo.CurrentValue + RegenAmount, StatInfo.MaxValue);
	double ActualChange = StatInfo.CurrentValue - OldValue;

	UE_LOG(LogTemp, Verbose, TEXT("[B_Stat] Regen tick - Added %.2f (%.1f%% of %.2f max), now at %.2f"),
		ActualChange, StatInfo.RegenInfo.RegenPercent * 100.0, StatInfo.MaxValue, StatInfo.CurrentValue);

	// Broadcast the change (no cascade, no further regen trigger)
	if (!FMath::IsNearlyZero(ActualChange))
	{
		OnStatUpdated.Broadcast(this, ActualChange, false, ESLFValueType::CurrentValue);
	}
}
