// SLFDamageNegationLightning.cpp
#include "SLFDamageNegationLightning.h"

USLFDamageNegationLightning::USLFDamageNegationLightning()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.DamageNegation.Lightning"));
	StatInfo.DisplayName = FText::FromString(TEXT("Lightning Defense"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 100.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = true;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = -100.0;
	UE_LOG(LogTemp, Log, TEXT("[DN_Lightning] Initialized with %.1f%%"), StatInfo.CurrentValue);
}
