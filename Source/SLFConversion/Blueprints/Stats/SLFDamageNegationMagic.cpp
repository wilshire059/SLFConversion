// SLFDamageNegationMagic.cpp
#include "SLFDamageNegationMagic.h"

USLFDamageNegationMagic::USLFDamageNegationMagic()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.DamageNegation.Magic"));
	StatInfo.DisplayName = FText::FromString(TEXT("Magic Defense"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 100.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = true;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = -100.0;
	UE_LOG(LogTemp, Log, TEXT("[DN_Magic] Initialized with %.1f%%"), StatInfo.CurrentValue);
}
