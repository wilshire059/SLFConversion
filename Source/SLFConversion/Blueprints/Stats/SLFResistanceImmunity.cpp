// SLFResistanceImmunity.cpp
#include "SLFResistanceImmunity.h"

USLFResistanceImmunity::USLFResistanceImmunity()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Resistance.Immunity"));
	StatInfo.DisplayName = FText::FromString(TEXT("Immunity"));
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[ResistanceImmunity] Initialized with %.0f"), StatInfo.CurrentValue);
}
