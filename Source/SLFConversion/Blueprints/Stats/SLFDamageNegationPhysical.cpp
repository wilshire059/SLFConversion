// SLFDamageNegationPhysical.cpp
#include "SLFDamageNegationPhysical.h"

USLFDamageNegationPhysical::USLFDamageNegationPhysical()
{
	// Defaults from bp_only B_DN_Physical CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Defense.Negation.Physical"));
	StatInfo.DisplayName = FText::FromString(TEXT("Physical Negation"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
