// SLFAttackPowerPhysical.cpp
#include "SLFAttackPowerPhysical.h"

USLFAttackPowerPhysical::USLFAttackPowerPhysical()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.AttackPower.Physical"));
	StatInfo.DisplayName = FText::FromString(TEXT("Physical Attack"));
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 9999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[AP_Physical] Initialized with %.0f"), StatInfo.CurrentValue);
}
