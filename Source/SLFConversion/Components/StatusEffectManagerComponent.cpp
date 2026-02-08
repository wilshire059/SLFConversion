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
#include "SLFPrimaryDataAssets.h"
#include "TimerManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/StatManagerComponent.h"
#include "Curves/CurveFloat.h"

UStatusEffectManagerComponent::UStatusEffectManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

FGameplayTag UStatusEffectManagerComponent::GetTagFromStatusEffectAsset(UDataAsset* StatusEffect) const
{
	if (UPDA_StatusEffect* EffectData = Cast<UPDA_StatusEffect>(StatusEffect))
	{
		return EffectData->Tag;
	}
	return FGameplayTag();
}

double UStatusEffectManagerComponent::GetOwnerResistanceStatValue(const FGameplayTag& ResistiveStatTag) const
{
	if (!ResistiveStatTag.IsValid())
	{
		return 0.0;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return 0.0;
	}

	// Find stat manager on owner
	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (!StatManager)
	{
		return 0.0;
	}

	// Get the stat value for the resistance tag
	UObject* StatObj = nullptr;
	FStatInfo StatInfo;
	StatManager->GetStat(ResistiveStatTag, StatObj, StatInfo);

	return StatInfo.CurrentValue;
}

void UStatusEffectManagerComponent::TriggerStatusEffect(const FGameplayTag& EffectTag, int32 Rank)
{
	FSLFStatusEffectBuildupState* State = BuildupStates.Find(EffectTag);
	if (!State || !State->SourceAsset)
	{
		return;
	}

	UPDA_StatusEffect* EffectData = Cast<UPDA_StatusEffect>(State->SourceAsset);
	if (!EffectData)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] TriggerStatusEffect: %s at Rank %d"),
		*EffectTag.ToString(), Rank);

	// Broadcast triggered text
	if (!EffectData->TriggeredText.IsEmpty())
	{
		OnStatusEffectTriggeredEvent(EffectData->TriggeredText);
	}

	// Load and spawn the effect class
	if (!EffectData->Effect.IsNull())
	{
		TSoftClassPtr<UObject> EffectClass = EffectData->Effect;

		if (EffectClass.IsValid())
		{
			// Class already loaded - spawn immediately
			UClass* LoadedClass = EffectClass.Get();
			if (LoadedClass)
			{
				UObject* NewEffect = NewObject<UObject>(this, LoadedClass);
				if (NewEffect)
				{
					ActiveStatusEffects.Add(EffectTag, NewEffect);
					OnStatusEffectAdded.Broadcast(NewEffect);
					UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Spawned effect instance: %s"), *NewEffect->GetName());
				}
			}
		}
		else
		{
			// Need to async load the class
			FString ClassPath = EffectClass.ToString();
			PendingAsyncLoads.Add(ClassPath, EffectTag);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			StreamableManager.RequestAsyncLoad(
				EffectClass.ToSoftObjectPath(),
				FStreamableDelegate::CreateUObject(this, &UStatusEffectManagerComponent::OnLoaded_185D3AEC4B5162C1F2C50C87BF007D3F, (UClass*)nullptr)
			);
		}
	}

	// Reset buildup after triggering
	if (!EffectData->bCanRetrigger)
	{
		State->CurrentBuildup = 0.0;
	}
}

void UStatusEffectManagerComponent::OnBuildupTimerTick(FGameplayTag EffectTag)
{
	FSLFStatusEffectBuildupState* State = BuildupStates.Find(EffectTag);
	if (!State || !State->bIsBuildingUp)
	{
		return;
	}

	UPDA_StatusEffect* EffectData = Cast<UPDA_StatusEffect>(State->SourceAsset);
	if (!EffectData)
	{
		return;
	}

	// Add buildup based on rate (called every 0.1 seconds)
	double BuildupAmount = EffectData->BaseBuildupRate * 0.1;
	State->CurrentBuildup = FMath::Min(State->CurrentBuildup + BuildupAmount, State->MaxBuildup);

	UE_LOG(LogTemp, Verbose, TEXT("[StatusEffectManager] Buildup tick: %s - %.1f/%.1f"),
		*EffectTag.ToString(), State->CurrentBuildup, State->MaxBuildup);

	// Check if threshold reached
	if (State->CurrentBuildup >= State->MaxBuildup)
	{
		TriggerStatusEffect(EffectTag, State->Rank);
	}
}

