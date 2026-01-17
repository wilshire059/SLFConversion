// AC_CombatManager.h
// C++ component for AC_CombatManager
//
// 20-PASS VALIDATION: 2026-01-02 Full Migration
// Source: BlueprintDNA_v2/Component/AC_CombatManager.json
// Variables: 37 | Functions: 31 (20 core + 11 events) | Dispatchers: 0
//
// Full logic migrated from Blueprint graphs:
// - COMBAT DAMAGE HANDLING (weapon, projectile, fist)
// - GUARD SYSTEM (regular, perfect, grace period)
// - COMBO SYSTEM (sections, soft combos, registers)
// - HIT REACTIONS & DEATH
// - HAND TRACING (unarmed combat)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "AC_CombatManager.generated.h"

// Forward declarations
class UStatManagerComponent;
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class UB_Stat;
class UDamageType;
class AController;

// Event Dispatcher Declarations


UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_CombatManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_CombatManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (37)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FName ComboSection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FName DefaultComboSequenceSection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsDead;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsGuarding;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool WantsToGuard;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsInvincible;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool PoiseBroken;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool HyperArmor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	AActor* ExecutionTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	UPrimaryDataAsset* UnarmedAnimset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Hit React")
	TSubclassOf<UCameraShakeBase> GenericReactionShake;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Hit React")
	bool RagdollOnDeath;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle RightHandTraceTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	USkeletalMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle LeftHandTraceTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<AActor*> FistTracedActors;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	FName HandSocket_R;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	FName HandSocket_L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle GuardTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	double TimeSinceGuard;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Guarding")
	double GuardingGracePeriod;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Guarding")
	double MinPerfectGuardPoiseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Guarding")
	double MaxPerfectGuardPoiseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Guarding")
	ESLFDotProductThreshold FacingDirectionAcceptance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Guarding")
	double PerfectGuardDuration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Guarding")
	UNiagaraSystem* PerfectGuardEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Guarding")
	TSubclassOf<UCameraShakeBase> GuardCameraShake;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UNiagaraSystem> HitVFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Guarding")
	UNiagaraSystem* GuardEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	double IKWeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FVector CurrentHitNormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Hit React")
	UPDA_CombatReactionAnimData_Player* ReactionAnimset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Hit React")
	ESLFHitReactType HitReactType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	double MinUnarmedPoiseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	double MaxUnarmedPoiseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<USoundBase> HitSFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle GuardGracePeriodTimer;

	// ═══════════════════════════════════════════════════════════════════════
	// POISE SYSTEM (Elden Ring Style)
	// ═══════════════════════════════════════════════════════════════════════

	/** Timer for poise regeneration delay (poise starts regenerating after this delay from last hit) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Poise")
	FTimerHandle PoiseRegenDelayTimer;

	/** Timer for poise regeneration tick */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Poise")
	FTimerHandle PoiseRegenTickTimer;

	/** Timer for poise break recovery (can't be staggered again during this) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Poise")
	FTimerHandle PoiseBreakRecoveryTimer;

	/** Delay before poise starts regenerating after last damage (seconds) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Poise")
	double PoiseRegenDelay = 3.0;

	/** Rate of poise regeneration (points per second) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Poise")
	double PoiseRegenRate = 20.0;

	/** Duration of poise break recovery state (can't be chain-staggered during this) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Poise")
	double PoiseBreakRecoveryDuration = 1.5;

	/** Knockback magnitude when poise breaks (larger than normal hit) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Poise")
	double PoiseBreakKnockbackMin = 400.0;

	/** Knockback magnitude when poise breaks (larger than normal hit) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Poise")
	double PoiseBreakKnockbackMax = 600.0;

	/** Called when poise regen delay expires - starts regenerating poise */
	UFUNCTION()
	void OnPoiseRegenDelayExpired();

	/** Called periodically to regenerate poise */
	UFUNCTION()
	void OnPoiseRegenTick();

	/** Called when poise break recovery ends - can be staggered again */
	UFUNCTION()
	void OnPoiseBreakRecoveryEnd();

	/** Trigger poise break stagger (called when poise reaches 0) */
	void TriggerPoiseBreakStagger(const FHitResult& HitInfo);

	/** Reset poise regen timer (called when taking damage) */
	void ResetPoiseRegenTimer();

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (20)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void SetGuardState(bool Toggled, bool IgnoreGracePeriod);
	virtual void SetGuardState_Implementation(bool Toggled, bool IgnoreGracePeriod);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	bool GetIsGuarding();
	virtual bool GetIsGuarding_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void SetExecutionTarget(AActor* InExecutionTarget);
	virtual void SetExecutionTarget_Implementation(AActor* InExecutionTarget);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void HandleProjectileDamage(double IncomingDamage, UNiagaraSystem* ProjectileHitEffect, const FGameplayTag& NegationStat, const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects);
	virtual void HandleProjectileDamage_Implementation(double IncomingDamage, UNiagaraSystem* ProjectileHitEffect, const FGameplayTag& NegationStat, const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void HandleIncomingWeaponDamage(AActor* WeaponOwnerActor, USoundBase* GuardSound, USoundBase* PerfectGuardSound, const FHitResult& HitInfo, double IncomingDamage, double IncomingPoiseDamage, const TMap<FGameplayTag, UPrimaryDataAsset*>& IncomingStatusEffect);
	virtual void HandleIncomingWeaponDamage_Implementation(AActor* WeaponOwnerActor, USoundBase* GuardSound, USoundBase* PerfectGuardSound, const FHitResult& HitInfo, double IncomingDamage, double IncomingPoiseDamage, const TMap<FGameplayTag, UPrimaryDataAsset*>& IncomingStatusEffect);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	double GetStaminaDrainAmountForDamage(double IncomingDamage);
	virtual double GetStaminaDrainAmountForDamage_Implementation(double IncomingDamage);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	FVector GetKnockbackAmountForDamage(const FHitResult& Hit, double Damage, double MaxClamp);
	virtual FVector GetKnockbackAmountForDamage_Implementation(const FHitResult& Hit, double Damage, double MaxClamp);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void ProcessHandTrace(const TArray<FHitResult>& Hits);
	virtual void ProcessHandTrace_Implementation(const TArray<FHitResult>& Hits);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void ApplyFistDamage(AActor* TargetActor, const FHitResult& Hit);
	virtual void ApplyFistDamage_Implementation(AActor* TargetActor, const FHitResult& Hit);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void SetInvincibility(bool InIsInvincible);
	virtual void SetInvincibility_Implementation(bool InIsInvincible);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void IncreaseTimeSinceGuard();
	virtual void IncreaseTimeSinceGuard_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	double GetTimeSinceGuardActivation();
	virtual double GetTimeSinceGuardActivation_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void SetHyperArmor(bool InHyperArmor);
	virtual void SetHyperArmor_Implementation(bool InHyperArmor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	bool GetIsFacingEnemy(AActor* TargetActor);
	virtual bool GetIsFacingEnemy_Implementation(AActor* TargetActor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void HandleHitReaction(const FHitResult& HitInfo);
	virtual void HandleHitReaction_Implementation(const FHitResult& HitInfo);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void HandleDeath(UStatManagerComponent* StatManager, const FHitResult& HitInfo);
	virtual void HandleDeath_Implementation(UStatManagerComponent* StatManager, const FHitResult& HitInfo);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void ApplyIncomingStatusEffects(const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects, double Multiplier);
	virtual void ApplyIncomingStatusEffects_Implementation(const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects, double Multiplier);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void DropCurrency();
	virtual void DropCurrency_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void GetBossDoorCurrencyDropLocation(FVector& OutReturnValue, FVector& OutReturnValue_1);
	virtual void GetBossDoorCurrencyDropLocation_Implementation(FVector& OutReturnValue, FVector& OutReturnValue_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CombatManager")
	void GuardGracePeriod();
	virtual void GuardGracePeriod_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// CUSTOM EVENTS (11)
	// ═══════════════════════════════════════════════════════════════════════

	// --- Hand Tracing for Unarmed Combat ---
	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|HandTrace")
	void TraceLeftHand();

	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|HandTrace")
	void TraceRightHand();

	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|HandTrace")
	void EventToggleHandTrace(bool bEnabled, ESLFTraceType Type);

	// --- Combo System ---
	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|Combo")
	void EventResetAttackCombo();

	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|Combo")
	void EventBeginSoftCombo(USkeletalMeshComponent* InMesh, TSoftObjectPtr<UAnimMontage> SoftMontage, double PlayRate);

	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|Combo")
	void EventRegisterNextCombo(FName InComboSection);

	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|Combo")
	void EventBeginCombo(USkeletalMeshComponent* InMesh, UAnimMontage* MontageToPlay, float PlayRate);

	// --- Stat Change Callbacks ---
	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|Events")
	void EventOnHealthChanged(UObject* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);

	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|Events")
	void EventOnAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|Events")
	void EventOnPoiseChanged(UObject* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);

	UFUNCTION(BlueprintCallable, Category = "AC_CombatManager|Events")
	void EventOnDeath(bool bRagdoll, ESLFDirection KillingBlowDirection);

private:
	// Internal helper for sphere trace hand combat
	void PerformHandTrace(FName SocketName);
};
