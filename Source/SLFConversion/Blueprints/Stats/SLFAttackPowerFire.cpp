// SLFAttackPowerFire.cpp
#include "SLFAttackPowerFire.h"

USLFAttackPowerFire::USLFAttackPowerFire()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.AttackPower.Fire"));
	StatInfo.DisplayName = FText::FromString(TEXT("Fire Attack"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[AP_Fire] Initialized with %.0f"), StatInfo.CurrentValue);
}
