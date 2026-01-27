// SLFAIStateMachineComponent.cpp
// Custom C++ State Machine for Soulslike AI

#include "Components/SLFAIStateMachineComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/AIBossComponent.h"
#include "Components/AC_CombatManager.h"
#include "Components/AC_ActionManager.h"
#include "Components/StatManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SLFPrimaryDataAssets.h"
#include "Blueprints/SLFStatBase.h"

// ═══════════════════════════════════════════════════════════════════════════════
// CONSTRUCTOR & LIFECYCLE
// ═══════════════════════════════════════════════════════════════════════════════

USLFAIStateMachineComponent::USLFAIStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0f; // Every frame for smooth combat
}

void USLFAIStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheReferences();

	// Store spawn location for out-of-bounds checking
	if (CachedPawn.IsValid())
	{
		SpawnLocation = CachedPawn->GetActorLocation();
	}

	// Initialize boss phase if configured as boss
	if (Config.bIsBoss)
	{
		BossPhase = ESLFBossPhase::Phase1;
	}

	// Apply Elden Ring style movement settings (instant acceleration/deceleration)
	ApplyEldenRingMovementSettings();

	UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] BeginPlay on %s - IsBoss: %s, InputReading: %s"),
		CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
		Config.bIsBoss ? TEXT("Yes") : TEXT("No"),
		Config.bEnableInputReading ? TEXT("ON") : TEXT("OFF"));
}

void USLFAIStateMachineComponent::CacheReferences()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	CachedPawn = Cast<APawn>(Owner);
	if (CachedPawn.IsValid())
	{
		CachedAIController = Cast<AAIController>(CachedPawn->GetController());

		// Cache combat manager and bind to OnPoiseBroken event
		CachedCombatManager = CachedPawn->FindComponentByClass<UAICombatManagerComponent>();
		if (CachedCombatManager.IsValid())
		{
			// Bind to OnPoiseBroken event - triggers PoiseBroken state when poise breaks
			CachedCombatManager->OnPoiseBroken.AddDynamic(this, &USLFAIStateMachineComponent::HandlePoiseBroken);
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] Bound to OnPoiseBroken event"));
		}

		// Cache anim instance
		if (ACharacter* Character = Cast<ACharacter>(CachedPawn.Get()))
		{
			if (USkeletalMeshComponent* Mesh = Character->GetMesh())
			{
				CachedAnimInstance = Mesh->GetAnimInstance();
			}
		}
	}

	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] CacheReferences - Pawn: %s, Controller: %s, CombatMgr: %s"),
			CachedPawn.IsValid() ? TEXT("Valid") : TEXT("NULL"),
			CachedAIController.IsValid() ? TEXT("Valid") : TEXT("NULL"),
			CachedCombatManager.IsValid() ? TEXT("Valid") : TEXT("NULL"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// TICK - MAIN STATE MACHINE LOOP
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// CRITICAL: Check if enemy is dead via AICombatManager
	// If dead, enter Dead state and stop processing
	if (CurrentState != ESLFAIState::Dead)
	{
		AActor* Owner = GetOwner();
		if (Owner)
		{
			UAICombatManagerComponent* CombatMgr = Owner->FindComponentByClass<UAICombatManagerComponent>();
			if (CombatMgr && CombatMgr->bIsDead)
			{
				UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - AICombatManager reports DEAD, entering Dead state"),
					*Owner->GetName());
				SetState(ESLFAIState::Dead);
				return;
			}
		}
	}

	// Already dead - do nothing
	if (CurrentState == ESLFAIState::Dead)
	{
		return;
	}

	// Update time tracking
	TimeInCurrentState += DeltaTime;
	TimeInCombatSubState += DeltaTime;

	// Check grace period expiration (for target acquisition after player respawn)
	if (bInTargetAcquisitionGracePeriod)
	{
		float ElapsedGrace = GetWorld() ? (GetWorld()->GetTimeSeconds() - TargetAcquisitionGraceStartTime) : 0.0f;
		if (ElapsedGrace >= TargetAcquisitionGraceDuration)
		{
			bInTargetAcquisitionGracePeriod = false;
			if (bDebugEnabled)
			{
				UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Target acquisition grace period EXPIRED after %.1f seconds"),
					CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
					ElapsedGrace);
			}
		}
	}

	// Boss phase checking
	if (Config.bIsBoss)
	{
		CheckBossPhaseTransition();
	}

	// Main state tick
	switch (CurrentState)
	{
	case ESLFAIState::Idle:
		TickIdle(DeltaTime);
		break;
	case ESLFAIState::Patrol:
		TickPatrol(DeltaTime);
		break;
	case ESLFAIState::RandomRoam:
		TickRandomRoam(DeltaTime);
		break;
	case ESLFAIState::Investigating:
		TickInvestigating(DeltaTime);
		break;
	case ESLFAIState::Combat:
		TickCombat(DeltaTime);
		break;
	case ESLFAIState::PoiseBroken:
		TickPoiseBroken(DeltaTime);
		break;
	case ESLFAIState::Uninterruptable:
		TickUninterruptable(DeltaTime);
		break;
	case ESLFAIState::OutOfBounds:
		TickOutOfBounds(DeltaTime);
		break;
	case ESLFAIState::Dead:
		// Do nothing
		break;
	}

	// Debug logging
	if (bDebugEnabled && FMath::Fmod(TimeInCurrentState, 2.0f) < DeltaTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - State: %d, SubState: %d, Target: %s, Dist: %.1f"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
			(int32)CurrentState,
			(int32)CombatSubState,
			HasValidTarget() ? *CurrentTarget->GetName() : TEXT("None"),
			GetDistanceToTarget());
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATE CONTROL
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::SetState(ESLFAIState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	if (CurrentState == ESLFAIState::Dead)
	{
		// Cannot leave dead state
		return;
	}

	ESLFAIState OldState = CurrentState;

	// Exit old state
	OnExitState(CurrentState);

	// Update state
	PreviousState = CurrentState;
	CurrentState = NewState;
	TimeInCurrentState = 0.0f;

	// Enter new state
	OnEnterState(NewState);

	// Broadcast
	OnStateChanged.Broadcast(OldState, NewState);

	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - State changed: %d -> %d"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
			(int32)OldState, (int32)NewState);
	}
}

void USLFAIStateMachineComponent::SetCombatSubState(ESLFCombatSubState NewSubState)
{
	if (CombatSubState == NewSubState)
	{
		return;
	}

	ESLFCombatSubState OldSubState = CombatSubState;

	// Exit old sub-state
	OnExitCombatSubState(CombatSubState);

	// Update
	CombatSubState = NewSubState;
	TimeInCombatSubState = 0.0f;

	// Enter new sub-state
	OnEnterCombatSubState(NewSubState);

	// Broadcast
	OnCombatSubStateChanged.Broadcast(OldSubState, NewSubState);

	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Combat sub-state: %d -> %d"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
			(int32)OldSubState, (int32)NewSubState);
	}
}

void USLFAIStateMachineComponent::SetTarget(AActor* NewTarget)
{
	CurrentTarget = NewTarget;

	if (NewTarget && CurrentState == ESLFAIState::Idle)
	{
		SetState(ESLFAIState::Combat);
	}
}

void USLFAIStateMachineComponent::ClearTarget()
{
	CurrentTarget = nullptr;

	if (CurrentState == ESLFAIState::Combat)
	{
		SetState(ESLFAIState::Investigating);
	}
}

void USLFAIStateMachineComponent::TriggerPoiseBroken()
{
	if (CurrentState != ESLFAIState::Dead)
	{
		SetState(ESLFAIState::PoiseBroken);
	}
}

void USLFAIStateMachineComponent::HandlePoiseBroken(bool bBroken)
{
	if (bBroken)
	{
		// Poise broke - enter staggered state
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - POISE BROKEN! Entering stagger state."),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"));

		// Reset attack tracking (we're staggered, not attacking)
		SetAttackTrackingPhase(ESLFAttackTrackingPhase::None);

		TriggerPoiseBroken();
	}
	else
	{
		// Poise recovered - return to combat if we have a target
		UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Poise recovered, returning to combat"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"));

		if (CurrentState == ESLFAIState::PoiseBroken)
		{
			if (HasValidTarget())
			{
				SetState(ESLFAIState::Combat);
			}
			else
			{
				SetState(ESLFAIState::Idle);
			}
		}
	}
}

void USLFAIStateMachineComponent::OnDeath()
{
	SetState(ESLFAIState::Dead);
	StopMovement();
}

