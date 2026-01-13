// SLFStatStamina.cpp
// C++ implementation for B_Stamina
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Stamina
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Stamina tag, 100 current/max, regenerates at 25%/s
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatStamina.h"

USLFStatStamina::USLFStatStamina()
{
	// Set Stamina tag
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
	StatInfo.DisplayName = FText::FromString(TEXT("Stamina"));
	StatInfo.CurrentValue = 50.0;
	StatInfo.MaxValue = 50.0;
	StatInfo.bShowMaxValue = true;

	// Stamina regenerates (Souls-like: ~3-4 seconds to full)
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenPercent = 0.03;  // 3% of max per interval = ~30%/second
	StatInfo.RegenInfo.RegenInterval = 0.1;  // Every 0.1 seconds

	bShowMaxValueOnLevelUp = true;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[StatStamina] Initialized with %.0f/%.0f, Regen: %.1f%%"),
		StatInfo.CurrentValue, StatInfo.MaxValue, StatInfo.RegenInfo.RegenPercent);
}
