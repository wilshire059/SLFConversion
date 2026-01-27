// SLFAttackPowerMagic.cpp
#include "SLFAttackPowerMagic.h"

USLFAttackPowerMagic::USLFAttackPowerMagic()
{
	// Defaults from bp_only B_AP_Magic CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Magic"));
	StatInfo.DisplayName = FText::FromString(TEXT("Magic Attack Power"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
