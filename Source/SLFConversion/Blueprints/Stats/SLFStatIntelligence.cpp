// SLFStatIntelligence.cpp
#include "SLFStatIntelligence.h"

USLFStatIntelligence::USLFStatIntelligence()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Intelligence"));
	StatInfo.DisplayName = FText::FromString(TEXT("Intelligence"));
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;
	UE_LOG(LogTemp, Log, TEXT("[StatIntelligence] Initialized with %.0f"), StatInfo.CurrentValue);
}
