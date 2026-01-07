// B_DN_Holy.cpp
// C++ implementation for Blueprint B_DN_Holy
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DN_Holy.json

#include "Blueprints/B_DN_Holy.h"

UB_DN_Holy::UB_DN_Holy()
{
	// Defense - Holy
	MinValue = 0.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Defense.Holy"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Holy Defense"));
	StatInfo.Description = FText::FromString(TEXT("Reduces holy damage taken"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 0.0;

	// Defense doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

