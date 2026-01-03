// SLFStatStrength.cpp
// C++ implementation for B_Strength
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Strength
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Strength tag, primary attribute
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatStrength.h"

USLFStatStrength::USLFStatStrength()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Strength"));
	StatInfo.DisplayName = FText::FromString(TEXT("Strength"));
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;

	StatInfo.RegenInfo.bCanRegenerate = false;

	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 1.0;

	UE_LOG(LogTemp, Log, TEXT("[StatStrength] Initialized with %.0f"), StatInfo.CurrentValue);
}
