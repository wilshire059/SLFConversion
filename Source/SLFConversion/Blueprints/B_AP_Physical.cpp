// B_AP_Physical.cpp
// C++ implementation for Blueprint B_AP_Physical
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_AP_Physical.json

#include "Blueprints/B_AP_Physical.h"

UB_AP_Physical::UB_AP_Physical()
{
	// Attack Power - Physical
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.AttackPower.Physical"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Physical Attack"));
	StatInfo.Description = FText::FromString(TEXT("Physical damage dealt by attacks"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// Attack power doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

