// SLFProjectileBase.cpp
// C++ implementation for B_BaseProjectile
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_BaseProjectile
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         12/12 (10 config + 2 runtime, initialized in constructor)
// Functions:         5/5 implemented
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Components/AC_AI_CombatManager.h"
#include "Components/AC_CombatManager.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/CombatManagerComponent.h"

ASLFProjectileBase::ASLFProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	// Create trigger sphere for collision detection
	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(DefaultSceneRoot);
	Trigger->SetSphereRadius(32.0f);
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Trigger->SetGenerateOverlapEvents(true);

	// Create projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = DefaultSceneRoot;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Create Niagara effect component (trail VFX - asset set in Blueprint child)
	Effect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));
	Effect->SetupAttachment(DefaultSceneRoot);
	Effect->bAutoActivate = false;  // Activate in BeginPlay

	// Initialize targeting
	TargetTag = FName("Enemy");

	// Initialize damage
	MinDamage = 10.0f;
	MaxDamage = 20.0f;

	// Initialize homing
	bIsHoming = false;
	HomingDuration = 2.0f;

	// Initialize lifetime
	Lifespan = 5.0f;
	DestroyDelay = 0.5f;

	// Initialize effects
	HitEffect = nullptr;

	// Initialize runtime
	HomingTarget = nullptr;
	RemainingHomingTime = 0.0f;
}

void ASLFProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] BeginPlay: %s"), *GetName());

	// Setup lifespan
	SetLifeSpan(Lifespan);
	RemainingHomingTime = HomingDuration;

	// Bind trigger overlap event
	if (Trigger)
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &ASLFProjectileBase::OnTriggerOverlap);
		UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] Bound trigger overlap (radius=%.1f)"),
			Trigger->GetScaledSphereRadius());
	}

	// Set initial velocity in forward direction
	if (ProjectileMovement)
	{
		// Apply speed from Blueprint if set, otherwise use constructor defaults
		if (ProjectileSpeed > 0.0f)
		{
			ProjectileMovement->InitialSpeed = ProjectileSpeed;
			ProjectileMovement->MaxSpeed = ProjectileSpeed;
		}

		// Set velocity in actor's forward direction
		ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;

		UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] ProjectileMovement: Speed=%.1f, Velocity=%s"),
			ProjectileMovement->InitialSpeed, *ProjectileMovement->Velocity.ToString());
	}

	// Activate Niagara trail effect
	if (Effect && Effect->GetAsset())
	{
		Effect->Activate(true);
		UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] Activated Niagara effect: %s"),
			*Effect->GetAsset()->GetName());
	}

	UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] Components: Root=%s, Movement=%s, Trigger=%s, Effect=%s"),
		DefaultSceneRoot ? TEXT("YES") : TEXT("NO"),
		ProjectileMovement ? TEXT("YES") : TEXT("NO"),
		Trigger ? TEXT("YES") : TEXT("NO"),
		Effect ? TEXT("YES") : TEXT("NO"));
}

void ASLFProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle homing
	if (bIsHoming && HomingTarget && RemainingHomingTime > 0.0f)
	{
		RemainingHomingTime -= DeltaTime;

		FVector TargetDir = (HomingTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector CurrentVelocity = GetVelocity();
		float Speed = CurrentVelocity.Size();

		FVector NewVelocity = FMath::VInterpTo(CurrentVelocity.GetSafeNormal(), TargetDir, DeltaTime, 5.0f) * Speed;

		if (UProjectileMovementComponent* ProjMove = FindComponentByClass<UProjectileMovementComponent>())
		{
			ProjMove->Velocity = NewVelocity;
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// SETUP [1/5]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFProjectileBase::SetupProjectile_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[Projectile] SetupProjectile"));
	// Override in child classes for specific setup
}

// ═══════════════════════════════════════════════════════════════════════════════
// LIFECYCLE [2-4/5]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFProjectileBase::OnProjectileHit_Implementation(AActor* HitActor, const FHitResult& HitResult)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] OnProjectileHit: %s"),
		HitActor ? *HitActor->GetName() : TEXT("null"));

	if (!HitActor)
	{
		return;
	}

	// Calculate damage
	float Damage = CalculateDamage();

	// Check if target has "Enemy" tag - apply damage via combat manager
	if (HitActor->ActorHasTag(FName("Enemy")))
	{
		bool bDamageApplied = false;

		// Try UAICombatManagerComponent first (native C++ enemies)
		UAICombatManagerComponent* AICombatComp = HitActor->FindComponentByClass<UAICombatManagerComponent>();
		if (AICombatComp)
		{
			UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] Applying %.1f damage to Enemy via UAICombatManagerComponent"), Damage);
			AICombatComp->HandleProjectileDamage_AI(this, Damage, 0.0f, HitResult);
			bDamageApplied = true;
		}

		// Fallback to UAC_AI_CombatManager (Blueprint component)
		if (!bDamageApplied)
		{
			UAC_AI_CombatManager* AICombatManager = HitActor->FindComponentByClass<UAC_AI_CombatManager>();
			if (AICombatManager)
			{
				UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] Applying %.1f damage to Enemy via UAC_AI_CombatManager"), Damage);

				TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffectsMap;
				for (const FGameplayTag& EffectTag : StatusEffects)
				{
					StatusEffectsMap.Add(EffectTag, nullptr);
				}

				AICombatManager->HandleProjectileDamage_AI(Damage, HitEffect, NegationStat, StatusEffectsMap);
				bDamageApplied = true;
			}
		}

		if (bDamageApplied)
		{
			// Spawn hit effect at impact point
			if (HitEffect)
			{
				FVector SpawnLocation = HitResult.ImpactPoint.IsZero() ? HitActor->GetActorLocation() : FVector(HitResult.ImpactPoint);
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, SpawnLocation);
			}

			// Destroy projectile after hit
			SetLifeSpan(DestroyDelay);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFProjectileBase] Enemy has no combat manager component!"));
		}
	}

	// Check if target has "Player" tag - apply damage via combat manager
	if (HitActor->ActorHasTag(FName("Player")))
	{
		bool bDamageApplied = false;

		// Try UCombatManagerComponent first (native C++ player)
		UCombatManagerComponent* CombatComp = HitActor->FindComponentByClass<UCombatManagerComponent>();
		if (CombatComp)
		{
			UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] Applying %.1f damage to Player via UCombatManagerComponent"), Damage);
			CombatComp->HandleProjectileDamage(this, Damage, 0.0f, HitResult);
			bDamageApplied = true;
		}

		// Fallback to UAC_CombatManager (Blueprint component)
		if (!bDamageApplied)
		{
			UAC_CombatManager* CombatManager = HitActor->FindComponentByClass<UAC_CombatManager>();
			if (CombatManager)
			{
				UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] Applying %.1f damage to Player via UAC_CombatManager"), Damage);

				TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffectsMap;
				for (const FGameplayTag& EffectTag : StatusEffects)
				{
					StatusEffectsMap.Add(EffectTag, nullptr);
				}

				CombatManager->HandleProjectileDamage(Damage, HitEffect, NegationStat, StatusEffectsMap);
				bDamageApplied = true;
			}
		}

		if (bDamageApplied)
		{
			if (HitEffect)
			{
				FVector SpawnLocation = HitResult.ImpactPoint.IsZero() ? HitActor->GetActorLocation() : FVector(HitResult.ImpactPoint);
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, SpawnLocation);
			}

			SetLifeSpan(DestroyDelay);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFProjectileBase] Player has no combat manager component!"));
		}
	}

	// Hit something else (world geometry, etc.) - just spawn effect and destroy
	UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] Hit non-target: %s (no Enemy/Player tag)"), *HitActor->GetName());

	if (HitEffect)
	{
		FVector SpawnLocation = HitResult.ImpactPoint.IsZero() ? GetActorLocation() : FVector(HitResult.ImpactPoint);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, SpawnLocation);
	}

	SetLifeSpan(DestroyDelay);
}

void ASLFProjectileBase::Launch_Implementation(FVector Direction, float Speed)
{
	UE_LOG(LogTemp, Log, TEXT("[Projectile] Launch: Speed %.2f"), Speed);

	if (UProjectileMovementComponent* ProjMove = FindComponentByClass<UProjectileMovementComponent>())
	{
		ProjMove->Velocity = Direction.GetSafeNormal() * Speed;
	}
}

void ASLFProjectileBase::SetHomingTarget_Implementation(AActor* Target)
{
	HomingTarget = Target;
	RemainingHomingTime = HomingDuration;

	UE_LOG(LogTemp, Log, TEXT("[Projectile] SetHomingTarget: %s"),
		Target ? *Target->GetName() : TEXT("null"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// DAMAGE [5/5]
// ═══════════════════════════════════════════════════════════════════════════════

float ASLFProjectileBase::CalculateDamage_Implementation()
{
	return FMath::RandRange(MinDamage, MaxDamage);
}

void ASLFProjectileBase::OnTriggerOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Ignore self
	if (OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[SLFProjectileBase] OnTriggerOverlap: Hit %s"),
		OtherActor ? *OtherActor->GetName() : TEXT("null"));

	// Call the projectile hit handler
	OnProjectileHit(OtherActor, SweepResult);
}
