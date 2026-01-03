// SLFStatIncantationPower.cpp
#include "SLFStatIncantationPower.h"

USLFStatIncantationPower::USLFStatIncantationPower()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Derived.IncantationPower"));
	StatInfo.DisplayName = FText::FromString(TEXT("Incantation Power"));
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[StatIncantationPower] Initialized with %.0f"), StatInfo.CurrentValue);
}
