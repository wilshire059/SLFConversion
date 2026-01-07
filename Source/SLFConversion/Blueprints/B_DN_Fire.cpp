// B_DN_Fire.cpp
// C++ implementation for Blueprint B_DN_Fire
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DN_Fire.json

#include "Blueprints/B_DN_Fire.h"

UB_DN_Fire::UB_DN_Fire()
{
	// Defense - Fire
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Defense.Fire"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Fire Defense"));
	StatInfo.Description = FText::FromString(TEXT("Reduces fire damage taken"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Defense doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

