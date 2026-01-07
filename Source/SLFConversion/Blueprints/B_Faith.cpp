// B_Faith.cpp
// C++ implementation for Blueprint B_Faith
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Faith.json

#include "Blueprints/B_Faith.h"

UB_Faith::UB_Faith()
{
	// Faith is a primary stat
	MinValue = 1.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Faith"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Faith"));
	StatInfo.Description = FText::FromString(TEXT("Attribute governing incantation power"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 10.0;

	// Primary stats don't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

