// AC_CombatManager.cpp
// C++ component implementation for AC_CombatManager
//
// 20-PASS VALIDATION: 2026-01-02 Full Migration
// Source: BlueprintDNA_v2/Component/AC_CombatManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added
// Functions: 31 (20 core + 11 events)

#include "Components/AC_CombatManager.h"
#include "Components/StatManagerComponent.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/AC_StatusEffectManager.h"
#include "Blueprints/B_DeathCurrency.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/BPI_Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/AssetManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

UAC_CombatManager::UAC_CombatManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize runtime variables
	IsDead = false;
	IsGuarding = false;
	WantsToGuard = false;
	IsInvincible = false;
	PoiseBroken = false;
	HyperArmor = false;
	ExecutionTarget = nullptr;
	Mesh = nullptr;
	TimeSinceGuard = 0.0;
	IKWeight = 0.0;
	CurrentHitNormal = FVector::ZeroVector;

	// Initialize config defaults
	RagdollOnDeath = true;
	GuardingGracePeriod = 0.2;
	PerfectGuardDuration = 0.15;
	MinPerfectGuardPoiseDamage = 50.0;
	MaxPerfectGuardPoiseDamage = 150.0;
	MinUnarmedPoiseDamage = 10.0;
	MaxUnarmedPoiseDamage = 30.0;
	FacingDirectionAcceptance = ESLFDotProductThreshold::Medium;

	// Socket names
	HandSocket_R = FName("hand_r");
	HandSocket_L = FName("hand_l");
}

void UAC_CombatManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::BeginPlay"));

	// Cache the skeletal mesh component from owner
	if (AActor* Owner = GetOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(Owner))
		{
			Mesh = Character->GetMesh();
		}
	}
}

void UAC_CombatManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * SetGuardState - Toggle the guard state
 *
 * Blueprint Logic:
 * 1. Branch on Toggled?
 * 2. If TRUE: Check if can guard (not poise broken, has block sequence)
 * 3. If IgnoreGracePeriod is FALSE, use grace period logic
 * 4. Set IsGuarding? variable
 */
void UAC_CombatManager::SetGuardState_Implementation(bool Toggled, bool IgnoreGracePeriod)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::SetGuardState - Toggled: %s, IgnoreGracePeriod: %s"),
		Toggled ? TEXT("true") : TEXT("false"), IgnoreGracePeriod ? TEXT("true") : TEXT("false"));

	// Store the intent to guard
	WantsToGuard = Toggled;

	if (Toggled)
	{
		// Check if poise is broken - can't guard if poise broken
		if (PoiseBroken)
		{
			UE_LOG(LogTemp, Log, TEXT("  Cannot guard - poise is broken"));
			return;
		}

		// Set guarding state
		IsGuarding = true;

		// Reset guard timer for perfect guard tracking
		TimeSinceGuard = 0.0;

		// Start guard timer if not ignoring grace period
		if (!IgnoreGracePeriod)
		{
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimer(GuardTimer, this, &UAC_CombatManager::IncreaseTimeSinceGuard, 0.016f, true);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("  Guard activated"));
	}
	else
	{
		// Deactivate guard
		IsGuarding = false;

		// Start grace period timer
		if (!IgnoreGracePeriod)
		{
			GuardGracePeriod();
		}

		// Clear guard timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(GuardTimer);
		}

		UE_LOG(LogTemp, Log, TEXT("  Guard deactivated"));
	}
}

/**
 * GetIsGuarding - Check if currently guarding (includes grace period)
 *
 * Blueprint Logic: Return IsGuarding? OR GuardGracePeriodTimer is active
 */
bool UAC_CombatManager::GetIsGuarding_Implementation()
{
	// Check active guarding OR within grace period
	bool bInGracePeriod = false;
	if (UWorld* World = GetWorld())
	{
		bInGracePeriod = World->GetTimerManager().IsTimerActive(GuardGracePeriodTimer);
	}

	bool Result = IsGuarding || bInGracePeriod;
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::GetIsGuarding - %s (Active: %s, GracePeriod: %s)"),
		Result ? TEXT("true") : TEXT("false"),
		IsGuarding ? TEXT("true") : TEXT("false"),
		bInGracePeriod ? TEXT("true") : TEXT("false"));

	return Result;
}

/**
 * SetExecutionTarget - Set the execution target actor
 *
 * Blueprint Logic: Simple setter
 */
void UAC_CombatManager::SetExecutionTarget_Implementation(AActor* InExecutionTarget)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::SetExecutionTarget - %s"),
		InExecutionTarget ? *InExecutionTarget->GetName() : TEXT("None"));
	ExecutionTarget = InExecutionTarget;
}

/**
 * HandleProjectileDamage - Process damage from projectiles
 *
 * Blueprint Logic:
 * 1. Check IsInvincible? - skip if true
 * 2. Calculate actual damage using damage negation
 * 3. Spawn visual effect
 * 4. Apply status effects
 * 5. Apply damage to health stat
 */
