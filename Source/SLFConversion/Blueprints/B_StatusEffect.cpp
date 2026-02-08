// B_StatusEffect.cpp
// C++ implementation for Blueprint B_StatusEffect
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_StatusEffect.json
// Logic traced from JSON node-by-node

#include "Blueprints/B_StatusEffect.h"
#include "Blueprints/B_Stat.h"
#include "Components/AC_StatManager.h"
#include "Components/StatManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/SkeletalMeshComponent.h"

UB_StatusEffect::UB_StatusEffect()
{
	// Initialize default values from Blueprint
	Owner = nullptr;
	Data = nullptr;
	bIsTriggered = false;
	ActiveRank = 0;
	OwnerResistiveStatValue = 0.0;
	BuildupPercent = 0.0;
	EffectDuration = 0.0;
	EffectSteps = 0.0;

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::Constructor - Initialized"));
}

// ═══════════════════════════════════════════════════════════════════════════
// PURE GETTER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

UAC_StatManager* UB_StatusEffect::GetOwnerStatManager_Implementation()
{
	// Logic from JSON GetOwnerStatManager graph:
	// 1. Get Owner variable
	// 2. Call Owner->GetComponentByClass<UAC_StatManager>()
	// 3. Is Valid check on result
	// 4. If valid: return the component
	// 5. If not valid: return nullptr

	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetOwnerStatManager - Owner is null"));
		return nullptr;
	}

	// First try UAC_StatManager (Blueprint-based)
	UAC_StatManager* StatManager = Owner->FindComponentByClass<UAC_StatManager>();

	if (IsValid(StatManager))
	{
		UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetOwnerStatManager - Found UAC_StatManager"));
		return StatManager;
	}

	UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetOwnerStatManager - UAC_StatManager not found, will use UStatManagerComponent fallback"));
	return nullptr;
}

// Helper to apply stat change using either UAC_StatManager or UStatManagerComponent
bool UB_StatusEffect::TryAdjustOwnerStat(FGameplayTag StatTag, ESLFValueType ValueType, double Amount, bool bLevelUp, bool bTriggerRegen)
{
	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryAdjustOwnerStat - Owner is null"));
		return false;
	}

	// First try UAC_StatManager
	UAC_StatManager* AC_StatMgr = Owner->FindComponentByClass<UAC_StatManager>();
	if (IsValid(AC_StatMgr))
	{
		AC_StatMgr->AdjustStat(StatTag, ValueType, Amount, bLevelUp, bTriggerRegen);
		UE_LOG(LogTemp, Log, TEXT("TryAdjustOwnerStat - Applied via UAC_StatManager: %s = %.2f"), *StatTag.ToString(), Amount);
		return true;
	}

	// Fallback to UStatManagerComponent (C++ component)
	UStatManagerComponent* StatMgrComp = Owner->FindComponentByClass<UStatManagerComponent>();
	if (IsValid(StatMgrComp))
	{
		StatMgrComp->AdjustStat(StatTag, ValueType, Amount, bLevelUp, bTriggerRegen);
		UE_LOG(LogTemp, Log, TEXT("TryAdjustOwnerStat - Applied via UStatManagerComponent: %s = %.2f"), *StatTag.ToString(), Amount);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("TryAdjustOwnerStat - No StatManager found on Owner %s"), *Owner->GetName());
	return false;
}

double UB_StatusEffect::GetBuildupPercent_Implementation()
{
	// Logic from JSON GetBuildupPercent graph:
	// 1. Get Data variable with validation
	// 2. If Data is valid: return BuildupPercent / 100.0
	// 3. If Data is not valid: return 0.0

	if (IsValid(Data))
	{
		double Result = BuildupPercent / 100.0;
		UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetBuildupPercent - Returning %f"), Result);
		return Result;
	}

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetBuildupPercent - Data invalid, returning 0.0"));
	return 0.0;
}

UPrimaryDataAsset* UB_StatusEffect::GetEffectData_Implementation()
{
	// Logic from JSON GetEffectData graph:
	// 1. Get Data variable with validation
	// 2. If Data is valid: return Data
	// 3. If Data is not valid: return nullptr

	if (IsValid(Data))
	{
		UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetEffectData - Returning Data"));
		return Data;
	}

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetEffectData - Data invalid, returning nullptr"));
	return nullptr;
}

