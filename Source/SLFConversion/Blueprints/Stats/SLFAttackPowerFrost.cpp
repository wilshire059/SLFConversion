// SLFAttackPowerFrost.cpp
#include "SLFAttackPowerFrost.h"

USLFAttackPowerFrost::USLFAttackPowerFrost()
{
	// Defaults from bp_only B_AP_Frost CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Frost"));
	StatInfo.DisplayName = FText::FromString(TEXT("Frost Attack Power"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