void USLFAIStateMachineComponent::ResetFromDeath()
{
	// CRITICAL: This function bypasses the Dead state check in SetState()
	// It's used by the respawn system to reset enemies after player dies

	UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - ResetFromDeath called, CurrentState was: %d"),
		CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
		(int32)CurrentState);

	// Force state out of Dead (bypassing the check in SetState)
	CurrentState = ESLFAIState::Idle;
	PreviousState = ESLFAIState::Idle;
	TimeInCurrentState = 0.0f;
	TimeInCombatSubState = 0.0f;
	CombatSubState = ESLFCombatSubState::None;

	// Clear target
	CurrentTarget = nullptr;
	LastKnownTargetLocation = FVector::ZeroVector;

	// Reset attack state
	bIsAttacking = false;
	bWindUpHeld = false;
	LastAttackTime = 0.0f;
	NextAttackDelay = 0.0f;
	bInCombo = false;
	CurrentComboCount = 0;
	PendingAbility = nullptr;

	// Reset tracking phases
	AttackTrackingPhase = ESLFAttackTrackingPhase::None;
	TimeInTrackingPhase = 0.0f;

	// Reset strafe state
	bStrafeClockwise = FMath::RandBool();
	TimeSinceStrafeDirectionChange = 0.0f;

	// Re-cache references (controller may have changed)
	CacheReferences();

	// Broadcast state change
	OnStateChanged.Broadcast(ESLFAIState::Dead, ESLFAIState::Idle);

	// CRITICAL: Start grace period for target acquisition (skips FOV check)
	// This ensures enemies can detect the player immediately after respawn
	// even if player spawns behind them
	bInTargetAcquisitionGracePeriod = true;
	TargetAcquisitionGraceStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	// CRITICAL: Immediately try to acquire the player as target
	// Don't wait for TickIdle - find player NOW
	AActor* Player = FindNearestPlayer();
	if (Player && CachedPawn.IsValid())
	{
		float Distance = FVector::Dist(CachedPawn->GetActorLocation(), Player->GetActorLocation());
		// Use extended detection radius during grace period
		float GraceDetectionRadius = Config.DetectionRadius * TargetAcquisitionGraceRadiusMultiplier;

		if (Distance <= GraceDetectionRadius)
		{
			// Skip FOV check during grace period - immediately acquire target
			SetTarget(Player);
			UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - ResetFromDeath: IMMEDIATELY acquired player at distance %.1f (grace period active)"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				Distance);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - ResetFromDeath: Player too far (%.1f > %.1f grace radius)"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				Distance, GraceDetectionRadius);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - ResetFromDeath COMPLETE, now in %s state, Target: %s"),
		CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
		CurrentState == ESLFAIState::Combat ? TEXT("Combat") : TEXT("Idle"),
		HasValidTarget() ? *CurrentTarget->GetName() : TEXT("None"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATE ENTER/EXIT
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::OnEnterState(ESLFAIState State)
{
	switch (State)
	{
	case ESLFAIState::Combat:
		SetCombatSubState(ESLFCombatSubState::Engaging);
		NextAttackDelay = CalculateNextAttackDelay();
		// Initialize aggression for combat
		UpdateAggression();
		// Reset combo state
		bInCombo = false;
		CurrentComboCount = 0;
		// Reset strafe state
		bStrafeClockwise = FMath::RandBool();
		TimeSinceStrafeDirectionChange = 0.0f;

		// BOSS: Trigger boss healthbar when entering combat
		if (Config.bIsBoss && CachedPawn.IsValid())
		{
			if (UAIBossComponent* BossComp = CachedPawn->FindComponentByClass<UAIBossComponent>())
			{
				UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] Boss entering combat - calling SetFightActive(true)"));
				BossComp->SetFightActive(true);
			}
		}
		break;

	case ESLFAIState::Investigating:
		if (HasValidTarget())
		{
			LastKnownTargetLocation = CurrentTarget->GetActorLocation();
		}
		break;

	case ESLFAIState::PoiseBroken:
		StopMovement();
		bIsAttacking = false;
		// Break any combo
		bInCombo = false;
		CurrentComboCount = 0;
		break;

	case ESLFAIState::Dead:
		StopMovement();
		SetCombatSubState(ESLFCombatSubState::None);

		// BOSS: Hide boss healthbar when boss dies
		if (Config.bIsBoss && CachedPawn.IsValid())
		{
			if (UAIBossComponent* BossComp = CachedPawn->FindComponentByClass<UAIBossComponent>())
			{
				UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] Boss died - calling SetFightActive(false)"));
				BossComp->SetFightActive(false);
			}
		}
		break;

	default:
		break;
	}
}

void USLFAIStateMachineComponent::OnExitState(ESLFAIState State)
{
	switch (State)
	{
	case ESLFAIState::Combat:
		SetCombatSubState(ESLFCombatSubState::None);
		bIsAttacking = false;
		break;

	default:
		break;
	}
}

void USLFAIStateMachineComponent::OnEnterCombatSubState(ESLFCombatSubState SubState)
{
	switch (SubState)
	{
	case ESLFCombatSubState::WindingUp:
		bWindUpHeld = false;
		FaceTarget();
		break;

	case ESLFCombatSubState::Attacking:
		bIsAttacking = true;
		OnAttackStarted.Broadcast();
		break;

	case ESLFCombatSubState::Recovering:
		LastAttackTime = GetWorld()->GetTimeSeconds();
		NextAttackDelay = CalculateNextAttackDelay();
		break;

	case ESLFCombatSubState::Positioning:
		LastRepositionTime = GetWorld()->GetTimeSeconds();
		// Reset cached strafe position so it recalculates fresh on entry
		CachedStrafePosition = FVector::ZeroVector;
		TimeSinceStrafePositionUpdate = StrafePositionUpdateInterval;  // Force immediate calculation
		break;

	default:
		break;
	}
}

void USLFAIStateMachineComponent::OnExitCombatSubState(ESLFCombatSubState SubState)
{
	switch (SubState)
	{
	case ESLFCombatSubState::Attacking:
		bIsAttacking = false;
		OnAttackEnded.Broadcast();
		break;

	default:
		break;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATE TICK IMPLEMENTATIONS
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::TickIdle(float DeltaTime)
{
	// DIAGNOSTIC: Log every tick in idle
	static float IdleLogTimer = 0.0f;
	IdleLogTimer += DeltaTime;
	bool bShouldLogThisTick = (IdleLogTimer >= 1.0f); // Log once per second
	if (bShouldLogThisTick) IdleLogTimer = 0.0f;

	// Check for targets if we don't have one
	// This handles cases where perception isn't set up or doesn't trigger
	if (!HasValidTarget())
	{
		// Search for player in detection radius
		AActor* FoundTarget = FindNearestPlayer();

		if (bShouldLogThisTick)
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - TickIdle: CachedPawn=%s, FoundTarget=%s, DetectionRadius=%.1f"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("INVALID"),
				CachedPawn.IsValid() ? TEXT("Valid") : TEXT("INVALID"),
				FoundTarget ? *FoundTarget->GetName() : TEXT("NULL"),
				Config.DetectionRadius);
		}

		if (FoundTarget)
		{
			if (!CachedPawn.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("[AIStateMachine] TickIdle - CachedPawn is INVALID, cannot calculate distance!"));
				return;
			}

			float DistToTarget = FVector::Dist(CachedPawn->GetActorLocation(), FoundTarget->GetActorLocation());

			// Use extended detection radius during grace period (after player respawn)
			float EffectiveRadius = Config.DetectionRadius;
			if (bInTargetAcquisitionGracePeriod)
			{
				EffectiveRadius = Config.DetectionRadius * TargetAcquisitionGraceRadiusMultiplier;
			}

			if (bShouldLogThisTick)
			{
				UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - TickIdle: Dist=%.1f, EffectiveRadius=%.1f, InRange=%s"),
					*CachedPawn->GetName(),
					DistToTarget,
					EffectiveRadius,
					(DistToTarget <= EffectiveRadius) ? TEXT("YES") : TEXT("NO"));
			}

			if (DistToTarget <= EffectiveRadius)
			{
				bool bCanSee = CanSeeActor(FoundTarget);
				if (bShouldLogThisTick)
				{
					UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - TickIdle: CanSeeActor=%s"),
						*CachedPawn->GetName(),
						bCanSee ? TEXT("YES") : TEXT("NO"));
				}

				// Check line of sight (FOV check is skipped during grace period in CanSeeActor)
				if (bCanSee)
				{
					UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - Detected player at distance %.1f (grace=%s, radius=%.1f) - ENTERING COMBAT"),
						*CachedPawn->GetName(),
						DistToTarget,
						bInTargetAcquisitionGracePeriod ? TEXT("YES") : TEXT("NO"),
						EffectiveRadius);
					SetTarget(FoundTarget);
					return;
				}
			}
		}
	}
	else
	{
		// Have target, enter combat
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - TickIdle: Already have target, entering Combat"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"));
		SetState(ESLFAIState::Combat);
	}
}

