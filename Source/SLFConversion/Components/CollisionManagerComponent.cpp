// CollisionManagerComponent.cpp
// C++ implementation for AC_CollisionManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_CollisionManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         11/11 (initialized in constructor)
// Functions:         9/9 implemented (stub implementations for BlueprintNativeEvent)
// Event Dispatchers: 1/1 (OnActorTraced)
// ═══════════════════════════════════════════════════════════════════════════════

#include "CollisionManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AC_CombatManager.h"
#include "Components/AICombatManagerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"

UCollisionManagerComponent::UCollisionManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Initialize config variables
	TraceSocketStart = FName("TraceStart");
	TraceSocketEnd = FName("TraceEnd");
	TraceRadius = 20.0;
	TraceDebugMode = EDrawDebugTrace::None;

	// Initialize runtime variables
	TargetMesh = nullptr;
	DamageMultiplier = 1.0;
	TraceSizeMultiplier = 1.0;
	LastStartPosition = FVector::ZeroVector;
	LastEndPosition = FVector::ZeroVector;

	// CRITICAL: Initialize TraceTypes to detect hittable objects
	// Without this, SphereTraceMultiForObjects will never hit anything!
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));           // Characters
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));   // Destructibles, physics props
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));    // Physics-simulated bodies
}

void UCollisionManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[CollisionManager] BeginPlay on %s"), *GetOwner()->GetName());

	InitializeTracePoints();
}

void UCollisionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Perform trace each tick when enabled
	SubsteppedTrace(50.0); // Default step size
}

// ═══════════════════════════════════════════════════════════════════════════════
// MULTIPLIER FUNCTIONS [1-2/9]
// ═══════════════════════════════════════════════════════════════════════════════

void UCollisionManagerComponent::SetMultipliers_Implementation(double InDamageMultiplier, double InTraceSizeMultiplier)
{
	DamageMultiplier = InDamageMultiplier;
	TraceSizeMultiplier = InTraceSizeMultiplier;

	UE_LOG(LogTemp, Log, TEXT("[CollisionManager] SetMultipliers - Damage: %.2f, TraceSize: %.2f"),
		DamageMultiplier, TraceSizeMultiplier);
}

void UCollisionManagerComponent::GetMultipliers_Implementation(double& OutDamageMultiplier, double& OutTraceSizeMultiplier)
{
	OutDamageMultiplier = DamageMultiplier;
	OutTraceSizeMultiplier = TraceSizeMultiplier;
}

// ═══════════════════════════════════════════════════════════════════════════════
// TRACE FUNCTIONS [3-6/9]
// ═══════════════════════════════════════════════════════════════════════════════

void UCollisionManagerComponent::GetTraceLocations(FVector& OutTraceStart, FVector& OutTraceEnd) const
{
	OutTraceStart = FVector::ZeroVector;
	OutTraceEnd = FVector::ZeroVector;

	if (USkeletalMeshComponent* SkelMesh = Cast<USkeletalMeshComponent>(TargetMesh))
	{
		OutTraceStart = SkelMesh->GetSocketLocation(TraceSocketStart);
		OutTraceEnd = SkelMesh->GetSocketLocation(TraceSocketEnd);
	}
	else if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(TargetMesh))
	{
		OutTraceStart = StaticMesh->GetSocketLocation(TraceSocketStart);
		OutTraceEnd = StaticMesh->GetSocketLocation(TraceSocketEnd);
	}
}

void UCollisionManagerComponent::SubsteppedTrace_Implementation(double StepSize)
{
	FVector CurrentStart, CurrentEnd;
	GetTraceLocations(CurrentStart, CurrentEnd);

	// If last positions are zero, just do single trace
	if (LastStartPosition.IsNearlyZero() || LastEndPosition.IsNearlyZero())
	{
		LastStartPosition = CurrentStart;
		LastEndPosition = CurrentEnd;
	}

	// Calculate distance moved
	float DistanceMoved = FVector::Dist(LastStartPosition, CurrentStart);
	int32 NumSteps = FMath::Max(1, FMath::CeilToInt(DistanceMoved / StepSize));

	for (int32 Step = 0; Step <= NumSteps; ++Step)
	{
		float Alpha = static_cast<float>(Step) / static_cast<float>(NumSteps);

		FVector StepStart = FMath::Lerp(LastStartPosition, CurrentStart, Alpha);
		FVector StepEnd = FMath::Lerp(LastEndPosition, CurrentEnd, Alpha);

		TArray<FHitResult> HitResults;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner());

		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			StepStart,
			StepEnd,
			TraceRadius * TraceSizeMultiplier,
			TraceTypes,
			false, // bTraceComplex
			ActorsToIgnore,
			TraceDebugMode,
			HitResults,
			true // bIgnoreSelf
		);

		if (bHit)
		{
			ProcessTrace(HitResults);
		}
	}

	// Update last positions
	LastStartPosition = CurrentStart;
	LastEndPosition = CurrentEnd;
}

