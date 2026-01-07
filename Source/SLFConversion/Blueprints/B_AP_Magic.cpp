// B_AP_Magic.cpp
// C++ implementation for Blueprint B_AP_Magic
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_AP_Magic.json

#include "Blueprints/B_AP_Magic.h"

UB_AP_Magic::UB_AP_Magic()
{
	// Attack Power - Magic
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.AttackPower.Magic"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Magic Attack"));
	StatInfo.Description = FText::FromString(TEXT("Magic damage dealt by attacks"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Attack power doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

