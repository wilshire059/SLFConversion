// SLFResistanceFocus.cpp
#include "SLFResistanceFocus.h"

USLFResistanceFocus::USLFResistanceFocus()
{
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Resistance.Focus"));
	StatInfo.DisplayName = FText::FromString(TEXT("Focus"));
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 999.0;
	StatInfo.bShowMaxValue = false;
	StatInfo.RegenInfo.bCanRegenerate = false;
	bShowMaxValueOnLevelUp = false;
	bOnlyMaxValueRelevant = false;
	MinValue = 0.0;
	UE_LOG(LogTemp, Log, TEXT("[ResistanceFocus] Initialized with %.0f"), StatInfo.CurrentValue);
}