void USLFAIStateMachineComponent::TickPatrol(float DeltaTime)
{
	// Move to next patrol point
	// Check for targets

	if (HasValidTarget())
	{
		SetState(ESLFAIState::Combat);
		return;
	}

	// Patrol logic would go here
	// For now, switch to idle if no patrol points
	SetState(ESLFAIState::Idle);
}

void USLFAIStateMachineComponent::TickRandomRoam(float DeltaTime)
{
	if (HasValidTarget())
	{
		SetState(ESLFAIState::Combat);
		return;
	}

	// Random roam logic
	if (TimeInCurrentState > 5.0f)
	{
		FVector RoamPoint = GetRandomRoamPoint();
		MoveToLocation(RoamPoint);
		TimeInCurrentState = 0.0f;
	}
}

void USLFAIStateMachineComponent::TickInvestigating(float DeltaTime)
{
	// Move to last known target location
	if (!LastKnownTargetLocation.IsZero())
	{
		MoveToLocation(LastKnownTargetLocation);

		float DistToLastKnown = FVector::Dist(CachedPawn->GetActorLocation(), LastKnownTargetLocation);
		if (DistToLastKnown < 100.0f)
		{
			// Reached last known location, didn't find target
			SetState(ESLFAIState::Idle);
		}
	}

	// If we see target again, go back to combat
	if (HasValidTarget() && CanSeeTarget())
	{
		SetState(ESLFAIState::Combat);
	}

	// Timeout
	if (TimeInCurrentState > Config.LoseTargetTime)
	{
		ClearTarget();
		SetState(ESLFAIState::Idle);
	}
}

void USLFAIStateMachineComponent::TickCombat(float DeltaTime)
{
	// Validate target
	if (!HasValidTarget())
	{
		SetState(ESLFAIState::Investigating);
		return;
	}

	// Check out of bounds
	if (IsOutOfBounds())
	{
		SetState(ESLFAIState::OutOfBounds);
		return;
	}

	// ELDEN RING STYLE: Continuously detect player state (healing/rolling)
	UpdatePlayerStateDetection();

	// ELDEN RING STYLE: Update attack tracking rotation during attack
	if (CombatSubState == ESLFCombatSubState::Attacking)
	{
		UpdateAttackTracking(DeltaTime);
	}

	// Combat sub-state tick
	switch (CombatSubState)
	{
	case ESLFCombatSubState::Engaging:
		TickCombat_Engaging(DeltaTime);
		break;
	case ESLFCombatSubState::Positioning:
		TickCombat_Positioning(DeltaTime);
		break;
	case ESLFCombatSubState::WindingUp:
		TickCombat_WindingUp(DeltaTime);
		break;
	case ESLFCombatSubState::Attacking:
		TickCombat_Attacking(DeltaTime);
		break;
	case ESLFCombatSubState::Recovering:
		TickCombat_Recovering(DeltaTime);
		break;
	case ESLFCombatSubState::Retreating:
		TickCombat_Retreating(DeltaTime);
		break;
	default:
		SetCombatSubState(ESLFCombatSubState::Engaging);
		break;
	}
}

void USLFAIStateMachineComponent::TickPoiseBroken(float DeltaTime)
{
	// Wait for stagger animation to finish
	// This would be driven by animation notify in practice
	if (TimeInCurrentState > 2.0f) // Default stagger time
	{
		if (HasValidTarget())
		{
			SetState(ESLFAIState::Combat);
		}
		else
		{
			SetState(ESLFAIState::Idle);
		}
	}
}

void USLFAIStateMachineComponent::TickUninterruptable(float DeltaTime)
{
	// Special move in progress - wait for it to complete
	// Driven by animation/ability system
}

void USLFAIStateMachineComponent::TickOutOfBounds(float DeltaTime)
{
	ReturnToSpawn();

	// Check if we're back in bounds
	float DistToSpawn = FVector::Dist(CachedPawn->GetActorLocation(), SpawnLocation);
	if (DistToSpawn < 200.0f)
	{
		SetState(ESLFAIState::Idle);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMBAT SUB-STATE IMPLEMENTATIONS
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::TickCombat_Engaging(float DeltaTime)
{
	float Distance = GetDistanceToTarget();
	float TimeSinceLastAttackDebug = GetWorld()->GetTimeSeconds() - LastAttackTime;

	// DIAGNOSTIC: Log combat state every 0.5 seconds to debug freeze issue
	static float DebugLogTimer = 0.0f;
	DebugLogTimer += DeltaTime;
	if (DebugLogTimer >= 0.5f)
	{
		DebugLogTimer = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] COMBAT TICK: %s | Dist=%.0f | Range=%.0f | InRange=%s | TimeSinceAttack=%.1f | Delay=%.1f | Aggro=%.2f"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
			Distance,
			Config.AttackRange,
			Distance <= Config.AttackRange ? TEXT("YES") : TEXT("NO"),
			TimeSinceLastAttackDebug,
			NextAttackDelay,
			CurrentAggression);
	}

	// ELDEN RING STYLE: Punish player if they're healing/rolling
	// This takes priority over normal attack decisions
	if (Config.bEnableInputReading && (bPlayerIsHealing || bPlayerIsRolling))
	{
		UDataAsset* PunishAbility = nullptr;
		if (TryPunishPlayer(PunishAbility))
		{
			// Found a punish ability - use it immediately!
			StartAttack(PunishAbility);
			return;
		}
	}

	// ELDEN RING STYLE: Turn-in-place for large angle changes
	if (ShouldTurnInPlace())
	{
		float AngleToTarget = GetAngleToTarget();
		PlayTurnAnimation(AngleToTarget);
		return;
	}

	// Update movement speed based on distance (sprint when far, walk when close)
	UpdateMovementSpeed();

	// CRITICAL: Attack range with tolerance to prevent edge-of-range freezing
	// Add 10% tolerance buffer - if within 110% of attack range, consider in range
	const float AttackRangeWithTolerance = Config.AttackRange * 1.1f;
	const bool bInAttackRange = (Distance <= AttackRangeWithTolerance);

	if (!bInAttackRange)
	{
		// Too far to attack - check if we should use a gap-closer
		const float GapCloserMinDistance = 500.0f; // Use gap-closer if more than 500 units away

		if (Distance > GapCloserMinDistance && ShouldUseGapCloser())
		{
			UDataAsset* GapCloserAbility = nullptr;
			if (TrySelectGapCloser(GapCloserAbility))
			{
				UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Using gap-closer at distance %.1f"),
					CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
					Distance);
				StartAttack(GapCloserAbility);
				return;
			}
		}

		// No gap-closer available or not using one - move closer normally
		MoveToTarget();
		FaceTarget();

		if (bDebugEnabled && FMath::Fmod(TimeInCombatSubState, 1.0f) < DeltaTime)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Moving to target (Distance: %.1f, AttackRange: %.1f, Tolerance: %.1f)"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				Distance, Config.AttackRange, AttackRangeWithTolerance);
		}
		return;
	}

	// In attack range - decide what to do
	StopMovement();
	FaceTarget();

	float TimeSinceLastAttack = GetWorld()->GetTimeSeconds() - LastAttackTime;

	// Continue combo if in one
	if (bInCombo && CurrentComboCount < Config.MaxComboLength)
	{
		// Short delay between combo attacks
		if (TimeSinceLastAttack >= 0.15f)
		{
			UDataAsset* SelectedAbility = nullptr;
			if (TrySelectAbility(SelectedAbility))
			{
				StartAttack(SelectedAbility);
				CurrentComboCount++;
				return;
			}
		}
		return;
	}

	// Reset combo state
	bInCombo = false;
	CurrentComboCount = 0;

	if (TimeSinceLastAttack >= NextAttackDelay)
	{
		// Aggression check - higher aggression means more likely to attack vs strafe
		float AggressionRoll = FMath::FRand();
		bool bShouldAttack = AggressionRoll < CurrentAggression;

		// Always log aggression checks when in range
		UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Aggression check: Roll=%.2f, Aggression=%.2f, Attack=%s, Dist=%.1f"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
			AggressionRoll, CurrentAggression,
			bShouldAttack ? TEXT("YES") : TEXT("NO-STRAFE"),
			Distance);

		if (!bShouldAttack)
		{
			// Not aggressive enough, strafe instead
			SetCombatSubState(ESLFCombatSubState::Positioning);
			return;
		}

		// Ready to attack
		UDataAsset* SelectedAbility = nullptr;
		if (TrySelectAbility(SelectedAbility))
		{
			// Check if should feint
			if (ShouldFeint())
			{
				// Fake wind-up then strafe
				SetCombatSubState(ESLFCombatSubState::WindingUp);
				// Will cancel to positioning after partial wind-up
				return;
			}

			if (Config.WindUpHoldTime > 0.0f || ShouldDelayAttack())
			{
				// Use wind-up phase for input reading or random delay
				// Store the selected ability so we don't have to re-select after wind-up
				PendingAbility = SelectedAbility;
				SetCombatSubState(ESLFCombatSubState::WindingUp);
			}
			else
			{
				// Attack immediately
				StartAttack(SelectedAbility);

				// Check if starting a combo
				if (ShouldStartCombo())
				{
					bInCombo = true;
					CurrentComboCount = 1;
				}
			}
		}
		else
		{
			// No ability available, reposition
			UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - NO ABILITY AVAILABLE! Going to Positioning"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"));
			SetCombatSubState(ESLFCombatSubState::Positioning);
		}
	}
	else
	{
		// Waiting for attack cooldown - strafe to be unpredictable
		// Higher aggression = less strafing while waiting
		float StrafeChance = (1.0f - CurrentAggression) * 0.3f;
		if (FMath::FRand() < StrafeChance)
		{
			SetCombatSubState(ESLFCombatSubState::Positioning);
		}
	}
}

