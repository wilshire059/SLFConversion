// B_IncantationPower.cpp
// C++ implementation for Blueprint B_IncantationPower
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_IncantationPower.json

#include "Blueprints/B_IncantationPower.h"

UB_IncantationPower::UB_IncantationPower()
{
	// Incantation Power is a backend scaling stat
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.IncantationPower"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Incantation Power"));
	StatInfo.Description = FText::FromString(TEXT("Scaling for incantation damage and effects"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// Backend scaling stat doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

