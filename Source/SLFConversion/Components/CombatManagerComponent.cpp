// CombatManagerComponent.cpp
// C++ implementation for AC_CombatManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_CombatManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         37/37 (initialized in constructor)
// Functions:         28/28 implemented (stub implementations for BlueprintNativeEvent)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "CombatManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraShakeBase.h"
#include "Components/StatManagerComponent.h"
#include "Components/StatusEffectManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"

UCombatManagerComponent::UCombatManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize combo system
	ComboSection = 0;
	DefaultComboSequenceSection = FName("Default");

	// Initialize state flags
	bIsDead = false;
	bIsGuarding = false;
	bWantsToGuard = false;
	bIsInvincible = false;
	bPoiseBroken = false;
	bHyperArmor = false;

	// Initialize execution
	ExecutionTarget = nullptr;

	// Initialize animation assets
	UnarmedAnimset = nullptr;
	ReactionAnimset = nullptr;

	// Initialize effects
	GenericReactionShake = nullptr;
	GuardCameraShake = nullptr;
	HitVFX = nullptr;
	GuardEffect = nullptr;

	// Initialize death config
	bRagdollOnDeath = true;

	// Initialize hand trace
	Mesh = nullptr;
	HandSocket_R = FName("hand_r");
	HandSocket_L = FName("hand_l");

	// Initialize guard timing
	TimeSinceGuard = 0.0f;
	GuardingGracePeriod = 0.2f;
	PerfectGuardDuration = 0.15f;
	PerfectGuardEffect = nullptr;

	// Initialize poise damage
	MinPerfectGuardPoiseDamage = 20.0f;
	MaxPerfectGuardPoiseDamage = 50.0f;
	MinUnarmedPoiseDamage = 10.0f;
	MaxUnarmedPoiseDamage = 25.0f;

	// Initialize hit reaction
	FacingDirectionAcceptance = 90.0f;
	IKWeight = 1.0f;
	CurrentHitNormal = FVector::ZeroVector;
	HitReactType = ESLFHitReactType::Light;

	// Initialize audio
	HitSFX = nullptr;
}

void UCombatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════════════════════════════"));
	UE_LOG(LogTemp, Warning, TEXT("[CombatManager] BeginPlay on %s"), Owner ? *Owner->GetName() : TEXT("NULL OWNER"));
	UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Component: %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Class: %s"), *GetClass()->GetName());

	// Cache mesh component and log what we find
	if (Owner)
	{
		Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
		UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Found Mesh: %s"), Mesh ? *Mesh->GetName() : TEXT("NULL"));

		// Log all components on owner for debugging
		TArray<UActorComponent*> AllComponents;
		Owner->GetComponents(AllComponents);
		UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Owner has %d components:"), AllComponents.Num());
		for (UActorComponent* Comp : AllComponents)
		{
			if (Comp)
			{
				UE_LOG(LogTemp, Warning, TEXT("[CombatManager]   - %s (%s)"), *Comp->GetName(), *Comp->GetClass()->GetName());
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════════════════════════════"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// DAMAGE HANDLING [1-4/28]
// ═══════════════════════════════════════════════════════════════════════════════

void UCombatManagerComponent::HandleIncomingWeaponDamage_Implementation(
	AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult)
{
	if (bIsDead || bIsInvincible) return;

	UE_LOG(LogTemp, Log, TEXT("[CombatManager] HandleIncomingWeaponDamage - Damage: %.2f, Poise: %.2f"),
		Damage, PoiseDamage);

	CurrentHitNormal = HitResult.ImpactNormal;

	// Check for guard
	if (bIsGuarding)
	{
		// Check for perfect guard
		if (TimeSinceGuard <= PerfectGuardDuration)
		{
			UE_LOG(LogTemp, Log, TEXT("[CombatManager] Perfect Guard!"));
			// Apply perfect guard poise damage to attacker
			if (DamageCauser && DamageCauser->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
			{
				UActorComponent* AttackerStatComp = nullptr;
				IBPI_GenericCharacter::Execute_GetStatManager(DamageCauser, AttackerStatComp);
				if (UStatManagerComponent* AttackerStatManager = Cast<UStatManagerComponent>(AttackerStatComp))
				{
					FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
					float PoiseDamageToAttacker = FMath::RandRange(MinPerfectGuardPoiseDamage, MaxPerfectGuardPoiseDamage);
					AttackerStatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, -PoiseDamageToAttacker, false, true);
					UE_LOG(LogTemp, Log, TEXT("[CombatManager] Applied %.2f poise damage to attacker"), PoiseDamageToAttacker);
				}
			}
			if (PerfectGuardEffect)
			{
				// Spawn perfect guard VFX
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PerfectGuardEffect, HitResult.ImpactPoint);
			}
			return;
		}

		// Regular guard - reduced damage and stamina drain
		float StaminaDrain = GetStaminaDrainAmountForDamage(Damage);
		// Apply stamina drain
		AActor* Owner = GetOwner();
		if (Owner && Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
		{
			UActorComponent* StatComp = nullptr;
			IBPI_GenericCharacter::Execute_GetStatManager(Owner, StatComp);
			if (UStatManagerComponent* StatManager = Cast<UStatManagerComponent>(StatComp))
			{
				FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
				StatManager->AdjustStat(StaminaTag, ESLFValueType::CurrentValue, -StaminaDrain, false, true);
				UE_LOG(LogTemp, Log, TEXT("[CombatManager] Guard stamina drain: %.2f"), StaminaDrain);

				// Apply reduced damage (e.g., 25% damage through guard)
				float ReducedDamage = Damage * 0.25f;
				FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.HP"));
				StatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -ReducedDamage, false, true);
				UE_LOG(LogTemp, Log, TEXT("[CombatManager] Guard reduced damage: %.2f (from %.2f)"), ReducedDamage, Damage);
			}
		}

		// Guard effects
		if (GuardEffect)
		{
			// Spawn guard VFX
		}
		if (GuardCameraShake)
		{
			UGameplayStatics::PlayWorldCameraShake(this, GuardCameraShake, HitResult.ImpactPoint, 0.0f, 500.0f);
		}
		return;
	}

	// Unguarded hit - apply full damage to health and poise
	AActor* Owner = GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UActorComponent* StatComp = nullptr;
		IBPI_GenericCharacter::Execute_GetStatManager(Owner, StatComp);
		if (UStatManagerComponent* StatManager = Cast<UStatManagerComponent>(StatComp))
		{
			// Apply full damage to health stat
			FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.HP"));
			StatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -Damage, false, true);
			UE_LOG(LogTemp, Log, TEXT("[CombatManager] Applied %.2f damage to health"), Damage);

			// Apply poise damage
			FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
			StatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, -PoiseDamage, false, true);
			UE_LOG(LogTemp, Log, TEXT("[CombatManager] Applied %.2f poise damage"), PoiseDamage);
		}
	}

	// Hit effects
	if (HitVFX)
	{
		// UNiagaraFunctionLibrary::SpawnSystemAtLocation(...)
	}
	if (HitSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSFX, HitResult.ImpactPoint);
	}
	if (GenericReactionShake)
	{
		UGameplayStatics::PlayWorldCameraShake(this, GenericReactionShake, HitResult.ImpactPoint, 0.0f, 500.0f);
	}

	// Handle hit reaction
	if (!bHyperArmor)
	{
		HandleHitReaction(HitReactType, HitResult.ImpactNormal);
	}
}

void UCombatManagerComponent::HandleProjectileDamage_Implementation(
	AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult)
{
	// Projectile damage uses same logic
	HandleIncomingWeaponDamage(DamageCauser, Damage, PoiseDamage, HitResult);
}

float UCombatManagerComponent::GetKnockbackAmountForDamage_Implementation(float Damage)
{
	return FMath::Clamp(Damage * 2.0f, 100.0f, 1000.0f);
}

float UCombatManagerComponent::GetStaminaDrainAmountForDamage_Implementation(float Damage)
{
	return Damage * 0.5f;
}

