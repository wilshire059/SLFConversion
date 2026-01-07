// B_AP_Frost.cpp
// C++ implementation for Blueprint B_AP_Frost
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_AP_Frost.json

#include "Blueprints/B_AP_Frost.h"

UB_AP_Frost::UB_AP_Frost()
{
	// Attack Power - Frost
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.AttackPower.Frost"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Frost Attack"));
	StatInfo.Description = FText::FromString(TEXT("Frost damage dealt by attacks"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Attack power doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

