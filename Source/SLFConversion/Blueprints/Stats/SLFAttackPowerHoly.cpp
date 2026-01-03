// SLFAttackPowerHoly.cpp
#include "SLFAttackPowerHoly.h"

USLFAttackPowerHoly::USLFAttackPowerHoly()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.AttackPower.Holy"));
	StatInfo.DisplayName = FText::FromString(TEXT("Holy Attack"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[AP_Holy] Initialized with %.0f"), StatInfo.CurrentValue);
}
