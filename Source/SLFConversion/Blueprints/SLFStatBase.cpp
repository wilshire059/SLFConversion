// SLFStatBase.cpp
// C++ implementation for B_Stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Stat
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         4/4 (initialized in constructor)
// Functions:         8/8 implemented
// Event Dispatchers: 2/2 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatBase.h"

USLFStatBase::USLFStatBase()
{
	MinValue = 0.0;
	bOnlyMaxValueRelevant = false;
	bShowMaxValueOnLevelUp = true;

	// Initialize StatInfo defaults
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// VALUE FUNCTIONS [1-2/8]
// ═══════════════════════════════════════════════════════════════════════════════

void USLFStatBase::AdjustValue_Implementation(double Amount, bool bClamp)
{
	double OldValue = StatInfo.CurrentValue;
	StatInfo.CurrentValue += Amount;

	if (bClamp)
	{
		StatInfo.CurrentValue = FMath::Clamp(StatInfo.CurrentValue, MinValue, StatInfo.MaxValue);
	}

	UE_LOG(LogTemp, Log, TEXT("[Stat] AdjustValue: %s %.2f -> %.2f (delta: %.2f)"),
		*StatInfo.Tag.ToString(), OldValue, StatInfo.CurrentValue, Amount);

	TriggerOnStatUpdated();
}

void USLFStatBase::AdjustAffectedValue_Implementation(FGameplayTag AffectingStatTag, double Amount)
{
	UE_LOG(LogTemp, Log, TEXT("[Stat] AdjustAffectedValue: %s affected by %s (%.2f)"),
		*StatInfo.Tag.ToString(), *AffectingStatTag.ToString(), Amount);

	// Adjust max value when affected by another stat (e.g., Vigor affects HP)
	StatInfo.MaxValue += Amount;
	if (StatInfo.CurrentValue > StatInfo.MaxValue)
	{
		StatInfo.CurrentValue = StatInfo.MaxValue;
	}

	TriggerOnStatUpdated();
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUERY FUNCTIONS [3-4/8]
// ═══════════════════════════════════════════════════════════════════════════════

double USLFStatBase::CalculatePercent_Implementation()
{
	if (StatInfo.MaxValue <= 0.0) return 0.0;
	return StatInfo.CurrentValue / StatInfo.MaxValue;
}

FSLFRegen USLFStatBase::GetRegenInfo_Implementation()
{
	return StatInfo.RegenInfo;
}

// ═══════════════════════════════════════════════════════════════════════════════
// UPDATE FUNCTIONS [5-6/8]
// ═══════════════════════════════════════════════════════════════════════════════

void USLFStatBase::UpdateStatInfo_Implementation(const FSLFStatInfo& NewInfo)
{
	StatInfo = NewInfo;
	UE_LOG(LogTemp, Log, TEXT("[Stat] UpdateStatInfo: %s - Current: %.2f, Max: %.2f"),
		*StatInfo.Tag.ToString(), StatInfo.CurrentValue, StatInfo.MaxValue);
}

void USLFStatBase::InitializeBaseClassValue_Implementation(double BaseValue)
{
	StatInfo.MaxValue = BaseValue;
	StatInfo.CurrentValue = BaseValue;

	UE_LOG(LogTemp, Log, TEXT("[Stat] InitializeBaseClassValue: %s = %.2f"),
		*StatInfo.Tag.ToString(), BaseValue);
}

// ═══════════════════════════════════════════════════════════════════════════════
// DISPATCHER TRIGGERS [7-8/8]
// ═══════════════════════════════════════════════════════════════════════════════

void USLFStatBase::TriggerOnStatUpdated_Implementation()
{
	OnStatUpdated.Broadcast(StatInfo.Tag, StatInfo.CurrentValue, StatInfo.MaxValue);
}

void USLFStatBase::TriggerOnLeveledUp_Implementation(int32 NewLevel)
{
	UE_LOG(LogTemp, Log, TEXT("[Stat] TriggerOnLeveledUp: %s level %d"),
		*StatInfo.Tag.ToString(), NewLevel);
	OnLeveledUp.Broadcast(StatInfo.Tag, NewLevel);
}
