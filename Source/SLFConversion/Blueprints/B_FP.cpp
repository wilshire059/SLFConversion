// B_FP.cpp
// C++ implementation for Blueprint B_FP
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_FP.json

#include "Blueprints/B_FP.h"

UB_FP::UB_FP()
{
	// FP is a resource stat with current/max values
	MinValue = 0.0;
	OnlyMaxValueRelevant = false;
	ShowMaxValueOnLevelUp = true;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.FP"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("FP"));
	StatInfo.Description = FText::FromString(TEXT("Focus Points"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = true;
	StatInfo.CurrentValue = 50.0;
	StatInfo.MaxValue = 50.0;

	// FP can regenerate slowly
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenInterval = 1.0f;
	StatInfo.RegenInfo.RegenPercent = 0.005f;
}

