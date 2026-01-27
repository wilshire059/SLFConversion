// SLFStatStrength.cpp
// C++ implementation for B_Strength
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Strength
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Strength tag, affects Physical Attack Power
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatStrength.h"

USLFStatStrength::USLFStatStrength()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Strength"));
	StatInfo.DisplayName = FText::FromString(TEXT("Strength"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;

	StatInfo.RegenInfo.bCanRegenerate = false;

	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 1.0;

	// Strength affects Physical Attack Power: each point adds 5 to CurrentValue
	// At Strength=10, Physical AP = base(0) + 10*5 = 50
	FGameplayTag PhysAPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Physical"));
	FAffectedStat PhysAPAffect;
	PhysAPAffect.FromLevel = 0;
	PhysAPAffect.UntilLevel = 99;
	PhysAPAffect.bAffectMaxValue = false;  // Affects CurrentValue (AP is displayed as current)
	PhysAPAffect.Modifier = 5.0;           // 5 AP per Strength point
	PhysAPAffect.Calculation = nullptr;
	PhysAPAffect.ChangeByCurve = nullptr;

	FAffectedStats PhysAPAffectedStats;
	PhysAPAffectedStats.SoftcapData.Add(PhysAPAffect);

	// CRITICAL: Must set StatInfo.StatModifiers.StatsToAffect (not StatBehavior.StatsToAffect)
	// W_StatEntry_LevelUp reads from StatInfo.StatModifiers to get affected stats for highlighting
	StatInfo.StatModifiers.StatsToAffect.Add(PhysAPTag, PhysAPAffectedStats);
	UE_LOG(LogTemp, Log, TEXT("[SLFStatStrength] Added Physical AP to StatsToAffect in StatInfo.StatModifiers"));

	UE_LOG(LogTemp, Log, TEXT("[StatStrength] Initialized with %.0f, affects Physical AP with modifier 5"), StatInfo.CurrentValue);
}
