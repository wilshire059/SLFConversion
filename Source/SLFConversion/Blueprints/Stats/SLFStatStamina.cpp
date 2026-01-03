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
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;
	StatInfo.bShowMaxValue = true;

	// Stamina regenerates quickly
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenPercent = 25.0;  // 25% per interval
	StatInfo.RegenInfo.RegenInterval = 0.1;  // Every 0.1 seconds

	bShowMaxValueOnLevelUp = true;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[StatStamina] Initialized with %.0f/%.0f, Regen: %.1f%%"),
		StatInfo.CurrentValue, StatInfo.MaxValue, StatInfo.RegenInfo.RegenPercent);
}
