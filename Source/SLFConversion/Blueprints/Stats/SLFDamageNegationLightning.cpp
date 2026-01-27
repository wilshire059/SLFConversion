// SLFDamageNegationLightning.cpp
#include "SLFDamageNegationLightning.h"

USLFDamageNegationLightning::USLFDamageNegationLightning()
{
	// Defaults from bp_only B_DN_Lightning CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Defense.Negation.Lightning"));
	StatInfo.DisplayName = FText::FromString(TEXT("Lightning Negation"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
