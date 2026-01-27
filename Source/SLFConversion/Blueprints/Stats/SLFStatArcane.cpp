// SLFStatArcane.cpp
// Arcane affects Fire Attack Power (blood/dragon incantations style)
#include "SLFStatArcane.h"

USLFStatArcane::USLFStatArcane()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Arcane"));
	StatInfo.DisplayName = FText::FromString(TEXT("Arcane"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;

	// Arcane affects Fire Attack Power
	FGameplayTag FireAPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Fire"));
	FAffectedStat FireAPAffect;
	FireAPAffect.FromLevel = 0;
	FireAPAffect.UntilLevel = 99;
	FireAPAffect.bAffectMaxValue = false;
	FireAPAffect.Modifier = 3.0;
	FireAPAffect.Calculation = nullptr;
	FireAPAffect.ChangeByCurve = nullptr;

	FAffectedStats FireAPAffectedStats;
	FireAPAffectedStats.SoftcapData.Add(FireAPAffect);
	// CRITICAL: Must set StatInfo.StatModifiers.StatsToAffect (not StatBehavior.StatsToAffect)
	// W_StatEntry_LevelUp reads from StatInfo.StatModifiers to get affected stats for highlighting
	StatInfo.StatModifiers.StatsToAffect.Add(FireAPTag, FireAPAffectedStats);
	UE_LOG(LogTemp, Log, TEXT("[SLFStatArcane] Added Fire AP to StatsToAffect in StatInfo.StatModifiers"));

	UE_LOG(LogTemp, Log, TEXT("[StatArcane] Initialized with %.0f, affects Fire AP"), StatInfo.CurrentValue);
}
