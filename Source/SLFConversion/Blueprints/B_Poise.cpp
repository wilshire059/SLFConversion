// B_Poise.cpp
// C++ implementation for Blueprint B_Poise
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Poise.json

#include "Blueprints/B_Poise.h"

UB_Poise::UB_Poise()
{
	// Poise is a backend stat (hyper armor / stagger resistance)
	MinValue = 0.0;
	OnlyMaxValueRelevant = false;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Poise"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Poise"));
	StatInfo.Description = FText::FromString(TEXT("Resistance to stagger"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = true;
	StatInfo.CurrentValue = 50.0;
	StatInfo.MaxValue = 50.0;

	// Poise regenerates after being broken
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenInterval = 2.0f;
	StatInfo.RegenInfo.RegenPercent = 0.1f;
}

