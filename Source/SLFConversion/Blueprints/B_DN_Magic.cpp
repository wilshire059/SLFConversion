// B_DN_Magic.cpp
// C++ implementation for Blueprint B_DN_Magic
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DN_Magic.json

#include "Blueprints/B_DN_Magic.h"

UB_DN_Magic::UB_DN_Magic()
{
	// Defense - Magic
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Defense.Magic"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Magic Defense"));
	StatInfo.Description = FText::FromString(TEXT("Reduces magic damage taken"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Defense doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

