// SLFStatFP.cpp
// C++ implementation for B_FP
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_FP
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      FP tag, 100 current/max, no regen
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatFP.h"

USLFStatFP::USLFStatFP()
{
	// Set FP tag
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.FP"));
	StatInfo.DisplayName = FText::FromString(TEXT("FP"));
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;
	StatInfo.bShowMaxValue = true;

	// FP typically doesn't regenerate automatically
	StatInfo.RegenInfo.bCanRegenerate = false;
	StatInfo.RegenInfo.RegenPercent = 0.0;
	StatInfo.RegenInfo.RegenInterval = 0.0;

	bShowMaxValueOnLevelUp = true;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[StatFP] Initialized with %.0f/%.0f"), StatInfo.CurrentValue, StatInfo.MaxValue);
}
