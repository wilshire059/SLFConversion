// B_BaseProjectile.cpp
// C++ implementation for Blueprint B_BaseProjectile
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Updated 2026-01-28: Fixed component handling - Blueprint SCS owns components
// Source: BlueprintDNA/Blueprint/B_BaseProjectile.json
//
// COMPONENT OWNERSHIP: Blueprint SCS owns all components.
// C++ only caches references at runtime. See CLAUDE.md for pattern.

#include "Blueprints/B_BaseProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AB_BaseProjectile::AB_BaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Components are created by Blueprint SCS, not C++
	// We cache references in BeginPlay
	DefaultSceneRoot = nullptr;
	ProjectileMovement = nullptr;
	Effect = nullptr;
	Trigger = nullptr;

	// Initialize variables with defaults
	TargetTag = FName("Enemy");
	MinDamage = 10.0;
	MaxDamage = 20.0;
	IsHoming = false;
	HomingDuration = 3.0;
	Lifespan = 5.0;
	DestroyDelay = 0.5;
}

void AB_BaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Cache component references from Blueprint SCS
	CacheComponentReferences();

	// Set actor lifespan (auto-destroy after this time)
	SetLifeSpan(Lifespan);

	// Bind overlap event for the trigger sphere
	if (Trigger)
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &AB_BaseProjectile::OnTriggerOverlap);
	}

	// Configure projectile movement
	if (ProjectileMovement)
	{
		// Configure homing if enabled
		if (IsHoming)
		{
			ProjectileMovement->bIsHomingProjectile = true;
			ProjectileMovement->HomingAccelerationMagnitude = 5000.0f;
		}

		UE_LOG(LogTemp, Log, TEXT("[B_BaseProjectile] BeginPlay - Speed: %.1f, Lifespan: %.1f, Homing: %s"),
			ProjectileMovement->InitialSpeed,
			Lifespan,
			IsHoming ? TEXT("Yes") : TEXT("No"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_BaseProjectile] BeginPlay - No ProjectileMovement component found!"));
	}
}

void AB_BaseProjectile::CacheComponentReferences()
{
	// Find DefaultSceneRoot (or use existing RootComponent)
	DefaultSceneRoot = Cast<USceneComponent>(GetRootComponent());

	// Find ProjectileMovementComponent
	ProjectileMovement = FindComponentByClass<UProjectileMovementComponent>();
	if (!ProjectileMovement)
	{
		// Try by name
		TArray<UActorComponent*> AllComps;
		GetComponents(AllComps);
		for (UActorComponent* Comp : AllComps)
		{
			if (Comp && Comp->GetName().Contains(TEXT("ProjectileMovement")))
			{
				ProjectileMovement = Cast<UProjectileMovementComponent>(Comp);
				break;
			}
		}
	}

	// Find Trigger (SphereComponent)
	TArray<USphereComponent*> SphereComps;
	GetComponents<USphereComponent>(SphereComps);
	for (USphereComponent* Sphere : SphereComps)
	{
		if (Sphere && Sphere->GetName().Contains(TEXT("Trigger")))
		{
			Trigger = Sphere;
			break;
		}
	}
	// Fallback: use first sphere if no "Trigger" found
	if (!Trigger && SphereComps.Num() > 0)
	{
		Trigger = SphereComps[0];
	}

	// Find Effect (NiagaraComponent)
	TArray<UNiagaraComponent*> NiagaraComps;
	GetComponents<UNiagaraComponent>(NiagaraComps);
	for (UNiagaraComponent* NC : NiagaraComps)
	{
		if (NC && (NC->GetName().Contains(TEXT("Effect")) || NC->GetName().Contains(TEXT("Trail"))))
		{
			Effect = NC;
			break;
		}
	}
	// Fallback: use first niagara if no matching name
	if (!Effect && NiagaraComps.Num() > 0)
	{
		Effect = NiagaraComps[0];
	}

	UE_LOG(LogTemp, Log, TEXT("[B_BaseProjectile] CacheComponentReferences - Root: %s, Movement: %s, Trigger: %s, Effect: %s"),
		DefaultSceneRoot ? TEXT("OK") : TEXT("MISSING"),
		ProjectileMovement ? TEXT("OK") : TEXT("MISSING"),
		Trigger ? TEXT("OK") : TEXT("MISSING"),
		Effect ? TEXT("OK") : TEXT("MISSING"));
}

void AB_BaseProjectile::OnTriggerOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Don't hit self or instigator
	if (OtherActor == this || OtherActor == GetInstigator())
	{
		return;
	}

	// Check if the other actor has the target tag
	if (!OtherActor->Tags.Contains(TargetTag))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[B_BaseProjectile] Hit %s"), *OtherActor->GetName());

	// Calculate damage
	float Damage = FMath::RandRange(MinDamage, MaxDamage);

	// Apply damage via interface if available
	// TODO: Implement damage application via BPI_GenericCharacter::ApplyDamage

	// Spawn hit effect
	if (HitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	// Destroy after delay
	SetLifeSpan(DestroyDelay);

	// Disable further collisions
	if (Trigger)
	{
		Trigger->SetGenerateOverlapEvents(false);
	}

	// Stop movement
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
	}
}

void AB_BaseProjectile::InitializeProjectile_Implementation(AActor* TargetActor)
{
	UE_LOG(LogTemp, Log, TEXT("[B_BaseProjectile] InitializeProjectile - Target: %s"),
		TargetActor ? *TargetActor->GetName() : TEXT("None"));

	// Set homing target if homing is enabled
	if (IsHoming && ProjectileMovement && TargetActor)
	{
		// Find a component to home in on
		USceneComponent* TargetComponent = TargetActor->GetRootComponent();
		if (TargetComponent)
		{
			ProjectileMovement->HomingTargetComponent = TargetComponent;
			ProjectileMovement->bIsHomingProjectile = true;
		}
	}
}
