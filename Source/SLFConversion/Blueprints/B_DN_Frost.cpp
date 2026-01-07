// B_DN_Frost.cpp
// C++ implementation for Blueprint B_DN_Frost
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DN_Frost.json

#include "Blueprints/B_DN_Frost.h"

UB_DN_Frost::UB_DN_Frost()
{
	// Defense - Frost
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Defense.Frost"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Frost Defense"));
	StatInfo.Description = FText::FromString(TEXT("Reduces frost damage taken"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Defense doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

