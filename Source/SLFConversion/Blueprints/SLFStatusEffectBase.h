// SLFStatusEffectBase.h
// C++ base class for B_StatusEffect
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_StatusEffect
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         12/12 migrated (excluding 3 dispatchers counted separately)
// Functions:         10/10 migrated (excluding EventGraph)
// Event Dispatchers: 3/3 migrated
// Graphs:            11 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Data/StatusEffects/Logic/B_StatusEffect
//
// PURPOSE: Base status effect - damage-over-time, debuffs with buildup
// CHILDREN: B_StatusEffect_Poison, B_StatusEffect_Bleed, B_StatusEffect_Burn, etc.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "SLFStatusEffectBase.generated.h"

// Forward declarations
class UDataAsset;
class UStatManagerComponent;
class UNiagaraSystem;
class UNiagaraComponent;

/**
 * Status effect stat change - amount to adjust stats by
 */
USTRUCT(BlueprintType)
struct FSLFStatusEffectStatChange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	float Amount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	bool bIsPercent = false;
};

/**
 * Status effect rank data - per-rank configuration
 */
USTRUCT(BlueprintType)
struct FSLFStatusEffectRankData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	int32 Rank = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	float Duration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	float TickInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	TArray<FSLFStatusEffectStatChange> TickEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	TArray<FSLFStatusEffectStatChange> OneShotEffects;
};

/**
 * Status effect data - from PDA_StatusEffect
 */
USTRUCT(BlueprintType)
struct FSLFStatusEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FGameplayTag EffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FGameplayTag ResistiveStatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	float BuildupDecayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	UNiagaraSystem* LoopingVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	TArray<FSLFStatusEffectRankData> RankData;
};

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 3/3 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/3] Called when buildup percent changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatusBuildupUpdated, FGameplayTag, EffectTag, float, BuildupPercent);

/** [2/3] Called when status effect finishes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLFStatusEffectBaseFinished, FGameplayTag, EffectTag);

/** [3/3] Called when status effect triggers (buildup reaches 100%) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSLFStatusEffectBaseTriggered, FGameplayTag, EffectTag, int32, Rank);

/**
 * Base status effect object - damage over time / debuff with buildup system
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatusEffectBase : public UObject
{
	GENERATED_BODY()

public:
	USLFStatusEffectBase();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 12/12 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- References (2) ---

	/** [1/12] Owner actor being affected */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	AActor* Owner;

	/** [2/12] Status effect data asset (PDA_StatusEffect) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataAsset* Data;

	// --- State (4) ---

	/** [3/12] Whether effect has been triggered */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsTriggered;

	/** [4/12] Current effect rank (for scaling) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	int32 ActiveRank;

	/** [5/12] Owner's resistive stat value (affects buildup rate) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	float OwnerResistiveStatValue;

	/** [6/12] Current buildup percent (0.0 - 1.0) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	float BuildupPercent;

	// --- Duration/Tick (2) ---

	/** [7/12] Remaining effect duration */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	float EffectDuration;

	/** [8/12] Number of effect steps/ticks remaining */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	int32 EffectSteps;

	// --- Stat Changes (4) ---

	/** [9/12] Stat changes per tick */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<FSLFStatusEffectStatChange> TickStatChange;

	/** [10/12] One-shot stat changes (applied once on trigger) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<FSLFStatusEffectStatChange> OneShotStatChange;

	/** [11/12] Combined tick and one-shot changes */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<FSLFStatusEffectStatChange> TickAndOneShotStatChange;

	/** [12/12] Stats currently being adjusted */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<FGameplayTag> StatsToAdjust;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 3/3 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/3] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatusBuildupUpdated OnBuildupUpdated;

	/** [2/3] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FSLFStatusEffectBaseFinished OnStatusEffectFinished;

	/** [3/3] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FSLFStatusEffectBaseTriggered OnStatusEffectTriggered;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 10/10 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Component Access (1) ---

	/** [1/10] Get stat manager from owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect")
	UStatManagerComponent* GetOwnerStatManager();
	virtual UStatManagerComponent* GetOwnerStatManager_Implementation();

	// --- State Getters (5) ---

	/** [2/10] Get current buildup percent */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect")
	float GetBuildupPercent();
	virtual float GetBuildupPercent_Implementation();

	/** [3/10] Get owner's resistive stat value */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect")
	float GetResistiveStatValue();
	virtual float GetResistiveStatValue_Implementation();

	/** [4/10] Get effect data struct from data asset */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect")
	FSLFStatusEffectData GetEffectData();
	virtual FSLFStatusEffectData GetEffectData_Implementation();

	/** [5/10] Get rank-specific effect data */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect")
	FSLFStatusEffectRankData GetEffectRankData(int32 Rank);
	virtual FSLFStatusEffectRankData GetEffectRankData_Implementation(int32 Rank);

	/** [6/10] Get triggered text for UI display */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect")
	FText GetTriggeredText();
	virtual FText GetTriggeredText_Implementation();

	// --- VFX (1) ---

	/** [7/10] Spawn looping VFX attached to owner
	 * @param VFX - Niagara system to spawn
	 * @param SocketName - Socket to attach to
	 * @return Spawned Niagara component
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect|VFX")
	UNiagaraComponent* SpawnLoopingVfxAttached(UNiagaraSystem* VFX, FName SocketName);
	virtual UNiagaraComponent* SpawnLoopingVfxAttached_Implementation(UNiagaraSystem* VFX, FName SocketName);

	// --- Dispatcher Triggers (3) ---

	/** [8/10] Trigger OnBuildupUpdated dispatcher */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect|Events")
	void TriggerOnBuildupUpdated();
	virtual void TriggerOnBuildupUpdated_Implementation();

	/** [9/10] Trigger OnStatusEffectFinished dispatcher */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect|Events")
	void TriggerOnStatusEffectFinished();
	virtual void TriggerOnStatusEffectFinished_Implementation();

	/** [10/10] Trigger OnStatusEffectTriggered dispatcher */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect|Events")
	void TriggerOnStatusEffectTriggered();
	virtual void TriggerOnStatusEffectTriggered_Implementation();

	// --- Lifecycle (called by StatusEffectManager) ---

	/** Add buildup to this effect */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect")
	void AddBuildup(float Amount, int32 IncomingRank);
	virtual void AddBuildup_Implementation(float Amount, int32 IncomingRank);

	/** Tick the effect (apply damage, etc.) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect")
	void TickEffect(float DeltaTime);
	virtual void TickEffect_Implementation(float DeltaTime);

	/** End the effect */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatusEffect")
	void EndEffect();
	virtual void EndEffect_Implementation();
};
