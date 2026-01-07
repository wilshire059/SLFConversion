// B_Stance.cpp
// C++ implementation for Blueprint B_Stance
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Stance.json

#include "Blueprints/B_Stance.h"

UB_Stance::UB_Stance()
{
	// Stance is a backend stat (enemy poise break threshold)
	MinValue = 0.0;
	OnlyMaxValueRelevant = false;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Stance"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Stance"));
	StatInfo.Description = FText::FromString(TEXT("Enemy stance threshold before poise break"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = true;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// Stance regenerates slowly when not being attacked
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenInterval = 3.0f;
	StatInfo.RegenInfo.RegenPercent = 0.05f;
}

