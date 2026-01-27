// SLFAttackPowerLightning.cpp
#include "SLFAttackPowerLightning.h"

USLFAttackPowerLightning::USLFAttackPowerLightning()
{
	// Defaults from bp_only B_AP_Lightning CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Lightning"));
	StatInfo.DisplayName = FText::FromString(TEXT("Lightning Attack Power"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