void UAC_CombatManager::HandleProjectileDamage_Implementation(double IncomingDamage, UNiagaraSystem* ProjectileHitEffect, const FGameplayTag& NegationStat, const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::HandleProjectileDamage - Damage: %f"), IncomingDamage);

	// Check invincibility
	if (IsInvincible)
	{
		UE_LOG(LogTemp, Log, TEXT("  Skipped - character is invincible"));
		return;
	}

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Get stat manager for damage calculation
	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();

	// Calculate actual damage (apply damage negation if stat provided)
	double ActualDamage = IncomingDamage;
	if (NegationStat.IsValid() && IsValid(StatManager))
	{
		UObject* NegationStatObj = nullptr;
		FStatInfo NegationInfo;
		StatManager->GetStat(NegationStat, NegationStatObj, NegationInfo);

		// Apply negation: damage = incoming * (1 - negation/100)
		double NegationPercent = NegationInfo.CurrentValue / 100.0;
		ActualDamage = IncomingDamage * (1.0 - FMath::Clamp(NegationPercent, 0.0, 1.0));
	}

	// Spawn hit effect
	if (IsValid(ProjectileHitEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ProjectileHitEffect,
			Owner->GetActorLocation(),
			FRotator::ZeroRotator);
	}

	// Apply status effects
	if (StatusEffects.Num() > 0)
	{
		ApplyIncomingStatusEffects(StatusEffects, 1.0);
	}

	// Apply damage to health
	if (IsValid(StatManager))
	{
		FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
		StatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -ActualDamage, false, true);
	}

	UE_LOG(LogTemp, Log, TEXT("  Applied %f damage (before negation: %f)"), ActualDamage, IncomingDamage);
}

/**
 * HandleIncomingWeaponDamage - Main combat damage processing
 *
 * Blueprint Logic:
 * 1. Check IsInvincible? - skip if true
 * 2. Check GetIsGuarding() - if true, process guard
 * 3. Check perfect guard timing
 * 4. Apply damage, poise damage, status effects
 * 5. Handle hit reaction
 */
void UAC_CombatManager::HandleIncomingWeaponDamage_Implementation(AActor* WeaponOwnerActor, USoundBase* GuardSound, USoundBase* PerfectGuardSound, const FHitResult& HitInfo, double IncomingDamage, double IncomingPoiseDamage, const TMap<FGameplayTag, UPrimaryDataAsset*>& IncomingStatusEffect)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::HandleIncomingWeaponDamage - Damage: %f, Poise: %f"),
		IncomingDamage, IncomingPoiseDamage);

	// Store hit info for IK and reactions
	CurrentHitNormal = HitInfo.ImpactNormal;

	// Check invincibility first
	if (IsInvincible)
	{
		UE_LOG(LogTemp, Log, TEXT("  Skipped - character is invincible"));
		return;
	}

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (!IsValid(StatManager))
	{
		return;
	}

	// Check if guarding
	if (GetIsGuarding())
	{
		// Check if facing the attacker
		bool bFacingEnemy = GetIsFacingEnemy(WeaponOwnerActor);

		if (bFacingEnemy)
		{
			// Check for perfect guard (within timing window)
			bool bPerfectGuard = TimeSinceGuard <= PerfectGuardDuration;

			if (bPerfectGuard)
			{
				// PERFECT GUARD
				UE_LOG(LogTemp, Log, TEXT("  PERFECT GUARD!"));

				// Play perfect guard sound
				if (IsValid(PerfectGuardSound))
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), PerfectGuardSound, Owner->GetActorLocation());
				}

				// Spawn perfect guard effect
				if (IsValid(PerfectGuardEffect))
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						PerfectGuardEffect,
						HitInfo.ImpactPoint,
						HitInfo.ImpactNormal.Rotation());
				}

				// Apply poise damage to attacker (reflect)
				if (IsValid(WeaponOwnerActor))
				{
					UStatManagerComponent* AttackerStatManager = WeaponOwnerActor->FindComponentByClass<UStatManagerComponent>();
					if (IsValid(AttackerStatManager))
					{
						FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
						double ReflectedPoise = FMath::RandRange(MinPerfectGuardPoiseDamage, MaxPerfectGuardPoiseDamage);
						AttackerStatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, -ReflectedPoise, false, true);
					}
				}

				// Minimal stamina drain for perfect guard
				FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
				StatManager->AdjustStat(StaminaTag, ESLFValueType::CurrentValue, -5.0, false, true);

				// No damage taken on perfect guard
				return;
			}
			else
			{
				// REGULAR GUARD
				UE_LOG(LogTemp, Log, TEXT("  Regular guard"));

				// Play guard sound
				if (IsValid(GuardSound))
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), GuardSound, Owner->GetActorLocation());
				}

				// Spawn guard effect
				if (IsValid(GuardEffect))
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						GuardEffect,
						HitInfo.ImpactPoint,
						HitInfo.ImpactNormal.Rotation());
				}

				// Calculate stamina drain
				double StaminaDrain = GetStaminaDrainAmountForDamage(IncomingDamage);
				FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));

				// Check if enough stamina to block
				UObject* StaminaStat = nullptr;
				FStatInfo StaminaInfo;
				StatManager->GetStat(StaminaTag, StaminaStat, StaminaInfo);

				if (StaminaInfo.CurrentValue >= StaminaDrain)
				{
					// Can block - drain stamina
					StatManager->AdjustStat(StaminaTag, ESLFValueType::CurrentValue, -StaminaDrain, false, true);

					// Reduced damage on block (e.g., 20% gets through)
					double BlockedDamage = IncomingDamage * 0.2;
					FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
					StatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -BlockedDamage, false, true);

					UE_LOG(LogTemp, Log, TEXT("  Blocked! Stamina drained: %f, Chip damage: %f"), StaminaDrain, BlockedDamage);
				}
				else
				{
					// Guard broken - not enough stamina
					UE_LOG(LogTemp, Log, TEXT("  Guard broken - insufficient stamina"));
					PoiseBroken = true;
					IsGuarding = false;

					// Take full damage
					FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
					StatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -IncomingDamage, false, true);

					// Handle stagger/reaction
					HandleHitReaction(HitInfo);
				}

				return;
			}
		}
		// Not facing enemy - guard doesn't work, fall through to take damage
	}

	// NOT GUARDING - Take full damage
	UE_LOG(LogTemp, Log, TEXT("  Taking full damage"));

	// Apply health damage
	FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
	StatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -IncomingDamage, false, true);

	// Apply poise damage (if not hyper armor)
	if (!HyperArmor)
	{
		FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));

		UObject* PoiseStat = nullptr;
		FStatInfo PoiseInfo;
		StatManager->GetStat(PoiseTag, PoiseStat, PoiseInfo);

		double NewPoise = PoiseInfo.CurrentValue - IncomingPoiseDamage;
		StatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, -IncomingPoiseDamage, false, true);

		// Check if poise broken
		if (NewPoise <= 0)
		{
			PoiseBroken = true;
		}
	}

	// Apply status effects
	if (IncomingStatusEffect.Num() > 0)
	{
		ApplyIncomingStatusEffects(IncomingStatusEffect, 1.0);
	}

	// Handle hit reaction
	HandleHitReaction(HitInfo);

	// Check for death
	UObject* HealthStat = nullptr;
	FStatInfo HealthInfo;
	StatManager->GetStat(HealthTag, HealthStat, HealthInfo);

	if (HealthInfo.CurrentValue <= 0 && !IsDead)
	{
		HandleDeath(StatManager, HitInfo);
	}
}

