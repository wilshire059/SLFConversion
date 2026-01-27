// SLFAttackPowerFire.cpp
#include "SLFAttackPowerFire.h"

USLFAttackPowerFire::USLFAttackPowerFire()
{
	// Defaults from bp_only B_AP_Fire CDO
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.AttackPower.Fire"));
	StatInfo.DisplayName = FText::FromString(TEXT("Fiery Attack Power"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
}
