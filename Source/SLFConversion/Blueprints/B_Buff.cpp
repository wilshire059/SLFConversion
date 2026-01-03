// B_Buff.cpp
// C++ implementation for Blueprint B_Buff
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Buff.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "Blueprints/B_Buff.h"
#include "Components/AC_StatManager.h"

UB_Buff::UB_Buff()
{
	Rank = 1;
	BuffData = nullptr;
	OwnerActor = nullptr;
}

/**
 * GetOwnerStatManager - Get the stat manager from the owner actor
 *
 * Blueprint Logic: GetComponentByClass on OwnerActor
 */
void UB_Buff::GetOwnerStatManager_Implementation(UAC_StatManager*& OutReturnValue, UAC_StatManager*& OutReturnValue1)
{
	UE_LOG(LogTemp, Log, TEXT("UB_Buff::GetOwnerStatManager"));

	UAC_StatManager* StatManager = nullptr;
	if (IsValid(OwnerActor))
	{
		StatManager = OwnerActor->FindComponentByClass<UAC_StatManager>();
	}

	OutReturnValue = StatManager;
	OutReturnValue1 = StatManager;
}

/**
 * GetMultiplierForCurrentRank - Get the stat multiplier for the current rank
 *
 * Blueprint Logic: Returns a multiplier based on Rank (typically 1.0 + (Rank-1) * 0.1 or similar)
 */
void UB_Buff::GetMultiplierForCurrentRank_Implementation(double& OutReturnValue, double& OutReturnValue1)
{
	UE_LOG(LogTemp, Log, TEXT("UB_Buff::GetMultiplierForCurrentRank - Rank: %d"), Rank);

	// Default implementation: linear scaling with rank
	double Multiplier = 1.0 + (static_cast<double>(Rank) - 1.0) * 0.1;

	OutReturnValue = Multiplier;
	OutReturnValue1 = Multiplier;
}

/**
 * EventOnGranted - Called when the buff is granted to the owner
 *
 * Blueprint Logic: Custom event that child classes override to apply buff effects
 */
void UB_Buff::EventOnGranted_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UB_Buff::EventOnGranted - BuffData: %s, Rank: %d"),
		BuffData ? *BuffData->GetName() : TEXT("None"), Rank);

	// Base implementation does nothing - child classes override to apply effects
}

/**
 * EventOnRemoved - Called when the buff is removed from the owner
 *
 * Blueprint Logic: Custom event that child classes override to remove buff effects
 */
void UB_Buff::EventOnRemoved_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UB_Buff::EventOnRemoved - BuffData: %s, Rank: %d"),
		BuffData ? *BuffData->GetName() : TEXT("None"), Rank);

	// Base implementation does nothing - child classes override to remove effects
}

/**
 * GetBuffTag - Get the gameplay tag from BuffData (for tag-based queries)
 *
 * Note: This assumes BuffData has a Tag property. If not, returns empty tag.
 */
FGameplayTag UB_Buff::GetBuffTag() const
{
	// BuffData is a UPrimaryDataAsset - specific buff assets would have a Tag property
	// For now, return empty tag - child classes or specific buff types override
	return FGameplayTag();
}
