// AICombatManagerComponent.h
// C++ base class for AC_AI_CombatManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_AI_CombatManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         41/41 migrated (excluding 2 dispatchers counted separately)
// Functions:         25/25 migrated (excluding ExecuteUbergraph and montage callbacks)
// Event Dispatchers: 2/2 migrated
// Graphs:            18 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager
//
// PURPOSE: AI combat system - damage, abilities, health bar, poise break, death

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AICombatManagerComponent.generated.h"

// Forward declarations
class UDataAsset;
class UAnimMontage;
class UNiagaraSystem;
class USoundBase;
class USkeletalMeshComponent;

/**
 * AI ability data structure
 */
USTRUCT(BlueprintType)
struct FSLFAIAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ability")
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ability")
	UDataAsset* AbilityAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ability")
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ability")
	float Cooldown = 0.0f;
};

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 2/2 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/2] Called when poise is broken - bBroken indicates poise break state */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIPoiseBroken, bool, bBroken);

/** [2/2] Called when AI dies */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIDeath, AActor*, Killer);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UAICombatManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAICombatManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 41/41 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- State Flags (5) ---

	/** [1/41] Whether AI is dead */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|State")
	bool bIsDead;

	/** [2/41] Whether poise is broken */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|State")
	bool bPoiseBroken;

	/** [3/41] Whether hyper armor is active */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|State")
	bool bHyperArmor;

	/** [4/41] Whether invincible */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|State")
	bool bInvincible;

	/** [5/41] Whether healthbar is currently active */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|State")
	bool bHealthbarActive;

	// --- Healthbar Config (4) ---

	/** [6/41] Whether to use world-space healthbar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Healthbar")
	bool bUseWorldHealthbar;

	/** [7/41] Healthbar display duration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Healthbar")
	float HealthBarDuration;

	/** [8/41] Healthbar timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Healthbar")
	FTimerHandle HealthbarTimer;

	/** [9/41] Healthbar line-of-sight timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Healthbar")
	FTimerHandle HealthbarLosTimer;

	// --- Poise Break Config (4) ---

	/** [10/41] Poise break animation asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Poise")
	UDataAsset* PoiseBreakAsset;

	/** [11/41] Duration of broken poise state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Poise")
	float BrokenPoiseDuration;

	/** [12/41] Poise break timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Poise")
	FTimerHandle PoiseBreakTimer;

	/** Active poise break loop montage (for stopping when recovery happens) */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Poise")
	UAnimMontage* PoiseBreakLoopMontage;

	/** Timer for poise regeneration delay */
	FTimerHandle PoiseRegenDelayTimer;

	/** Timer for poise regeneration tick */
	FTimerHandle PoiseRegenTickTimer;

	/** Delay before poise starts regenerating (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Poise")
	float PoiseRegenDelay;

	/** Poise regeneration rate (points per second) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Poise")
	float PoiseRegenRate;

	/** Reset poise regen timer (called when taking damage) */
	void ResetPoiseRegenTimer();

	/** Called when poise regen delay expires - starts regenerating poise */
	void OnPoiseRegenDelayExpired();

	/** Called periodically to regenerate poise */
	void OnPoiseRegenTick();

	// --- Combat Config (7) ---

	/** [13/41] Line of sight check interval */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Config")
	float LineOfSightCheckInterval;

	/** [14/41] Delay after death before cleanup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Config")
	float DelayAfterDeath;

	/** [15/41] Whether AI causes hit stop on hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Config")
	bool bCausesHitStop;

	/** [16/41] Hit stop duration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Config")
	float HitStopDuration;

	/** [17/41] Whether AI can be pushed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Config")
	bool bCanBePushed;

	/** [18/41] Whether to ragdoll on death */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Config")
	bool bRagdollOnDeath;

	/** [19/41] Currency reward on death */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Config")
	int32 CurrencyReward;

	// --- Damage/Combat Runtime (3) ---

	/** [20/41] Actor that caused last damage */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Runtime")
	AActor* DamagingActor;

	/** [21/41] IK blend weight */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Runtime")
	float IkWeight;

	/** Timer handle for IK flinch animation */
	FTimerHandle IKReactionTimerHandle;

	/** IK flinch animation start time */
	double IKReactionStartTime = 0.0;

	/** IK flinch animation duration (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HitReact")
	double IKReactionDuration = 0.3;

	/** Peak IK weight during flinch */
	double IKReactionPeakWeight = 1.0;

	/** Update IK reaction animation (called by timer) */
	void UpdateIKReaction();

	/** Start IK flinch animation */
	void PlayIKFlinch(double Scale);

	/** [22/41] Current hit normal vector */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Runtime")
	FVector CurrentHitNormal;

	// --- Hit Reaction (3) ---

	/** [23/41] Current hit reaction type */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|HitReact")
	ESLFHitReactType HitReactType;

	/** [24/41] Reaction animset data asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HitReact")
	UDataAsset* ReactionAnimset;

	/** [25/41] DataTable of executed montages (FExecutionInfo rows with Tag + Animation) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HitReact")
	UDataTable* ExecutedMontagesTable;

	// --- Effects (3) ---

	/** [26/41] Hit VFX effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Effects")
	UNiagaraSystem* HitVFX;

	/** [27/41] Hit sound effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Effects")
	USoundBase* HitSFX;

	/** [28/41] Mesh component reference */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Runtime")
	USkeletalMeshComponent* Mesh;

	// --- Hand Trace (11) ---

	/** [29/41] Right hand trace timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|HandTrace")
	FTimerHandle RightHandTraceTimer;

	/** [30/41] Left hand trace timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|HandTrace")
	FTimerHandle LeftHandTraceTimer;

	/** [31/41] Hand trace sphere radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HandTrace")
	float HandTraceRadius;

	/** [32/41] Left hand socket start name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HandTrace")
	FName HandSocket_L_Start;

	/** [33/41] Left hand socket end name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HandTrace")
	FName HandSocket_L_End;

	/** [34/41] Right hand socket start name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HandTrace")
	FName HandSocket_R_Start;

	/** [35/41] Right hand socket end name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HandTrace")
	FName HandSocket_R_End;

	/** [36/41] Actors traced by fist attacks */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|HandTrace")
	TArray<AActor*> FistTracedActors;

	/** [37/41] Minimum unarmed poise damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HandTrace")
	float MinUnarmedPoiseDamage;

	/** [38/41] Maximum unarmed poise damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|HandTrace")
	float MaxUnarmedPoiseDamage;

	// --- Abilities (3) ---

	/** [39/41] Available abilities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat|Abilities")
	TArray<FSLFAIAbility> Abilities;

	/** [40/41] Ability cooldown timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Abilities")
	FTimerHandle AbilityTimer;

	/** [41/41] Currently selected ability */
	UPROPERTY(BlueprintReadWrite, Category = "AI Combat|Abilities")
	UDataAsset* SelectedAbility;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAIPoiseBroken OnPoiseBroken;

	/** [2/2] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAIDeath OnDeath;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 25/25 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Damage Handling (4) ---

	/** [1/25] Handle incoming weapon damage for AI */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Damage")
	void HandleIncomingWeaponDamage_AI(AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult);
	virtual void HandleIncomingWeaponDamage_AI_Implementation(AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult);

	/** [2/25] Handle projectile damage for AI */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Damage")
	void HandleProjectileDamage_AI(AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult);
	virtual void HandleProjectileDamage_AI_Implementation(AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult);

	/** [3/25] Get knockback amount for damage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Damage")
	float GetKnockbackAmountForDamage(float Damage);
	virtual float GetKnockbackAmountForDamage_Implementation(float Damage);

	/** [4/25] Apply incoming status effects */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Damage")
	void ApplyIncomingStatusEffects(AActor* DamageCauser, const TArray<FGameplayTag>& StatusEffects);
	virtual void ApplyIncomingStatusEffects_Implementation(AActor* DamageCauser, const TArray<FGameplayTag>& StatusEffects);

	// --- Hit Reaction (3) ---

	/** [5/25] Handle hit reaction */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|HitReact")
	void HandleHitReaction(ESLFHitReactType ReactionType, const FVector& HitDirection);
	virtual void HandleHitReaction_Implementation(ESLFHitReactType ReactionType, const FVector& HitDirection);

	/** [6/25] Get relevant executed montage for critical state by tag lookup in DataTable */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|HitReact")
	UAnimMontage* GetRelevantExecutedMontage(const FGameplayTag& ExecutionTag);
	virtual UAnimMontage* GetRelevantExecutedMontage_Implementation(const FGameplayTag& ExecutionTag);

	// --- Death (2) ---

	/** [7/25] Handle AI death */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Death")
	void HandleDeath(AActor* Killer);
	virtual void HandleDeath_Implementation(AActor* Killer);

	/** [8/25] End encounter (cleanup) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Death")
	void EndEncounter();
	virtual void EndEncounter_Implementation();

	// --- State Setters (2) ---

	/** [9/25] Set invincibility state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|State")
	void SetInvincibility(UPARAM(DisplayName = "Is Invincible?") bool IsInvincible);
	virtual void SetInvincibility_Implementation(bool IsInvincible);

	/** [10/25] Set hyper armor state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|State")
	void SetHyperArmor(bool bEnabled);
	virtual void SetHyperArmor_Implementation(bool bEnabled);

	// --- Abilities (4) ---

	/** [11/25] Try to get an ability for current situation */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Abilities")
	bool TryGetAbility(UDataAsset*& OutAbility);
	virtual bool TryGetAbility_Implementation(UDataAsset*& OutAbility);

	/** [12/25] Evaluate ability rule for selection */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Abilities")
	bool EvaluateAbilityRule(const FSLFAIAbility& Ability);
	virtual bool EvaluateAbilityRule_Implementation(const FSLFAIAbility& Ability);

	/** [13/25] Get currently selected ability */
	UFUNCTION(BlueprintPure, Category = "AI Combat|Abilities")
	UDataAsset* GetSelectedAbility() const { return SelectedAbility; }

	/** [14/25] Override abilities array */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Abilities")
	void OverrideAbilities(const TArray<FSLFAIAbility>& NewAbilities);
	virtual void OverrideAbilities_Implementation(const TArray<FSLFAIAbility>& NewAbilities);

	// --- Hand Trace (4) ---

	/** [15/25] Trace right hand */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|HandTrace")
	void TraceRightHand();
	virtual void TraceRightHand_Implementation();

	/** [16/25] Trace left hand */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|HandTrace")
	void TraceLeftHand();
	virtual void TraceLeftHand_Implementation();

	/** [17/25] Process hand trace results */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|HandTrace")
	void ProcessHandTrace(const TArray<FHitResult>& HitResults);
	virtual void ProcessHandTrace_Implementation(const TArray<FHitResult>& HitResults);

	/** [18/25] Apply fist damage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|HandTrace")
	void ApplyFistDamage(AActor* Target, const FHitResult& HitResult);
	virtual void ApplyFistDamage_Implementation(AActor* Target, const FHitResult& HitResult);

	/** [19/25] Toggle hand trace on/off */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|HandTrace")
	void ToggleHandTrace(bool bEnable, bool bRightHand);
	virtual void ToggleHandTrace_Implementation(bool bEnable, bool bRightHand);

	// --- Poise Break (3) ---

	/** [20/25] Finish poise break state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Poise")
	void FinishPoiseBreak();
	virtual void FinishPoiseBreak_Implementation();

	/** [21/25] Called when poise break ends */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Poise")
	void OnPoiseBreakEnd();
	virtual void OnPoiseBreakEnd_Implementation();

	/** [22/25] Clear poise break reset timer */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Poise")
	void ClearPoiseBreakResetTimer();
	virtual void ClearPoiseBreakResetTimer_Implementation();

	// --- Healthbar (2) ---

	/** [23/25] Check line of sight for healthbar */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Healthbar")
	void CheckLineOfSight();
	virtual void CheckLineOfSight_Implementation();

	/** [24/25] Disable healthbar display */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Healthbar")
	void DisableHealthbar();
	virtual void DisableHealthbar_Implementation();

	// --- Event Binding (1) ---

	/** [25/25] Bind to stat update events */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Combat|Events")
	void BindStatUpdates();
	virtual void BindStatUpdates_Implementation();
};