/**
 * GetStaminaDrainAmountForDamage - Calculate stamina cost for blocking damage
 *
 * Blueprint Logic: (MaxStamina * (IncomingDamage / MaxHealth)) * ScalingFactor
 */
double UAC_CombatManager::GetStaminaDrainAmountForDamage_Implementation(double IncomingDamage)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::GetStaminaDrainAmountForDamage - Damage: %f"), IncomingDamage);

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return IncomingDamage;
	}

	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (!IsValid(StatManager))
	{
		return IncomingDamage;
	}

	// Get max stamina
	FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
	UObject* StaminaStat = nullptr;
	FStatInfo StaminaInfo;
	StatManager->GetStat(StaminaTag, StaminaStat, StaminaInfo);

	// Get max health
	FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
	UObject* HealthStat = nullptr;
	FStatInfo HealthInfo;
	StatManager->GetStat(HealthTag, HealthStat, HealthInfo);

	// Calculate: (MaxStamina * (IncomingDamage / MaxHealth)) * 0.5
	double ScalingFactor = 0.5;
	double StaminaDrain = 0.0;

	if (HealthInfo.MaxValue > 0)
	{
		StaminaDrain = (StaminaInfo.MaxValue * (IncomingDamage / HealthInfo.MaxValue)) * ScalingFactor;
	}

	UE_LOG(LogTemp, Log, TEXT("  Calculated stamina drain: %f"), StaminaDrain);
	return StaminaDrain;
}

/**
 * GetKnockbackAmountForDamage - Calculate knockback force based on damage
 *
 * Blueprint Logic: Scale knockback based on damage percentage
 */
FVector UAC_CombatManager::GetKnockbackAmountForDamage_Implementation(const FHitResult& Hit, double Damage, double MaxClamp)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::GetKnockbackAmountForDamage - Damage: %f, MaxClamp: %f"), Damage, MaxClamp);

	// Calculate knockback direction from hit normal
	FVector KnockbackDirection = -Hit.ImpactNormal;
	KnockbackDirection.Z = 0.0f; // Keep horizontal
	KnockbackDirection.Normalize();

	// Scale knockback by damage (clamped) - Blueprint uses 1.0 multiplier, not 2.0
	double KnockbackForce = FMath::Clamp(Damage, 0.0, MaxClamp);

	FVector Knockback = KnockbackDirection * KnockbackForce;

	UE_LOG(LogTemp, Log, TEXT("  Calculated knockback: %s"), *Knockback.ToString());
	return Knockback;
}

/**
 * ProcessHandTrace - Process hits from unarmed attacks
 *
 * Blueprint Logic: Iterate through hits, apply damage to valid targets
 */
