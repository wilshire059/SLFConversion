// SLFResistanceRobustness.cpp
#include "SLFResistanceRobustness.h"

USLFResistanceRobustness::USLFResistanceRobustness()
{
	// Defaults from bp_only B_Resistance_Robustness CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Defense.Resistances.Robustness"));
	StatInfo.DisplayName = FText::FromString(TEXT("Robustness"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