TMap<int32, FSLFStatusEffectRankInfo> UB_StatusEffect::GetEffectRankData_Implementation()
{
	// Logic from JSON GetEffectRankData graph:
	// 1. Check if Data is valid
	// 2. If valid: return Data->RankInfo (TMap<int32, FSLFStatusEffectRankInfo>)
	// 3. If not valid: return empty map

	TMap<int32, FSLFStatusEffectRankInfo> EmptyMap;

	if (!IsValid(Data))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetEffectRankData - Data invalid, returning empty map"));
		return EmptyMap;
	}

	// Cast Data to UPDA_StatusEffect to access RankInfo property
	if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
	{
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::GetEffectRankData - Returning RankInfo with %d ranks"),
			StatusData->RankInfo.Num());
		return StatusData->RankInfo;
	}

	UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetEffectRankData - Data cast failed, returning empty map"));
	return EmptyMap;
}

FText UB_StatusEffect::GetTriggeredText_Implementation()
{
	// Logic from JSON GetTriggeredText graph:
	// 1. Get Data->TriggeredText
	// 2. If TriggeredText is empty:
	//    a. Get Data->Tag (FGameplayTag)
	//    b. GetTagName() → convert to string
	//    c. FindSubstring(".") → get index
	//    d. RightChop(index + 1) → get text after last "."
	//    e. Convert to FText
	// 3. Return result (either TriggeredText or derived text)

	if (!IsValid(Data))
	{
		return FText::GetEmpty();
	}

	// Cast Data to UPDA_StatusEffect to access TriggeredText and Tag
	if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
	{
		// If TriggeredText is not empty, return it
		if (!StatusData->TriggeredText.IsEmpty())
		{
			return StatusData->TriggeredText;
		}

		// Otherwise derive text from Tag
		if (StatusData->Tag.IsValid())
		{
			FString TagString = StatusData->Tag.ToString();
			int32 DotIndex = TagString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			if (DotIndex != INDEX_NONE)
			{
				FString DerivedName = TagString.RightChop(DotIndex + 1);
				return FText::FromString(DerivedName);
			}
			return FText::FromString(TagString);
		}
	}

	return FText::GetEmpty();
}

// ═══════════════════════════════════════════════════════════════════════════
// NON-PURE FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

double UB_StatusEffect::GetResistiveStatValue_Implementation()
{
	// Logic from JSON GetResistiveStatValue graph:
	// 1. Call GetOwnerStatManager()
	// 2. Is Valid check on stat manager
	// 3. If valid: store in LOCAL_StatManager
	// 4. Get Data->ResistiveStat (FGameplayTag)
	// 5. Call LOCAL_StatManager->GetStat(ResistiveStat) → returns (FoundStat, StatInfo)
	// 6. Is Valid check on FoundStat
	// 7. If valid: Break FStatInfo → get CurrentValue → return CurrentValue
	// 8. If not valid: return 0.0 (implicit)

	UAC_StatManager* StatManager = GetOwnerStatManager();
	if (!IsValid(StatManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetResistiveStatValue - StatManager invalid"));
		return 0.0;
	}

	if (!IsValid(Data))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetResistiveStatValue - Data invalid"));
		return 0.0;
	}

	// Cast Data to UPDA_StatusEffect to access ResistiveStat
	if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
	{
		if (StatusData->ResistiveStat.IsValid())
		{
			// Call StatManager->GetStat to get the stat value
			UB_Stat* FoundStat = nullptr;
			FStatInfo StatInfo;
			StatManager->GetStat(StatusData->ResistiveStat, FoundStat, StatInfo);

			if (IsValid(FoundStat))
			{
				UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetResistiveStatValue - Found stat, value: %f"), StatInfo.CurrentValue);
				return StatInfo.CurrentValue;
			}
		}
	}

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetResistiveStatValue - Returning 0.0"));
	return 0.0;
}