void UStatusEffectManagerComponent::OnDecayTimerTick(FGameplayTag EffectTag)
{
	FSLFStatusEffectBuildupState* State = BuildupStates.Find(EffectTag);
	if (!State || State->bIsBuildingUp)
	{
		return;
	}

	UPDA_StatusEffect* EffectData = Cast<UPDA_StatusEffect>(State->SourceAsset);
	if (!EffectData)
	{
		return;
	}

	// Decay buildup based on rate (called every 0.1 seconds)
	double DecayAmount = EffectData->BaseDecayRate * 0.1;
	State->CurrentBuildup = FMath::Max(State->CurrentBuildup - DecayAmount, 0.0);

	UE_LOG(LogTemp, Verbose, TEXT("[StatusEffectManager] Decay tick: %s - %.1f/%.1f"),
		*EffectTag.ToString(), State->CurrentBuildup, State->MaxBuildup);

	// Stop decay timer when buildup reaches zero
	if (State->CurrentBuildup <= 0.0)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(State->DecayTimerHandle);
		}
		BuildupStates.Remove(EffectTag);
	}
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

	// Get status effect tag from data asset (IMPLEMENTED - was TODO line 39)
	FGameplayTag EffectTag = GetTagFromStatusEffectAsset(StatusEffect);
	if (!EffectTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusEffectManager] AddOneShotBuildup: Invalid tag from %s"), *StatusEffect->GetName());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] AddOneShotBuildup: %s, Rank %d, RawDelta %.2f"),
		*EffectTag.ToString(), EffectRank, Delta);

	// Apply resistance reduction from owner's resistive stat
	double AdjustedDelta = Delta;
	UPDA_StatusEffect* EffectData = Cast<UPDA_StatusEffect>(StatusEffect);
	if (EffectData && EffectData->ResistiveStat.IsValid())
	{
		double OwnerResistance = GetOwnerResistanceStatValue(EffectData->ResistiveStat);
		if (OwnerResistance > 0.0)
		{
			if (EffectData->ResistiveStatCurve)
			{
				// Use curve if available (X = resistance stat, Y = multiplier)
				float CurveMultiplier = EffectData->ResistiveStatCurve->GetFloatValue(static_cast<float>(OwnerResistance));
				AdjustedDelta = Delta * FMath::Clamp(CurveMultiplier, 0.0f, 1.0f);
				UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Applied curve reduction: Resistance=%.1f, Multiplier=%.3f, AdjustedDelta=%.2f"),
					OwnerResistance, CurveMultiplier, AdjustedDelta);
			}
			else
			{
				// Fallback formula: 100 / (100 + resistance) - at 100 resistance, 50% buildup
				double ResistanceFactor = 100.0 / (100.0 + OwnerResistance);
				AdjustedDelta = Delta * ResistanceFactor;
				UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Applied formula reduction: Resistance=%.1f, Factor=%.3f, AdjustedDelta=%.2f"),
					OwnerResistance, ResistanceFactor, AdjustedDelta);
			}
		}
	}

	// Find or create buildup state
	FSLFStatusEffectBuildupState* State = BuildupStates.Find(EffectTag);
	if (!State)
	{
		FSLFStatusEffectBuildupState NewState;
		NewState.SourceAsset = StatusEffect;
		NewState.Rank = EffectRank;
		NewState.MaxBuildup = 100.0; // Default threshold
		BuildupStates.Add(EffectTag, NewState);
		State = BuildupStates.Find(EffectTag);
	}

	// Add adjusted delta to buildup
	State->CurrentBuildup = FMath::Min(State->CurrentBuildup + AdjustedDelta, State->MaxBuildup);

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Buildup now: %.1f/%.1f"),
		State->CurrentBuildup, State->MaxBuildup);

	// Check if threshold reached -> trigger effect
	if (State->CurrentBuildup >= State->MaxBuildup)
	{
		TriggerStatusEffect(EffectTag, EffectRank);
	}
	else
	{
		// Threshold not reached - start decay timer after delay
		// This ensures the status effect bar eventually disappears if not triggered
		UWorld* World = GetWorld();
		if (World)
		{
			// Clear any existing timers
			World->GetTimerManager().ClearTimer(State->BuildupTimerHandle);
			World->GetTimerManager().ClearTimer(State->DecayTimerHandle);

			// Get decay delay from data asset (default 2.0 seconds)
			double DecayDelay = 2.0;
			if (EffectData)
			{
				DecayDelay = EffectData->DecayDelay > 0.0 ? EffectData->DecayDelay : 2.0;
			}

			// Start decay timer after delay
			State->bIsBuildingUp = false;
			FTimerDelegate DecayDelegate;
			DecayDelegate.BindUObject(this, &UStatusEffectManagerComponent::OnDecayTimerTick, EffectTag);
			World->GetTimerManager().SetTimer(State->DecayTimerHandle, DecayDelegate, 0.1f, true, DecayDelay);

			UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Started decay timer for %s (delay: %.1fs)"),
				*EffectTag.ToString(), DecayDelay);
		}
	}
}

