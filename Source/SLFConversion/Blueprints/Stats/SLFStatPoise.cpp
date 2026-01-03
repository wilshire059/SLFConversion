// SLFStatPoise.cpp
// C++ implementation for B_Poise
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Poise
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Poise tag, regenerates after stagger
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatPoise.h"

USLFStatPoise::USLFStatPoise()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
	StatInfo.DisplayName = FText::FromString(TEXT("Poise"));
	StatInfo.CurrentValue = 50.0;
	StatInfo.MaxValue = 50.0;
	StatInfo.bShowMaxValue = true;

	// Poise regenerates after taking hits
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenPercent = 10.0;
	StatInfo.RegenInfo.RegenInterval = 0.5;

	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = true;  // Only max matters for display
	MinValue = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[StatPoise] Initialized with %.0f/%.0f"), StatInfo.CurrentValue, StatInfo.MaxValue);
}
