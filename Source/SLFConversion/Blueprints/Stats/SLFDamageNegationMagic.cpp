// SLFDamageNegationMagic.cpp
#include "SLFDamageNegationMagic.h"

USLFDamageNegationMagic::USLFDamageNegationMagic()
{
	// Defaults from bp_only B_DN_Magic CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Defense.Negation.Magic"));
	StatInfo.DisplayName = FText::FromString(TEXT("Magic Negation"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
