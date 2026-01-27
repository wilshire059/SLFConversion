// SLFStatMind.cpp
// C++ implementation for B_Mind
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Mind
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Mind tag, primary attribute affecting FP
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatMind.h"

USLFStatMind::USLFStatMind()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Mind"));
	StatInfo.DisplayName = FText::FromString(TEXT("Mind"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;

	StatInfo.RegenInfo.bCanRegenerate = false;

	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 1.0;

	// Mind affects FP: each point of Mind adds 3 to FP Max
	// BP_ONLY: Mind=10 -> FP=130 (base 100 + 10*3 = 130)
	FGameplayTag FPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.FP"));
	FAffectedStat FPAffect;
	FPAffect.FromLevel = 0;
	FPAffect.UntilLevel = 99;
	FPAffect.bAffectMaxValue = true;
	FPAffect.Modifier = 3.0;
	FPAffect.Calculation = nullptr;
	FPAffect.ChangeByCurve = nullptr;

	FAffectedStats FPAffectedStats;
	FPAffectedStats.SoftcapData.Add(FPAffect);

	// CRITICAL: Must set StatInfo.StatModifiers.StatsToAffect (not StatBehavior.StatsToAffect)
	// W_StatEntry_LevelUp reads from StatInfo.StatModifiers to get affected stats for highlighting
	StatInfo.StatModifiers.StatsToAffect.Add(FPTag, FPAffectedStats);
	UE_LOG(LogTemp, Log, TEXT("[SLFStatMind] Added FP to StatsToAffect in StatInfo.StatModifiers"));

	UE_LOG(LogTemp, Log, TEXT("[StatMind] Initialized with %.0f, affects FP with modifier 3"), StatInfo.CurrentValue);
}
