// SLFDamageNegationFrost.cpp
#include "SLFDamageNegationFrost.h"

USLFDamageNegationFrost::USLFDamageNegationFrost()
{
	// Defaults from bp_only B_DN_Frost CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Defense.Negation.Frost"));
	StatInfo.DisplayName = FText::FromString(TEXT("Frost Negation"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