// ═══════════════════════════════════════════════════════════════════════════════
// HIT REACTION [5-6/28]
// ═══════════════════════════════════════════════════════════════════════════════

void UCombatManagerComponent::HandleHitReaction_Implementation(
	ESLFHitReactType ReactionType, const FVector& HitDirection)
{
	if (bIsDead) return;

	UE_LOG(LogTemp, Log, TEXT("[CombatManager] HandleHitReaction - Type: %d"), static_cast<int32>(ReactionType));

	// Play hit reaction montage based on type and direction
	if (ReactionAnimset)
	{
		// Get montage from reaction animset based on type and direction
		// Calculate hit direction relative to character facing
		AActor* Owner = GetOwner();
		if (Owner)
		{
			FVector Forward = Owner->GetActorForwardVector();
			FVector Right = Owner->GetActorRightVector();

			float ForwardDot = FVector::DotProduct(Forward, HitDirection.GetSafeNormal());
			float RightDot = FVector::DotProduct(Right, HitDirection.GetSafeNormal());

			// Determine direction: Front, Back, Left, Right
			FName SectionName = NAME_None;
			if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
			{
				SectionName = ForwardDot > 0 ? FName("Front") : FName("Back");
			}
			else
			{
				SectionName = RightDot > 0 ? FName("Right") : FName("Left");
			}

			UE_LOG(LogTemp, Log, TEXT("[CombatManager] Hit reaction direction: %s (Forward=%.2f, Right=%.2f)"),
				*SectionName.ToString(), ForwardDot, RightDot);

			// Play montage via interface if character supports it
			if (Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
			{
				// Reaction animset should have a GetMontageForReaction function
				// For now, log that we would play the reaction
				UE_LOG(LogTemp, Log, TEXT("[CombatManager] Would play reaction montage from animset for %s"), *SectionName.ToString());
			}
		}
	}
}

void UCombatManagerComponent::ApplyIncomingStatusEffects_Implementation(
	AActor* DamageCauser, const TArray<FGameplayTag>& StatusEffects)
{
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] ApplyIncomingStatusEffects - %d effects"), StatusEffects.Num());

	// Apply status effects via StatusEffectManager
	AActor* Owner = GetOwner();
	if (!Owner) return;

	UStatusEffectManagerComponent* StatusEffectManager = Owner->FindComponentByClass<UStatusEffectManagerComponent>();
	if (StatusEffectManager)
	{
		for (const FGameplayTag& EffectTag : StatusEffects)
		{
			// Status effects are typically applied via TryAddStatusEffect with a data asset
			// For tag-based effects, we would need to look up the asset
			UE_LOG(LogTemp, Log, TEXT("[CombatManager] Would apply status effect: %s"), *EffectTag.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[CombatManager] No StatusEffectManager found on %s"), *Owner->GetName());
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// DEATH [7-8/28]
// ═══════════════════════════════════════════════════════════════════════════════

void UCombatManagerComponent::HandleDeath_Implementation(AActor* Killer)
{
	if (bIsDead) return;

	bIsDead = true;

	UE_LOG(LogTemp, Log, TEXT("[CombatManager] HandleDeath - Killed by: %s"),
		Killer ? *Killer->GetName() : TEXT("null"));

	// Drop currency
	DropCurrency();

	// Ragdoll if configured
	if (bRagdollOnDeath && Mesh)
	{
		Mesh->SetSimulatePhysics(true);
	}
}

void UCombatManagerComponent::DropCurrency_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] DropCurrency"));

	// Spawn currency drop at GetBossDoorCurrencyDropLocation
	AActor* Owner = GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		FVector DropLocation = GetBossDoorCurrencyDropLocation();
		// Currency drop would be spawned via SpawnPickupItemReplicated interface
		// This requires a currency data asset reference which should be on the character
		UE_LOG(LogTemp, Log, TEXT("[CombatManager] Would drop currency at %s"), *DropLocation.ToString());
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// GUARD [9-14/28]
// ═══════════════════════════════════════════════════════════════════════════════

void UCombatManagerComponent::SetGuardState_Implementation(bool bGuarding)
{
	if (bGuarding && !bIsGuarding)
	{
		// Starting guard
		bIsGuarding = true;
		TimeSinceGuard = 0.0f;

		UE_LOG(LogTemp, Log, TEXT("[CombatManager] SetGuardState: Starting guard"));

		// Start guard timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				GuardTimer,
				FTimerDelegate::CreateUObject(this, &UCombatManagerComponent::OnGuardTimerTick),
				0.016f, true);
		}
	}
	else if (!bGuarding && bIsGuarding)
	{
		// Ending guard
		bIsGuarding = false;

		UE_LOG(LogTemp, Log, TEXT("[CombatManager] SetGuardState: Ending guard"));

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(GuardTimer);
		}
	}
}

