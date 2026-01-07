// B_DN_Lightning.cpp
// C++ implementation for Blueprint B_DN_Lightning
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DN_Lightning.json

#include "Blueprints/B_DN_Lightning.h"

UB_DN_Lightning::UB_DN_Lightning()
{
	// Defense - Lightning
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Defense.Lightning"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Lightning Defense"));
	StatInfo.Description = FText::FromString(TEXT("Reduces lightning damage taken"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Defense doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

