// SLFStatVigor.cpp
#include "SLFStatVigor.h"

USLFStatVigor::USLFStatVigor()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Vigor"));
	StatInfo.DisplayName = FText::FromString(TEXT("Vigor"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;
	UE_LOG(LogTemp, Log, TEXT("[StatVigor] Initialized with %.0f"), StatInfo.CurrentValue);
}
