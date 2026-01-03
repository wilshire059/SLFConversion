// SLFStatEndurance.cpp
#include "SLFStatEndurance.h"

USLFStatEndurance::USLFStatEndurance()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.Endurance"));
	StatInfo.DisplayName = FText::FromString(TEXT("Endurance"));
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 99.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 1.0;
	UE_LOG(LogTemp, Log, TEXT("[StatEndurance] Initialized with %.0f"), StatInfo.CurrentValue);
}