void UCollisionManagerComponent::ProcessTrace_Implementation(const TArray<FHitResult>& HitResults)
{
	AActor* WeaponOwner = GetOwner();
	if (!WeaponOwner)
	{
		return;
	}

	// Get the character that owns this weapon (weapon is attached to character)
	AActor* AttackingCharacter = WeaponOwner->GetAttachParentActor();
	if (!AttackingCharacter)
	{
		AttackingCharacter = WeaponOwner; // Fallback if weapon is the character itself
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || TracedActors.Contains(HitActor))
		{
			continue;
		}

		// Skip hitting ourselves or our owner
		if (HitActor == WeaponOwner || HitActor == AttackingCharacter)
		{
			continue;
		}

		TracedActors.Add(HitActor);

		UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Hit: %s (Damage: %.1f)"),
			*HitActor->GetName(), 10.0 * DamageMultiplier);

		// Broadcast the hit (for any external listeners)
		OnActorTraced.Broadcast(HitActor, Hit, DamageMultiplier);

		// Apply damage directly to the hit actor's combat manager
		double Damage = 50.0 * DamageMultiplier;  // Base weapon damage with multiplier (10% of typical 500 HP)
		double PoiseDamage = 25.0 * DamageMultiplier;
		TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffects;

		// Try player combat manager first
		UAC_CombatManager* TargetCombatManager = HitActor->FindComponentByClass<UAC_CombatManager>();
		if (TargetCombatManager)
		{
			UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Applying %.1f damage to %s via CombatManager"),
				Damage, *HitActor->GetName());

			TargetCombatManager->HandleIncomingWeaponDamage(
				AttackingCharacter,
				nullptr,  // GuardSound
				nullptr,  // PerfectGuardSound
				Hit,
				Damage,
				PoiseDamage,
				StatusEffects
			);
		}
		else
		{
			// Try AI combat manager (UAICombatManagerComponent is the actual class used by enemies)
			UAICombatManagerComponent* AICombatManager = HitActor->FindComponentByClass<UAICombatManagerComponent>();
			if (AICombatManager)
			{
				UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Applying %.1f damage to AI %s"),
					Damage, *HitActor->GetName());

				AICombatManager->HandleIncomingWeaponDamage_AI(AttackingCharacter, Damage, PoiseDamage, Hit);
			}
		}
	}
}

void UCollisionManagerComponent::ToggleTrace_Implementation(bool bEnabled)
{
	UE_LOG(LogTemp, Log, TEXT("[CollisionManager] ToggleTrace: %s"), bEnabled ? TEXT("true") : TEXT("false"));

	if (bEnabled)
	{
		// Reset traced actors and last positions
		TracedActors.Empty();
		LastStartPosition = FVector::ZeroVector;
		LastEndPosition = FVector::ZeroVector;

		// Enable tick
		SetComponentTickEnabled(true);
	}
	else
	{
		// Disable tick
		SetComponentTickEnabled(false);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// INITIALIZATION [7/9]
// ═══════════════════════════════════════════════════════════════════════════════

void UCollisionManagerComponent::InitializeTracePoints_Implementation()
{
	if (AActor* Owner = GetOwner())
	{
		// Try skeletal mesh first
		if (USkeletalMeshComponent* SkelMesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
		{
			TargetMesh = SkelMesh;
			UE_LOG(LogTemp, Log, TEXT("[CollisionManager] InitializeTracePoints - Using SkeletalMesh"));
		}
		// Fall back to static mesh
		else if (UStaticMeshComponent* StaticMesh = Owner->FindComponentByClass<UStaticMeshComponent>())
		{
			TargetMesh = StaticMesh;
			UE_LOG(LogTemp, Log, TEXT("[CollisionManager] InitializeTracePoints - Using StaticMesh"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[CollisionManager] InitializeTracePoints - No mesh found"));
		}
	}
}
