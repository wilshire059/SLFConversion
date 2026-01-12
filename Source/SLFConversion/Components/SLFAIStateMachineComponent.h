// SLFAIStateMachineComponent.h
// Custom C++ State Machine for Soulslike AI
// Replaces Behavior Tree for better performance and Elden Ring-style combat
//
// ═══════════════════════════════════════════════════════════════════════════════
// ARCHITECTURE OVERVIEW
// ═══════════════════════════════════════════════════════════════════════════════
//
// MAIN STATES (ESLFAIState):
//   Idle          - Standing at spawn, no target
//   Patrol        - Following patrol path
//   RandomRoam    - Wandering randomly
//   Investigating - Heard something, checking it out
//   Combat        - Engaged with target (has sub-states)
//   PoiseBroken   - Staggered, vulnerable
//   Uninterruptable - Executing special move, cannot be interrupted
//   Dead          - Dead (terminal state)
//
// COMBAT SUB-STATES (ESLFCombatSubState):
//   Engaging      - Closing distance to target
//   Positioning   - Circling/strafing around target
//   WindingUp     - Preparing attack (can read player input here)
//   Attacking     - Executing attack animation
//   Recovering    - Post-attack cooldown
//   Blocking      - Holding block (if has shield)
//   Retreating    - Backing away to reset
//
// BOSS FEATURES:
//   - Phase transitions at health thresholds
//   - Special moves with dedicated states
//   - Enrage mechanics
//   - Input reading (react to player healing/rolling)
//
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFAIStateMachineComponent.generated.h"

// Forward declarations
class AAIController;
class APawn;
class UAnimMontage;
class UAICombatManagerComponent;
class UNavigationSystemV1;

// ═══════════════════════════════════════════════════════════════════════════════
// ENUMS
// ═══════════════════════════════════════════════════════════════════════════════

/** Main AI States */
UENUM(BlueprintType)
enum class ESLFAIState : uint8
{
	Idle            = 0,
	RandomRoam      = 1,
	Patrol          = 2,
	Investigating   = 3,
	Combat          = 4,
	PoiseBroken     = 5,
	Uninterruptable = 6,
	Dead            = 7,
	OutOfBounds     = 8
};

/** Combat Sub-States (only valid when MainState == Combat) */
UENUM(BlueprintType)
enum class ESLFCombatSubState : uint8
{
	None            = 0,   // Not in combat
	Engaging        = 1,   // Closing distance
	Positioning     = 2,   // Strafing/circling
	WindingUp       = 3,   // Pre-attack (can hold for input reading)
	Attacking       = 4,   // Attack animation playing
	Recovering      = 5,   // Post-attack cooldown
	Blocking        = 6,   // Holding block
	Retreating      = 7    // Backing away
};

/** Boss Phase */
UENUM(BlueprintType)
enum class ESLFBossPhase : uint8
{
	NotBoss     = 0,
	Phase1      = 1,
	Phase2      = 2,
	Phase3      = 3,
	Enraged     = 4
};

// ═══════════════════════════════════════════════════════════════════════════════
// DELEGATES
// ═══════════════════════════════════════════════════════════════════════════════

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAIStateChanged, ESLFAIState, OldState, ESLFAIState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatSubStateChanged, ESLFCombatSubState, OldState, ESLFCombatSubState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossPhaseChanged, ESLFBossPhase, OldPhase, ESLFBossPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);

// ═══════════════════════════════════════════════════════════════════════════════
// STRUCTS
// ═══════════════════════════════════════════════════════════════════════════════

/** Configuration for AI behavior tuning */
USTRUCT(BlueprintType)
struct FSLFAIConfig
{
	GENERATED_BODY()

	// === DETECTION ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float SightRange = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float HearingRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float LoseTargetTime = 5.0f;

	// === COMBAT DISTANCES ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float PreferredCombatDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxChaseDistance = 1500.0f;

	// === TIMING ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float MinAttackDelay = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float MaxAttackDelay = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float WindUpHoldTime = 0.0f;  // Time to hold wind-up (for input reading)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float PostAttackRecovery = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float RepositionInterval = 3.0f;

	// === MOVEMENT ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StrafeSpeed = 150.0f;

	// === ELDEN RING STYLE ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	bool bEnableInputReading = false;  // React to player healing/rolling

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	float InputReadingReactionTime = 0.2f;  // Delay before reacting

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	float PunishHealingChance = 0.7f;  // Chance to punish healing

	// === BOSS ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	bool bIsBoss = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float Phase2HealthThreshold = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float Phase3HealthThreshold = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float EnrageHealthThreshold = 0.15f;
};

