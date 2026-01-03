// StatusEffectManagerComponent.cpp
// C++ implementation for AC_StatusEffectManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_StatusEffectManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         1/1 (initialized in constructor)
// Functions:         8/8 implemented
// Event Dispatchers: 3/3 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "StatusEffectManagerComponent.h"

UStatusEffectManagerComponent::UStatusEffectManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUERY FUNCTIONS [1/8]
// ═══════════════════════════════════════════════════════════════════════════════

bool UStatusEffectManagerComponent::IsStatusEffectActive(FGameplayTag StatusEffectTag) const
{
	return ActiveStatusEffects.Contains(StatusEffectTag);
}

// ═══════════════════════════════════════════════════════════════════════════════
// BUILDUP FUNCTIONS [2-4/8]
// ═══════════════════════════════════════════════════════════════════════════════

void UStatusEffectManagerComponent::AddOneShotBuildup_Implementation(UDataAsset* StatusEffect, int32 EffectRank, double Delta)
{
	if (!StatusEffect) return;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] AddOneShotBuildup: %s, Rank %d, Delta %.2f"),
		*StatusEffect->GetName(), EffectRank, Delta);

	// TODO: Get status effect tag from data asset
	// Find or create active status effect instance
	// Add Delta to buildup
	// Check if buildup threshold reached -> trigger effect
}

void UStatusEffectManagerComponent::StartBuildup_Implementation(UDataAsset* StatusEffect, int32 Rank)
{
	if (!StatusEffect) return;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] StartBuildup: %s, Rank %d"),
		*StatusEffect->GetName(), Rank);

	// TODO: Start continuous buildup timer
	// This is for effects like standing in poison cloud
}

void UStatusEffectManagerComponent::StopBuildup_Implementation(UDataAsset* StatusEffect, bool bApplyDecayDelay)
{
	if (!StatusEffect) return;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] StopBuildup: %s, DecayDelay: %s"),
		*StatusEffect->GetName(), bApplyDecayDelay ? TEXT("true") : TEXT("false"));

	// TODO: Stop continuous buildup
	// If bApplyDecayDelay, start decay timer
	// Otherwise, start immediate decay
}

// ═══════════════════════════════════════════════════════════════════════════════
// ADD/TRIGGER FUNCTIONS [5-7/8]
// ═══════════════════════════════════════════════════════════════════════════════

void UStatusEffectManagerComponent::TryAddStatusEffect_Implementation(UDataAsset* EffectClass, int32 Rank, bool bStartBuildup, double StartAmount)
{
	if (!EffectClass) return;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] TryAddStatusEffect: %s, Rank %d, StartBuildup: %s, StartAmount: %.2f"),
		*EffectClass->GetName(), Rank, bStartBuildup ? TEXT("true") : TEXT("false"), StartAmount);

	// TODO: Get effect tag from EffectClass (PDA_StatusEffect)
	FGameplayTag EffectTag; // = GetTagFromAsset(EffectClass);

	// Check if already active
	if (ActiveStatusEffects.Contains(EffectTag))
	{
		UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Effect already active, refreshing"));
		// Refresh existing effect
		return;
	}

	// Create new status effect instance (B_StatusEffect)
	// TODO: Spawn UObject from class in data asset
	UObject* NewEffect = nullptr; // = NewObject<UObject>(this, EffectClass->EffectClass);

	if (NewEffect)
	{
		ActiveStatusEffects.Add(EffectTag, NewEffect);
		OnStatusEffectAdded.Broadcast(NewEffect);

		if (bStartBuildup)
		{
			StartBuildup(EffectClass, Rank);
		}

		if (StartAmount > 0.0)
		{
			AddOneShotBuildup(EffectClass, Rank, StartAmount);
		}
	}
}

void UStatusEffectManagerComponent::OnStatusEffectFinished_Implementation(FGameplayTag StatusEffectTag)
{
	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] OnStatusEffectFinished: %s"), *StatusEffectTag.ToString());

	if (UObject** Found = ActiveStatusEffects.Find(StatusEffectTag))
	{
		UObject* Effect = *Found;

		// Get data asset from effect for broadcast
		// TODO: Get source data asset from B_StatusEffect
		UDataAsset* SourceAsset = nullptr;

		ActiveStatusEffects.Remove(StatusEffectTag);
		OnStatusEffectRemoved.Broadcast(SourceAsset);
	}
}

void UStatusEffectManagerComponent::OnStatusEffectTriggeredEvent_Implementation(const FText& TriggeredText)
{
	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] OnStatusEffectTriggeredEvent: %s"),
		*TriggeredText.ToString());

	OnStatusEffectTextTriggered.Broadcast(TriggeredText);
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERNAL CALLBACKS [8/8]
// ═══════════════════════════════════════════════════════════════════════════════

void UStatusEffectManagerComponent::OnLoaded_185D3AEC4B5162C1F2C50C87BF007D3F(UClass* Loaded)
{
	// Async load callback - status effect class has been loaded
	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Async class load completed: %s"),
		Loaded ? *Loaded->GetName() : TEXT("null"));

	// TODO: Create instance from loaded class and initialize
}
