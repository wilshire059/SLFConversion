// B_Vigor.cpp
// C++ implementation for Blueprint B_Vigor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Vigor.json

#include "Blueprints/B_Vigor.h"

UB_Vigor::UB_Vigor()
{
	// Vigor is a primary stat (only max value matters for leveling)
	MinValue = 1.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Vigor"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Vigor"));
	StatInfo.Description = FText::FromString(TEXT("Attribute governing HP"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 10.0;

	// Primary stats don't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

