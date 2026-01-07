// B_Action_TwoHandedStance_L.cpp
// C++ implementation for Blueprint B_Action_TwoHandedStance_L
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_TwoHandedStance_L.json

#include "Blueprints/B_Action_TwoHandedStance_L.h"
#include "Components/AC_EquipmentManager.h"

UB_Action_TwoHandedStance_L::UB_Action_TwoHandedStance_L()
{
}

void UB_Action_TwoHandedStance_L::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStance_L] ExecuteAction"));

	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (EquipMgr)
	{
		EquipMgr->AdjustForTwoHandStance(false);  // false = left hand
		UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStance_L] Called AdjustForTwoHandStance(false)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionTwoHandedStance_L] No EquipmentManager found"));
	}
}