void UB_StatusEffect::SpawnLoopingVfxAttached_Implementation()
{
	// Logic from JSON SpawnLoopingVfxAttached graph:
	// 1. Get Data->RankInfo (map)
	// 2. Map_Find with ActiveRank key
	// 3. If found: get FStatusEffectRankInfo->LoopingVfx
	// 4. Cast Owner to BPI_GenericCharacter interface
	// 5. If cast succeeds: Spawn Niagara system attached to owner

	if (!IsValid(Data) || !IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - Data or Owner invalid"));
		return;
	}

	// Cast Data to get status effect properties
	UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data);
	if (!StatusData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - Data cast failed"));
		return;
	}

	// Get RankInfo for the current ActiveRank (with fallback to lower ranks)
	const FSLFStatusEffectRankInfo* RankData = StatusData->RankInfo.Find(ActiveRank);
	int32 EffectiveRank = ActiveRank;

	// Fallback: if exact rank not found, try lower ranks down to 1
	if (!RankData && ActiveRank > 1)
	{
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - No RankInfo for rank %d, trying fallback"), ActiveRank);
		for (int32 FallbackRank = ActiveRank - 1; FallbackRank >= 1; --FallbackRank)
		{
			RankData = StatusData->RankInfo.Find(FallbackRank);
			if (RankData)
			{
				EffectiveRank = FallbackRank;
				UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - Using fallback rank %d"), FallbackRank);
				break;
			}
		}
	}

	if (!RankData)
	{
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - No RankInfo for rank %d (no fallback available)"), ActiveRank);
		return;
	}

	// Get the LoopVFX Niagara system
	UNiagaraSystem* LoopVFX = RankData->LoopVFX.VFXSystem.LoadSynchronous();
	if (!LoopVFX)
	{
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - No LoopVFX for rank %d"), ActiveRank);
		return;
	}

	// Get attach socket (default to "Chest" if not specified)
	FName AttachSocket = RankData->LoopVFX.AttachSocket;
	if (AttachSocket.IsNone())
	{
		AttachSocket = FName("Chest");
	}

	UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - Spawning LoopVFX %s at socket %s for effect %s (rank %d)"),
		*LoopVFX->GetName(), *AttachSocket.ToString(), *StatusData->Tag.ToString(), ActiveRank);

	// Find the mesh component to attach to
	USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (MeshComp)
	{
		// Spawn attached Niagara system
		UNiagaraComponent* SpawnedComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			LoopVFX,
			MeshComp,
			AttachSocket,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true,  // bAutoDestroy
			true,  // bAutoActivate
			ENCPoolMethod::None,
			true   // bPreCullCheck
		);

		if (SpawnedComp)
		{
			UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - Successfully spawned LoopVFX"));
			// Store reference for cleanup later if needed
			LoopingVFXComponent = SpawnedComp;
		}
	}
	else
	{
		// Fallback: spawn at actor location
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoopVFX,
			Owner->GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.0f),
			true,  // bAutoDestroy
			true,  // bAutoActivate
			ENCPoolMethod::None,
			true   // bPreCullCheck
		);
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - Spawned at location (no mesh found)"));
	}
}

void UB_StatusEffect::Buildup()
{
	// Logic from JSON Buildup event graph (timer callback):
	// Called every 0.016667 seconds (60fps) during buildup phase
	// Calls AddBuildup to increment buildup

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::Buildup - Timer tick"));
	AddBuildup();
}

void UB_StatusEffect::Decay()
{
	// Logic from JSON Decay event graph (timer callback):
	// Called every tick during decay phase
	// Decrements BuildupPercent, broadcasts OnBuildupUpdated
	// If BuildupPercent <= 0: Call EffectFinished to reset state
	//
	// NOTE: Decay runs REGARDLESS of bIsTriggered state
	// For one-shot effects, decay runs AFTER trigger to reduce the bar

	if (!IsValid(Data))
	{
		return;
	}

	// Decrease buildup using decay rate from Data
	// bp_only uses BaseDecayRate = 2.0, with 0.1 scale factor = 0.2 per tick
	// At 60fps, this means ~500 ticks to go from 100% to 0% = ~8.3 seconds
	double DecayAmount = 0.2; // Default slow decay
	if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
	{
		// Use BaseDecayRate with 0.1 scale factor to match StatusEffectManagerComponent
		if (StatusData->BaseDecayRate > 0.0)
		{
			DecayAmount = StatusData->BaseDecayRate * 0.1;
		}
	}

	double OldPercent = BuildupPercent;
	BuildupPercent = FMath::Clamp(BuildupPercent - DecayAmount, 0.0, 100.0);
	OnBuildupUpdated.Broadcast();

	// Log every 60 ticks (~1 second) to avoid spam
	static int32 DecayLogCounter = 0;
	if (DecayLogCounter++ % 60 == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::Decay - BuildupPercent: %.1f%% (decay rate: %.2f/tick)"), BuildupPercent, DecayAmount);
	}

	if (BuildupPercent <= 0.0)
	{
		// Clear decay timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(DecayTimerHandle);
		}
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::Decay - Buildup depleted, calling EffectFinished"));

		// Call EffectFinished to reset bIsTriggered and allow retrigger
		EffectFinished();
	}
}

