// AC_AI_CombatManager.h
// C++ component for AC_AI_CombatManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_AI_CombatManager.json
// Variables: 41 | Functions: 15 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "AC_AI_CombatManager.generated.h"

// Forward declarations
class UAC_StatManager;
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_AI_CombatManager_OnPoiseBroken, bool, Broken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAC_AI_CombatManager_OnDeath);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_AI_CombatManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_AI_CombatManager();

protected:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (41)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsDead;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool HealthbarActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Abilities")
	TArray<UPrimaryDataAsset*> Abilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|UI")
	bool UseWorldHealthbar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|UI")
	double HealthBarDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Poise Break", meta = (DisplayName = "Poise Break Asset"))
	UPDA_PoiseBreakAnimData* PoiseBreakAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Poise Break")
	double BrokenPoiseDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|UI")
	double LineOfSightCheckInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|UI")
	double DelayAfterDeath;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool PoiseBroken;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool HyperArmor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool Invincible;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle HealthbarTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle HealthbarLosTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	AActor* DamagingActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	double IkWeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FVector CurrentHitNormal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hitstop")
	bool CausesHitStop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hitstop")
	double HitStopDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 CurrencyReward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle PoiseBreakTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Reactions")
	bool CanBePushed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Reactions")
	bool RagdollOnDeath;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Reactions")
	ESLFHitReactType HitReactType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Reactions")
	UPrimaryDataAsset* ReactionAnimset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Reactions")
	UDataTable* ExecutedMontages;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UNiagaraSystem> HitVFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	USkeletalMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle RightHandTraceTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle LeftHandTraceTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	double HandTraceRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	FName HandSocket_L_Start;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	FName HandSocket_L_End;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	FName HandSocket_R_Start;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<AActor*> FistTracedActors;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	FName HandSocket_R_End;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	double MinUnarmedPoiseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	double MaxUnarmedPoiseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	double MinUnarmedDamage = 25.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Unarmed")
	double MaxUnarmedDamage = 35.0;

	/** Default status effects to apply when this enemy attacks.
	 *  Key: Status effect data asset (e.g., DA_StatusEffect_Bleed)
	 *  Value: Application data with Rank and BuildupAmount
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|StatusEffects")
	TMap<UPrimaryDataAsset*, FSLFStatusEffectApplication> DefaultAttackStatusEffects;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UPrimaryDataAsset*> AbilityTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UPrimaryDataAsset* SelectedAbility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<USoundBase> HitSFX;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_AI_CombatManager_OnPoiseBroken OnPoiseBroken;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_AI_CombatManager_OnDeath OnDeath;

	// ═══════════════════════════════════════════════════════════════════════
	// POISE SYSTEM (Elden Ring Style)
	// ═══════════════════════════════════════════════════════════════════════

	/** Timer for poise regeneration delay */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Poise")
	FTimerHandle PoiseRegenDelayTimer;

	/** Timer for poise regeneration tick */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Poise")
	FTimerHandle PoiseRegenTickTimer;

	/** Delay before poise starts regenerating (seconds) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Poise")
	double PoiseRegenDelay = 2.0;

	/** Poise regeneration rate (points per second) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Poise")
	double PoiseRegenRate = 30.0;

	/** Trigger poise break stagger */
	void TriggerPoiseBreakStagger(const FHitResult& HitInfo);

	/** Called when poise break recovery ends */
	UFUNCTION()
	void OnPoiseBreakRecoveryEnd();

	/** Reset poise regen timer (called on damage) */
	void ResetPoiseRegenTimer();

	/** Called when poise regen delay expires */
	UFUNCTION()
	void OnPoiseRegenDelayExpired();

	/** Called to regenerate poise */
	UFUNCTION()
	void OnPoiseRegenTick();

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (15)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void TryGetAbility(UPrimaryDataAsset*& OutSelectedAbility, UPrimaryDataAsset*& OutSelectedAbility_1, UPrimaryDataAsset*& OutSelectedAbility_2, UPrimaryDataAsset*& OutSelectedAbility_3, UPrimaryDataAsset*& OutSelectedAbility_4);
	virtual void TryGetAbility_Implementation(UPrimaryDataAsset*& OutSelectedAbility, UPrimaryDataAsset*& OutSelectedAbility_1, UPrimaryDataAsset*& OutSelectedAbility_2, UPrimaryDataAsset*& OutSelectedAbility_3, UPrimaryDataAsset*& OutSelectedAbility_4);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void EvaluateAbilityRule(const FInstancedStruct& Rule, bool& OutApproved, bool& OutApproved_1, bool& OutApproved_2, bool& OutApproved_3, bool& OutApproved_4, bool& OutApproved_5, bool& OutApproved_6, bool& OutApproved_7, bool& OutApproved_8, bool& OutApproved_9, bool& OutApproved_10, bool& OutApproved_11, bool& OutApproved_12, bool& OutApproved_13);
	virtual void EvaluateAbilityRule_Implementation(const FInstancedStruct& Rule, bool& OutApproved, bool& OutApproved_1, bool& OutApproved_2, bool& OutApproved_3, bool& OutApproved_4, bool& OutApproved_5, bool& OutApproved_6, bool& OutApproved_7, bool& OutApproved_8, bool& OutApproved_9, bool& OutApproved_10, bool& OutApproved_11, bool& OutApproved_12, bool& OutApproved_13);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void HandleDeath(UAC_StatManager* StatManager, const FHitResult& HitInfo);
	virtual void HandleDeath_Implementation(UAC_StatManager* StatManager, const FHitResult& HitInfo);

	/** Internal death event handler - called when HP check confirms death
	 *  This replaces the Blueprint "Event OnDeath" custom event
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void EventOnDeath(bool bRagdoll, ESLFDirection KillingBlowDirection);
	virtual void EventOnDeath_Implementation(bool bRagdoll, ESLFDirection KillingBlowDirection);

	/** Calculate hit direction from hit result relative to owner actor
	 *  Replaces calls to BFL_Helper::GetDirectionFromHit
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AC_AI_CombatManager")
	ESLFDirection CalculateHitDirection(const FHitResult& HitInfo) const;

	/** Calculate stat percent value (current/max)
	 *  Replaces calls to BFL_Helper::GetStatPercentValue
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AC_AI_CombatManager")
	static double CalculateStatPercent(const FStatInfo& StatInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void HandleHitReaction(const FHitResult& HitInfo);
	virtual void HandleHitReaction_Implementation(const FHitResult& HitInfo);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	TSoftObjectPtr<UAnimMontage> GetRelevantExecutedMontage(const FGameplayTag& ExecutionTag);
	virtual TSoftObjectPtr<UAnimMontage> GetRelevantExecutedMontage_Implementation(const FGameplayTag& ExecutionTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void SetHyperArmor(bool InHyperArmor);
	virtual void SetHyperArmor_Implementation(bool InHyperArmor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void HandleIncomingWeaponDamage_AI(AActor* WeaponOwnerActor, const FHitResult& HitInfo, double IncomingDamage, double IncomingPoiseDamage, const TMap<FGameplayTag, UPrimaryDataAsset*>& IncomingStatusEffects);
	virtual void HandleIncomingWeaponDamage_AI_Implementation(AActor* WeaponOwnerActor, const FHitResult& HitInfo, double IncomingDamage, double IncomingPoiseDamage, const TMap<FGameplayTag, UPrimaryDataAsset*>& IncomingStatusEffects);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void HandleProjectileDamage_AI(double IncomingDamage, UNiagaraSystem* ProjectileHitEffect, const FGameplayTag& NegationStat, const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects);
	virtual void HandleProjectileDamage_AI_Implementation(double IncomingDamage, UNiagaraSystem* ProjectileHitEffect, const FGameplayTag& NegationStat, const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void ApplyIncomingStatusEffects(const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects, double Multiplier);
	virtual void ApplyIncomingStatusEffects_Implementation(const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects, double Multiplier);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void ApplyFistDamage(AActor* TargetActor, const FHitResult& Hit);
	virtual void ApplyFistDamage_Implementation(AActor* TargetActor, const FHitResult& Hit);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void ProcessHandTrace(const TArray<FHitResult>& Hits);
	virtual void ProcessHandTrace_Implementation(const TArray<FHitResult>& Hits);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	UPrimaryDataAsset* GetSelectedAbility();
	virtual UPrimaryDataAsset* GetSelectedAbility_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void SetInvincibility(bool IsInvincible);
	virtual void SetInvincibility_Implementation(bool IsInvincible);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	FVector GetKnockbackAmountForDamage(const FHitResult& Hit, double Damage, double MaxClamp);
	virtual FVector GetKnockbackAmountForDamage_Implementation(const FHitResult& Hit, double Damage, double MaxClamp);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_CombatManager")
	void OverrideAbilities(const TArray<UPrimaryDataAsset*>& InAbilities);
	virtual void OverrideAbilities_Implementation(const TArray<UPrimaryDataAsset*>& InAbilities);
};
