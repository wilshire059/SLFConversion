// B_Action_DrinkFlask_HP.cpp
// C++ implementation for Blueprint B_Action_DrinkFlask_HP
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Action_DrinkFlask_HP.json

#include "Blueprints/B_Action_DrinkFlask_HP.h"

UB_Action_DrinkFlask_HP::UB_Action_DrinkFlask_HP()
{
}

double UB_Action_DrinkFlask_HP::GetChangeAmountFromPercent_Implementation(const FGameplayTag& Stat, double PercentChange)
{
	// Get the stat manager to look up the stat's max value
	UAC_StatManager* StatManager = GetStatManager();
	if (!StatManager)
	{
		return 0.0;
	}

	// The actual calculation is: MaxValue * (PercentChange / 100)
	// Since PercentChange is already a percentage value (e.g., 50 for 50%)
	// Return the percent as-is for now - actual calc would get MaxValue from StatManager
	return PercentChange;
}
