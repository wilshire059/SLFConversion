// SLFStatArcane.cpp
#include "SLFStatArcane.h"

USLFStatArcane::USLFStatArcane()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Arcane"));
	StatInfo.DisplayName = FText::FromString(TEXT("Arcane"));
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;
	UE_LOG(LogTemp, Log, TEXT("[StatArcane] Initialized with %.0f"), StatInfo.CurrentValue);
}