void UStatusEffectManagerComponent::StartBuildup_Implementation(UDataAsset* StatusEffect, int32 Rank)
{
	if (!StatusEffect) return;

	FGameplayTag EffectTag = GetTagFromStatusEffectAsset(StatusEffect);
	if (!EffectTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusEffectManager] StartBuildup: Invalid tag from %s"), *StatusEffect->GetName());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] StartBuildup: %s, Rank %d"),
		*EffectTag.ToString(), Rank);

	// Find or create buildup state
	FSLFStatusEffectBuildupState* State = BuildupStates.Find(EffectTag);
	if (!State)
	{
		FSLFStatusEffectBuildupState NewState;
		NewState.SourceAsset = StatusEffect;
		NewState.Rank = Rank;
		NewState.MaxBuildup = 100.0;
		BuildupStates.Add(EffectTag, NewState);
		State = BuildupStates.Find(EffectTag);
	}

	// Start continuous buildup timer (IMPLEMENTED - was TODO line 52)
	State->bIsBuildingUp = true;

	// Clear any existing decay timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(State->DecayTimerHandle);

		// Start buildup timer - ticks every 0.1 seconds
		FTimerDelegate BuildupDelegate;
		BuildupDelegate.BindUObject(this, &UStatusEffectManagerComponent::OnBuildupTimerTick, EffectTag);
		World->GetTimerManager().SetTimer(State->BuildupTimerHandle, BuildupDelegate, 0.1f, true);
	}
}

void UStatusEffectManagerComponent::StopBuildup_Implementation(UDataAsset* StatusEffect, bool bApplyDecayDelay)
{
	if (!StatusEffect) return;

	FGameplayTag EffectTag = GetTagFromStatusEffectAsset(StatusEffect);
	if (!EffectTag.IsValid())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] StopBuildup: %s, DecayDelay: %s"),
		*EffectTag.ToString(), bApplyDecayDelay ? TEXT("true") : TEXT("false"));

	FSLFStatusEffectBuildupState* State = BuildupStates.Find(EffectTag);
	if (!State)
	{
		return;
	}

	// Stop continuous buildup (IMPLEMENTED - was TODO line 63)
	State->bIsBuildingUp = false;

	UWorld* World = GetWorld();
	if (!World) return;

	// Clear buildup timer
	World->GetTimerManager().ClearTimer(State->BuildupTimerHandle);

	// Get decay delay from data asset
	double DecayDelay = 0.0;
	if (bApplyDecayDelay)
	{
		if (UPDA_StatusEffect* EffectData = Cast<UPDA_StatusEffect>(StatusEffect))
		{
			DecayDelay = EffectData->DecayDelay;
		}
	}

	// Start decay timer after delay
	FTimerDelegate DecayDelegate;
	DecayDelegate.BindUObject(this, &UStatusEffectManagerComponent::OnDecayTimerTick, EffectTag);
	World->GetTimerManager().SetTimer(State->DecayTimerHandle, DecayDelegate, 0.1f, true, DecayDelay);
}

