// B_Endurance.cpp
// C++ implementation for Blueprint B_Endurance
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Endurance.json

#include "Blueprints/B_Endurance.h"

UB_Endurance::UB_Endurance()
{
	// Endurance is a primary stat
	MinValue = 1.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Endurance"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Endurance"));
	StatInfo.Description = FText::FromString(TEXT("Attribute governing Stamina and Equip Load"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 10.0;

	// Primary stats don't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

