// CombatManagerComponent.h
// C++ base class for AC_CombatManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_CombatManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         37/37 migrated
// Functions:         28/28 migrated (excluding ExecuteUbergraph and montage callbacks)
// Event Dispatchers: 0/0
// Graphs:            21 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager
//
// PURPOSE: Player combat system - damage, guarding, hit reactions, death, combos

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "CombatManagerComponent.generated.h"

// Forward declarations
class UDataAsset;
class UAnimMontage;
class UCameraShakeBase;
class UNiagaraSystem;
class USoundBase;
class USkeletalMeshComponent;

// Types used from SLFEnums.h:
// - ESLFHitReactType

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UCombatManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 37/37 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Combo System (2) ---

	/** [1/37] Current combo section index */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Combo")
	int32 ComboSection;

	/** [2/37] Default combo sequence section name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Combo")
	FName DefaultComboSequenceSection;

	// --- State Flags (6) ---

	/** [3/37] Whether character is dead */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|State")
	bool bIsDead;

	/** [4/37] Whether character is currently guarding */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|State")
	bool bIsGuarding;

	/** [5/37] Whether character wants to guard (input held) */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|State")
	bool bWantsToGuard;

	/** [6/37] Whether character is invincible */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|State")
	bool bIsInvincible;

	/** [7/37] Whether poise is broken */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|State")
	bool bPoiseBroken;

	/** [8/37] Whether hyper armor is active */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|State")
	bool bHyperArmor;

	// --- Execution (1) ---

	/** [9/37] Current execution target actor */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Execution")
	AActor* ExecutionTarget;

	// --- Animation Assets (2) ---

	/** [10/37] Unarmed combat animset data asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UDataAsset* UnarmedAnimset;

	/** [11/37] Reaction animset data asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UDataAsset* ReactionAnimset;

	// --- Effects (4) ---

	/** [12/37] Generic reaction camera shake */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effects")
	TSubclassOf<UCameraShakeBase> GenericReactionShake;

	/** [13/37] Guard camera shake */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effects")
	TSubclassOf<UCameraShakeBase> GuardCameraShake;

	/** [14/37] Hit VFX effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effects")
	UNiagaraSystem* HitVFX;

	/** [15/37] Guard VFX effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effects")
	UNiagaraSystem* GuardEffect;

	// --- Death Config (1) ---

	/** [16/37] Whether to ragdoll on death */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Death")
	bool bRagdollOnDeath;

	// --- Hand Trace (7) ---

	/** [17/37] Right hand trace timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|HandTrace")
	FTimerHandle RightHandTraceTimer;

	/** [18/37] Left hand trace timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|HandTrace")
	FTimerHandle LeftHandTraceTimer;

	/** [19/37] Mesh component for traces */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|HandTrace")
	USkeletalMeshComponent* Mesh;

	/** [20/37] Actors traced by fist attacks */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|HandTrace")
	TArray<AActor*> FistTracedActors;

	/** [21/37] Right hand socket name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|HandTrace")
	FName HandSocket_R;

	/** [22/37] Left hand socket name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|HandTrace")
	FName HandSocket_L;

	// --- Guard Timing (6) ---

	/** [23/37] Guard timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Guard")
	FTimerHandle GuardTimer;

	/** [24/37] Guard grace period timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Guard")
	FTimerHandle GuardGracePeriodTimer;

	/** [25/37] Time since guard was activated */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Guard")
	float TimeSinceGuard;

	/** [26/37] Guard grace period duration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Guard")
	float GuardingGracePeriod;

	/** [27/37] Perfect guard window duration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Guard")
	float PerfectGuardDuration;

	/** [28/37] Perfect guard VFX effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Guard")
	UNiagaraSystem* PerfectGuardEffect;

	// --- Poise Damage (4) ---

	/** [29/37] Minimum poise damage for perfect guard */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Poise")
	float MinPerfectGuardPoiseDamage;

	/** [30/37] Maximum poise damage for perfect guard */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Poise")
	float MaxPerfectGuardPoiseDamage;

	/** [31/37] Minimum unarmed poise damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Poise")
	float MinUnarmedPoiseDamage;

	/** [32/37] Maximum unarmed poise damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Poise")
	float MaxUnarmedPoiseDamage;

	// --- Hit Reaction (4) ---

	/** [33/37] Facing direction acceptance angle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|HitReact")
	float FacingDirectionAcceptance;

	/** [34/37] IK blend weight */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|HitReact")
	float IKWeight;

	/** [35/37] Current hit normal vector */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|HitReact")
	FVector CurrentHitNormal;

	/** [36/37] Current hit reaction type */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|HitReact")
	ESLFHitReactType HitReactType;

	// --- Audio (1) ---

	/** [37/37] Hit sound effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Audio")
	USoundBase* HitSFX;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 28/28 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Damage Handling (4) ---

	/** [1/28] Handle incoming weapon damage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Damage")
	void HandleIncomingWeaponDamage(AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult);
	virtual void HandleIncomingWeaponDamage_Implementation(AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult);

	/** [2/28] Handle projectile damage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Damage")
	void HandleProjectileDamage(AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult);
	virtual void HandleProjectileDamage_Implementation(AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult);

	/** [3/28] Get knockback amount for damage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Damage")
	float GetKnockbackAmountForDamage(float Damage);
	virtual float GetKnockbackAmountForDamage_Implementation(float Damage);

	/** [4/28] Get stamina drain amount for damage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Damage")
	float GetStaminaDrainAmountForDamage(float Damage);
	virtual float GetStaminaDrainAmountForDamage_Implementation(float Damage);

	// --- Hit Reaction (2) ---

	/** [5/28] Handle hit reaction animation/state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|HitReact")
	void HandleHitReaction(ESLFHitReactType ReactionType, const FVector& HitDirection);
	virtual void HandleHitReaction_Implementation(ESLFHitReactType ReactionType, const FVector& HitDirection);

	/** [6/28] Apply incoming status effects from damage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|HitReact")
	void ApplyIncomingStatusEffects(AActor* DamageCauser, const TArray<FGameplayTag>& StatusEffects);
	virtual void ApplyIncomingStatusEffects_Implementation(AActor* DamageCauser, const TArray<FGameplayTag>& StatusEffects);

	// --- Death (2) ---

	/** [7/28] Handle character death */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Death")
	void HandleDeath(AActor* Killer);
	virtual void HandleDeath_Implementation(AActor* Killer);

	/** [8/28] Drop currency on death */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Death")
	void DropCurrency();
	virtual void DropCurrency_Implementation();

	// --- Guard (6) ---

	/** [9/28] Set guard state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Guard")
	void SetGuardState(bool bGuarding);
	virtual void SetGuardState_Implementation(bool bGuarding);

	/** [10/28] Get is guarding */
	UFUNCTION(BlueprintPure, Category = "Combat|Guard")
	bool GetIsGuarding() const { return bIsGuarding; }

	/** [11/28] Get time since guard activation */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Guard")
	float GetTimeSinceGuardActivation();
	virtual float GetTimeSinceGuardActivation_Implementation();

	/** [12/28] Increase time since guard */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Guard")
	void IncreaseTimeSinceGuard(float DeltaTime);
	virtual void IncreaseTimeSinceGuard_Implementation(float DeltaTime);

	/** Timer callback for guard time tracking (calls IncreaseTimeSinceGuard with fixed delta) */
	void OnGuardTimerTick();

	/** [13/28] Guard grace period handler */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Guard")
	void GuardGracePeriod();
	virtual void GuardGracePeriod_Implementation();

	/** [14/28] Get boss door currency drop location */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Death")
	FVector GetBossDoorCurrencyDropLocation();
	virtual FVector GetBossDoorCurrencyDropLocation_Implementation();

	// --- State Setters (3) ---

	/** [15/28] Set invincibility state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|State")
	void SetInvincibility(UPARAM(DisplayName = "Is Invincible?") bool IsInvincible);
	virtual void SetInvincibility_Implementation(bool IsInvincible);

	/** [16/28] Set hyper armor state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|State")
	void SetHyperArmor(bool bEnabled);
	virtual void SetHyperArmor_Implementation(bool bEnabled);

	/** [17/28] Set execution target */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Execution")
	void SetExecutionTarget(AActor* Target);
	virtual void SetExecutionTarget_Implementation(AActor* Target);

	// --- Hand Trace (3) ---

	/** [18/28] Trace right hand for unarmed combat */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|HandTrace")
	void TraceRightHand();
	virtual void TraceRightHand_Implementation();

	/** [19/28] Trace left hand for unarmed combat */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|HandTrace")
	void TraceLeftHand();
	virtual void TraceLeftHand_Implementation();

	/** [20/28] Process hand trace hit results */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|HandTrace")
	void ProcessHandTrace(const TArray<FHitResult>& HitResults);
	virtual void ProcessHandTrace_Implementation(const TArray<FHitResult>& HitResults);

	/** [21/28] Apply fist damage to target */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|HandTrace")
	void ApplyFistDamage(AActor* Target, const FHitResult& HitResult);
	virtual void ApplyFistDamage_Implementation(AActor* Target, const FHitResult& HitResult);

	// --- Utility (2) ---

	/** [22/28] Check if facing enemy */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	bool GetIsFacingEnemy(AActor* Enemy);
	virtual bool GetIsFacingEnemy_Implementation(AActor* Enemy);

	// --- Combo System (6) - Called from Anim Notifies ---

	/** [23/28] Begin combo sequence */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Combo")
	void BeginCombo();
	virtual void BeginCombo_Implementation();

	/** [24/28] Register next combo input */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Combo")
	void RegisterNextCombo();
	virtual void RegisterNextCombo_Implementation();

	/** [25/28] Begin soft combo (branch point) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Combo")
	void BeginSoftCombo();
	virtual void BeginSoftCombo_Implementation();

	/** [26/28] Reset attack combo */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Combo")
	void ResetAttackCombo();
	virtual void ResetAttackCombo_Implementation();

	/** [27/28] Toggle hand trace on/off */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|HandTrace")
	void ToggleHandTrace(bool bEnable, bool bRightHand);
	virtual void ToggleHandTrace_Implementation(bool bEnable, bool bRightHand);

	// --- Event Handlers (1) - These bind to stat/damage events ---

	/** [28/28] Handler for health changed event */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Events")
	void OnHealthChanged(float NewHealth, float MaxHealth);
	virtual void OnHealthChanged_Implementation(float NewHealth, float MaxHealth);
};
