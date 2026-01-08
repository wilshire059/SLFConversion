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
		UE_LOG(LogTemp, Warning, TEXT("  StatusEffect asset is null"));
		return;
	}

	// Get the status effect data asset
	UPDA_StatusEffect* StatusEffectAsset = Cast<UPDA_StatusEffect>(StatusEffect);
	if (!IsValid(StatusEffectAsset))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Failed to cast to UPDA_StatusEffect"));
		return;
	}

	// Get the tag from the asset
	FGameplayTag EffectTag = StatusEffectAsset->Tag;
	if (!EffectTag.IsValid())
	{
		// Fallback to creating tag from asset name
		EffectTag = FGameplayTag::RequestGameplayTag(StatusEffectAsset->AssetName, false);
	}

	// Check if already active - if so, just refresh the rank
	if (UB_StatusEffect** ExistingEffect = ActiveStatusEffects.Find(EffectTag))
	{
		if (IsValid(*ExistingEffect))
		{
			UE_LOG(LogTemp, Log, TEXT("  Status effect already active, refreshing rank: %s"), *EffectTag.ToString());
			(*ExistingEffect)->RefreshRank(Rank);
			(*ExistingEffect)->StartBuildup();
			return;
		}
	}

	// Create new B_StatusEffect instance
	UB_StatusEffect* NewEffect = NewObject<UB_StatusEffect>(this);
	if (!IsValid(NewEffect))
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to create UB_StatusEffect instance"));
		return;
	}

	// Initialize the effect
	NewEffect->Owner = GetOwner();
	NewEffect->Data = StatusEffect;
	NewEffect->Initialize(Rank);

	// Bind to events
	NewEffect->OnStatusEffectTriggered.AddDynamic(this, &UAC_StatusEffectManager::HandleStatusEffectTriggered);
	NewEffect->OnStatusEffectFinished.AddDynamic(this, &UAC_StatusEffectManager::HandleStatusEffectFinished);

	// Add to map
	ActiveStatusEffects.Add(EffectTag, NewEffect);
	ActiveStatusEffectTags.Add(EffectTag, EffectTag);

	// Start the buildup timer
	NewEffect->StartBuildup();

	// Broadcast that effect was added
	OnStatusEffectAdded.Broadcast(NewEffect);

	UE_LOG(LogTemp, Log, TEXT("  Created and started buildup for: %s"), *EffectTag.ToString());
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

	// Get tag from asset
	FGameplayTag EffectTag = StatusEffectAsset->Tag;
	if (!EffectTag.IsValid())
	{
		EffectTag = FGameplayTag::RequestGameplayTag(StatusEffectAsset->AssetName, false);
	}

	// Find and stop the active effect
	if (UB_StatusEffect** FoundEffect = ActiveStatusEffects.Find(EffectTag))
	{
		if (IsValid(*FoundEffect))
		{
			(*FoundEffect)->StopBuildup(ApplyDecayDelay);
		}
		UE_LOG(LogTemp, Log, TEXT("  Stopped buildup for: %s"), *EffectTag.ToString());
	}
}

/**
 * AddOneShotBuildup - Add one-shot status effect buildup (typically from weapon hits)
 */
