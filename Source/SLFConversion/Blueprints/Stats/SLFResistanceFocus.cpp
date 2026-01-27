// SLFResistanceFocus.cpp
#include "SLFResistanceFocus.h"

USLFResistanceFocus::USLFResistanceFocus()
{
	// Defaults from bp_only B_Resistance_Focus CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Defense.Resistances.Focus"));
	StatInfo.DisplayName = FText::FromString(TEXT("Focus"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;
}