void UAC_CombatManager::ProcessHandTrace_Implementation(const TArray<FHitResult>& Hits)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::ProcessHandTrace - %d hits"), Hits.Num());

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();

		// Skip self and already hit actors
		if (!IsValid(HitActor) || HitActor == GetOwner())
		{
			continue;
		}

		// Check if already traced this actor
		if (FistTracedActors.Contains(HitActor))
		{
			continue;
		}

		// Add to traced actors
		FistTracedActors.Add(HitActor);

		// Apply fist damage
		ApplyFistDamage(HitActor, Hit);
	}
}

/**
 * ApplyFistDamage - Apply damage from unarmed attacks
 *
 * Blueprint Logic:
 * 1. Calculate damage and poise damage
 * 2. Apply to target's health and poise
 * 3. Trigger hit reaction
 */
void UAC_CombatManager::ApplyFistDamage_Implementation(AActor* TargetActor, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::ApplyFistDamage - Target: %s"),
		TargetActor ? *TargetActor->GetName() : TEXT("None"));

	if (!IsValid(TargetActor))
	{
		return;
	}

	// Get target's combat manager
	UAC_CombatManager* TargetCombat = TargetActor->FindComponentByClass<UAC_CombatManager>();
	if (!IsValid(TargetCombat))
	{
		return;
	}

	// Calculate unarmed damage (based on stats if available)
	double UnarmedDamage = FMath::RandRange(10.0, 25.0); // Base unarmed damage
	double PoiseDamage = FMath::RandRange(MinUnarmedPoiseDamage, MaxUnarmedPoiseDamage);

	// Create empty status effects map for unarmed attacks
	TMap<FGameplayTag, UPrimaryDataAsset*> EmptyStatusEffects;

	// Apply damage to target
	TargetCombat->HandleIncomingWeaponDamage(
		GetOwner(),
		nullptr, // No guard sound for fist
		nullptr, // No perfect guard sound
		Hit,
		UnarmedDamage,
		PoiseDamage,
		EmptyStatusEffects);

	UE_LOG(LogTemp, Log, TEXT("  Applied fist damage: %f, Poise: %f"), UnarmedDamage, PoiseDamage);
}

/**
 * SetInvincibility - Set invincibility state
 *
 * Blueprint Logic: Simple setter
 */
void UAC_CombatManager::SetInvincibility_Implementation(bool InIsInvincible)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::SetInvincibility - %s"),
		InIsInvincible ? TEXT("true") : TEXT("false"));
	IsInvincible = InIsInvincible;
}

/**
 * IncreaseTimeSinceGuard - Timer callback to track guard duration
 *
 * Blueprint Logic: Increment time counter
 */
void UAC_CombatManager::IncreaseTimeSinceGuard_Implementation()
{
	TimeSinceGuard += 0.016; // ~60 FPS tick
}

/**
 * GetTimeSinceGuardActivation - Get time since guard was activated
 *
 * Blueprint Logic: Simple getter
 */
double UAC_CombatManager::GetTimeSinceGuardActivation_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::GetTimeSinceGuardActivation - %f"), TimeSinceGuard);
	return TimeSinceGuard;
}

/**
 * SetHyperArmor - Set hyper armor state
 *
 * Blueprint Logic: Simple setter
 */
void UAC_CombatManager::SetHyperArmor_Implementation(bool InHyperArmor)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::SetHyperArmor - %s"),
		InHyperArmor ? TEXT("true") : TEXT("false"));
	HyperArmor = InHyperArmor;
}

/**
 * GetIsFacingEnemy - Check if facing the target actor
 *
 * Blueprint Logic: Dot product check between forward vector and direction to target
 */
bool UAC_CombatManager::GetIsFacingEnemy_Implementation(AActor* TargetActor)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::GetIsFacingEnemy"));

	if (!IsValid(TargetActor))
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return false;
	}

	// Get forward vector
	FVector ForwardVector = Owner->GetActorForwardVector();

	// Get direction to target
	FVector DirectionToTarget = (TargetActor->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();

	// Calculate dot product
	double DotProduct = FVector::DotProduct(ForwardVector, DirectionToTarget);

	// Check against threshold based on FacingDirectionAcceptance enum
	double Threshold = 0.5; // Default forward cone

	// Map threshold enum to dot product value
	// Low = generous cone (lower threshold), High = tight cone (higher threshold)
	switch (FacingDirectionAcceptance)
	{
	case ESLFDotProductThreshold::Low:
		Threshold = 0.0; // Very generous, 90 degrees
		break;
	case ESLFDotProductThreshold::Medium:
		Threshold = 0.5; // Medium, ~60 degrees
		break;
	case ESLFDotProductThreshold::High:
		Threshold = 0.7; // Tight, ~45 degrees
		break;
	case ESLFDotProductThreshold::Generous:
		Threshold = -0.5; // Very generous, ~120 degrees
		break;
	default:
		Threshold = 0.5;
		break;
	}

	bool bFacing = DotProduct >= Threshold;

	UE_LOG(LogTemp, Log, TEXT("  Dot: %f, Threshold: %f, Facing: %s"),
		DotProduct, Threshold, bFacing ? TEXT("true") : TEXT("false"));

	return bFacing;
}

