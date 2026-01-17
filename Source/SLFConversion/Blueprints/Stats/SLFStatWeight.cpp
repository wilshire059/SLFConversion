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
	// bp_only: Weight stat uses X/Y format (e.g., "1/23"), NOT percentage
	// Tag matches armor StatChanges: SoulslikeFramework.Stat.Misc.Weight
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Misc.Weight"));
	StatInfo.DisplayName = FText::FromString(TEXT("Equip Load"));
	StatInfo.Description = FText::FromString(TEXT("Current equipment weight vs maximum carry capacity"));
	StatInfo.CurrentValue = 0.0;   // Current equipment weight
	StatInfo.MaxValue = 50.0;      // Max carry capacity (matches B_Weight default)
	StatInfo.bShowMaxValue = true; // Show as X/Y format
	StatInfo.bDisplayAsPercent = false; // NOT percentage, shows as "X / Y"

	StatInfo.RegenInfo.bCanRegenerate = false;

	bShowMaxValueOnLevelUp = true;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[StatWeight] Initialized with %.0f/%.0f (X/Y format)"), StatInfo.CurrentValue, StatInfo.MaxValue);
}
