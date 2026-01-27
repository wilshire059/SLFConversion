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

/** Attack Tracking Phase (Elden Ring style: Track -> Track -> LOCK) */
UENUM(BlueprintType)
enum class ESLFAttackTrackingPhase : uint8
{
	None        = 0,   // Not attacking
	Windup      = 1,   // Fast rotation tracking (preparing attack)
	Hold        = 2,   // Slower tracking (held pose, can still adjust)
	Commit      = 3    // Rotation LOCKED (attack swing in progress)
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

	/** Radius to detect player when no target (used in Idle state) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float DetectionRadius = 1500.0f;

	/** Field of view angle in degrees (180 = full hemisphere, enemy can see anything in front)
	 *  NOTE: This is HALF the total FOV (90 means 90 degrees to each side = 180 total) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float FieldOfViewHalfAngle = 90.0f;

	// === COMBAT DISTANCES ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float PreferredCombatDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxChaseDistance = 1500.0f;

	// === TIMING ===
	/** Minimum time between attacks (Elden Ring enemies attack quickly) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float MinAttackDelay = 0.2f;

	/** Maximum time between attacks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float MaxAttackDelay = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float WindUpHoldTime = 0.0f;  // Time to hold wind-up (for input reading)

	/** Recovery time after attack before next action (keep short for aggressive feel) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float PostAttackRecovery = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float RepositionInterval = 1.5f;  // Time to strafe before deciding to re-engage (longer = smoother)

	// === MOVEMENT ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StrafeSpeed = 150.0f;

	/** Distance beyond which AI sprints instead of walks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintThresholdDistance = 500.0f;

	/** Chance to circle left vs right (0.5 = 50/50) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StrafeDirectionBias = 0.5f;

	/** How long to strafe in one direction before potentially switching */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StrafeDirectionChangeInterval = 2.0f;

	// === ELDEN RING STYLE: INPUT READING ===
	/** Enable reacting to player healing/rolling (the "input read") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|InputReading")
	bool bEnableInputReading = true;

	/** Delay before AI reacts to player state (small delay feels natural) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|InputReading")
	float InputReadingReactionTime = 0.15f;

	/** Chance to punish player when they heal (0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|InputReading", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PunishHealingChance = 0.8f;

	/** Chance to punish player when they roll away (0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|InputReading", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PunishRollingChance = 0.5f;

	/** Cooldown between punish attempts (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|InputReading")
	float PunishCooldown = 3.0f;

	// === ELDEN RING STYLE: ATTACK TRACKING ===
	/** Rotation speed during windup phase (degrees/sec). Higher = tracks player better */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|Tracking")
	float WindupRotationSpeed = 720.0f;

	/** Duration of windup phase - fast initial tracking to snap to target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|Tracking")
	float WindupDuration = 0.1f;

	/** Rotation speed during hold phase (degrees/sec). Still tracks but slower */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|Tracking")
	float HoldRotationSpeed = 540.0f;

	/** General combat rotation speed (interpolation speed for FaceTarget outside attacks) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|Tracking")
	float CombatRotationSpeed = 15.0f;

	/** Duration of hold phase before committing to attack (the "delayed swing") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|Tracking")
	float HoldDuration = 0.15f;

	/** If true, rotation locks completely when attack commits (player can dodge) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|Tracking")
	bool bLockRotationOnCommit = true;

	// === ELDEN RING STYLE: TURN IN PLACE ===
	/** Angle threshold (degrees) beyond which AI plays turn animation instead of rotating */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|TurnInPlace")
	float TurnInPlaceThreshold = 90.0f;

	/** If true, use turn-in-place animations for large rotations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|TurnInPlace")
	bool bUseTurnInPlace = true;

	// === ELDEN RING STYLE: MOVEMENT ===
	/** Maximum acceleration. Higher = instant speed changes (Souls-like feel) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|Movement")
	float MaxAcceleration = 4000.0f;

	/** Braking deceleration when stopping */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EldenRing|Movement")
	float BrakingDeceleration = 4000.0f;

	// === BOSS ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	bool bIsBoss = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float Phase2HealthThreshold = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float Phase3HealthThreshold = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float EnrageHealthThreshold = 0.15f;

	// === AGGRESSION (Elden Ring Style) ===
	/** Base aggression (0-1). Higher = attacks more frequently, less strafing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseAggression = 0.85f;  // Elden Ring style: attack frequently, strafe rarely

	/** Aggression increase per boss phase (additive) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression")
	float AggressionPerPhase = 0.15f;

	/** Chance to perform combo attack (chain multiple attacks) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ComboChance = 0.3f;

	/** Max attacks in a combo chain */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression")
	int32 MaxComboLength = 3;