/**
 * HandleHitReaction - Play hit reaction animation
 *
 * Blueprint Logic:
 * 1. Calculate hit direction
 * 2. Play appropriate reaction animation
 * 3. Apply camera shake
 */
void UAC_CombatManager::HandleHitReaction_Implementation(const FHitResult& HitInfo)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::HandleHitReaction"));

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Play camera shake
	if (IsValid(GenericReactionShake))
	{
		UGameplayStatics::PlayWorldCameraShake(
			GetWorld(),
			GenericReactionShake,
			Owner->GetActorLocation(),
			500.0f,
			1000.0f);
	}

	// Play hit reaction animation based on direction
	if (IsValid(ReactionAnimset) && IsValid(Mesh))
	{
		UAnimMontage* ReactionMontage = ReactionAnimset->ReactionMontage.LoadSynchronous();
		if (IsValid(ReactionMontage))
		{
			UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				AnimInstance->Montage_Play(ReactionMontage);
				UE_LOG(LogTemp, Log, TEXT("  Playing hit reaction montage: %s"), *ReactionMontage->GetName());
			}
		}
	}
}

/**
 * HandleDeath - Process character death
 *
 * Blueprint Logic:
 * 1. Set IsDead? to true
 * 2. Play death animation or ragdoll
 * 3. Drop currency
 * 4. Disable input
 */
void UAC_CombatManager::HandleDeath_Implementation(UStatManagerComponent* StatManager, const FHitResult& HitInfo)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::HandleDeath"));

	if (IsDead)
	{
		return; // Already dead
	}

	IsDead = true;

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Handle ragdoll if configured
	if (RagdollOnDeath && IsValid(Mesh))
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

		// Apply impulse from hit direction
		FVector Impulse = GetKnockbackAmountForDamage(HitInfo, 500.0, 1000.0);
		Mesh->AddImpulse(Impulse, NAME_None, true);

		UE_LOG(LogTemp, Log, TEXT("  Ragdoll enabled"));
	}

	// Drop currency
	DropCurrency();

	// Calculate hit direction for death animation
	ESLFDirection HitDirection = ESLFDirection::Fwd;
	if (HitInfo.bBlockingHit)
	{
		FVector HitNormal = HitInfo.ImpactNormal;
		FVector OwnerForward = Owner->GetActorForwardVector();
		float DotProduct = FVector::DotProduct(HitNormal, OwnerForward);
		if (DotProduct > 0.5f)
		{
			HitDirection = ESLFDirection::Fwd;
		}
		else if (DotProduct < -0.5f)
		{
			HitDirection = ESLFDirection::Bwd;
		}
	}

	// Trigger death event (handles ragdoll, currency drop, etc.)
	EventOnDeath(RagdollOnDeath, HitDirection);

	// Start respawn sequence (fade out, show "You Died" screen)
	// NOTE: Must be in HandleDeath, NOT EventOnDeath, because EventOnDeath has
	// an early return when IsDead is true, and we set IsDead=true above.
	if (APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			if (PC->Implements<UBPI_Controller>())
			{
				IBPI_Controller::Execute_StartRespawn(PC, 2.0f); // 2 second fade delay
				UE_LOG(LogTemp, Log, TEXT("  StartRespawn called on PlayerController"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  PlayerController does not implement BPI_Controller"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  No PlayerController found"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Character died"));
}

/**
 * ApplyIncomingStatusEffects - Apply status effects from incoming damage
 *
 * Blueprint Logic:
 * 1. Get status effect manager
 * 2. For each effect in map, apply to target
 */
void UAC_CombatManager::ApplyIncomingStatusEffects_Implementation(const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects, double Multiplier)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::ApplyIncomingStatusEffects - %d effects, Multiplier: %f"),
		StatusEffects.Num(), Multiplier);

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Get status effect manager
	UAC_StatusEffectManager* StatusEffectManager = Owner->FindComponentByClass<UAC_StatusEffectManager>();
	if (!IsValid(StatusEffectManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No status effect manager found"));
		return;
	}

	// Apply each status effect
	for (const auto& EffectEntry : StatusEffects)
	{
		FGameplayTag EffectTag = EffectEntry.Key;
		UPrimaryDataAsset* EffectData = EffectEntry.Value;

		if (IsValid(EffectData))
		{
			// Apply one-shot buildup with multiplier
			StatusEffectManager->AddOneShotBuildup(EffectData, 1, Multiplier);
			UE_LOG(LogTemp, Log, TEXT("  Applied status effect: %s with multiplier: %f"), *EffectTag.ToString(), Multiplier);
		}
	}
}

/**
 * DropCurrency - Drop currency on death
 *
 * Blueprint Logic:
 * 1. Get death currency stat
 * 2. Spawn currency pickup at location
 */
void UAC_CombatManager::DropCurrency_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::DropCurrency"));

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Get stat manager for currency amount
	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (!IsValid(StatManager))
	{
		return;
	}

	// Get death currency value (don't error if tag doesn't exist)
	FGameplayTag CurrencyTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Currency.DeathCurrency"), false);
	if (!CurrencyTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("  DeathCurrency tag not found, skipping currency drop"));
		return;
	}
	UObject* CurrencyStat = nullptr;
	FStatInfo CurrencyInfo;
	StatManager->GetStat(CurrencyTag, CurrencyStat, CurrencyInfo);

	double CurrencyAmount = CurrencyInfo.CurrentValue;

	if (CurrencyAmount > 0)
	{
		// Get drop location (check for boss door)
		FVector DropLocation;
		FVector DropLocation2;
		GetBossDoorCurrencyDropLocation(DropLocation, DropLocation2);

		// Spawn currency pickup actor at DropLocation
		UWorld* World = GetWorld();
		if (IsValid(World))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AB_DeathCurrency* CurrencyPickup = World->SpawnActor<AB_DeathCurrency>(
				AB_DeathCurrency::StaticClass(),
				DropLocation,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (IsValid(CurrencyPickup))
			{
				CurrencyPickup->CurrencyAmount = FMath::RoundToInt(CurrencyAmount);
				UE_LOG(LogTemp, Log, TEXT("  Spawned currency pickup with %d at %s"), CurrencyPickup->CurrencyAmount, *DropLocation.ToString());
			}
		}
	}
}

