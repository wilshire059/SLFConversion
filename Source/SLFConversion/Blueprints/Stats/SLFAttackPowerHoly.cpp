// SLFAttackPowerHoly.cpp
#include "SLFAttackPowerHoly.h"

USLFAttackPowerHoly::USLFAttackPowerHoly()
{
	// Defaults from bp_only B_AP_Holy CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Holy"));
	StatInfo.DisplayName = FText::FromString(TEXT("Holy Attack Power"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