void UB_StatusEffect::TickDamage()
{
	// Logic from JSON TickDamage event graph (timer callback):
	// Called periodically when effect is triggered
	// Applies tick damage based on TickStatChange data

	if (!bIsTriggered)
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::TickDamage - Called but not triggered"));
		return;
	}

	// Determine which stat changes to apply (prefer combined struct, fallback to TickStatChange)
	TArray<FStatChange>* StatChangesToApply = nullptr;

	if (TickAndOneShotStatChange.TickingStatAdjustment.Num() > 0)
	{
		StatChangesToApply = &TickAndOneShotStatChange.TickingStatAdjustment;
	}
	else if (TickStatChange.TickingStatAdjustment.Num() > 0)
	{
		StatChangesToApply = &TickStatChange.TickingStatAdjustment;
	}
	else if (StatsToAdjust.Num() > 0)
	{
		StatChangesToApply = &StatsToAdjust;
	}

	if (StatChangesToApply && StatChangesToApply->Num() > 0)
	{
		FString EffectName = TEXT("Unknown");
		if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
		{
			EffectName = StatusData->Tag.ToString();
		}

		for (const FStatChange& Change : *StatChangesToApply)
		{
			// Calculate actual change amount (random between min and max)
			double Amount = FMath::RandRange(Change.MinAmount, Change.MaxAmount);

			UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::TickDamage - [%s] Applying %.2f to %s (%s)"),
				*EffectName,
				Amount,
				*Change.StatTag.ToString(),
				Change.ValueType == ESLFValueType::CurrentValue ? TEXT("Current") : TEXT("Max"));

			// Apply stat change via TryAdjustOwnerStat (handles both stat manager types)
			bool bApplied = TryAdjustOwnerStat(
				Change.StatTag,
				Change.ValueType,
				Amount,
				false,  // Not a level up
				Change.bTryActivateRegen
			);

			if (!bApplied)
			{
				UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::TickDamage - Failed to apply stat change to %s"), *Change.StatTag.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::TickDamage - No tick stat changes configured"));
	}
}

void UB_StatusEffect::EffectFinished()
{
	// Logic from JSON EffectFinished event graph:
	// 1. Clear all timers
	// 2. Reset IsTriggered to false
	// 3. Broadcast OnStatusEffectFinished with tag from Data

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::EffectFinished - Effect completed"));

	// Clear all timers
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BuildupTimerHandle);
		World->GetTimerManager().ClearTimer(DecayTimerHandle);
		World->GetTimerManager().ClearTimer(TickDamageTimerHandle);
		World->GetTimerManager().ClearTimer(WaitForDecayTimerHandle);
	}

	// Reset triggered state
	bIsTriggered = false;
	BuildupPercent = 0.0;

	// Broadcast finished event with tag from Data
	FGameplayTag StatusTag;
	if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
	{
		StatusTag = StatusData->Tag;
	}
	OnStatusEffectFinished.Broadcast(StatusTag);
}

