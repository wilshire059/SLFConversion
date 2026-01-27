// SLFStatDexterity.cpp
// Dexterity affects Lightning Attack Power
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Dexterity
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Dexterity tag, affects Lightning AP
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatDexterity.h"

USLFStatDexterity::USLFStatDexterity()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Dexterity"));
	StatInfo.DisplayName = FText::FromString(TEXT("Dexterity"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;

	StatInfo.RegenInfo.bCanRegenerate = false;

	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 1.0;

	// Dexterity affects Lightning Attack Power
	FGameplayTag LightningAPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Lightning"));
	FAffectedStat LightningAPAffect;
	LightningAPAffect.FromLevel = 0;
	LightningAPAffect.UntilLevel = 99;
	LightningAPAffect.bAffectMaxValue = false;
	LightningAPAffect.Modifier = 3.0;
	LightningAPAffect.Calculation = nullptr;
	LightningAPAffect.ChangeByCurve = nullptr;

	FAffectedStats LightningAPAffectedStats;
	LightningAPAffectedStats.SoftcapData.Add(LightningAPAffect);
	// CRITICAL: Must set StatInfo.StatModifiers.StatsToAffect (not StatBehavior.StatsToAffect)
	// W_StatEntry_LevelUp reads from StatInfo.StatModifiers to get affected stats for highlighting
	StatInfo.StatModifiers.StatsToAffect.Add(LightningAPTag, LightningAPAffectedStats);
	UE_LOG(LogTemp, Log, TEXT("[SLFStatDexterity] Added Lightning AP to StatsToAffect in StatInfo.StatModifiers"));

	UE_LOG(LogTemp, Log, TEXT("[StatDexterity] Initialized with %.0f, affects Lightning AP"), StatInfo.CurrentValue);
}
