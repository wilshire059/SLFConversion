// SLFDamageNegationHoly.cpp
#include "SLFDamageNegationHoly.h"

USLFDamageNegationHoly::USLFDamageNegationHoly()
{
	// Defaults from bp_only B_DN_Holy CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Defense.Negation.Holy"));
	StatInfo.DisplayName = FText::FromString(TEXT("Holy Negation"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
