// B_AP_Fire.cpp
// C++ implementation for Blueprint B_AP_Fire
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_AP_Fire.json

#include "Blueprints/B_AP_Fire.h"

UB_AP_Fire::UB_AP_Fire()
{
	// Attack Power - Fire
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.AttackPower.Fire"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Fire Attack"));
	StatInfo.Description = FText::FromString(TEXT("Fire damage dealt by attacks"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Attack power doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

