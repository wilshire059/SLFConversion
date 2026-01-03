// SLFStatStance.cpp
#include "SLFStatStance.h"

USLFStatStance::USLFStatStance()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stance"));
	StatInfo.DisplayName = FText::FromString(TEXT("Stance"));
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;
	StatInfo.bShowMaxValue = true;

	// Stance regenerates after being damaged
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenPercent = 15.0;
	StatInfo.RegenInfo.RegenInterval = 0.5;

	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = true;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[StatStance] Initialized with %.0f/%.0f"), StatInfo.CurrentValue, StatInfo.MaxValue);
}