/**
 * GetBossDoorCurrencyDropLocation - Get safe drop location for currency
 *
 * Blueprint Logic: Check if near boss door, return appropriate location
 */
void UAC_CombatManager::GetBossDoorCurrencyDropLocation_Implementation(FVector& OutReturnValue, FVector& OutReturnValue_1)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::GetBossDoorCurrencyDropLocation"));

	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		// Default to owner location
		OutReturnValue = Owner->GetActorLocation();
		OutReturnValue_1 = Owner->GetActorLocation();
	}
	else
	{
		OutReturnValue = FVector::ZeroVector;
		OutReturnValue_1 = FVector::ZeroVector;
	}
}

/**
 * GuardGracePeriod - Start guard grace period timer
 *
 * Blueprint Logic: Set timer that keeps GetIsGuarding returning true briefly after release
 */
void UAC_CombatManager::GuardGracePeriod_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::GuardGracePeriod - Duration: %f"), GuardingGracePeriod);

	if (UWorld* World = GetWorld())
	{
		// Clear any existing timer
		World->GetTimerManager().ClearTimer(GuardGracePeriodTimer);

		// Set new grace period timer
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			UE_LOG(LogTemp, Log, TEXT("  Guard grace period ended"));
		});

		World->GetTimerManager().SetTimer(GuardGracePeriodTimer, TimerDelegate, GuardingGracePeriod, false);
	}
}

// ═══════════════════════════════════════════════════════════════════════
// CUSTOM EVENTS IMPLEMENTATION (11)
// ═══════════════════════════════════════════════════════════════════════

/**
 * PerformHandTrace - Internal helper for sphere trace hand combat
 *
 * Blueprint Logic: Perform sphere trace from hand socket, process hits
 */
void UAC_CombatManager::PerformHandTrace(FName SocketName)
{
	if (!IsValid(Mesh))
	{
		return;
	}

	// Get socket location
	FVector SocketLocation = Mesh->GetSocketLocation(SocketName);

	// Sphere trace parameters
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	TArray<FHitResult> Hits;

	// Perform sphere trace
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		SocketLocation,
		SocketLocation, // Same start/end for overlap check
		25.0f, // Radius for fist
		ObjectTypes,
		false, // bTraceComplex
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hits,
		true // bIgnoreSelf
	);

	if (bHit)
	{
		ProcessHandTrace(Hits);
	}
}

/**
 * TraceLeftHand - Perform left hand trace for unarmed combat
 *
 * Blueprint Logic: Timer callback to trace left hand socket
 */
void UAC_CombatManager::TraceLeftHand()
{
	UE_LOG(LogTemp, Verbose, TEXT("UAC_CombatManager::TraceLeftHand"));
	PerformHandTrace(HandSocket_L);
}

/**
 * TraceRightHand - Perform right hand trace for unarmed combat
 *
 * Blueprint Logic: Timer callback to trace right hand socket
 */
void UAC_CombatManager::TraceRightHand()
{
	UE_LOG(LogTemp, Verbose, TEXT("UAC_CombatManager::TraceRightHand"));
	PerformHandTrace(HandSocket_R);
}

/**
 * EventToggleHandTrace - Toggle hand trace timers for unarmed combat
 *
 * Blueprint Logic:
 * 1. If Enabled, start timer for specified hand(s)
 * 2. If Disabled, stop timers and clear traced actors
 */
