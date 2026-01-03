// SLFResistanceRobustness.cpp
#include "SLFResistanceRobustness.h"

USLFResistanceRobustness::USLFResistanceRobustness()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Resistance.Robustness"));
	StatInfo.DisplayName = FText::FromString(TEXT("Robustness"));
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[ResistanceRobustness] Initialized with %.0f"), StatInfo.CurrentValue);
}
