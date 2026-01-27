// SLFResistanceImmunity.cpp
#include "SLFResistanceImmunity.h"

USLFResistanceImmunity::USLFResistanceImmunity()
{
	// Defaults from bp_only B_Resistance_Immunity CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Defense.Resistances.Immunity"));
	StatInfo.DisplayName = FText::FromString(TEXT("Immunity"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