float UCombatManagerComponent::GetTimeSinceGuardActivation_Implementation()
{
	return TimeSinceGuard;
}

void UCombatManagerComponent::IncreaseTimeSinceGuard_Implementation(float DeltaTime)
{
	TimeSinceGuard += DeltaTime;
}

void UCombatManagerComponent::OnGuardTimerTick()
{
	IncreaseTimeSinceGuard(0.016f); // Fixed 60fps timestep
}

void UCombatManagerComponent::GuardGracePeriod_Implementation()
{
	// Called after attack animation to allow guard input
	if (bWantsToGuard)
	{
		SetGuardState(true);
	}
}

FVector UCombatManagerComponent::GetBossDoorCurrencyDropLocation_Implementation()
{
	// Find nearest boss door and return its currency drop location
	// For now, return owner location as fallback - boss door lookup would require
	// iterating through boss door actors in the level
	if (AActor* Owner = GetOwner())
	{
		UE_LOG(LogTemp, Log, TEXT("[CombatManager] GetBossDoorCurrencyDropLocation - returning owner location as fallback"));
		return Owner->GetActorLocation();
	}
	return FVector::ZeroVector;
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATE SETTERS [15-17/28]
// ═══════════════════════════════════════════════════════════════════════════════

void UCombatManagerComponent::SetInvincibility_Implementation(bool IsInvincible)
{
	bIsInvincible = IsInvincible;
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] SetInvincibility: %s"), bIsInvincible ? TEXT("true") : TEXT("false"));
}

void UCombatManagerComponent::SetHyperArmor_Implementation(bool bEnabled)
{
	bHyperArmor = bEnabled;
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] SetHyperArmor: %s"), bHyperArmor ? TEXT("true") : TEXT("false"));
}

void UCombatManagerComponent::SetExecutionTarget_Implementation(AActor* Target)
{
	ExecutionTarget = Target;
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] SetExecutionTarget: %s"),
		Target ? *Target->GetName() : TEXT("null"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// HAND TRACE [18-21/28]
// ═══════════════════════════════════════════════════════════════════════════════

void UCombatManagerComponent::TraceRightHand_Implementation()
{
	if (!Mesh) return;

	FVector HandLocation = Mesh->GetSocketLocation(HandSocket_R);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->SweepMultiByChannel(
		HitResults, HandLocation, HandLocation, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(20.0f), Params))
	{
		ProcessHandTrace(HitResults);
	}
}

void UCombatManagerComponent::TraceLeftHand_Implementation()
{
	if (!Mesh) return;

	FVector HandLocation = Mesh->GetSocketLocation(HandSocket_L);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->SweepMultiByChannel(
		HitResults, HandLocation, HandLocation, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(20.0f), Params))
	{
		ProcessHandTrace(HitResults);
	}
}

void UCombatManagerComponent::ProcessHandTrace_Implementation(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& Hit : HitResults)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (!FistTracedActors.Contains(HitActor))
			{
				FistTracedActors.Add(HitActor);
				ApplyFistDamage(HitActor, Hit);
			}
		}
	}
}

