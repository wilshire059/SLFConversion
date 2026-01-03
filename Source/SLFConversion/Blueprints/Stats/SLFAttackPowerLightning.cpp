// SLFAttackPowerLightning.cpp
#include "SLFAttackPowerLightning.h"

USLFAttackPowerLightning::USLFAttackPowerLightning()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.AttackPower.Lightning"));
	StatInfo.DisplayName = FText::FromString(TEXT("Lightning Attack"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[AP_Lightning] Initialized with %.0f"), StatInfo.CurrentValue);
}
