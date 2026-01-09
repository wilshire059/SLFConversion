// SLFStatDexterity.cpp
// C++ implementation for B_Dexterity
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Dexterity
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Dexterity tag, primary attribute
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatDexterity.h"

USLFStatDexterity::USLFStatDexterity()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Dexterity"));
	StatInfo.DisplayName = FText::FromString(TEXT("Dexterity"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;

	StatInfo.RegenInfo.bCanRegenerate = false;

	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 1.0;

	UE_LOG(LogTemp, Log, TEXT("[StatDexterity] Initialized with %.0f"), StatInfo.CurrentValue);
}
