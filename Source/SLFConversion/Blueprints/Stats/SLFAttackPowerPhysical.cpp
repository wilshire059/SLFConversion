// SLFAttackPowerPhysical.cpp
#include "SLFAttackPowerPhysical.h"

USLFAttackPowerPhysical::USLFAttackPowerPhysical()
{
	// Defaults from bp_only B_AP_Physical CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Physical"));
	StatInfo.DisplayName = FText::FromString(TEXT("Physical Attack Power"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;
}
