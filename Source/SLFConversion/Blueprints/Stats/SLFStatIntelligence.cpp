// SLFStatIntelligence.cpp
// Intelligence affects Magic Attack Power
#include "SLFStatIntelligence.h"

USLFStatIntelligence::USLFStatIntelligence()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Intelligence"));
	StatInfo.DisplayName = FText::FromString(TEXT("Intelligence"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;

	// Intelligence affects Magic Attack Power: each point adds 4 to CurrentValue
	// At Intelligence=10, Magic AP = base(0) + 10*4 = 40
	FGameplayTag MagicAPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Magic"));
	FAffectedStat MagicAPAffect;
	MagicAPAffect.FromLevel = 0;
	MagicAPAffect.UntilLevel = 99;
	MagicAPAffect.bAffectMaxValue = false;  // Affects CurrentValue
	MagicAPAffect.Modifier = 4.0;           // 4 AP per Intelligence point
	MagicAPAffect.Calculation = nullptr;
	MagicAPAffect.ChangeByCurve = nullptr;

	FAffectedStats MagicAPAffectedStats;
	MagicAPAffectedStats.SoftcapData.Add(MagicAPAffect);

	// CRITICAL: Must set StatInfo.StatModifiers.StatsToAffect (not StatBehavior.StatsToAffect)
	// W_StatEntry_LevelUp reads from StatInfo.StatModifiers to get affected stats for highlighting
	StatInfo.StatModifiers.StatsToAffect.Add(MagicAPTag, MagicAPAffectedStats);
	UE_LOG(LogTemp, Log, TEXT("[SLFStatIntelligence] Added Magic AP to StatsToAffect in StatInfo.StatModifiers"));

	UE_LOG(LogTemp, Log, TEXT("[StatIntelligence] Initialized with %.0f, affects Magic AP with modifier 4"), StatInfo.CurrentValue);
}
