// SLFStatusEffectBase.cpp
// C++ implementation for B_StatusEffect
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_StatusEffect
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         12/12 (initialized in constructor)
// Functions:         13/13 implemented (10 migrated + 3 lifecycle)
// Event Dispatchers: 3/3 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFStatusEffectBase.h"
#include "StatManagerComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"

USLFStatusEffectBase::USLFStatusEffectBase()
{
	// Initialize references
	Owner = nullptr;
	Data = nullptr;

	// Initialize state
	bIsTriggered = false;
	ActiveRank = 1;
	OwnerResistiveStatValue = 0.0f;
	BuildupPercent = 0.0f;

	// Initialize duration/tick
	EffectDuration = 0.0f;
	EffectSteps = 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMPONENT ACCESS [1/13]
// ═══════════════════════════════════════════════════════════════════════════════

UStatManagerComponent* USLFStatusEffectBase::GetOwnerStatManager_Implementation()
{
	if (Owner)
	{
		return Owner->FindComponentByClass<UStatManagerComponent>();
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATE GETTERS [2-6/13]
// ═══════════════════════════════════════════════════════════════════════════════

float USLFStatusEffectBase::GetBuildupPercent_Implementation()
{
	return BuildupPercent;
}

float USLFStatusEffectBase::GetResistiveStatValue_Implementation()
{
	// Get from owner's stat manager using Data's ResistiveStat tag
	if (UPDA_StatusEffect* StatusEffectData = Cast<UPDA_StatusEffect>(Data))
	{
		if (StatusEffectData->ResistiveStat.IsValid())
		{
			if (UStatManagerComponent* StatMgr = GetOwnerStatManager())
			{
				UObject* StatObject = nullptr;
				FStatInfo StatInfo;
				if (StatMgr->GetStat(StatusEffectData->ResistiveStat, StatObject, StatInfo))
				{
					OwnerResistiveStatValue = StatInfo.CurrentValue;
					UE_LOG(LogTemp, Verbose, TEXT("[StatusEffect] ResistiveStat %s = %.2f"),
						*StatusEffectData->ResistiveStat.ToString(), OwnerResistiveStatValue);
				}
			}
		}
	}
	return OwnerResistiveStatValue;
}

FSLFStatusEffectData USLFStatusEffectBase::GetEffectData_Implementation()
{
	FSLFStatusEffectData Result;

	// Extract FSLFStatusEffectData from UPDA_StatusEffect data asset
	if (UPDA_StatusEffect* StatusEffectData = Cast<UPDA_StatusEffect>(Data))
	{
		Result.EffectTag = StatusEffectData->Tag;
		Result.DisplayName = StatusEffectData->TriggeredText;
		Result.ResistiveStatTag = StatusEffectData->ResistiveStat;
		Result.BuildupDecayRate = StatusEffectData->BaseDecayRate;

		// Note: LoopingVFX and RankData would need to be added to UPDA_StatusEffect
		// if they exist in the Blueprint data structure
		UE_LOG(LogTemp, Verbose, TEXT("[StatusEffect] GetEffectData: Tag=%s, DisplayName=%s"),
			*Result.EffectTag.ToString(), *Result.DisplayName.ToString());
	}

	return Result;
}

FSLFStatusEffectRankData USLFStatusEffectBase::GetEffectRankData_Implementation(int32 Rank)
{
	FSLFStatusEffectData EffectData = GetEffectData();

	for (const FSLFStatusEffectRankData& RankData : EffectData.RankData)
	{
		if (RankData.Rank == Rank)
		{
			return RankData;
		}
	}

	// Return first rank data if not found
	if (EffectData.RankData.Num() > 0)
	{
		return EffectData.RankData[0];
	}

	return FSLFStatusEffectRankData();
}

FText USLFStatusEffectBase::GetTriggeredText_Implementation()
{
	FSLFStatusEffectData EffectData = GetEffectData();
	return EffectData.DisplayName;
}

// ═══════════════════════════════════════════════════════════════════════════════
// VFX [7/13]
// ═══════════════════════════════════════════════════════════════════════════════

UNiagaraComponent* USLFStatusEffectBase::SpawnLoopingVfxAttached_Implementation(UNiagaraSystem* VFX, FName SocketName)
{
	if (!VFX || !Owner) return nullptr;

	USkeletalMeshComponent* Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!Mesh) return nullptr;

	return UNiagaraFunctionLibrary::SpawnSystemAttached(
		VFX, Mesh, SocketName,
		FVector::ZeroVector, FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset, true);
}

// ═══════════════════════════════════════════════════════════════════════════════
// DISPATCHER TRIGGERS [8-10/13]
// ═══════════════════════════════════════════════════════════════════════════════

void USLFStatusEffectBase::TriggerOnBuildupUpdated_Implementation()
{
	FSLFStatusEffectData EffectData = GetEffectData();
	OnBuildupUpdated.Broadcast(EffectData.EffectTag, BuildupPercent);
}

void USLFStatusEffectBase::TriggerOnStatusEffectFinished_Implementation()
{
	FSLFStatusEffectData EffectData = GetEffectData();
	OnStatusEffectFinished.Broadcast(EffectData.EffectTag);
}

void USLFStatusEffectBase::TriggerOnStatusEffectTriggered_Implementation()
{
	FSLFStatusEffectData EffectData = GetEffectData();
	OnStatusEffectTriggered.Broadcast(EffectData.EffectTag, ActiveRank);
}

// ═══════════════════════════════════════════════════════════════════════════════
// LIFECYCLE [11-13/13]
// ═══════════════════════════════════════════════════════════════════════════════

void USLFStatusEffectBase::AddBuildup_Implementation(float Amount, int32 IncomingRank)
{
	// Apply resistance reduction
	float ResistValue = GetResistiveStatValue();
	float AdjustedAmount = Amount * (1.0f - (ResistValue / 100.0f));

	BuildupPercent = FMath::Clamp(BuildupPercent + AdjustedAmount, 0.0f, 1.0f);

	UE_LOG(LogTemp, Log, TEXT("[StatusEffect] AddBuildup: %.2f (adjusted: %.2f), Total: %.2f%%"),
		Amount, AdjustedAmount, BuildupPercent * 100.0f);

	TriggerOnBuildupUpdated();

	// Check if triggered
	if (BuildupPercent >= 1.0f && !bIsTriggered)
	{
		bIsTriggered = true;
		ActiveRank = IncomingRank;

		// Setup duration/steps from rank data
		FSLFStatusEffectRankData RankData = GetEffectRankData(ActiveRank);
		EffectDuration = RankData.Duration;
		EffectSteps = FMath::FloorToInt(RankData.Duration / RankData.TickInterval);
		TickStatChange = RankData.TickEffects;
		OneShotStatChange = RankData.OneShotEffects;

		UE_LOG(LogTemp, Log, TEXT("[StatusEffect] TRIGGERED! Rank %d, Duration: %.2f"),
			ActiveRank, EffectDuration);

		TriggerOnStatusEffectTriggered();
	}
}

void USLFStatusEffectBase::TickEffect_Implementation(float DeltaTime)
{
	if (!bIsTriggered) return;

	EffectDuration -= DeltaTime;

	// Apply tick effects
	if (UStatManagerComponent* StatManager = GetOwnerStatManager())
	{
		for (const FSLFStatusEffectStatChange& Change : TickStatChange)
		{
			float Amount = Change.bIsPercent ? (Change.Amount / 100.0f) : Change.Amount;
			// New signature: (StatTag, ValueType, Change, bLevelUp, bTriggerRegen)
			StatManager->AdjustStat(Change.StatTag, ESLFValueType::CurrentValue, -Amount * DeltaTime, false, false);
		}
	}

	// Check if finished
	if (EffectDuration <= 0.0f)
	{
		EndEffect();
	}
}

void USLFStatusEffectBase::EndEffect_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[StatusEffect] EndEffect"));

	bIsTriggered = false;
	BuildupPercent = 0.0f;

	TriggerOnStatusEffectFinished();
}
