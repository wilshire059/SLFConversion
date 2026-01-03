// SLFAttackPowerFrost.cpp
#include "SLFAttackPowerFrost.h"

USLFAttackPowerFrost::USLFAttackPowerFrost()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.AttackPower.Frost"));
	StatInfo.DisplayName = FText::FromString(TEXT("Frost Attack"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[AP_Frost] Initialized with %.0f"), StatInfo.CurrentValue);
}
