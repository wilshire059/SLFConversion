// SLFDamageNegationFire.cpp
#include "SLFDamageNegationFire.h"

USLFDamageNegationFire::USLFDamageNegationFire()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.DamageNegation.Fire"));
	StatInfo.DisplayName = FText::FromString(TEXT("Fire Defense"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 100.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = true;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = -100.0;
	UE_LOG(LogTemp, Log, TEXT("[DN_Fire] Initialized with %.1f%%"), StatInfo.CurrentValue);
}