void UAC_CombatManager::EventToggleHandTrace(bool bEnabled, ESLFTraceType Type)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::EventToggleHandTrace - Enabled: %s, Type: %d"),
		bEnabled ? TEXT("true") : TEXT("false"), (int32)Type);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (bEnabled)
	{
		// Clear previously traced actors
		FistTracedActors.Empty();

		// Start appropriate timer based on type
		float TraceInterval = 0.016f; // ~60 FPS

		switch (Type)
		{
		case ESLFTraceType::LeftHand:
			World->GetTimerManager().SetTimer(LeftHandTraceTimer, this, &UAC_CombatManager::TraceLeftHand, TraceInterval, true);
			break;
		case ESLFTraceType::RightHand:
			World->GetTimerManager().SetTimer(RightHandTraceTimer, this, &UAC_CombatManager::TraceRightHand, TraceInterval, true);
			break;
		case ESLFTraceType::Both:
			World->GetTimerManager().SetTimer(LeftHandTraceTimer, this, &UAC_CombatManager::TraceLeftHand, TraceInterval, true);
			World->GetTimerManager().SetTimer(RightHandTraceTimer, this, &UAC_CombatManager::TraceRightHand, TraceInterval, true);
			break;
		default:
			break;
		}
	}
	else
	{
		// Stop timers
		World->GetTimerManager().ClearTimer(LeftHandTraceTimer);
		World->GetTimerManager().ClearTimer(RightHandTraceTimer);

		// Clear traced actors
		FistTracedActors.Empty();
	}
}

/**
 * EventResetAttackCombo - Reset combo section to default
 *
 * Blueprint Logic: Set ComboSection back to DefaultComboSequenceSection
 */
void UAC_CombatManager::EventResetAttackCombo()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::EventResetAttackCombo - Resetting to: %s"),
		*DefaultComboSequenceSection.ToString());

	ComboSection = DefaultComboSequenceSection;
}

/**
 * EventBeginSoftCombo - Start a soft combo with async montage load
 *
 * Blueprint Logic:
 * 1. Check if SoftMontage is valid
 * 2. If loaded, play immediately
 * 3. If not loaded, async load and play when ready
 */
void UAC_CombatManager::EventBeginSoftCombo(USkeletalMeshComponent* InMesh, TSoftObjectPtr<UAnimMontage> SoftMontage, double PlayRate)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::EventBeginSoftCombo - PlayRate: %f"), PlayRate);

	if (!IsValid(InMesh))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid mesh"));
		return;
	}

	// Check if soft reference is valid
	if (SoftMontage.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("  Null soft montage reference"));
		return;
	}

	// Check if already loaded
	if (UAnimMontage* LoadedMontage = SoftMontage.Get())
	{
		// Already loaded, play immediately
		EventBeginCombo(InMesh, LoadedMontage, static_cast<float>(PlayRate));
	}
	else
	{
		// Need to async load
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(
			SoftMontage.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this, InMesh, SoftMontage, PlayRate]()
			{
				if (UAnimMontage* LoadedMontage = SoftMontage.Get())
				{
					EventBeginCombo(InMesh, LoadedMontage, static_cast<float>(PlayRate));
				}
			})
		);
	}
}

/**
 * EventRegisterNextCombo - Register the next combo section to play
 *
 * Blueprint Logic: Set ComboSection variable for next attack
 */
void UAC_CombatManager::EventRegisterNextCombo(FName InComboSection)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::EventRegisterNextCombo - Section: %s"),
		*InComboSection.ToString());

	ComboSection = InComboSection;
}

/**
 * EventBeginCombo - Start a combo attack montage
 *
 * Blueprint Logic:
 * 1. Play montage on mesh's anim instance
 * 2. Set up montage callbacks for combo continuation
 * 3. Jump to registered combo section
 */
void UAC_CombatManager::EventBeginCombo(USkeletalMeshComponent* InMesh, UAnimMontage* MontageToPlay, float PlayRate)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::EventBeginCombo - Montage: %s, PlayRate: %f"),
		MontageToPlay ? *MontageToPlay->GetName() : TEXT("None"), PlayRate);

	if (!IsValid(InMesh) || !IsValid(MontageToPlay))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid mesh or montage"));
		return;
	}

	UAnimInstance* AnimInstance = InMesh->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No anim instance"));
		return;
	}

	// Play the montage
	float MontageLength = AnimInstance->Montage_Play(MontageToPlay, PlayRate);

	if (MontageLength > 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("  Montage playing, length: %f"), MontageLength);

		// If we have a registered combo section, jump to it
		if (!ComboSection.IsNone())
		{
			AnimInstance->Montage_JumpToSection(ComboSection, MontageToPlay);
			UE_LOG(LogTemp, Log, TEXT("  Jumped to section: %s"), *ComboSection.ToString());
		}
	}
}

/**
 * EventOnHealthChanged - Handle health stat changes
 *
 * Blueprint Logic:
 * 1. Check if health dropped to or below 0
 * 2. If so, trigger death sequence
 */
void UAC_CombatManager::EventOnHealthChanged(UObject* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::EventOnHealthChanged - Change: %f"), Change);

	// Only care about current value changes for death check
	if (ValueType != ESLFValueType::CurrentValue)
	{
		return;
	}

	// Negative change means damage
	if (Change < 0 && !IsDead)
	{
		AActor* Owner = GetOwner();
		if (!IsValid(Owner))
		{
			return;
		}

		// Get current health
		UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
		if (IsValid(StatManager))
		{
			FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			UObject* HealthStat = nullptr;
			FStatInfo HealthInfo;
			StatManager->GetStat(HealthTag, HealthStat, HealthInfo);

			if (HealthInfo.CurrentValue <= 0)
			{
				// Trigger death
				EventOnDeath(RagdollOnDeath, ESLFDirection::Fwd);
			}
		}
	}
}

