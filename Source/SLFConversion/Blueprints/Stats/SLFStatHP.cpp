// SLFStatHP.cpp
// C++ implementation for B_HP
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_HP
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      HP tag, 500 current/max, no regen by default
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatHP.h"

USLFStatHP::USLFStatHP()
{
	// Set HP tag
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
	StatInfo.DisplayName = FText::FromString(TEXT("HP"));
	StatInfo.CurrentValue = 500.0;
	StatInfo.MaxValue = 500.0;
	StatInfo.bShowMaxValue = true;

	// HP typically doesn't regenerate automatically in Soulslike games
	StatInfo.RegenInfo.bCanRegenerate = false;
	StatInfo.RegenInfo.RegenPercent = 0.0;
	StatInfo.RegenInfo.RegenInterval = 0.0;

	// Show max value on level up screen
	bShowMaxValueOnLevelUp = true;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[StatHP] Initialized with %.0f/%.0f"), StatInfo.CurrentValue, StatInfo.MaxValue);
}
