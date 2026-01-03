// SLFAttackPowerMagic.cpp
#include "SLFAttackPowerMagic.h"

USLFAttackPowerMagic::USLFAttackPowerMagic()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.AttackPower.Magic"));
	StatInfo.DisplayName = FText::FromString(TEXT("Magic Attack"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[AP_Magic] Initialized with %.0f"), StatInfo.CurrentValue);
}
