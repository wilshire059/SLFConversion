// SLFStatDeathCurrency.cpp
// C++ implementation for B_DeathCurrency
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_DeathCurrency
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Currency tag, no max limit
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatDeathCurrency.h"

USLFStatDeathCurrency::USLFStatDeathCurrency()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Currency.Souls"));
	StatInfo.DisplayName = FText::FromString(TEXT("Souls"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 999999999.0;  // Very high max
	StatInfo.bShowMaxValue = false;

	StatInfo.RegenInfo.bCanRegenerate = false;

	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[StatDeathCurrency] Initialized with %.0f"), StatInfo.CurrentValue);
}
