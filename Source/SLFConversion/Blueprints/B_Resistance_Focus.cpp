// B_Resistance_Focus.cpp
// C++ implementation for Blueprint B_Resistance_Focus
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Resistance_Focus.json

#include "Blueprints/B_Resistance_Focus.h"

UB_Resistance_Focus::UB_Resistance_Focus()
{
	// Resistance - Focus (resists madness, sleep)
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Resistance.Focus"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Focus"));
	StatInfo.Description = FText::FromString(TEXT("Resistance to madness and sleep"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// Resistance doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

