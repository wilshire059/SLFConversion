// B_AP_Lightning.cpp
// C++ implementation for Blueprint B_AP_Lightning
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_AP_Lightning.json

#include "Blueprints/B_AP_Lightning.h"

UB_AP_Lightning::UB_AP_Lightning()
{
	// Attack Power - Lightning
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.AttackPower.Lightning"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Lightning Attack"));
	StatInfo.Description = FText::FromString(TEXT("Lightning damage dealt by attacks"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Attack power doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

