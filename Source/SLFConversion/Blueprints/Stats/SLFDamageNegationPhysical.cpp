// SLFDamageNegationPhysical.cpp
#include "SLFDamageNegationPhysical.h"

USLFDamageNegationPhysical::USLFDamageNegationPhysical()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.DamageNegation.Physical"));
	StatInfo.DisplayName = FText::FromString(TEXT("Physical Defense"));
	StatInfo.CurrentValue = 0.0;  // Percentage reduction
	StatInfo.MaxValue = 100.0;    // Max 100% reduction
	StatInfo.bShowMaxValue = false;
	StatInfo.bDisplayAsPercent = true;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = -100.0;  // Can be negative (increased damage taken)
	UE_LOG(LogTemp, Log, TEXT("[DN_Physical] Initialized with %.1f%%"), StatInfo.CurrentValue);
}
