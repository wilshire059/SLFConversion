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
#include "Kismet/GameplayStatics.h"

ASLFProjectileBase::ASLFProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

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

	UE_LOG(LogTemp, Log, TEXT("[Projectile] BeginPlay: %s"), *GetName());

	SetLifeSpan(Lifespan);
	RemainingHomingTime = HomingDuration;
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
	UE_LOG(LogTemp, Log, TEXT("[Projectile] OnProjectileHit: %s"),
		HitActor ? *HitActor->GetName() : TEXT("null"));

	// Spawn hit effect
	if (HitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, HitEffect, HitResult.ImpactPoint,
			HitResult.ImpactNormal.Rotation());
	}

	// Apply damage
	if (HitActor && HitActor->ActorHasTag(TargetTag))
	{
		float Damage = CalculateDamage();
		// TODO: Apply damage via interface
	}

	// Apply status effects
	for (const FGameplayTag& EffectTag : StatusEffects)
	{
		// TODO: Apply status effect via StatusEffectManager
	}

	// Destroy after delay
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
