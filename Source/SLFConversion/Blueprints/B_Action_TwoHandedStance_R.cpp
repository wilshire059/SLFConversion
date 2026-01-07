// B_Action_TwoHandedStance_R.cpp
// C++ implementation for Blueprint B_Action_TwoHandedStance_R
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_TwoHandedStance_R.json

#include "Blueprints/B_Action_TwoHandedStance_R.h"
#include "Components/AC_EquipmentManager.h"

UB_Action_TwoHandedStance_R::UB_Action_TwoHandedStance_R()
{
}

void UB_Action_TwoHandedStance_R::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStance_R] ExecuteAction"));

	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (EquipMgr)
	{
		EquipMgr->AdjustForTwoHandStance(true);  // true = right hand
		UE_LOG(LogTemp, Log, TEXT("[ActionTwoHandedStance_R] Called AdjustForTwoHandStance(true)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionTwoHandedStance_R] No EquipmentManager found"));
	}
}
