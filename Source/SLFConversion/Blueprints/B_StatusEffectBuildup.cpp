// B_StatusEffectBuildup.cpp
// C++ implementation for Blueprint B_StatusEffectBuildup
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_StatusEffectBuildup.json

#include "Blueprints/B_StatusEffectBuildup.h"

UB_StatusEffectBuildup::UB_StatusEffectBuildup()
{
	// Status Effect Buildup is a backend stat for tracking status accumulation
	MinValue = 0.0;
	OnlyMaxValueRelevant = false;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.StatusEffectBuildup"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Status Buildup"));
	StatInfo.Description = FText::FromString(TEXT("Current status effect accumulation"));
	StatInfo.bDisplayAsPercent = true;
	StatInfo.bShowMaxValue = true;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 100.0;

	// Status buildup decays over time
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenInterval = 1.0f;
	StatInfo.RegenInfo.RegenPercent = -0.02f;  // Negative = decay
}

