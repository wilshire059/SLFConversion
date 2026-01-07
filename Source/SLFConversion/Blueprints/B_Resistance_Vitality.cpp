// B_Resistance_Vitality.cpp
// C++ implementation for Blueprint B_Resistance_Vitality
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Resistance_Vitality.json

#include "Blueprints/B_Resistance_Vitality.h"

UB_Resistance_Vitality::UB_Resistance_Vitality()
{
	// Resistance - Vitality (resists instant death)
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Resistance.Vitality"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Vitality"));
	StatInfo.Description = FText::FromString(TEXT("Resistance to instant death"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// Resistance doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

