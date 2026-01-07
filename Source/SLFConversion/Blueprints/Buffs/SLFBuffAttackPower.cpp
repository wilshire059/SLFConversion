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
#include "Components/StatManagerComponent.h"

USLFBuffAttackPower::USLFBuffAttackPower()
{
	UE_LOG(LogTemp, Log, TEXT("[BuffAttackPower] Initialized"));
}

void USLFBuffAttackPower::OnGranted_Implementation()
{
	Super::OnGranted_Implementation();

	UE_LOG(LogTemp, Log, TEXT("[BuffAttackPower] OnGranted - Applying attack power bonus"));

	// Get stat manager from owner and adjust attack power stat
	if (UStatManagerComponent* StatManager = GetOwnerStatManager())
	{
		// Get multiplier based on buff rank
		float Multiplier = GetMultiplierForCurrentRank();
		
		// Apply attack power bonus - using a generic attack power tag
		// The actual bonus value comes from the buff data multiplied by rank
		FGameplayTag AttackPowerTag = FGameplayTag::RequestGameplayTag(FName("Stat.AttackPower.Physical"), false);
		if (AttackPowerTag.IsValid())
		{
			// Adjust max value as a bonus
			StatManager->AdjustStat(AttackPowerTag, ESLFValueType::MaxValue, Multiplier, false, false);
			UE_LOG(LogTemp, Log, TEXT("[BuffAttackPower] Applied %.2f attack power bonus"), Multiplier);
		}
	}
}

void USLFBuffAttackPower::OnRemoved_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[BuffAttackPower] OnRemoved - Removing attack power bonus"));

	// Remove the bonus from stat manager
	if (UStatManagerComponent* StatManager = GetOwnerStatManager())
	{
		// Get multiplier based on buff rank
		float Multiplier = GetMultiplierForCurrentRank();
		
		// Remove attack power bonus
		FGameplayTag AttackPowerTag = FGameplayTag::RequestGameplayTag(FName("Stat.AttackPower.Physical"), false);
		if (AttackPowerTag.IsValid())
		{
			// Remove the bonus by negating
			StatManager->AdjustStat(AttackPowerTag, ESLFValueType::MaxValue, -Multiplier, false, false);
			UE_LOG(LogTemp, Log, TEXT("[BuffAttackPower] Removed %.2f attack power bonus"), Multiplier);
		}
	}

	Super::OnRemoved_Implementation();
}