void UB_StatusEffect::EffectTriggered()
{
	// Logic from JSON EffectTriggered event graph:
	// Called when buildup reaches 100%
	// 1. Apply one-shot stat changes
	// 2. Start tick damage timer if applicable
	// 3. Start effect duration timer
	// 4. Spawn looping VFX

	UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::EffectTriggered - Effect triggered!"));

	// Determine which one-shot stat changes to apply
	TArray<FStatChange>* OneShotChanges = nullptr;

	if (TickAndOneShotStatChange.InstantStatAdjustment.Num() > 0)
	{
		OneShotChanges = &TickAndOneShotStatChange.InstantStatAdjustment;
	}
	else if (OneShotStatChange.StatChanges.Num() > 0)
	{
		OneShotChanges = &OneShotStatChange.StatChanges;
	}

	if (OneShotChanges && OneShotChanges->Num() > 0)
	{
		FString EffectName = TEXT("Unknown");
		if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
		{
			EffectName = StatusData->Tag.ToString();
		}

		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::EffectTriggered - [%s] Applying %d one-shot stat changes"),
			*EffectName, OneShotChanges->Num());

		for (const FStatChange& Change : *OneShotChanges)
		{
			// Calculate actual change amount (random between min and max)
			double Amount = FMath::RandRange(Change.MinAmount, Change.MaxAmount);

			UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::EffectTriggered - [%s] Instant %.2f to %s (%s)"),
				*EffectName,
				Amount,
				*Change.StatTag.ToString(),
				Change.ValueType == ESLFValueType::CurrentValue ? TEXT("Current") : TEXT("Max"));

			// Apply stat change via TryAdjustOwnerStat (handles both stat manager types)
			bool bApplied = TryAdjustOwnerStat(
				Change.StatTag,
				Change.ValueType,
				Amount,
				false,  // Not a level up
				Change.bTryActivateRegen
			);

			if (bApplied)
			{
				UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::EffectTriggered - [%s] DAMAGE APPLIED: %.2f to %s"),
					*EffectName, Amount, *Change.StatTag.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::EffectTriggered - [%s] FAILED to apply stat change to %s"),
					*EffectName, *Change.StatTag.ToString());
			}
		}
	}

	// Spawn Trigger VFX (one-shot) from RankInfo
	if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
	{
		const FSLFStatusEffectRankInfo* RankData = StatusData->RankInfo.Find(ActiveRank);

		// Fallback: if exact rank not found, try lower ranks down to 1
		if (!RankData && ActiveRank > 1)
		{
			for (int32 FallbackRank = ActiveRank - 1; FallbackRank >= 1; --FallbackRank)
			{
				RankData = StatusData->RankInfo.Find(FallbackRank);
				if (RankData)
				{
					UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::EffectTriggered - Using fallback rank %d for VFX"), FallbackRank);
					break;
				}
			}
		}

		if (RankData)
		{
			UNiagaraSystem* TriggerVFX = RankData->TriggerVFX.VFXSystem.LoadSynchronous();
			if (TriggerVFX)
			{
				FName AttachSocket = RankData->TriggerVFX.AttachSocket;
				if (AttachSocket.IsNone())
				{
					AttachSocket = FName("Chest");
				}

				UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::EffectTriggered - Spawning TriggerVFX %s at socket %s"),
					*TriggerVFX->GetName(), *AttachSocket.ToString());

				USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
				if (MeshComp)
				{
					UNiagaraFunctionLibrary::SpawnSystemAttached(
						TriggerVFX,
						MeshComp,
						AttachSocket,
						FVector::ZeroVector,
						FRotator::ZeroRotator,
						EAttachLocation::SnapToTarget,
						true,  // bAutoDestroy
						true,  // bAutoActivate
						ENCPoolMethod::None,
						true   // bPreCullCheck
					);
				}
				else
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						TriggerVFX,
						Owner->GetActorLocation(),
						FRotator::ZeroRotator,
						FVector(1.0f),
						true,  // bAutoDestroy
						true   // bAutoActivate
					);
				}
			}
		}
	}

	// Spawn looping VFX
	SpawnLoopingVfxAttached();

	// Set up tick damage timer if EffectSteps > 0
	// Use duration/interval from combined struct or TickStatChange
	double TickDuration = EffectDuration;
	double TickInterval = 0.0;

	if (TickAndOneShotStatChange.Duration > 0.0)
	{
		TickDuration = TickAndOneShotStatChange.Duration;
		TickInterval = TickAndOneShotStatChange.Interval;
	}
	else if (TickStatChange.Duration > 0.0)
	{
		TickDuration = TickStatChange.Duration;
		TickInterval = TickStatChange.Interval;
	}
	else if (EffectSteps > 0.0 && EffectDuration > 0.0)
	{
		TickInterval = EffectDuration / EffectSteps;
	}

	if (TickInterval > 0.0)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				TickDamageTimerHandle,
				this,
				&UB_StatusEffect::TickDamage,
				TickInterval,
				true // Looping
			);
			UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::EffectTriggered - Started tick damage timer (interval: %.2fs)"),
				TickInterval);
		}
	}

	// Set up effect duration timer OR start decay for one-shot effects
	if (TickDuration > 0.0)
	{
		// Has explicit duration - set timer to call EffectFinished after duration
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				WaitForDecayTimerHandle,
				this,
				&UB_StatusEffect::EffectFinished,
				TickDuration,
				false // Not looping
			);
			UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::EffectTriggered - Effect will finish in %.2fs"), TickDuration);
		}
	}
	else
	{
		// One-shot effect with no duration (e.g., Bleed rank 1-2)
		// bp_only behavior: Wait for DecayDelay, then start decay to gradually reduce buildup bar
		// When decay reaches 0, EffectFinished will be called by Decay()
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::EffectTriggered - One-shot effect, waiting for decay delay"));
		WaitForDecay(); // Starts decay after DecayDelay (default 2.0s)
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CUSTOM EVENTS
// ═══════════════════════════════════════════════════════════════════════════

void UB_StatusEffect::Initialize_Implementation(int32 Rank)
{
	// Logic from JSON Event Initialize:
	// 1. Call GetResistiveStatValue()
	// 2. Set OwnerResistiveStatValue = result
	// 3. Call RefreshRank(Rank)

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::Initialize - Rank: %d"), Rank);

	OwnerResistiveStatValue = GetResistiveStatValue();
	RefreshRank(Rank);
}

void UB_StatusEffect::RefreshRank_Implementation(int32 NewActiveRank)
{
	// Logic from JSON Event RefreshRank:
	// 1. Set ActiveRank = NewActiveRank
	// 2. Extract damage data from RankInfo[NewActiveRank].RelevantData

	UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::RefreshRank - NewActiveRank: %d"), NewActiveRank);
	ActiveRank = NewActiveRank;

	// Clear previous stat change data
	TickStatChange = FStatusEffectTick();
	OneShotStatChange = FStatusEffectStatChanges();
	TickAndOneShotStatChange = FStatusEffectOneShotAndTick();
	EffectDuration = 0.0;
	EffectSteps = 0.0;

	// Extract damage data from RankInfo
	if (!IsValid(Data))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::RefreshRank - Data is null"));
		return;
	}

	UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data);
	if (!StatusData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::RefreshRank - Failed to cast Data to UPDA_StatusEffect"));
		return;
	}

	// Find RankInfo for this rank (with fallback to lower ranks)
	const FSLFStatusEffectRankInfo* RankData = StatusData->RankInfo.Find(NewActiveRank);
	int32 EffectiveRank = NewActiveRank;

	if (!RankData && NewActiveRank > 1)
	{
		for (int32 FallbackRank = NewActiveRank - 1; FallbackRank >= 1; --FallbackRank)
		{
			RankData = StatusData->RankInfo.Find(FallbackRank);
			if (RankData)
			{
				EffectiveRank = FallbackRank;
				UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::RefreshRank - Using fallback rank %d for damage data"), FallbackRank);
				break;
			}
		}
	}

	if (!RankData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::RefreshRank - No RankInfo for rank %d"), NewActiveRank);
		return;
	}

	// Extract damage data from RelevantData FInstancedStruct
	// Try each possible struct type
	if (RankData->RelevantData.IsValid())
	{
		// Try FStatusEffectOneShotAndTick first (most complete)
		if (const FStatusEffectOneShotAndTick* OneShotAndTickData = RankData->RelevantData.GetPtr<FStatusEffectOneShotAndTick>())
		{
			TickAndOneShotStatChange = *OneShotAndTickData;
			EffectDuration = OneShotAndTickData->Duration;
			if (OneShotAndTickData->Interval > 0.0)
			{
				EffectSteps = OneShotAndTickData->Duration / OneShotAndTickData->Interval;
			}
			UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::RefreshRank - Loaded FStatusEffectOneShotAndTick: Duration=%.1f, Interval=%.1f, Instant=%d changes, Ticking=%d changes"),
				OneShotAndTickData->Duration, OneShotAndTickData->Interval,
				OneShotAndTickData->InstantStatAdjustment.Num(), OneShotAndTickData->TickingStatAdjustment.Num());
		}
		// Try FStatusEffectTick
		else if (const FStatusEffectTick* TickData = RankData->RelevantData.GetPtr<FStatusEffectTick>())
		{
			TickStatChange = *TickData;
			EffectDuration = TickData->Duration;
			if (TickData->Interval > 0.0)
			{
				EffectSteps = TickData->Duration / TickData->Interval;
			}
			UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::RefreshRank - Loaded FStatusEffectTick: Duration=%.1f, Interval=%.1f, Ticking=%d changes"),
				TickData->Duration, TickData->Interval, TickData->TickingStatAdjustment.Num());
		}
		// Try FStatusEffectStatChanges (one-shot only)
		else if (const FStatusEffectStatChanges* OneShotData = RankData->RelevantData.GetPtr<FStatusEffectStatChanges>())
		{
			OneShotStatChange = *OneShotData;
			UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::RefreshRank - Loaded FStatusEffectStatChanges: %d one-shot changes"),
				OneShotData->StatChanges.Num());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::RefreshRank - RelevantData is valid but unknown struct type"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::RefreshRank - No RelevantData for rank %d (VFX only)"), EffectiveRank);
	}
}