/**
 * EventOnAnyDamage - Handle Unreal's standard any damage event
 *
 * Blueprint Logic: Process damage from ApplyDamage function calls
 */
void UAC_CombatManager::EventOnAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::EventOnAnyDamage - Damage: %f, Causer: %s"),
		Damage, DamageCauser ? *DamageCauser->GetName() : TEXT("None"));

	// This is a passthrough for standard UE damage system
	// The framework primarily uses HandleIncomingWeaponDamage for custom damage
	// This event can be used for environmental damage, fall damage, etc.

	if (IsInvincible)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (IsValid(StatManager))
	{
		FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
		StatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -static_cast<double>(Damage), false, true);
	}
}

/**
 * EventOnPoiseChanged - Handle poise stat changes
 *
 * Blueprint Logic:
 * 1. Check if poise dropped to or below 0
 * 2. If so, set PoiseBroken to true
 * 3. Trigger stagger animation
 */
void UAC_CombatManager::EventOnPoiseChanged(UObject* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::EventOnPoiseChanged - Change: %f"), Change);

	// Only care about current value changes
	if (ValueType != ESLFValueType::CurrentValue)
	{
		return;
	}

	// Negative change means poise damage
	if (Change < 0)
	{
		AActor* Owner = GetOwner();
		if (!IsValid(Owner))
		{
			return;
		}

		// Get current poise
		UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
		if (IsValid(StatManager))
		{
			FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
			UObject* PoiseStat = nullptr;
			FStatInfo PoiseInfo;
			StatManager->GetStat(PoiseTag, PoiseStat, PoiseInfo);

			if (PoiseInfo.CurrentValue <= 0 && !PoiseBroken)
			{
				// Poise broken!
				PoiseBroken = true;
				UE_LOG(LogTemp, Log, TEXT("  POISE BROKEN!"));

				// Play poise break animation using reaction animset as fallback
				// (Player typically uses hit reaction for poise break stagger)
				if (IsValid(ReactionAnimset) && IsValid(Mesh))
				{
					UAnimMontage* ReactionMontage = ReactionAnimset->ReactionMontage.LoadSynchronous();
					if (IsValid(ReactionMontage))
					{
						UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
						if (IsValid(AnimInstance))
						{
							AnimInstance->Montage_Play(ReactionMontage, 1.0f);
							UE_LOG(LogTemp, Log, TEXT("  Playing poise break reaction"));
						}
					}
				}
			}
		}
	}
}

/**
 * EventOnDeath - Handle character death
 *
 * Blueprint Logic:
 * 1. Set IsDead to true
 * 2. If Ragdoll, enable physics simulation
 * 3. Apply death impulse based on direction
 * 4. Drop currency
 */
void UAC_CombatManager::EventOnDeath(bool bRagdoll, ESLFDirection KillingBlowDirection)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::EventOnDeath - Ragdoll: %s, Direction: %d"),
		bRagdoll ? TEXT("true") : TEXT("false"), (int32)KillingBlowDirection);

	if (IsDead)
	{
		return; // Already dead
	}

	IsDead = true;

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Handle ragdoll
	if (bRagdoll && IsValid(Mesh))
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

		// Calculate impulse direction based on KillingBlowDirection
		FVector ImpulseDirection = FVector::ZeroVector;
		FVector OwnerForward = Owner->GetActorForwardVector();
		FVector OwnerRight = Owner->GetActorRightVector();

		switch (KillingBlowDirection)
		{
		case ESLFDirection::Fwd:
			ImpulseDirection = -OwnerForward;
			break;
		case ESLFDirection::Bwd:
			ImpulseDirection = OwnerForward;
			break;
		case ESLFDirection::Left:
			ImpulseDirection = OwnerRight;
			break;
		case ESLFDirection::Right:
			ImpulseDirection = -OwnerRight;
			break;
		default:
			ImpulseDirection = -OwnerForward;
			break;
		}

		// Apply impulse
		FVector DeathImpulse = ImpulseDirection * 500.0f;
		DeathImpulse.Z = 200.0f; // Add upward component
		Mesh->AddImpulse(DeathImpulse, NAME_None, true);

		UE_LOG(LogTemp, Log, TEXT("  Ragdoll enabled, impulse: %s"), *DeathImpulse.ToString());
	}

	// Drop currency
	DropCurrency();

	// Start respawn sequence (fade out, show "You Died" screen)
	if (APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			if (PC->Implements<UBPI_Controller>())
			{
				IBPI_Controller::Execute_StartRespawn(PC, 2.0); // 2 second fade delay
				UE_LOG(LogTemp, Log, TEXT("  StartRespawn called on PlayerController"));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Character died"));
}
