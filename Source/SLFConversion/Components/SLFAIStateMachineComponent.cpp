// SLFAIStateMachineComponent.cpp
// Custom C++ State Machine for Soulslike AI

#include "Components/SLFAIStateMachineComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SLFPrimaryDataAssets.h"

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

	UE_LOG(LogTemp, Log, TEXT("[AIStateMachine] BeginPlay on %s - IsBoss: %s"),
		CachedPawn.IsValid() ? *CachedPawn->GetName() : TEXT("Unknown"),
		Config.bIsBoss ? TEXT("Yes") : TEXT("No"));
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

		// Cache combat manager
		CachedCombatManager = CachedPawn->FindComponentByClass<UAICombatManagerComponent>();

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

	// Update time tracking
	TimeInCurrentState += DeltaTime;
	TimeInCombatSubState += DeltaTime;

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

void USLFAIStateMachineComponent::OnDeath()
{
	SetState(ESLFAIState::Dead);
	StopMovement();
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
		break;

	case ESLFAIState::Dead:
		StopMovement();
		SetCombatSubState(ESLFCombatSubState::None);
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
	// Check for targets via perception (handled externally via SetTarget)
	// For now, just stand still

	// Check if we should return to patrol
	// (Could add patrol point checking here)
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

	// Input reading - react to player healing
	if (Config.bEnableInputReading && ShouldReactToPlayerInput())
	{
		if (IsPlayerHealing() && CombatSubState != ESLFCombatSubState::Attacking)
		{
			// Punish healing!
			if (FMath::FRand() < Config.PunishHealingChance)
			{
				SetCombatSubState(ESLFCombatSubState::Engaging);
				NextAttackDelay = 0.0f; // Attack immediately
			}
		}
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

	// Close distance to target
	if (Distance > Config.AttackRange)
	{
		MoveToTarget();
		FaceTarget();
	}
	else
	{
		// In attack range - decide what to do
		StopMovement();
		FaceTarget();

		float TimeSinceLastAttack = GetWorld()->GetTimeSeconds() - LastAttackTime;
		if (TimeSinceLastAttack >= NextAttackDelay)
		{
			// Ready to attack
			UDataAsset* SelectedAbility = nullptr;
			if (TrySelectAbility(SelectedAbility))
			{
				if (Config.WindUpHoldTime > 0.0f)
				{
					// Use wind-up phase for input reading
					SetCombatSubState(ESLFCombatSubState::WindingUp);
				}
				else
				{
					// Attack immediately
					StartAttack(SelectedAbility);
				}
			}
			else
			{
				// No ability available, reposition
				SetCombatSubState(ESLFCombatSubState::Positioning);
			}
		}
		else
		{
			// Waiting for attack cooldown, strafe around target
			if (FMath::FRand() < 0.3f * DeltaTime) // Occasionally reposition
			{
				SetCombatSubState(ESLFCombatSubState::Positioning);
			}
		}
	}
}

void USLFAIStateMachineComponent::TickCombat_Positioning(float DeltaTime)
{
	float Distance = GetDistanceToTarget();

	// Maintain preferred distance while strafing
	if (Distance < Config.PreferredCombatDistance * 0.8f)
	{
		// Too close, back up a bit
		SetCombatSubState(ESLFCombatSubState::Retreating);
		return;
	}

	if (Distance > Config.PreferredCombatDistance * 1.2f)
	{
		// Too far, close in
		SetCombatSubState(ESLFCombatSubState::Engaging);
		return;
	}

	// Strafe around target
	bool bClockwise = FMath::RandBool();
	StrafeAroundTarget(DeltaTime, bClockwise);
	FaceTarget();

	// After some time, go back to engaging
	if (TimeInCombatSubState > Config.RepositionInterval)
	{
		SetCombatSubState(ESLFCombatSubState::Engaging);
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

		// Execute attack
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

void USLFAIStateMachineComponent::TickCombat_Attacking(float DeltaTime)
{
	// Attack animation is playing
	// State transition happens when montage ends (via OnAttackMontageEnded)

	// Optional: Track target during early attack frames
	if (TimeInCombatSubState < 0.2f)
	{
		FaceTarget();
	}
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
			SetCombatSubState(ESLFCombatSubState::Engaging);
		}
		else if (FMath::FRand() < 0.3f)
		{
			SetCombatSubState(ESLFCombatSubState::Positioning);
		}
		else
		{
			SetCombatSubState(ESLFCombatSubState::Engaging);
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

	CachedAIController->MoveToLocation(Location, Config.AttackRange * 0.5f);
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

	CachedPawn->SetActorRotation(FMath::RInterpTo(
		CachedPawn->GetActorRotation(),
		TargetRotation,
		GetWorld()->GetDeltaSeconds(),
		10.0f
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

	return CachedCombatManager->TryGetAbility(OutAbility);
}

void USLFAIStateMachineComponent::StartAttack(UDataAsset* Ability)
{
	if (!Ability)
	{
		SetCombatSubState(ESLFCombatSubState::Recovering);
		return;
	}

	SetCombatSubState(ESLFCombatSubState::Attacking);

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
	// This would need to be implemented based on your player character
	// For now, return false
	// TODO: Implement player state checking
	return false;
}

bool USLFAIStateMachineComponent::IsPlayerRolling() const
{
	// Check if player is rolling/dodging
	// This would need to be implemented based on your player character
	// For now, return false
	// TODO: Implement player state checking
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

	// Get current health percentage
	// This would need to be implemented based on your health system
	float HealthPercent = 1.0f; // TODO: Get actual health

	// For now, use a simple interface check or stat component
	// This is a placeholder - implement based on your health system

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

	// Broadcast phase change
	OnBossPhaseChanged.Broadcast(OldPhase, NewPhase);

	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIStateMachine] BOSS PHASE TRANSITION: %d -> %d"),
			(int32)OldPhase, (int32)NewPhase);
	}

	// Could trigger special moves, buff stats, etc. here
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

	float DistFromSpawn = FVector::Dist(CachedPawn->GetActorLocation(), SpawnLocation);
	return DistFromSpawn > Config.MaxChaseDistance;
}

void USLFAIStateMachineComponent::ReturnToSpawn()
{
	MoveToLocation(SpawnLocation);
}

// ═══════════════════════════════════════════════════════════════════════════════
// DEBUG
// ═══════════════════════════════════════════════════════════════════════════════

FString USLFAIStateMachineComponent::GetDebugString() const
{
	return FString::Printf(TEXT("State: %d | SubState: %d | Phase: %d | Target: %s | Dist: %.0f"),
		(int32)CurrentState,
		(int32)CombatSubState,
		(int32)BossPhase,
		HasValidTarget() ? *CurrentTarget->GetName() : TEXT("None"),
		GetDistanceToTarget());
}
