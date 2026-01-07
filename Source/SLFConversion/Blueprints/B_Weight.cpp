// B_Weight.cpp
// C++ implementation for Blueprint B_Weight
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Weight.json

#include "Blueprints/B_Weight.h"

UB_Weight::UB_Weight()
{
	// Weight is a miscellaneous stat (current/max for equip load)
	MinValue = 0.0;
	OnlyMaxValueRelevant = false;
	ShowMaxValueOnLevelUp = true;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Weight"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Equip Load"));
	StatInfo.Description = FText::FromString(TEXT("Current equipment weight vs maximum carry capacity"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = true;
	StatInfo.CurrentValue = 0.0;
	StatInfo.MaxValue = 50.0;

	// Weight doesn't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

