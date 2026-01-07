// B_Intelligence.cpp
// C++ implementation for Blueprint B_Intelligence
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Intelligence.json

#include "Blueprints/B_Intelligence.h"

UB_Intelligence::UB_Intelligence()
{
	// Intelligence is a primary stat
	MinValue = 1.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Intelligence"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Intelligence"));
	StatInfo.Description = FText::FromString(TEXT("Attribute governing magic attack power"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 10.0;

	// Primary stats don't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

