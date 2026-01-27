// SLFResistanceVitality.cpp
#include "SLFResistanceVitality.h"

USLFResistanceVitality::USLFResistanceVitality()
{
	// Defaults from bp_only B_Resistance_Vitality CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Defense.Resistances.Vitality"));
	StatInfo.DisplayName = FText::FromString(TEXT("Vitality"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