void USLFAIStateMachineComponent::TickCombat_Positioning(float DeltaTime)
{
	// ELDEN RING STYLE: Punish player if they're healing/rolling during strafe
	// This is the classic "try to heal and get punished" mechanic
	if (Config.bEnableInputReading && (bPlayerIsHealing || bPlayerIsRolling))
	{
		UDataAsset* PunishAbility = nullptr;
		if (TryPunishPlayer(PunishAbility))
		{
			// Player caught healing/rolling during our strafe - punish immediately!
			if (bDebugEnabled)
			{
				UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - PUNISHING from strafe! Healing=%s, Rolling=%s"),
					CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
					bPlayerIsHealing ? TEXT("YES") : TEXT("NO"),
					bPlayerIsRolling ? TEXT("YES") : TEXT("NO"));
			}
			StartAttack(PunishAbility);
			return;
		}
	}

	float Distance = GetDistanceToTarget();

	// ELDEN RING STYLE: Strafe at close range, only retreat if literally on top of player
	// Enemies should be comfortable fighting at melee distance
	if (Distance < 80.0f)  // Only retreat if very close (inside personal space)
	{
		// Too close, back up a bit
		SetCombatSubState(ESLFCombatSubState::Retreating);
		return;
	}

	if (Distance > Config.AttackRange * 2.0f)
	{
		// Too far to strafe effectively, close in to attack range
		SetCombatSubState(ESLFCombatSubState::Engaging);
		return;
	}

	// Update strafe direction periodically
	UpdateStrafeDirection(DeltaTime);

	// SMOOTH STRAFE: Only recalculate strafe position periodically, not every tick
	// This prevents jerky movement from constantly changing targets
	TimeSinceStrafePositionUpdate += DeltaTime;
	if (TimeSinceStrafePositionUpdate >= StrafePositionUpdateInterval || CachedStrafePosition.IsNearlyZero())
	{
		CachedStrafePosition = CalculateBestStrafePosition();
		TimeSinceStrafePositionUpdate = 0.0f;
	}

	// Move towards the cached strafe position
	MoveToLocation(CachedStrafePosition);
	FaceTarget();

	// Set movement speed for strafing (slower, more tactical)
	SetMovementSpeed(Config.StrafeSpeed);

	// After some time, decide next action based on aggression
	if (TimeInCombatSubState > Config.RepositionInterval)
	{
		// Higher aggression = more likely to attack, lower = keep circling
		if (FMath::FRand() < CurrentAggression)
		{
			SetCombatSubState(ESLFCombatSubState::Engaging);
		}
		else
		{
			// Reset timer and keep circling
			TimeInCombatSubState = 0.0f;

			// Chance to switch directions for unpredictability
			if (FMath::FRand() < 0.3f)
			{
				bStrafeClockwise = !bStrafeClockwise;
			}
		}
	}
}

void USLFAIStateMachineComponent::TickCombat_WindingUp(float DeltaTime)
{
	FaceTarget();

	// Hold wind-up for input reading
	if (TimeInCombatSubState >= Config.WindUpHoldTime)
	{
		// Check if player is doing something punishable
		if (Config.bEnableInputReading)
		{
			if (IsPlayerHealing() || IsPlayerRolling())
			{
				// Delay attack slightly to catch them
				if (!bWindUpHeld)
				{
					bWindUpHeld = true;
					// Reset timer to hold a bit longer
					TimeInCombatSubState = Config.WindUpHoldTime * 0.5f;
					return;
				}
			}
		}

		// Execute attack using the cached ability from when we entered WindingUp
		// This prevents the ability from being re-selected (and potentially rejected due to cooldown)
		if (PendingAbility.IsValid())
		{
			UDataAsset* AbilityToUse = PendingAbility.Get();
			if (bDebugEnabled)
			{
				UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - WindingUp complete, using cached ability: %s"),
					CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
					*AbilityToUse->GetName());
			}
			PendingAbility.Reset();  // Clear the cached ability
			StartAttack(AbilityToUse);
		}
		else
		{
			// Fallback: try to select a new ability if no cached ability
			UDataAsset* SelectedAbility = nullptr;
			if (TrySelectAbility(SelectedAbility))
			{
				StartAttack(SelectedAbility);
			}
			else
			{
				SetCombatSubState(ESLFCombatSubState::Recovering);
			}
		}
	}
}

void USLFAIStateMachineComponent::TickCombat_Attacking(float DeltaTime)
{
	// Attack animation is playing
	// State transition happens when montage ends (via OnAttackMontageEnded)

	// ELDEN RING STYLE: Rotation is controlled by the attack tracking phase system
	// Windup phase: Fast rotation (720 deg/s default) - catches initial position
	// Hold phase: Moderate rotation (360 deg/s default) - tracks player during delay
	// Commit phase: Locked rotation (by default) - attack is committed, no more tracking

	// UpdateAttackTracking is called in TickCombat above to handle rotation
	// The tracking phase transitions from Windup -> Hold -> Commit based on time
}

void USLFAIStateMachineComponent::TickCombat_Recovering(float DeltaTime)
{
	// Post-attack recovery
	if (TimeInCombatSubState >= Config.PostAttackRecovery)
	{
		// Decide next action
		float Distance = GetDistanceToTarget();

		if (Distance > Config.AttackRange * 1.5f)
		{
			// Too far, close the distance
			SetCombatSubState(ESLFCombatSubState::Engaging);
		}
		else
		{
			// In range - decide between strafe and attack based on inverse aggression
			// Higher aggression = more attacks, lower = more strafing
			// Strafe chance = 1 - CurrentAggression (so 0.5 aggression = 50% strafe)
			float StrafeChance = 1.0f - CurrentAggression;
			if (FMath::FRand() < StrafeChance)
			{
				SetCombatSubState(ESLFCombatSubState::Positioning);
			}
			else
			{
				SetCombatSubState(ESLFCombatSubState::Engaging);
			}
		}
	}
}

void USLFAIStateMachineComponent::TickCombat_Retreating(float DeltaTime)
{
	// Back away from target
	FVector AwayDir = -GetDirectionToTarget();
	FVector RetreatTarget = CachedPawn->GetActorLocation() + AwayDir * 200.0f;
	MoveToLocation(RetreatTarget);
	FaceTarget();

	float Distance = GetDistanceToTarget();
	if (Distance >= Config.PreferredCombatDistance || TimeInCombatSubState > 2.0f)
	{
		SetCombatSubState(ESLFCombatSubState::Engaging);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

bool USLFAIStateMachineComponent::HasValidTarget() const
{
	return CurrentTarget.IsValid() && !CurrentTarget->IsActorBeingDestroyed();
}

float USLFAIStateMachineComponent::GetDistanceToTarget() const
{
	if (!HasValidTarget() || !CachedPawn.IsValid())
	{
		return MAX_FLT;
	}
	return FVector::Dist(CachedPawn->GetActorLocation(), CurrentTarget->GetActorLocation());
}

FVector USLFAIStateMachineComponent::GetDirectionToTarget() const
{
	if (!HasValidTarget() || !CachedPawn.IsValid())
	{
		return FVector::ForwardVector;
	}
	return (CurrentTarget->GetActorLocation() - CachedPawn->GetActorLocation()).GetSafeNormal();
}

bool USLFAIStateMachineComponent::IsTargetInAttackRange() const
{
	return GetDistanceToTarget() <= Config.AttackRange;
}

bool USLFAIStateMachineComponent::CanSeeTarget() const
{
	// Simplified line-of-sight check
	if (!HasValidTarget() || !CachedPawn.IsValid())
	{
		return false;
	}

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CachedPawn.Get());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		CachedPawn->GetActorLocation() + FVector(0, 0, 50),
		CurrentTarget->GetActorLocation() + FVector(0, 0, 50),
		ECC_Visibility,
		Params
	);

	return !bHit || Hit.GetActor() == CurrentTarget.Get();
}