void UAC_StatusEffectManager::AddOneShotBuildup_Implementation(UPrimaryDataAsset* StatusEffect, int32 EffectRank, double Delta)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatusEffectManager::AddOneShotBuildup - Rank: %d, Delta: %.2f"), EffectRank, Delta);

	if (!IsValid(StatusEffect))
	{
		UE_LOG(LogTemp, Warning, TEXT("  StatusEffect asset is null"));
		return;
	}

	UPDA_StatusEffect* StatusEffectAsset = Cast<UPDA_StatusEffect>(StatusEffect);
	if (!IsValid(StatusEffectAsset))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Failed to cast to UPDA_StatusEffect"));
		return;
	}

	// Get tag from asset
	FGameplayTag EffectTag = StatusEffectAsset->Tag;
	if (!EffectTag.IsValid())
	{
		EffectTag = FGameplayTag::RequestGameplayTag(StatusEffectAsset->AssetName, false);
	}

	// Check if effect already exists
	UB_StatusEffect** ExistingEffect = ActiveStatusEffects.Find(EffectTag);

	if (ExistingEffect && IsValid(*ExistingEffect))
	{
		// Effect already active - add to existing buildup
		UE_LOG(LogTemp, Log, TEXT("  Adding %.2f buildup to existing effect: %s"), Delta, *EffectTag.ToString());
		(*ExistingEffect)->RefreshRank(EffectRank);
		(*ExistingEffect)->AdjustBuildupOneshot(Delta);
	}
	else
	{
		// Create new effect
		UB_StatusEffect* NewEffect = NewObject<UB_StatusEffect>(this);
		if (!IsValid(NewEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("  Failed to create UB_StatusEffect instance"));
			return;
		}

		// Initialize the effect
		NewEffect->Owner = GetOwner();
		NewEffect->Data = StatusEffect;
		NewEffect->Initialize(EffectRank);

		// Bind to events
		NewEffect->OnStatusEffectTriggered.AddDynamic(this, &UAC_StatusEffectManager::HandleStatusEffectTriggered);
		NewEffect->OnStatusEffectFinished.AddDynamic(this, &UAC_StatusEffectManager::HandleStatusEffectFinished);

		// Add to map
		ActiveStatusEffects.Add(EffectTag, NewEffect);
		ActiveStatusEffectTags.Add(EffectTag, EffectTag);

		// Apply the one-shot buildup
		NewEffect->AdjustBuildupOneshot(Delta);

		// Broadcast that effect was added
		OnStatusEffectAdded.Broadcast(NewEffect);

		UE_LOG(LogTemp, Log, TEXT("  Created new effect with %.2f buildup: %s"), Delta, *EffectTag.ToString());
	}
}

/**
 * IsStatusEffectActive - Check if a status effect is currently active
 */
bool UAC_StatusEffectManager::IsStatusEffectActive_Implementation(const FGameplayTag& StatusEffectTag)
{
	if (UB_StatusEffect** FoundEffect = ActiveStatusEffects.Find(StatusEffectTag))
	{
		bool bActive = IsValid(*FoundEffect);
		UE_LOG(LogTemp, Verbose, TEXT("UAC_StatusEffectManager::IsStatusEffectActive - Tag: %s, Active: %s"),
			*StatusEffectTag.ToString(), bActive ? TEXT("true") : TEXT("false"));
		return bActive;
	}

	return false;
}

/**
 * GetStatusEffect - Get a specific status effect by tag
 */
UB_StatusEffect* UAC_StatusEffectManager::GetStatusEffect_Implementation(const FGameplayTag& StatusEffectTag)
{
	if (UB_StatusEffect** FoundEffect = ActiveStatusEffects.Find(StatusEffectTag))
	{
		return *FoundEffect;
	}
	return nullptr;
}

/**
 * GetAllActiveStatusEffects - Get all currently active status effects
 */
TArray<UB_StatusEffect*> UAC_StatusEffectManager::GetAllActiveStatusEffects_Implementation()
{
	TArray<UB_StatusEffect*> Result;
	for (auto& Pair : ActiveStatusEffects)
	{
		if (IsValid(Pair.Value))
		{
			Result.Add(Pair.Value);
		}
	}
	return Result;
}

/**
 * HandleStatusEffectTriggered - Internal handler when a status effect triggers
 */
void UAC_StatusEffectManager::HandleStatusEffectTriggered(FText TriggeredText)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatusEffectManager::HandleStatusEffectTriggered - %s"), *TriggeredText.ToString());
	OnStatusEffectTriggered.Broadcast(TriggeredText);
}

/**
 * HandleStatusEffectFinished - Internal handler when a status effect finishes
 */
void UAC_StatusEffectManager::HandleStatusEffectFinished(FGameplayTag StatusEffectTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_StatusEffectManager::HandleStatusEffectFinished - %s"), *StatusEffectTag.ToString());

	// Remove from maps
	if (UB_StatusEffect** FoundEffect = ActiveStatusEffects.Find(StatusEffectTag))
	{
		if (IsValid(*FoundEffect))
		{
			OnStatusEffectRemoved.Broadcast((*FoundEffect)->Data);
		}
		ActiveStatusEffects.Remove(StatusEffectTag);
	}
	ActiveStatusEffectTags.Remove(StatusEffectTag);
}