// ═══════════════════════════════════════════════════════════════════════════════
// ADD/TRIGGER FUNCTIONS [5-7/8]
// ═══════════════════════════════════════════════════════════════════════════════

void UStatusEffectManagerComponent::TryAddStatusEffect_Implementation(UDataAsset* EffectClass, int32 Rank, bool bStartBuildup, double StartAmount)
{
	if (!EffectClass) return;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] TryAddStatusEffect: %s, Rank %d, StartBuildup: %s, StartAmount: %.2f"),
		*EffectClass->GetName(), Rank, bStartBuildup ? TEXT("true") : TEXT("false"), StartAmount);

	// Get effect tag from EffectClass (PDA_StatusEffect) (IMPLEMENTED - was TODO line 79)
	FGameplayTag EffectTag = GetTagFromStatusEffectAsset(EffectClass);
	if (!EffectTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusEffectManager] TryAddStatusEffect: Invalid tag from %s"), *EffectClass->GetName());
		return;
	}

	// Check if already active
	if (ActiveStatusEffects.Contains(EffectTag))
	{
		UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Effect already active, refreshing"));
		// Could refresh existing effect here if needed
		return;
	}

	// Create new status effect instance (B_StatusEffect) (IMPLEMENTED - was TODO line 91)
	UPDA_StatusEffect* EffectData = Cast<UPDA_StatusEffect>(EffectClass);
	if (!EffectData)
	{
		return;
	}

	UObject* NewEffect = nullptr;
	if (!EffectData->Effect.IsNull())
	{
		if (EffectData->Effect.IsValid())
		{
			// Class already loaded
			UClass* LoadedClass = EffectData->Effect.Get();
			if (LoadedClass)
			{
				NewEffect = NewObject<UObject>(this, LoadedClass);
			}
		}
		else
		{
			// Async load the class
			FString ClassPath = EffectData->Effect.ToString();
			PendingAsyncLoads.Add(ClassPath, EffectTag);

			// Store the source asset for later
			FSLFStatusEffectBuildupState NewState;
			NewState.SourceAsset = EffectClass;
			NewState.Rank = Rank;
			BuildupStates.Add(EffectTag, NewState);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			StreamableManager.RequestAsyncLoad(
				EffectData->Effect.ToSoftObjectPath(),
				FStreamableDelegate::CreateUObject(this, &UStatusEffectManagerComponent::OnLoaded_185D3AEC4B5162C1F2C50C87BF007D3F, (UClass*)nullptr)
			);
		}
	}

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

		// Get source data asset from buildup state (IMPLEMENTED - was TODO line 120)
		UDataAsset* SourceAsset = nullptr;
		if (FSLFStatusEffectBuildupState* State = BuildupStates.Find(StatusEffectTag))
		{
			SourceAsset = State->SourceAsset;
		}

		ActiveStatusEffects.Remove(StatusEffectTag);
		BuildupStates.Remove(StatusEffectTag);
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
	// (IMPLEMENTED - was TODO line 146)

	if (!Loaded)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusEffectManager] Async class load completed but class is null"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Async class load completed: %s"), *Loaded->GetName());

	// Find the effect tag for this loaded class
	FString LoadedClassPath = Loaded->GetPathName();
	FGameplayTag* FoundTag = nullptr;

	for (auto& Pair : PendingAsyncLoads)
	{
		if (LoadedClassPath.Contains(Pair.Key) || Pair.Key.Contains(LoadedClassPath))
		{
			FoundTag = &Pair.Value;
			break;
		}
	}

	if (!FoundTag || !FoundTag->IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusEffectManager] Could not find pending load for class: %s"), *LoadedClassPath);
		return;
	}

	FGameplayTag EffectTag = *FoundTag;
	PendingAsyncLoads.Remove(LoadedClassPath);

	// Create instance from loaded class and initialize
	UObject* NewEffect = NewObject<UObject>(this, Loaded);
	if (NewEffect)
	{
		ActiveStatusEffects.Add(EffectTag, NewEffect);
		OnStatusEffectAdded.Broadcast(NewEffect);
		UE_LOG(LogTemp, Log, TEXT("[StatusEffectManager] Created effect instance from async load: %s"), *NewEffect->GetName());
	}
}
