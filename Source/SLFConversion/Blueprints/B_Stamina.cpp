// B_Stamina.cpp
// C++ implementation for Blueprint B_Stamina
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Stamina.json

#include "Blueprints/B_Stamina.h"

UB_Stamina::UB_Stamina()
{
	// Stamina is a resource stat with current/max values
	MinValue = 0.0;
	OnlyMaxValueRelevant = false;
	ShowMaxValueOnLevelUp = true;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Stamina"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Stamina"));
	StatInfo.Description = FText::FromString(TEXT("Stamina Points"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = true;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// Stamina regenerates quickly
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenInterval = 0.1f;
	StatInfo.RegenInfo.RegenPercent = 0.05f;
}

