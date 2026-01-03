// B_StatusEffect.h
// C++ class for Blueprint B_StatusEffect
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_StatusEffect.json
// Parent: Object -> UObject
// Variables: 15 | Functions: 22 | Dispatchers: 3

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "B_StatusEffect.generated.h"

// Forward declarations
class UAC_StatManager;
class UPrimaryDataAsset;

// ═══════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS (3) - from JSON EventDispatchers.List
// ═══════════════════════════════════════════════════════════════════════════

// OnBuildupUpdated - No parameters
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildupUpdated);

// OnStatusEffectFinished - Parameter: StatusEffectTag (FGameplayTag)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectFinished, FGameplayTag, StatusEffectTag);

// OnStatusEffectTriggered - Parameter: TriggeredText (FText)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectTriggered, FText, TriggeredText);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UB_StatusEffect : public UObject
{
	GENERATED_BODY()

public:
	UB_StatusEffect();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (12 non-dispatcher) - from JSON Variables.List
	// ═══════════════════════════════════════════════════════════════════════

	// Owner actor reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	AActor* Owner;

	// Status effect data asset (PDA_StatusEffect_C in Blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	UPrimaryDataAsset* Data;

	// Whether the effect has been triggered (Blueprint name: "IsTriggered?")
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)", meta = (DisplayName = "IsTriggered?"))
	bool bIsTriggered;

	// Current active rank of the status effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	int32 ActiveRank;

	// Owner's resistive stat value (cached on initialization)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	double OwnerResistiveStatValue;

	// Current buildup percentage (0-100)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	double BuildupPercent;

	// Duration of the effect when triggered
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	double EffectDuration;

	// Number of effect steps/ticks
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	double EffectSteps;

	// Tick-based stat change data
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	FStatusEffectTick TickStatChange;

	// One-shot stat change data
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	FStatusEffectStatChanges OneShotStatChange;

	// Combined tick and one-shot stat change data
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	FStatusEffectOneShotAndTick TickAndOneShotStatChange;

	// Array of stat changes to apply
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime (Do Not Edit)")
	TArray<FStatChange> StatsToAdjust;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (3 delegate properties)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBuildupUpdated OnBuildupUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatusEffectFinished OnStatusEffectFinished;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatusEffectTriggered OnStatusEffectTriggered;

	// ═══════════════════════════════════════════════════════════════════════
	// TIMER HANDLES (for Blueprint timer logic)
	// ═══════════════════════════════════════════════════════════════════════

	FTimerHandle BuildupTimerHandle;
	FTimerHandle DecayTimerHandle;
	FTimerHandle TickDamageTimerHandle;
	FTimerHandle WaitForDecayTimerHandle;

	// ═══════════════════════════════════════════════════════════════════════
	// PURE GETTER FUNCTIONS (5) - from JSON FunctionSignatures.Functions
	// ═══════════════════════════════════════════════════════════════════════

	// Get owner's stat manager component
	// Logic: Owner->GetComponentByClass<UAC_StatManager>(), return if valid
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Getters")
	UAC_StatManager* GetOwnerStatManager();
	virtual UAC_StatManager* GetOwnerStatManager_Implementation();

	// Get buildup percent as 0.0-1.0 value
	// Logic: if Data valid, return BuildupPercent / 100.0, else return 0.0
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Getters")
	double GetBuildupPercent();
	virtual double GetBuildupPercent_Implementation();

	// Get the effect data asset
	// Logic: if Data valid, return Data, else return nullptr
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Getters")
	UPrimaryDataAsset* GetEffectData();
	virtual UPrimaryDataAsset* GetEffectData_Implementation();

	// Get effect rank data from the data asset
	// Logic: if Data valid, return Data->RankInfo, else return empty map
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Getters")
	TMap<int32, FSLFStatusEffectRankInfo> GetEffectRankData();
	virtual TMap<int32, FSLFStatusEffectRankInfo> GetEffectRankData_Implementation();

	// Get triggered text from data asset
	// Logic: Get tag name, extract text after last ".", format for display
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Getters")
	FText GetTriggeredText();
	virtual FText GetTriggeredText_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// NON-PURE FUNCTIONS (7) - from JSON FunctionSignatures.Functions
	// ═══════════════════════════════════════════════════════════════════════

	// Get resistive stat value from owner's stat manager
	// Logic: GetOwnerStatManager, get resistive stat tag from Data, get stat value
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	double GetResistiveStatValue();
	virtual double GetResistiveStatValue_Implementation();

	// Spawn looping VFX attached to owner
	// Logic: Cast owner to BPI_GenericCharacter, call PlaySoftNiagaraLoopingReplicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void SpawnLoopingVfxAttached();
	virtual void SpawnLoopingVfxAttached_Implementation();

	// Buildup timer callback - called every tick during buildup phase
	UFUNCTION(BlueprintCallable, Category = "B_StatusEffect")
	void Buildup();

	// Decay timer callback - called every tick during decay phase
	UFUNCTION(BlueprintCallable, Category = "B_StatusEffect")
	void Decay();

	// Tick damage callback - applies periodic damage when effect is triggered
	UFUNCTION(BlueprintCallable, Category = "B_StatusEffect")
	void TickDamage();

	// Called when effect duration finishes
	UFUNCTION(BlueprintCallable, Category = "B_StatusEffect")
	void EffectFinished();

	// Called when buildup reaches 100% and effect triggers
	UFUNCTION(BlueprintCallable, Category = "B_StatusEffect")
	void EffectTriggered();

	// ═══════════════════════════════════════════════════════════════════════
	// CUSTOM EVENTS (10) - from JSON FunctionSignatures "Event ..." entries
	// ═══════════════════════════════════════════════════════════════════════

	// Initialize the status effect with a rank
	// Logic: GetResistiveStatValue -> Set OwnerResistiveStatValue -> RefreshRank(Rank)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void Initialize(int32 Rank);
	virtual void Initialize_Implementation(int32 Rank);

	// Refresh rank settings
	// Logic: Set ActiveRank = NewRank
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void RefreshRank(int32 NewActiveRank);
	virtual void RefreshRank_Implementation(int32 NewActiveRank);

	// Adjust buildup by a one-shot delta amount
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void AdjustBuildupOneshot(double Delta);
	virtual void AdjustBuildupOneshot_Implementation(double Delta);

	// Start waiting for decay to begin
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void WaitForDecay();
	virtual void WaitForDecay_Implementation();

	// Remove all buildup instantly
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void RemoveBuildup();
	virtual void RemoveBuildup_Implementation();

	// Stop decay timer
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void StopDecay();
	virtual void StopDecay_Implementation();

	// Start decay timer
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void StartDecay();
	virtual void StartDecay_Implementation();

	// Stop buildup timer
	// Parameter: bApplyDelay - if true, wait before starting decay
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void StopBuildup(bool bApplyDelay);
	virtual void StopBuildup_Implementation(bool bApplyDelay);

	// Start buildup timer
	// Logic: StopDecay, Set looping timer for Buildup callback
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void StartBuildup();
	virtual void StartBuildup_Implementation();

	// Add buildup (single tick)
	// Logic: If not triggered and Data valid, add buildup, clamp, check >= 100
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_StatusEffect")
	void AddBuildup();
	virtual void AddBuildup_Implementation();
};
