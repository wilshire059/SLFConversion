// SLFStatEndurance.cpp
#include "SLFStatEndurance.h"

USLFStatEndurance::USLFStatEndurance()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Endurance"));
	StatInfo.DisplayName = FText::FromString(TEXT("Endurance"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;

	// Endurance affects Stamina: each point of Endurance adds 2 to Stamina Max
	// BP_ONLY: Endurance=10 -> Stamina=70 (base 50 + 10*2 = 70)
	FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
	FAffectedStat StaminaAffect;
	StaminaAffect.FromLevel = 0;
	StaminaAffect.UntilLevel = 99;
	StaminaAffect.bAffectMaxValue = true;
	StaminaAffect.Modifier = 2.0;
	StaminaAffect.Calculation = nullptr;
	StaminaAffect.ChangeByCurve = nullptr;

	FAffectedStats StaminaAffectedStats;
	StaminaAffectedStats.SoftcapData.Add(StaminaAffect);

	// CRITICAL: Must set StatInfo.StatModifiers.StatsToAffect (not StatBehavior.StatsToAffect)
	// W_StatEntry_LevelUp reads from StatInfo.StatModifiers to get affected stats for highlighting
	StatInfo.StatModifiers.StatsToAffect.Add(StaminaTag, StaminaAffectedStats);
	UE_LOG(LogTemp, Log, TEXT("[SLFStatEndurance] Added Stamina to StatsToAffect in StatInfo.StatModifiers"));

	UE_LOG(LogTemp, Log, TEXT("[StatEndurance] Initialized with %.0f, affects Stamina with modifier 2"), StatInfo.CurrentValue);
}