void USLFAIStateMachineComponent::MoveToTarget()
{
	if (!HasValidTarget())
	{
		return;
	}
	MoveToLocation(CurrentTarget->GetActorLocation());
}

void USLFAIStateMachineComponent::MoveToLocation(const FVector& Location)
{
	if (!CachedAIController.IsValid())
	{
		return;
	}

	// Use a smaller acceptance radius to ensure AI gets close enough to attack
	// 50 units should be close enough for melee without colliding with player
	const float AcceptanceRadius = 50.0f;
	CachedAIController->MoveToLocation(Location, AcceptanceRadius);
}

void USLFAIStateMachineComponent::StopMovement()
{
	if (CachedAIController.IsValid())
	{
		CachedAIController->StopMovement();
	}
}

void USLFAIStateMachineComponent::FaceTarget()
{
	if (!HasValidTarget() || !CachedPawn.IsValid())
	{
		return;
	}

	FVector Direction = GetDirectionToTarget();
	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Pitch = 0.0f;

	// Use faster rotation speed for bosses (Elden Ring style: bosses track player aggressively)
	// Regular enemies: 15.0f, Bosses: 40.0f (very aggressive tracking to prevent easy dodging)
	float RotSpeed = Config.bIsBoss ? 40.0f : Config.CombatRotationSpeed;

	CachedPawn->SetActorRotation(FMath::RInterpTo(
		CachedPawn->GetActorRotation(),
		TargetRotation,
		GetWorld()->GetDeltaSeconds(),
		RotSpeed
	));
}

void USLFAIStateMachineComponent::StrafeAroundTarget(float DeltaTime, bool bClockwise)
{
	if (!HasValidTarget() || !CachedPawn.IsValid())
	{
		return;
	}

	FVector ToTarget = CurrentTarget->GetActorLocation() - CachedPawn->GetActorLocation();
	ToTarget.Z = 0.0f;

	FVector StrafeDir = bClockwise ?
		FVector::CrossProduct(ToTarget, FVector::UpVector).GetSafeNormal() :
		FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal();

	FVector StrafeTarget = CachedPawn->GetActorLocation() + StrafeDir * Config.StrafeSpeed * DeltaTime * 10.0f;
	MoveToLocation(StrafeTarget);
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMBAT HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

bool USLFAIStateMachineComponent::TrySelectAbility(UDataAsset*& OutAbility)
{
	if (!CachedCombatManager.IsValid())
	{
		// Try to find combat manager again
		if (CachedPawn.IsValid())
		{
			CachedCombatManager = CachedPawn->FindComponentByClass<UAICombatManagerComponent>();
		}

		if (!CachedCombatManager.IsValid())
		{
			return false;
		}
	}

	// Set ability selection context for EvaluateAbilityRule to use
	CachedCombatManager->CachedDistanceToTarget = GetDistanceToTarget();
	CachedCombatManager->CachedHealthPercent = GetHealthPercent();

	return CachedCombatManager->TryGetAbility(OutAbility);
}

float USLFAIStateMachineComponent::GetHealthPercent() const
{
	if (!CachedPawn.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] GetHealthPercent - CachedPawn invalid, returning 1.0"));
		return 1.0f;
	}

	// CRITICAL: Use BPI_GenericCharacter interface to get the CORRECT StatManager
	// FindComponentByClass returns the WRONG one (Blueprint SCS component)
	// The interface returns the C++ constructor-created one that receives damage
	UStatManagerComponent* StatMgr = nullptr;
	if (CachedPawn->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UActorComponent* StatComp = nullptr;
		IBPI_GenericCharacter::Execute_GetStatManager(CachedPawn.Get(), StatComp);
		StatMgr = Cast<UStatManagerComponent>(StatComp);

		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] GetHealthPercent - Got StatManager via interface: %p"),
				StatMgr);
		}
	}

	if (!StatMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] GetHealthPercent - No StatManager via interface on %s, returning 1.0"),
			*CachedPawn->GetName());
		return 1.0f;
	}

	// Get HP stat
	FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Secondary.HP")), false);
	if (!HPTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] GetHealthPercent - HP tag not valid, returning 1.0"));
		return 1.0f;
	}

	UObject* HPStat = nullptr;
	FStatInfo HPInfo;
	StatMgr->GetStat(HPTag, HPStat, HPInfo);

	if (!HPStat || HPInfo.MaxValue <= 0.0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] GetHealthPercent - HPStat=%s, MaxValue=%.1f, returning 1.0"),
			HPStat ? TEXT("valid") : TEXT("null"), HPInfo.MaxValue);
		return 1.0f;
	}

	float Result = static_cast<float>(HPInfo.CurrentValue / HPInfo.MaxValue);
	UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] GetHealthPercent - %s HP: %.1f/%.1f = %.1f%%"),
		*CachedPawn->GetName(), HPInfo.CurrentValue, HPInfo.MaxValue, Result * 100.0f);
	return Result;
}

bool USLFAIStateMachineComponent::TrySelectGapCloser(UDataAsset*& OutAbility)
{
	OutAbility = nullptr;

	if (!CachedCombatManager.IsValid())
	{
		if (CachedPawn.IsValid())
		{
			CachedCombatManager = CachedPawn->FindComponentByClass<UAICombatManagerComponent>();
		}
		if (!CachedCombatManager.IsValid())
		{
			return false;
		}
	}

	// Set context for ability evaluation
	CachedCombatManager->CachedDistanceToTarget = GetDistanceToTarget();
	CachedCombatManager->CachedHealthPercent = GetHealthPercent();

	// Look for gap-closer abilities
	TArray<FSLFAIAbility> ValidGapClosers;
	float TotalWeight = 0.0f;

	for (const FSLFAIAbility& Ability : CachedCombatManager->Abilities)
	{
		if (Ability.bIsGapCloser && CachedCombatManager->EvaluateAbilityRule(Ability))
		{
			ValidGapClosers.Add(Ability);
			TotalWeight += Ability.Weight;
		}
	}

	if (ValidGapClosers.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - No valid gap-closers available"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"));
		return false;
	}

	// Weighted random selection
	float Roll = FMath::FRand() * TotalWeight;
	float AccumulatedWeight = 0.0f;

	for (FSLFAIAbility& Ability : ValidGapClosers)
	{
		AccumulatedWeight += Ability.Weight;
		if (Roll <= AccumulatedWeight)
		{
			OutAbility = Ability.AbilityAsset;
			Ability.LastUsedTime = GetWorld()->GetTimeSeconds();

			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Selected gap-closer: %s"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				*Ability.AbilityTag.ToString());
			return true;
		}
	}

	// Fallback to first valid
	if (ValidGapClosers.Num() > 0)
	{
		OutAbility = ValidGapClosers[0].AbilityAsset;
		return true;
	}

	return false;
}

void USLFAIStateMachineComponent::StartAttack(UDataAsset* Ability)
{
	if (!Ability)
	{
		SetCombatSubState(ESLFCombatSubState::Recovering);
		return;
	}

	SetCombatSubState(ESLFCombatSubState::Attacking);

	// ELDEN RING STYLE: Initialize attack tracking phase to Windup
	// The attack will progress: Windup -> Hold -> Commit
	SetAttackTrackingPhase(ESLFAttackTrackingPhase::Windup);

	// Get montage from ability
	UAnimMontage* AttackMontage = nullptr;

	if (UPDA_AI_Ability* AIAbility = Cast<UPDA_AI_Ability>(Ability))
	{
		AttackMontage = AIAbility->Montage.LoadSynchronous();
	}

	if (AttackMontage && CachedAnimInstance.IsValid())
	{
		// Play montage
		float Duration = CachedAnimInstance->Montage_Play(AttackMontage);

		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Playing attack montage: %s (%.2fs)"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				*AttackMontage->GetName(),
				Duration);
		}

		// Bind to montage end
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &USLFAIStateMachineComponent::OnAttackMontageEnded);
		CachedAnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
	}
	else
	{
		// No montage, just simulate attack time
		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - No attack montage found!"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"));
		}

		// Fake attack duration
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if (CombatSubState == ESLFCombatSubState::Attacking)
			{
				SetCombatSubState(ESLFCombatSubState::Recovering);
			}
		}, 1.0f, false);
	}
}

void USLFAIStateMachineComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Attack montage ended (interrupted: %s)"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
			bInterrupted ? TEXT("Yes") : TEXT("No"));
	}

	// ELDEN RING STYLE: Reset attack tracking phase
	SetAttackTrackingPhase(ESLFAttackTrackingPhase::None);

	if (CombatSubState == ESLFCombatSubState::Attacking)
	{
		SetCombatSubState(ESLFCombatSubState::Recovering);
	}
}

float USLFAIStateMachineComponent::CalculateNextAttackDelay() const
{
	return FMath::RandRange(Config.MinAttackDelay, Config.MaxAttackDelay);
}

// ═══════════════════════════════════════════════════════════════════════════════
// INPUT READING (ELDEN RING STYLE)
// ═══════════════════════════════════════════════════════════════════════════════

bool USLFAIStateMachineComponent::ShouldReactToPlayerInput() const
{
	return Config.bEnableInputReading && HasValidTarget() && GetDistanceToTarget() < Config.AttackRange * 2.0f;
}

bool USLFAIStateMachineComponent::IsPlayerHealing() const
{
	// Check if player is using healing item/flask
	// We check the ActionManager's IsResting flag (used during flask consumption animations)
	// or check if they're playing a healing-related montage

	if (!CurrentTarget.IsValid())
	{
		return false;
	}

	AActor* TargetActor = CurrentTarget.Get();

	// Check ActionManager for healing state
	if (UAC_ActionManager* ActionMgr = TargetActor->FindComponentByClass<UAC_ActionManager>())
	{
		// IsResting is set during flask/healing item consumption
		if (ActionMgr->IsResting)
		{
			return true;
		}
	}

	// Also check if the AnimInstance is playing a montage with "Flask" or "Heal" in the name
	if (ACharacter* TargetChar = Cast<ACharacter>(TargetActor))
	{
		if (UAnimInstance* AnimInst = TargetChar->GetMesh()->GetAnimInstance())
		{
			UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
			if (CurrentMontage)
			{
				FString MontageName = CurrentMontage->GetName();
				if (MontageName.Contains(TEXT("Flask")) || MontageName.Contains(TEXT("Heal")) || MontageName.Contains(TEXT("Drink")))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool USLFAIStateMachineComponent::IsPlayerRolling() const
{
	// Check if player is rolling/dodging by checking their CombatManager's IsInvincible flag
	// The ANS_InvincibilityFrame notify sets IsInvincible=true during dodge i-frames

	if (!CurrentTarget.IsValid())
	{
		return false;
	}

	AActor* TargetActor = CurrentTarget.Get();

	// Check player's CombatManager for invincibility (set during dodge i-frames)
	if (UAC_CombatManager* CombatMgr = TargetActor->FindComponentByClass<UAC_CombatManager>())
	{
		if (CombatMgr->IsInvincible)
		{
			return true;
		}
	}

	// Fallback: check if the AnimInstance is playing a dodge montage
	if (ACharacter* TargetChar = Cast<ACharacter>(TargetActor))
	{
		if (UAnimInstance* AnimInst = TargetChar->GetMesh()->GetAnimInstance())
		{
			UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
			if (CurrentMontage)
			{
				FString MontageName = CurrentMontage->GetName();
				if (MontageName.Contains(TEXT("Dodge")) || MontageName.Contains(TEXT("Roll")) || MontageName.Contains(TEXT("Evade")))
				{
					return true;
				}
			}
		}
	}

	return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// BOSS HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::CheckBossPhaseTransition()
{
	if (!Config.bIsBoss || !CachedPawn.IsValid())
	{
		return;
	}

	// Get current health percentage from StatManager
	float HealthPercent = 1.0f;

	if (UStatManagerComponent* StatMgr = CachedPawn->FindComponentByClass<UStatManagerComponent>())
	{
		// Get the HP stat using the correct full tag path
		FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Stat.Secondary.HP")), false);
		if (HPTag.IsValid())
		{
			UObject* StatObj = nullptr;
			FStatInfo StatInfo;
			if (StatMgr->GetStat(HPTag, StatObj, StatInfo))
			{
				if (StatInfo.MaxValue > 0.0)
				{
					HealthPercent = StatInfo.CurrentValue / StatInfo.MaxValue;
					if (bDebugEnabled)
					{
						UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] Boss HP check: %.1f%%"), HealthPercent * 100.0f);
					}
				}
			}
		}
	}

	ESLFBossPhase NewPhase = BossPhase;

	if (HealthPercent <= Config.EnrageHealthThreshold)
	{
		NewPhase = ESLFBossPhase::Enraged;
	}
	else if (HealthPercent <= Config.Phase3HealthThreshold)
	{
		NewPhase = ESLFBossPhase::Phase3;
	}
	else if (HealthPercent <= Config.Phase2HealthThreshold)
	{
		NewPhase = ESLFBossPhase::Phase2;
	}

	if (NewPhase != BossPhase)
	{
		OnBossPhaseTransition(NewPhase);
	}
}

void USLFAIStateMachineComponent::OnBossPhaseTransition(ESLFBossPhase NewPhase)
{
	ESLFBossPhase OldPhase = BossPhase;
	BossPhase = NewPhase;

	// Update aggression for new phase (bosses get more aggressive as fight progresses)
	UpdateAggression();

	// Broadcast phase change
	OnBossPhaseChanged.Broadcast(OldPhase, NewPhase);

	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] BOSS PHASE TRANSITION: %d -> %d (Aggression: %.2f)"),
			(int32)OldPhase, (int32)NewPhase, CurrentAggression);
	}

	// Phase transition effects:
	// - Could trigger special phase transition attack here
	// - Could swap ability sets based on phase
	// - Could buff movement speed

	// Become more aggressive in later phases
	if (NewPhase == ESLFBossPhase::Enraged)
	{
		// Enraged - significantly reduce attack delays
		NextAttackDelay = FMath::Max(NextAttackDelay * 0.5f, 0.2f);

		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] %s - ENRAGED! Attack delay reduced to %.2f"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				NextAttackDelay);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// PATROL/ROAM HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

FVector USLFAIStateMachineComponent::GetNextPatrolPoint()
{
	// Placeholder - would get from patrol path component
	return SpawnLocation;
}

FVector USLFAIStateMachineComponent::GetRandomRoamPoint()
{
	// Get random point within roam radius of spawn
	FVector RandomOffset = FMath::VRand() * FMath::RandRange(100.0f, 500.0f);
	RandomOffset.Z = 0.0f;
	return SpawnLocation + RandomOffset;
}

// ═══════════════════════════════════════════════════════════════════════════════
// BOUNDS CHECKING
// ═══════════════════════════════════════════════════════════════════════════════

bool USLFAIStateMachineComponent::IsOutOfBounds() const
{
	if (!CachedPawn.IsValid())
	{
		return false;
	}

	// BOSS FIX: Bosses should NEVER return to spawn due to distance
	// They fight until death, like Elden Ring bosses in arenas
	if (Config.bIsBoss)
	{
		return false;
	}

	float DistFromSpawn = FVector::Dist(CachedPawn->GetActorLocation(), SpawnLocation);
	return DistFromSpawn > Config.MaxChaseDistance;
}

void USLFAIStateMachineComponent::ReturnToSpawn()
{
	MoveToLocation(SpawnLocation);
}

// ═══════════════════════════════════════════════════════════════════════════════
// MOVEMENT HELPERS (Elden Ring Style)
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::UpdateMovementSpeed()
{
	float Distance = GetDistanceToTarget();

	// Sprint when far, walk when close for more tactical approach
	if (Distance > Config.SprintThresholdDistance)
	{
		if (!bIsSprinting)
		{
			bIsSprinting = true;
			SetMovementSpeed(Config.RunSpeed);
		}
	}
	else if (Distance < Config.PreferredCombatDistance)
	{
		if (bIsSprinting || Distance < Config.AttackRange)
		{
			bIsSprinting = false;
			SetMovementSpeed(Config.WalkSpeed);
		}
	}
}

void USLFAIStateMachineComponent::SetMovementSpeed(float NewSpeed)
{
	if (!CachedPawn.IsValid())
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(CachedPawn.Get()))
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = NewSpeed;
		}
	}
}