	/** Chance to use gap-closer attack when far from target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GapCloserChance = 0.4f;

	// === VARIETY ===
	/** Chance to feint (start wind-up then cancel into strafe) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variety", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FeintChance = 0.02f;  // Rare feints - mostly just attack

	/** Chance to delay attack at random during wind-up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variety", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AttackDelayChance = 0.05f;  // Rarely delay - attack quickly

	/** Random delay range when delaying attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variety")
	float AttackDelayMin = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variety")
	float AttackDelayMax = 0.25f;
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

	/** Handler for CombatManager's OnPoiseBroken event */
	UFUNCTION()
	void HandlePoiseBroken(bool bBroken);

	UFUNCTION(BlueprintCallable, Category = "AI State")
	void OnDeath();

	/** Reset the state machine from Dead state (used for enemy respawn).
	 *  Unlike SetState(), this bypasses the Dead state check and properly reinitializes. */
	UFUNCTION(BlueprintCallable, Category = "AI State")
	void ResetFromDeath();

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

	// Strafe state
	bool bStrafeClockwise = true;
	float TimeSinceStrafeDirectionChange = 0.0f;
	FVector CachedStrafePosition = FVector::ZeroVector;
	float TimeSinceStrafePositionUpdate = 0.0f;
	static constexpr float StrafePositionUpdateInterval = 0.5f;  // Only update target every 0.5 seconds

	// Combo state
	int32 CurrentComboCount = 0;
	bool bInCombo = false;

	// Cached ability for WindingUp state (to prevent re-selection and cooldown issues)
	UPROPERTY()
	TWeakObjectPtr<UDataAsset> PendingAbility;

	// Elden Ring style: Attack tracking phase
	ESLFAttackTrackingPhase AttackTrackingPhase = ESLFAttackTrackingPhase::None;
	float TimeInTrackingPhase = 0.0f;

	// Elden Ring style: Input reading / Punish
	float LastPunishTime = -999.0f;
	bool bPlayerIsHealing = false;
	bool bPlayerIsRolling = false;
	float PlayerStateDetectedTime = 0.0f;

	// Target acquisition grace period (after player respawn)
	// Skips FOV check and uses extended detection radius for a few seconds
	bool bInTargetAcquisitionGracePeriod = false;
	float TargetAcquisitionGraceStartTime = 0.0f;
	static constexpr float TargetAcquisitionGraceDuration = 3.0f;  // 3 seconds
	static constexpr float TargetAcquisitionGraceRadiusMultiplier = 3.0f;  // 3x detection radius

	// Movement state
	bool bIsSprinting = false;

	// Aggression modifiers
	float CurrentAggression = 0.5f;

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

	// Movement helpers
	void UpdateMovementSpeed();
	void SetMovementSpeed(float NewSpeed);
	FVector CalculateBestStrafePosition();
	void UpdateStrafeDirection(float DeltaTime);

	// Health/stat helpers
	float GetHealthPercent() const;

	// Aggression helpers
	void UpdateAggression();
	bool ShouldStartCombo() const;
	bool ShouldUseGapCloser() const;
	bool TrySelectGapCloser(UDataAsset*& OutAbility);
	bool ShouldFeint() const;
	bool ShouldDelayAttack() const;
	float GetRandomAttackDelay() const;

	// Input reading (Elden Ring style)
	bool ShouldReactToPlayerInput() const;
	bool IsPlayerHealing() const;
	bool IsPlayerRolling() const;
	void UpdatePlayerStateDetection();
	bool CanPunishPlayer() const;
	bool TryPunishPlayer(UDataAsset*& OutPunishAbility);

	// Elden Ring style: Attack tracking
	void SetAttackTrackingPhase(ESLFAttackTrackingPhase Phase);
	void UpdateAttackTracking(float DeltaTime);
	float GetCurrentRotationSpeed() const;

	// Elden Ring style: Turn in place
	bool ShouldTurnInPlace() const;
	float GetAngleToTarget() const;
	void PlayTurnAnimation(float AngleToTarget);

	// Elden Ring style: Movement
	void ApplyEldenRingMovementSettings();

	// Boss helpers
	void CheckBossPhaseTransition();
	void OnBossPhaseTransition(ESLFBossPhase NewPhase);

	// Patrol helpers
	FVector GetNextPatrolPoint();
	FVector GetRandomRoamPoint();

	// Bounds checking
	bool IsOutOfBounds() const;
	void ReturnToSpawn();

	// Target detection helpers
	AActor* FindNearestPlayer() const;
	bool CanSeeActor(AActor* Actor) const;
};
