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
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;

	StatInfo.RegenInfo.bCanRegenerate = false;

	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 1.0;

	UE_LOG(LogTemp, Log, TEXT("[StatMind] Initialized with %.0f"), StatInfo.CurrentValue);
}
