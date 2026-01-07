// B_Arcane.cpp
// C++ implementation for Blueprint B_Arcane
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Arcane.json

#include "Blueprints/B_Arcane.h"

UB_Arcane::UB_Arcane()
{
	// Arcane is a primary stat
	MinValue = 1.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Arcane"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Arcane"));
	StatInfo.Description = FText::FromString(TEXT("Attribute governing item discovery and certain weapon scaling"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 10.0;

	// Primary stats don't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