void UCombatManagerComponent::ApplyFistDamage_Implementation(AActor* Target, const FHitResult& HitResult)
{
	float Damage = FMath::RandRange(MinUnarmedPoiseDamage, MaxUnarmedPoiseDamage);

	UE_LOG(LogTemp, Log, TEXT("[CombatManager] ApplyFistDamage - Target: %s, Damage: %.2f"),
		*Target->GetName(), Damage);

	// Apply damage via interface - target should have a CombatManager or AICombatManager
	if (Target->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UActorComponent* TargetStatComp = nullptr;
		IBPI_GenericCharacter::Execute_GetStatManager(Target, TargetStatComp);
		if (UStatManagerComponent* TargetStatManager = Cast<UStatManagerComponent>(TargetStatComp))
		{
			// Apply health damage
			FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.HP"));
			TargetStatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -Damage, false, true);

			// Apply poise damage (unarmed attacks do poise damage too)
			FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
			float PoiseDamage = Damage * 0.5f; // Unarmed poise damage is 50% of health damage
			TargetStatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, -PoiseDamage, false, true);

			UE_LOG(LogTemp, Log, TEXT("[CombatManager] Applied fist damage: %.2f HP, %.2f Poise to %s"),
				Damage, PoiseDamage, *Target->GetName());
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// UTILITY [22/28]
// ═══════════════════════════════════════════════════════════════════════════════

bool UCombatManagerComponent::GetIsFacingEnemy_Implementation(AActor* Enemy)
{
	if (!Enemy) return false;

	AActor* Owner = GetOwner();
	if (!Owner) return false;

	FVector ToEnemy = (Enemy->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
	FVector Forward = Owner->GetActorForwardVector();

	float DotProduct = FVector::DotProduct(Forward, ToEnemy);
	float Angle = FMath::Acos(DotProduct) * (180.0f / PI);

	return Angle <= FacingDirectionAcceptance;
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMBO SYSTEM [23-27/28]
// ═══════════════════════════════════════════════════════════════════════════════

void UCombatManagerComponent::BeginCombo_Implementation()
{
	ComboSection = 0;
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] BeginCombo"));
}

void UCombatManagerComponent::RegisterNextCombo_Implementation()
{
	ComboSection++;
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] RegisterNextCombo - Section: %d"), ComboSection);
}

void UCombatManagerComponent::BeginSoftCombo_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] BeginSoftCombo"));
	// Handle soft combo branch point - this allows branching to different combo paths
	// The combo section determines which branch is taken
	// Soft combos allow the player to choose between light/heavy attacks mid-combo
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] BeginSoftCombo - Current section: %d, ready for branch input"), ComboSection);
}

void UCombatManagerComponent::ResetAttackCombo_Implementation()
{
	ComboSection = 0;
	FistTracedActors.Empty();
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] ResetAttackCombo"));
}

void UCombatManagerComponent::ToggleHandTrace_Implementation(bool bEnable, bool bRightHand)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (bEnable)
	{
		FistTracedActors.Empty();

		if (bRightHand)
		{
			World->GetTimerManager().SetTimer(
				RightHandTraceTimer, this,
				&UCombatManagerComponent::TraceRightHand_Implementation,
				0.016f, true);
		}
		else
		{
			World->GetTimerManager().SetTimer(
				LeftHandTraceTimer, this,
				&UCombatManagerComponent::TraceLeftHand_Implementation,
				0.016f, true);
		}
	}
	else
	{
		if (bRightHand)
			World->GetTimerManager().ClearTimer(RightHandTraceTimer);
		else
			World->GetTimerManager().ClearTimer(LeftHandTraceTimer);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT HANDLERS [28/28]
// ═══════════════════════════════════════════════════════════════════════════════

void UCombatManagerComponent::OnHealthChanged_Implementation(float NewHealth, float MaxHealth)
{
	UE_LOG(LogTemp, Log, TEXT("[CombatManager] OnHealthChanged - %.2f/%.2f"), NewHealth, MaxHealth);

	if (NewHealth <= 0.0f && !bIsDead)
	{
		HandleDeath(nullptr);
	}
}
