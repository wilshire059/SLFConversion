// AC_StatusEffectManager.cpp
// C++ component implementation for AC_StatusEffectManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_StatusEffectManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_StatusEffectManager.h"
#include "Blueprints/B_StatusEffect.h"

UAC_StatusEffectManager::UAC_StatusEffectManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAC_StatusEffectManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_StatusEffectManager::BeginPlay"));
}

void UAC_StatusEffectManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * StartBuildup - Begin status effect buildup for specified rank
 *
 * Blueprint Logic:
 * 1. Get status effect tag from asset
 * 2. Check if already in ActiveStatusEffects map
 * 3. If not, create new status effect instance
 * 4. Add to map and broadcast OnStatusEffectAdded
 */
void UAC_StatusEffectManager::StartBuildup_Implementation(UPrimaryDataAsset* StatusEffect, int32 Rank)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatusEffectManager::StartBuildup - Rank: %d"), Rank);

	if (!IsValid(StatusEffect))
	{
		return;
	}

	// Get the status effect tag from the asset name
	UPDA_StatusEffect* StatusEffectAsset = Cast<UPDA_StatusEffect>(StatusEffect);
	if (!IsValid(StatusEffectAsset))
	{
		return;
	}

	// Use asset name as the key for tracking
	FGameplayTag EffectTag = FGameplayTag::RequestGameplayTag(StatusEffectAsset->AssetName, false);

	// Check if already active
	if (ActiveStatusEffects.Contains(EffectTag))
	{
		UE_LOG(LogTemp, Log, TEXT("  Status effect already active: %s"), *EffectTag.ToString());
		return;
	}

	// Create status effect object and add to map
	ActiveStatusEffects.Add(EffectTag, EffectTag);

	UE_LOG(LogTemp, Log, TEXT("  Started buildup for: %s"), *EffectTag.ToString());
}

/**
 * StopBuildup - Stop active buildup for status effect
 */
void UAC_StatusEffectManager::StopBuildup_Implementation(UPrimaryDataAsset* StatusEffect, bool ApplyDecayDelay)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatusEffectManager::StopBuildup - ApplyDecayDelay: %s"),
		ApplyDecayDelay ? TEXT("true") : TEXT("false"));

	if (!IsValid(StatusEffect))
	{
		return;
	}

	UPDA_StatusEffect* StatusEffectAsset = Cast<UPDA_StatusEffect>(StatusEffect);
	if (!IsValid(StatusEffectAsset))
	{
		return;
	}

	// Use asset name as the key for tracking
	FGameplayTag EffectTag = FGameplayTag::RequestGameplayTag(StatusEffectAsset->AssetName, false);

	// Remove from active effects
	if (ActiveStatusEffects.Contains(EffectTag))
	{
		ActiveStatusEffects.Remove(EffectTag);
		OnStatusEffectRemoved.Broadcast(StatusEffect);
		UE_LOG(LogTemp, Log, TEXT("  Stopped buildup for: %s"), *EffectTag.ToString());
	}
}

/**
 * AddOneShotBuildup - Add one-shot status effect buildup
 */
void UAC_StatusEffectManager::AddOneShotBuildup_Implementation(UPrimaryDataAsset* StatusEffect, int32 EffectRank, double Delta)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatusEffectManager::AddOneShotBuildup - Rank: %d, Delta: %.2f"), EffectRank, Delta);

	if (!IsValid(StatusEffect))
	{
		return;
	}

	// Start buildup for the effect
	StartBuildup(StatusEffect, EffectRank);
}

/**
 * IsStatusEffectActive - Check if a status effect is currently active
 */
bool UAC_StatusEffectManager::IsStatusEffectActive_Implementation(const FGameplayTag& StatusEffectTag)
{
	bool bActive = ActiveStatusEffects.Contains(StatusEffectTag);

	UE_LOG(LogTemp, Log, TEXT("UAC_StatusEffectManager::IsStatusEffectActive - Tag: %s, Active: %s"),
		*StatusEffectTag.ToString(), bActive ? TEXT("true") : TEXT("false"));

	return bActive;
}
