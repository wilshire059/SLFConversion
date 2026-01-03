// SLFDamageNegationHoly.cpp
#include "SLFDamageNegationHoly.h"

USLFDamageNegationHoly::USLFDamageNegationHoly()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.DamageNegation.Holy"));
	StatInfo.DisplayName = FText::FromString(TEXT("Holy Defense"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 100.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = true;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = -100.0;
	UE_LOG(LogTemp, Log, TEXT("[DN_Holy] Initialized with %.1f%%"), StatInfo.CurrentValue);
}