FVector USLFAIStateMachineComponent::CalculateBestStrafePosition()
{
	if (!HasValidTarget() || !CachedPawn.IsValid())
	{
		return CachedPawn.IsValid() ? CachedPawn->GetActorLocation() : FVector::ZeroVector;
	}

	FVector MyLocation = CachedPawn->GetActorLocation();
	FVector TargetLocation = CurrentTarget->GetActorLocation();
	FVector ToTarget = TargetLocation - MyLocation;
	ToTarget.Z = 0.0f;
	float CurrentDistance = ToTarget.Size();
	FVector ToTargetNorm = ToTarget.GetSafeNormal();

	// Calculate three potential positions: Back, Left, Right
	// Score each based on:
	// 1. Distance from player (prefer PreferredCombatDistance)
	// 2. Not getting cornered (nav mesh check)
	// 3. Angle variety (don't always go to same position)

	struct FStrafeCandidate
	{
		FVector Position;
		float Score;
		FString Name;
	};

	TArray<FStrafeCandidate> Candidates;

	// Calculate perpendicular directions
	FVector RightDir = FVector::CrossProduct(ToTargetNorm, FVector::UpVector);
	FVector LeftDir = -RightDir;
	FVector BackDir = -ToTargetNorm;

	float StrafeRadius = Config.PreferredCombatDistance;

	// Right strafe position
	{
		FVector RightPos = TargetLocation + (RightDir * StrafeRadius) + (BackDir * StrafeRadius * 0.3f);
		float DistScore = 1.0f - FMath::Abs(FVector::Dist(RightPos, TargetLocation) - Config.PreferredCombatDistance) / Config.PreferredCombatDistance;
		float BiasScore = bStrafeClockwise ? 0.2f : 0.0f; // Slight preference to continue in same direction
		Candidates.Add({RightPos, DistScore + BiasScore + FMath::FRand() * 0.1f, TEXT("Right")});
	}

	// Left strafe position
	{
		FVector LeftPos = TargetLocation + (LeftDir * StrafeRadius) + (BackDir * StrafeRadius * 0.3f);
		float DistScore = 1.0f - FMath::Abs(FVector::Dist(LeftPos, TargetLocation) - Config.PreferredCombatDistance) / Config.PreferredCombatDistance;
		float BiasScore = !bStrafeClockwise ? 0.2f : 0.0f;
		Candidates.Add({LeftPos, DistScore + BiasScore + FMath::FRand() * 0.1f, TEXT("Left")});
	}

	// Back/retreat position (lower priority)
	{
		FVector BackPos = TargetLocation + (BackDir * StrafeRadius * 1.2f);
		float DistScore = 0.5f; // Lower base score for backing up
		Candidates.Add({BackPos, DistScore + FMath::FRand() * 0.1f, TEXT("Back")});
	}

	// Find best candidate
	FStrafeCandidate* BestCandidate = nullptr;
	for (FStrafeCandidate& Candidate : Candidates)
	{
		// Nav mesh validation would go here
		// For now, just check line of sight to target
		if (!BestCandidate || Candidate.Score > BestCandidate->Score)
		{
			BestCandidate = &Candidate;
		}
	}

	if (BestCandidate)
	{
		// Update strafe direction based on chosen position
		bStrafeClockwise = BestCandidate->Name == TEXT("Right");

		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Best strafe: %s (score: %.2f)"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				*BestCandidate->Name,
				BestCandidate->Score);
		}

		return BestCandidate->Position;
	}

	return MyLocation;
}

void USLFAIStateMachineComponent::UpdateStrafeDirection(float DeltaTime)
{
	TimeSinceStrafeDirectionChange += DeltaTime;

	if (TimeSinceStrafeDirectionChange >= Config.StrafeDirectionChangeInterval)
	{
		// Chance to switch direction based on bias
		if (FMath::FRand() > Config.StrafeDirectionBias)
		{
			bStrafeClockwise = !bStrafeClockwise;
		}
		TimeSinceStrafeDirectionChange = 0.0f;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// AGGRESSION HELPERS (Elden Ring Style)
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::UpdateAggression()
{
	CurrentAggression = Config.BaseAggression;

	// Increase aggression based on boss phase
	if (Config.bIsBoss)
	{
		int32 PhaseLevel = static_cast<int32>(BossPhase);
		CurrentAggression += PhaseLevel * Config.AggressionPerPhase;
	}

	// Clamp to valid range
	CurrentAggression = FMath::Clamp(CurrentAggression, 0.0f, 1.0f);

	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Aggression updated: %.2f (Phase: %d)"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
			CurrentAggression,
			(int32)BossPhase);
	}
}

bool USLFAIStateMachineComponent::ShouldStartCombo() const
{
	// Higher aggression = higher combo chance
	float AdjustedComboChance = Config.ComboChance * (0.5f + CurrentAggression * 0.5f);
	return FMath::FRand() < AdjustedComboChance;
}

bool USLFAIStateMachineComponent::ShouldUseGapCloser() const
{
	// Use gap closer when far and random chance passes
	return FMath::FRand() < Config.GapCloserChance;
}

bool USLFAIStateMachineComponent::ShouldFeint() const
{
	// Lower aggression = more likely to feint (more cautious)
	float AdjustedFeintChance = Config.FeintChance * (1.5f - CurrentAggression);
	return FMath::FRand() < AdjustedFeintChance;
}

bool USLFAIStateMachineComponent::ShouldDelayAttack() const
{
	return FMath::FRand() < Config.AttackDelayChance;
}

float USLFAIStateMachineComponent::GetRandomAttackDelay() const
{
	return FMath::RandRange(Config.AttackDelayMin, Config.AttackDelayMax);
}

// ═══════════════════════════════════════════════════════════════════════════════
// ELDEN RING STYLE: INPUT READING & PUNISH
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::UpdatePlayerStateDetection()
{
	if (!Config.bEnableInputReading || !HasValidTarget())
	{
		bPlayerIsHealing = false;
		bPlayerIsRolling = false;
		return;
	}

	bool bWasHealing = bPlayerIsHealing;
	bool bWasRolling = bPlayerIsRolling;

	// Check current player state
	bPlayerIsHealing = IsPlayerHealing();
	bPlayerIsRolling = IsPlayerRolling();

	// Track when state was first detected
	if ((bPlayerIsHealing && !bWasHealing) || (bPlayerIsRolling && !bWasRolling))
	{
		PlayerStateDetectedTime = GetWorld()->GetTimeSeconds();

		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - PLAYER STATE DETECTED: Healing=%s, Rolling=%s"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				bPlayerIsHealing ? TEXT("YES") : TEXT("NO"),
				bPlayerIsRolling ? TEXT("YES") : TEXT("NO"));
		}
	}
}

bool USLFAIStateMachineComponent::CanPunishPlayer() const
{
	if (!Config.bEnableInputReading)
	{
		return false;
	}

	// Check cooldown
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastPunishTime < Config.PunishCooldown)
	{
		return false;
	}

	// Check if player state was detected long enough ago (reaction time)
	float TimeSinceDetection = CurrentTime - PlayerStateDetectedTime;
	if (TimeSinceDetection < Config.InputReadingReactionTime)
	{
		return false;
	}

	// Must be in a state where we CAN react (not mid-attack)
	if (CombatSubState == ESLFCombatSubState::Attacking ||
		CombatSubState == ESLFCombatSubState::Recovering)
	{
		return false;
	}

	// Check if player is doing something punishable
	if (bPlayerIsHealing)
	{
		return FMath::FRand() < Config.PunishHealingChance;
	}
	if (bPlayerIsRolling)
	{
		return FMath::FRand() < Config.PunishRollingChance;
	}

	return false;
}

bool USLFAIStateMachineComponent::TryPunishPlayer(UDataAsset*& OutPunishAbility)
{
	OutPunishAbility = nullptr;

	if (!CanPunishPlayer())
	{
		return false;
	}

	// Try to find a gap-closer ability (fast punish move)
	if (TrySelectGapCloser(OutPunishAbility))
	{
		LastPunishTime = GetWorld()->GetTimeSeconds();

		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - PUNISHING PLAYER! Ability: %s"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				OutPunishAbility ? *OutPunishAbility->GetName() : TEXT("None"));
		}
		return true;
	}

	// No gap-closer, try regular ability but transition to attacking immediately
	if (TrySelectAbility(OutPunishAbility))
	{
		LastPunishTime = GetWorld()->GetTimeSeconds();

		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - PUNISHING PLAYER (regular attack)!"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"));
		}
		return true;
	}

	return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ELDEN RING STYLE: ATTACK TRACKING PHASES
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::SetAttackTrackingPhase(ESLFAttackTrackingPhase Phase)
{
	if (AttackTrackingPhase != Phase)
	{
		AttackTrackingPhase = Phase;
		TimeInTrackingPhase = 0.0f;

		if (bDebugEnabled)
		{
			const TCHAR* PhaseNames[] = { TEXT("None"), TEXT("Windup"), TEXT("Hold"), TEXT("Commit") };
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Attack tracking phase: %s"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
				PhaseNames[(int32)Phase]);
		}
	}
}

