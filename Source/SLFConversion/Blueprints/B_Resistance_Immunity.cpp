// B_Resistance_Immunity.cpp
// C++ implementation for Blueprint B_Resistance_Immunity
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Resistance_Immunity.json

#include "Blueprints/B_Resistance_Immunity.h"

UB_Resistance_Immunity::UB_Resistance_Immunity()
{
	// Resistance - Immunity (resists poison, scarlet rot)
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Resistance.Immunity"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Immunity"));
	StatInfo.Description = FText::FromString(TEXT("Resistance to poison and rot"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// Resistance doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

