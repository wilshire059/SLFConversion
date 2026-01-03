// SLFBuffAttackPower.cpp
// C++ implementation for B_Buff_AttackPower
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Buff_AttackPower
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from BuffBase)
// Functions:         2/2 (OnGranted, OnRemoved)
// Event Dispatchers: 0/0
// Logic:             Adds/removes attack power modifier via StatManager
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFBuffAttackPower.h"

USLFBuffAttackPower::USLFBuffAttackPower()
{
	UE_LOG(LogTemp, Log, TEXT("[BuffAttackPower] Initialized"));
}

void USLFBuffAttackPower::OnGranted_Implementation()
{
	Super::OnGranted_Implementation();

	UE_LOG(LogTemp, Log, TEXT("[BuffAttackPower] OnGranted - Applying attack power bonus"));

	// Get stat manager from owner and adjust attack power stat
	// TODO: Get StatManager->AdjustStat(AttackPowerTag, +BonusAmount)
}

void USLFBuffAttackPower::OnRemoved_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[BuffAttackPower] OnRemoved - Removing attack power bonus"));

	// Remove the bonus from stat manager
	// TODO: Get StatManager->AdjustStat(AttackPowerTag, -BonusAmount)

	Super::OnRemoved_Implementation();
}