void UB_StatusEffect::AdjustBuildupOneshot_Implementation(double Delta)
{
	// Logic from JSON Event AdjustBuildupOneshot:
	// Apply a one-time buildup adjustment
	// 1. If not triggered and Data valid:
	// 2. Apply resistance reduction to Delta
	// 3. BuildupPercent = Clamp(BuildupPercent + Delta, 0, 100)
	// 4. Broadcast OnBuildupUpdated
	// 5. Check if >= 100, trigger if so

	UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::AdjustBuildupOneshot - Raw Delta: %.2f"), Delta);

	if (!bIsTriggered && IsValid(Data))
	{
		// Apply resistance reduction (higher resistance = less buildup)
		// Use ResistiveStatCurve if available, otherwise use formula
		double AdjustedDelta = Delta;

		UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data);
		if (StatusData && OwnerResistiveStatValue > 0.0)
		{
			// Check if we have a resistance curve
			if (StatusData->ResistiveStatCurve)
			{
				// Use curve: X = resistance stat value, Y = buildup multiplier (0.0-1.0)
				float CurveMultiplier = StatusData->ResistiveStatCurve->GetFloatValue(OwnerResistiveStatValue);
				AdjustedDelta = Delta * FMath::Clamp(CurveMultiplier, 0.0f, 1.0f);
				UE_LOG(LogTemp, Log, TEXT("  Resistance curve: stat=%.1f -> multiplier=%.3f -> adjusted delta=%.2f"),
					OwnerResistiveStatValue, CurveMultiplier, AdjustedDelta);
			}
			else
			{
				// Use default formula: reduction = 100 / (100 + resistance)
				double ResistanceFactor = 100.0 / (100.0 + OwnerResistiveStatValue);
				AdjustedDelta = Delta * ResistanceFactor;
				UE_LOG(LogTemp, Log, TEXT("  Resistance formula: stat=%.1f -> factor=%.3f -> adjusted delta=%.2f"),
					OwnerResistiveStatValue, ResistanceFactor, AdjustedDelta);
			}
		}

		BuildupPercent = FMath::Clamp(BuildupPercent + AdjustedDelta, 0.0, 100.0);
		UE_LOG(LogTemp, Log, TEXT("  BuildupPercent now: %.1f%%"), BuildupPercent);
		OnBuildupUpdated.Broadcast();

		if (BuildupPercent >= 100.0)
		{
			// Clear buildup timer
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(BuildupTimerHandle);
			}

			// Broadcast triggered event
			FText TriggeredText = GetTriggeredText();
			OnStatusEffectTriggered.Broadcast(TriggeredText);

			// Call effect triggered
			EffectTriggered();

			// Set triggered flag
			bIsTriggered = true;
		}
		else
		{
			// Threshold not reached - start/reset decay timer
			// This ensures the status effect bar eventually disappears if not triggered
			// Clear any existing decay timer first (reset on new hit)
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(WaitForDecayTimerHandle);
				World->GetTimerManager().ClearTimer(DecayTimerHandle);
			}

			// Start decay process (with delay)
			WaitForDecay();
			UE_LOG(LogTemp, Log, TEXT("  Threshold not reached, starting decay timer"));
		}
	}
}

