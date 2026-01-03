// SLFStatWeight.cpp
// C++ implementation for B_Weight
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Weight
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Weight tag, display as percentage
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatWeight.h"

USLFStatWeight::USLFStatWeight()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Weight"));
	StatInfo.DisplayName = FText::FromString(TEXT("Equip Load"));
	StatInfo.CurrentValue = 0.0;   // Current equipment weight
	StatInfo.MaxValue = 100.0;     // Max carry capacity
	StatInfo.bShowMaxValue = true;
	StatInfo.bDisplayAsPercent = true;

	StatInfo.RegenInfo.bCanRegenerate = false;

	bShowMaxValueOnLevelUp = true;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[StatWeight] Initialized with %.0f/%.0f"), StatInfo.CurrentValue, StatInfo.MaxValue);
}
