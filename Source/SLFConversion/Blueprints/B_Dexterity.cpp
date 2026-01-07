// B_Dexterity.cpp
// C++ implementation for Blueprint B_Dexterity
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Dexterity.json

#include "Blueprints/B_Dexterity.h"

UB_Dexterity::UB_Dexterity()
{
	// Dexterity is a primary stat
	MinValue = 1.0;
	OnlyMaxValueRelevant = true;
	ShowMaxValueOnLevelUp = false;

	// Set default stat info
	StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("Stat.Dexterity"), false);
	StatInfo.DisplayName = FText::FromString(TEXT("Dexterity"));
	StatInfo.Description = FText::FromString(TEXT("Attribute governing weapon scaling and attack speed"));
	StatInfo.bDisplayAsPercent = false;
	StatInfo.bShowMaxValue = false;
	StatInfo.CurrentValue = 10.0;
	StatInfo.MaxValue = 10.0;

	// Primary stats don't regenerate
	StatInfo.RegenInfo.bCanRegenerate = false;
}

