// B_DN_Physical.cpp
// C++ implementation for Blueprint B_DN_Physical
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DN_Physical.json

#include "Blueprints/B_DN_Physical.h"

UB_DN_Physical::UB_DN_Physical()
{
	// Defense - Physical
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Defense.Physical"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Physical Defense"));
	StatInfo.Description = FText::FromString(TEXT("Reduces physical damage taken"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// Defense doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