void UB_StatusEffect::WaitForDecay_Implementation()
{
	// Logic from JSON Event WaitForDecay:
	// Set a timer to wait before starting decay
	// Note: Delay time from Data

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::WaitForDecay"));

	// Get delay time from Data
	double DecayDelayTime = 2.0; // Default delay
	if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
	{
		DecayDelayTime = StatusData->DecayDelay > 0.0 ? StatusData->DecayDelay : 2.0;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			WaitForDecayTimerHandle,
			this,
			&UB_StatusEffect::StartDecay,
			DecayDelayTime,
			false // Not looping
		);
	}
}

void UB_StatusEffect::RemoveBuildup_Implementation()
{
	// Logic from JSON Event RemoveBuildup:
	// 1. Set BuildupPercent = 0
	// 2. Clear timers
	// 3. Broadcast OnBuildupUpdated

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::RemoveBuildup"));

	BuildupPercent = 0.0;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BuildupTimerHandle);
		World->GetTimerManager().ClearTimer(DecayTimerHandle);
	}

	OnBuildupUpdated.Broadcast();
}

void UB_StatusEffect::StopDecay_Implementation()
{
	// Logic from JSON Event StopDecay:
	// Clear the decay timer

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::StopDecay"));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DecayTimerHandle);
	}
}

