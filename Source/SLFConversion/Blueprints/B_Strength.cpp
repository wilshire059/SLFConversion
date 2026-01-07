// B_Strength.cpp
// C++ implementation for Blueprint B_Strength
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Strength.json

#include "Blueprints/B_Strength.h"

UB_Strength::UB_Strength()
{
	// Strength is a primary stat
	MinValue = 1.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Strength"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Strength"));
	StatInfo.Description = FText::FromString(TEXT("Attribute governing physical attack power"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 10.0;

	// Primary stats don't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

