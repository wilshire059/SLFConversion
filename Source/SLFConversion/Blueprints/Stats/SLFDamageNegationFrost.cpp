// SLFDamageNegationFrost.cpp
#include "SLFDamageNegationFrost.h"

USLFDamageNegationFrost::USLFDamageNegationFrost()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.DamageNegation.Frost"));
	StatInfo.DisplayName = FText::FromString(TEXT("Frost Defense"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 100.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = true;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = -100.0;
	UE_LOG(LogTemp, Log, TEXT("[DN_Frost] Initialized with %.1f%%"), StatInfo.CurrentValue);
}