void UB_StatusEffect::StartDecay_Implementation()
{
	// Logic from JSON Event StartDecay:
	// Set looping timer for Decay callback

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::StartDecay"));

	// Timer interval (same as buildup: 0.016667 = ~60fps)
	const float DecayInterval = 0.016667f;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DecayTimerHandle,
			this,
			&UB_StatusEffect::Decay,
			DecayInterval,
			true // Looping
		);
	}
}

void UB_StatusEffect::StopBuildup_Implementation(bool bApplyDelay)
{
	// Logic from JSON Event StopBuildup:
	// 1. Clear buildup timer
	// 2. If bApplyDelay: call WaitForDecay
	// 3. Else: call StartDecay immediately

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::StopBuildup - bApplyDelay: %s"),
		bApplyDelay ? TEXT("true") : TEXT("false"));

	// Clear buildup timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BuildupTimerHandle);
	}

	// Start decay (with or without delay)
	if (bApplyDelay)
	{
		WaitForDecay();
	}
	else
	{
		StartDecay();
	}
}

void UB_StatusEffect::StartBuildup_Implementation()
{
	// Logic from JSON Event StartBuildup:
	// 1. Call StopDecay (stop any active decay)
	// 2. Set looping timer for Buildup callback (0.016667 seconds = ~60fps)

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::StartBuildup"));

	// Stop any active decay first
	StopDecay();

	// Start buildup timer
	const float BuildupInterval = 0.016667f; // ~60fps

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			BuildupTimerHandle,
			this,
			&UB_StatusEffect::Buildup,
			BuildupInterval,
			true // Looping
		);
	}
}

void UB_StatusEffect::AddBuildup_Implementation()
{
	// Logic from JSON Event AddBuildup:
	// 1. Branch: if NOT IsTriggered
	// 2. Get Data (with validation)
	// 3. If Data valid:
	//    a. Get BuildupPercent
	//    b. Get buildup rate from Data (RankInfo->BuildupRate)
	//    c. Multiply by owner resistance factor
	//    d. Add to BuildupPercent
	//    e. Clamp(0, 100)
	//    f. Set BuildupPercent
	// 4. Broadcast OnBuildupUpdated
	// 5. Branch: if BuildupPercent >= 100
	// 6. If true:
	//    a. Clear timer "Buildup"
	//    b. Broadcast OnStatusEffectTriggered
	//    c. Call EffectTriggered
	//    d. Set IsTriggered = true

	if (bIsTriggered)
	{
		return; // Already triggered, don't add more buildup
	}

	if (!IsValid(Data))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::AddBuildup - Data invalid"));
		return;
	}

	// Calculate buildup amount from Data
	double BuildupRate = 1.0;
	if (UPDA_StatusEffect* StatusData = Cast<UPDA_StatusEffect>(Data))
	{
		BuildupRate = StatusData->BaseBuildupRate > 0.0 ? StatusData->BaseBuildupRate : 1.0;
	}

	// Apply resistance factor (higher resistance = less buildup)
	double ResistanceFactor = 1.0;
	if (OwnerResistiveStatValue > 0.0)
	{
		ResistanceFactor = 100.0 / (100.0 + OwnerResistiveStatValue);
	}

	double BuildupAmount = BuildupRate * ResistanceFactor;

	// Update buildup
	BuildupPercent = FMath::Clamp(BuildupPercent + BuildupAmount, 0.0, 100.0);

	// Broadcast update
	OnBuildupUpdated.Broadcast();

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::AddBuildup - BuildupPercent: %f"), BuildupPercent);

	// Check if triggered
	if (BuildupPercent >= 100.0)
	{
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::AddBuildup - Buildup reached 100%%, triggering effect"));

		// Clear buildup timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(BuildupTimerHandle);
		}

		// Broadcast triggered event
		FText TriggeredText = GetTriggeredText();
		OnStatusEffectTriggered.Broadcast(TriggeredText);

		// Call effect triggered
		EffectTriggered();

		// Set triggered flag
		bIsTriggered = true;
	}
}