// ═══════════════════════════════════════════════════════════════════════════════
// MAIN COMPONENT
// ═══════════════════════════════════════════════════════════════════════════════

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API USLFAIStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLFAIStateMachineComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ═══════════════════════════════════════════════════════════════════════════
	// CONFIGURATION
	// ═══════════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
	FSLFAIConfig Config;

	// ═══════════════════════════════════════════════════════════════════════════
	// STATE ACCESS
	// ═══════════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintCallable, Category = "AI State")
	ESLFAIState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "AI State")
	ESLFAIState GetPreviousState() const { return PreviousState; }

	UFUNCTION(BlueprintCallable, Category = "AI State")
	ESLFCombatSubState GetCombatSubState() const { return CombatSubState; }

	UFUNCTION(BlueprintCallable, Category = "AI State")
	ESLFBossPhase GetBossPhase() const { return BossPhase; }

	UFUNCTION(BlueprintCallable, Category = "AI State")
	float GetTimeInCurrentState() const { return TimeInCurrentState; }

	UFUNCTION(BlueprintCallable, Category = "AI State")
	AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

	// ═══════════════════════════════════════════════════════════════════════════
	// STATE CONTROL
	// ═══════════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintCallable, Category = "AI State")
	void SetState(ESLFAIState NewState);

	UFUNCTION(BlueprintCallable, Category = "AI State")
	void SetTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "AI State")
	void ClearTarget();

	UFUNCTION(BlueprintCallable, Category = "AI State")
	void TriggerPoiseBroken();

	UFUNCTION(BlueprintCallable, Category = "AI State")
	void OnDeath();

	// ═══════════════════════════════════════════════════════════════════════════
	// DELEGATES
	// ═══════════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "AI Events")
	FOnAIStateChanged OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "AI Events")
	FOnCombatSubStateChanged OnCombatSubStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "AI Events")
	FOnBossPhaseChanged OnBossPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "AI Events")
	FOnAttackStarted OnAttackStarted;

	UPROPERTY(BlueprintAssignable, Category = "AI Events")
	FOnAttackEnded OnAttackEnded;

	// ═══════════════════════════════════════════════════════════════════════════
	// DEBUG
	// ═══════════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugEnabled = false;

	UFUNCTION(BlueprintCallable, Category = "Debug")
	FString GetDebugString() const;

private:
	// ═══════════════════════════════════════════════════════════════════════════
	// INTERNAL STATE
	// ═══════════════════════════════════════════════════════════════════════════

	UPROPERTY()
	ESLFAIState CurrentState = ESLFAIState::Idle;

	UPROPERTY()
	ESLFAIState PreviousState = ESLFAIState::Idle;

	UPROPERTY()
	ESLFCombatSubState CombatSubState = ESLFCombatSubState::None;

	UPROPERTY()
	ESLFBossPhase BossPhase = ESLFBossPhase::NotBoss;

	float TimeInCurrentState = 0.0f;
	float TimeInCombatSubState = 0.0f;
	float LastAttackTime = 0.0f;
	float NextAttackDelay = 0.0f;
	float LastRepositionTime = 0.0f;

	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;

	UPROPERTY()
	FVector SpawnLocation;

	UPROPERTY()
	FVector LastKnownTargetLocation;

	bool bIsAttacking = false;
	bool bWindUpHeld = false;

	// Cached references
	UPROPERTY()
	TWeakObjectPtr<AAIController> CachedAIController;

	UPROPERTY()
	TWeakObjectPtr<APawn> CachedPawn;

	UPROPERTY()
	TWeakObjectPtr<UAICombatManagerComponent> CachedCombatManager;

	UPROPERTY()
	TWeakObjectPtr<UAnimInstance> CachedAnimInstance;

	// ═══════════════════════════════════════════════════════════════════════════
	// STATE TICK FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════════

	void TickIdle(float DeltaTime);
	void TickPatrol(float DeltaTime);
	void TickRandomRoam(float DeltaTime);
	void TickInvestigating(float DeltaTime);
	void TickCombat(float DeltaTime);
	void TickPoiseBroken(float DeltaTime);
	void TickUninterruptable(float DeltaTime);
	void TickOutOfBounds(float DeltaTime);

	// ═══════════════════════════════════════════════════════════════════════════
	// COMBAT SUB-STATE FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════════

	void SetCombatSubState(ESLFCombatSubState NewSubState);
	void TickCombat_Engaging(float DeltaTime);
	void TickCombat_Positioning(float DeltaTime);
	void TickCombat_WindingUp(float DeltaTime);
	void TickCombat_Attacking(float DeltaTime);
	void TickCombat_Recovering(float DeltaTime);
	void TickCombat_Retreating(float DeltaTime);

	// ═══════════════════════════════════════════════════════════════════════════
	// STATE ENTER/EXIT
	// ═══════════════════════════════════════════════════════════════════════════

	void OnEnterState(ESLFAIState State);
	void OnExitState(ESLFAIState State);
	void OnEnterCombatSubState(ESLFCombatSubState SubState);
	void OnExitCombatSubState(ESLFCombatSubState SubState);

	// ═══════════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════════

	void CacheReferences();
	bool HasValidTarget() const;
	float GetDistanceToTarget() const;
	FVector GetDirectionToTarget() const;
	bool IsTargetInAttackRange() const;
	bool CanSeeTarget() const;
	void MoveToTarget();
	void MoveToLocation(const FVector& Location);
	void StopMovement();
	void FaceTarget();
	void StrafeAroundTarget(float DeltaTime, bool bClockwise);

	// Combat helpers
	bool TrySelectAbility(UDataAsset*& OutAbility);
	void StartAttack(UDataAsset* Ability);
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	float CalculateNextAttackDelay() const;

	// Input reading (Elden Ring style)
	bool ShouldReactToPlayerInput() const;
	bool IsPlayerHealing() const;
	bool IsPlayerRolling() const;

	// Boss helpers
	void CheckBossPhaseTransition();
	void OnBossPhaseTransition(ESLFBossPhase NewPhase);

	// Patrol helpers
	FVector GetNextPatrolPoint();
	FVector GetRandomRoamPoint();

	// Bounds checking
	bool IsOutOfBounds() const;
	void ReturnToSpawn();
};
