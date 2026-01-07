// B_AP_Holy.cpp
// C++ implementation for Blueprint B_AP_Holy
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_AP_Holy.json

#include "Blueprints/B_AP_Holy.h"

UB_AP_Holy::UB_AP_Holy()
{
	// Attack Power - Holy
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.AttackPower.Holy"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Holy Attack"));
	StatInfo.Description = FText::FromString(TEXT("Holy damage dealt by attacks"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Attack power doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

