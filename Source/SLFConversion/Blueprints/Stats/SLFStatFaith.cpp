// SLFStatFaith.cpp
// Faith affects Holy Attack Power and Incantation Power
#include "SLFStatFaith.h"

USLFStatFaith::USLFStatFaith()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Faith"));
	StatInfo.DisplayName = FText::FromString(TEXT("Faith"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;

	// Faith affects Holy Attack Power
	FGameplayTag HolyAPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Holy"));
	FAffectedStat HolyAPAffect;
	HolyAPAffect.FromLevel = 0;
	HolyAPAffect.UntilLevel = 99;
	HolyAPAffect.bAffectMaxValue = false;
	HolyAPAffect.Modifier = 4.0;
	HolyAPAffect.Calculation = nullptr;
	HolyAPAffect.ChangeByCurve = nullptr;

	FAffectedStats HolyAPAffectedStats;
	HolyAPAffectedStats.SoftcapData.Add(HolyAPAffect);
	// CRITICAL: Must set StatInfo.StatModifiers.StatsToAffect (not StatBehavior.StatsToAffect)
	// W_StatEntry_LevelUp reads from StatInfo.StatModifiers to get affected stats for highlighting
	StatInfo.StatModifiers.StatsToAffect.Add(HolyAPTag, HolyAPAffectedStats);
	UE_LOG(LogTemp, Log, TEXT("[SLFStatFaith] Added Holy AP to StatsToAffect in StatInfo.StatModifiers"));

	// Faith also affects Incantation Power
	FGameplayTag IncantTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Backend.IncantationPower"));
	FAffectedStat IncantAffect;
	IncantAffect.FromLevel = 0;
	IncantAffect.UntilLevel = 99;
	IncantAffect.bAffectMaxValue = false;
	IncantAffect.Modifier = 3.0;
	IncantAffect.Calculation = nullptr;
	IncantAffect.ChangeByCurve = nullptr;

	FAffectedStats IncantAffectedStats;
	IncantAffectedStats.SoftcapData.Add(IncantAffect);
	StatInfo.StatModifiers.StatsToAffect.Add(IncantTag, IncantAffectedStats);
	UE_LOG(LogTemp, Log, TEXT("[SLFStatFaith] Added Incantation Power to StatsToAffect in StatInfo.StatModifiers"));

	UE_LOG(LogTemp, Log, TEXT("[StatFaith] Initialized with %.0f, affects Holy AP and Incantation Power"), StatInfo.CurrentValue);
}
