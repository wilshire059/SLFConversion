// B_HP.cpp
// C++ implementation for Blueprint B_HP
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_HP.json

#include "Blueprints/B_HP.h"

UB_HP::UB_HP()
{
	// HP is a resource stat with current/max values
	MinValue = 0.0;
	OnlyMaxValueRelevant = false;
	ShowMaxValueOnLevelUp = true;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.HP"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("HP"));
	StatInfo.Description = FText::FromString(TEXT("Health Points"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = true;
	StatInfo.CurrentValue = 100.0;
	StatInfo.MaxValue = 100.0;

	// HP can regenerate
	StatInfo.RegenInfo.bCanRegenerate = true;
	StatInfo.RegenInfo.RegenInterval = 1.0f;
	StatInfo.RegenInfo.RegenPercent = 0.01f;
}