void USLFAIStateMachineComponent::UpdateAttackTracking(float DeltaTime)
{
	TimeInTrackingPhase += DeltaTime;

	// If in attack, apply rotation based on phase
	if (AttackTrackingPhase == ESLFAttackTrackingPhase::None ||
		AttackTrackingPhase == ESLFAttackTrackingPhase::Commit)
	{
		// Not tracking or locked - no rotation
		return;
	}

	if (!HasValidTarget() || !CachedPawn.IsValid())
	{
		return;
	}

	// Get current rotation speed based on phase
	float RotSpeed = GetCurrentRotationSpeed();
	if (RotSpeed <= 0.0f)
	{
		return;
	}

	// Rotate towards target
	FVector TargetLoc = CurrentTarget->GetActorLocation();
	FVector MyLoc = CachedPawn->GetActorLocation();
	FRotator LookAtRot = (TargetLoc - MyLoc).Rotation();
	LookAtRot.Pitch = 0.0f;
	LookAtRot.Roll = 0.0f;

	FRotator CurrentRot = CachedPawn->GetActorRotation();
	FRotator NewRot = FMath::RInterpConstantTo(CurrentRot, LookAtRot, DeltaTime, RotSpeed);
	CachedPawn->SetActorRotation(NewRot);

	// PHASE TRANSITIONS
	// Windup -> Hold: Fast initial snap, then slower tracking during the "hold"
	if (AttackTrackingPhase == ESLFAttackTrackingPhase::Windup &&
		TimeInTrackingPhase >= Config.WindupDuration)
	{
		SetAttackTrackingPhase(ESLFAttackTrackingPhase::Hold);
	}
	// Hold -> Commit: After hold duration, lock in and swing
	else if (AttackTrackingPhase == ESLFAttackTrackingPhase::Hold &&
		TimeInTrackingPhase >= Config.HoldDuration)
	{
		SetAttackTrackingPhase(ESLFAttackTrackingPhase::Commit);
	}
}

float USLFAIStateMachineComponent::GetCurrentRotationSpeed() const
{
	switch (AttackTrackingPhase)
	{
		case ESLFAttackTrackingPhase::Windup:
			return Config.WindupRotationSpeed;
		case ESLFAttackTrackingPhase::Hold:
			return Config.HoldRotationSpeed;
		case ESLFAttackTrackingPhase::Commit:
			return Config.bLockRotationOnCommit ? 0.0f : Config.HoldRotationSpeed * 0.5f;
		default:
			return 0.0f;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// ELDEN RING STYLE: TURN IN PLACE
// ═══════════════════════════════════════════════════════════════════════════════

bool USLFAIStateMachineComponent::ShouldTurnInPlace() const
{
	if (!Config.bUseTurnInPlace)
	{
		return false;
	}

	float Angle = FMath::Abs(GetAngleToTarget());
	return Angle > Config.TurnInPlaceThreshold;
}

float USLFAIStateMachineComponent::GetAngleToTarget() const
{
	if (!HasValidTarget() || !CachedPawn.IsValid())
	{
		return 0.0f;
	}

	FVector ToTarget = CurrentTarget->GetActorLocation() - CachedPawn->GetActorLocation();
	ToTarget.Z = 0.0f;
	ToTarget.Normalize();

	FVector Forward = CachedPawn->GetActorForwardVector();
	Forward.Z = 0.0f;
	Forward.Normalize();

	// Calculate angle
	float DotProduct = FVector::DotProduct(Forward, ToTarget);
	float AngleRad = FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f));
	float AngleDeg = FMath::RadiansToDegrees(AngleRad);

	// Determine direction (negative = left, positive = right)
	FVector Cross = FVector::CrossProduct(Forward, ToTarget);
	if (Cross.Z < 0.0f)
	{
		AngleDeg = -AngleDeg;
	}

	return AngleDeg;
}

void USLFAIStateMachineComponent::PlayTurnAnimation(float AngleToTarget)
{
	// Smooth turn-in-place when target is behind the enemy
	// Uses rotation interpolation for fluid movement
	if (!HasValidTarget() || !CachedPawn.IsValid())
	{
		return;
	}

	// Get current and target rotations
	FRotator CurrentRotation = CachedPawn->GetActorRotation();
	FVector Direction = GetDirectionToTarget();
	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;

	// Use smooth interpolation - 360 degrees per second is a fast but smooth turn
	// This is called every tick, so DeltaTime matters for consistent speed
	float TurnSpeed = 360.0f;  // Degrees per second
	float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, TurnSpeed / 60.0f);
	NewRotation.Pitch = 0.0f;
	NewRotation.Roll = 0.0f;

	CachedPawn->SetActorRotation(NewRotation);

	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - SMOOTH TURN: %.1f degrees remaining (speed: %.0f deg/s)"),
			CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
			AngleToTarget, TurnSpeed);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// ELDEN RING STYLE: MOVEMENT SETTINGS
// ═══════════════════════════════════════════════════════════════════════════════

void USLFAIStateMachineComponent::ApplyEldenRingMovementSettings()
{
	if (!CachedPawn.IsValid())
	{
		return;
	}

	// Get character movement component
	if (ACharacter* Character = Cast<ACharacter>(CachedPawn.Get()))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			// Zero acceleration = instant speed changes (Souls-like feel)
			MovementComp->MaxAcceleration = Config.MaxAcceleration;
			MovementComp->BrakingDecelerationWalking = Config.BrakingDeceleration;

			// Disable acceleration smoothing for snappier movement
			MovementComp->bRequestedMoveUseAcceleration = false;

			if (bDebugEnabled)
			{
				UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Applied Elden Ring movement settings (Accel: %.0f, Brake: %.0f)"),
					CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
					Config.MaxAcceleration,
					Config.BrakingDeceleration);
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// DEBUG
// ═══════════════════════════════════════════════════════════════════════════════

FString USLFAIStateMachineComponent::GetDebugString() const
{
	return FString::Printf(TEXT("State: %d | SubState: %d | Phase: %d | Aggro: %.2f | Target: %s | Dist: %.0f"),
		(int32)CurrentState,
		(int32)CombatSubState,
		(int32)BossPhase,
		CurrentAggression,
		HasValidTarget() ? *CurrentTarget->GetName() : TEXT("None"),
		GetDistanceToTarget());
}

// ═══════════════════════════════════════════════════════════════════════════════
// TARGET DETECTION HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

AActor* USLFAIStateMachineComponent::FindNearestPlayer() const
{
	if (!CachedPawn.IsValid())
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	// Find player pawn
	APlayerController* PC = World->GetFirstPlayerController();
	if (PC && PC->GetPawn())
	{
		return PC->GetPawn();
	}

	return nullptr;
}

bool USLFAIStateMachineComponent::CanSeeActor(AActor* Actor) const
{
	if (!Actor || !CachedPawn.IsValid())
	{
		return false;
	}

	// FOV check only applies during INITIAL DETECTION (Idle/Patrol states)
	// Once in combat, the enemy always knows where the player is - no FOV restriction
	// EXCEPTION: Skip FOV check during target acquisition grace period (after player respawn)
	bool bApplyFOVCheck = (CurrentState == ESLFAIState::Idle ||
	                       CurrentState == ESLFAIState::Patrol ||
	                       CurrentState == ESLFAIState::RandomRoam);

	// Skip FOV check during grace period (player just respawned)
	if (bInTargetAcquisitionGracePeriod)
	{
		bApplyFOVCheck = false;
		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Skipping FOV check (grace period active)"),
				CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"));
		}
	}

	if (bApplyFOVCheck)
	{
		// Check if target is within field of view
		FVector ToTarget = Actor->GetActorLocation() - CachedPawn->GetActorLocation();
		ToTarget.Z = 0.0f;
		ToTarget.Normalize();

		FVector Forward = CachedPawn->GetActorForwardVector();
		Forward.Z = 0.0f;
		Forward.Normalize();

		float DotProduct = FVector::DotProduct(Forward, ToTarget);
		float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f)));

		// FieldOfViewHalfAngle of 90 means enemy can see anything up to 90 degrees to either side
		// This creates a 180-degree cone (full hemisphere in front)
		if (AngleToTarget > Config.FieldOfViewHalfAngle)
		{
			if (bDebugEnabled)
			{
				UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] %s - Target NOT in FOV (angle=%.1f > FOV=%.1f) - INITIAL DETECTION ONLY"),
					CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
					AngleToTarget, Config.FieldOfViewHalfAngle);
			}
			return false;
		}
	}

	// Line of sight check (always applies - can't see through walls)
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CachedPawn.Get());

	FVector StartPos = CachedPawn->GetActorLocation() + FVector(0, 0, 50);  // Eye height
	FVector EndPos = Actor->GetActorLocation() + FVector(0, 0, 50);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartPos,
		EndPos,
		ECC_Visibility,
		Params
	);

	// Can see if no hit, or if we hit the target actor
	return !bHit || Hit.GetActor() == Actor;
}
