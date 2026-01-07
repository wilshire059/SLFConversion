// B_Mind.cpp
// C++ implementation for Blueprint B_Mind
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Mind.json

#include "Blueprints/B_Mind.h"

UB_Mind::UB_Mind()
{
	// Mind is a primary stat
	MinValue = 1.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Mind"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Mind"));
	StatInfo.Description = FText::FromString(TEXT("Attribute governing FP"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 10.0;

	// Primary stats don't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

