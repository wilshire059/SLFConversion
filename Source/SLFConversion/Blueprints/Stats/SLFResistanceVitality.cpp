// SLFResistanceVitality.cpp
#include "SLFResistanceVitality.h"

USLFResistanceVitality::USLFResistanceVitality()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Resistance.Vitality"));
	StatInfo.DisplayName = FText::FromString(TEXT("Vitality"));
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[ResistanceVitality] Initialized with %.0f"), StatInfo.CurrentValue);
}
