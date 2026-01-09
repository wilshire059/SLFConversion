// SLFStatFaith.cpp
#include "SLFStatFaith.h"

USLFStatFaith::USLFStatFaith()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Faith"));
	StatInfo.DisplayName = FText::FromString(TEXT("Faith"));
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;
	UE_LOG(LogTemp, Log, TEXT("[StatFaith] Initialized with %.0f"), StatInfo.CurrentValue);
}
