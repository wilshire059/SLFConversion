// SLFStatVigor.cpp
#include "SLFStatVigor.h"

USLFStatVigor::USLFStatVigor()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Vigor"));
	StatInfo.DisplayName = FText::FromString(TEXT("Vigor"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;

	// Vigor affects HP: each point of Vigor adds 26 to HP Max
	// BP_ONLY: Vigor=10 -> HP=760 (base 500 + 10*26 = 760)
	FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
	FAffectedStat HPAffect;
	HPAffect.FromLevel = 0;
	HPAffect.UntilLevel = 99;
	HPAffect.bAffectMaxValue = true;  // Affects MaxValue, not CurrentValue
	HPAffect.Modifier = 26.0;
	HPAffect.Calculation = nullptr;
	HPAffect.ChangeByCurve = nullptr;

	FAffectedStats HPAffectedStats;
	HPAffectedStats.SoftcapData.Add(HPAffect);

	// CRITICAL: Must set StatInfo.StatModifiers.StatsToAffect (not StatBehavior.StatsToAffect)
	// W_StatEntry_LevelUp reads from StatInfo.StatModifiers to get affected stats for highlighting
	StatInfo.StatModifiers.StatsToAffect.Add(HPTag, HPAffectedStats);
	UE_LOG(LogTemp, Log, TEXT("[SLFStatVigor] Added HP to StatsToAffect in StatInfo.StatModifiers"));

	UE_LOG(LogTemp, Log, TEXT("[StatVigor] Initialized with %.0f, affects HP with modifier 26"), StatInfo.CurrentValue);
}
