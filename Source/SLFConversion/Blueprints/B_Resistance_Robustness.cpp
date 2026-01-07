// B_Resistance_Robustness.cpp
// C++ implementation for Blueprint B_Resistance_Robustness
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Resistance_Robustness.json

#include "Blueprints/B_Resistance_Robustness.h"

UB_Resistance_Robustness::UB_Resistance_Robustness()
{
	// Resistance - Robustness (resists bleed, frostbite)
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Resistance.Robustness"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Robustness"));
	StatInfo.Description = FText::FromString(TEXT("Resistance to blood loss and frost"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// Resistance doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

