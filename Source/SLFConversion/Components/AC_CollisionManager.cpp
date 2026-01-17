// AC_CollisionManager.cpp
// C++ component implementation for AC_CollisionManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_CollisionManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_CollisionManager.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/EngineTypes.h"

UAC_CollisionManager::UAC_CollisionManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Initialize defaults
	TraceSocketStart = FName("WeaponTraceStart");
	TraceSocketEnd = FName("WeaponTraceEnd");
	TraceRadius = 10.0;
	TargetMesh = nullptr;
	TraceDebugMode = EDrawDebugTrace::None;
	DamageMultiplier = 1.0;
	TraceSizeMultiplier = 1.0;
	LastStartPosition = FVector::ZeroVector;
	LastEndPosition = FVector::ZeroVector;
	bTraceActive = false;

	// CRITICAL: Initialize TraceTypes to detect hittable objects
	// Without this, SphereTraceMultiForObjects will never hit anything!
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));           // Characters
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));   // Destructibles, physics props
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));    // Physics-simulated bodies
}

void UAC_CollisionManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_CollisionManager::BeginPlay"));

	// Get the target mesh from owning character
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		// Try to get mesh component (prioritize Mesh3P if exists, else Mesh)
		ACharacter* Character = Cast<ACharacter>(Owner);
		if (IsValid(Character))
		{
			TargetMesh = Character->GetMesh();
		}
		else
		{
			TargetMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
		}
	}
}

void UAC_CollisionManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTraceActive)
	{
		// Calculate step size based on DeltaTime and TraceSizeMultiplier
		double StepSize = DeltaTime * TraceSizeMultiplier;
		SubsteppedTrace(StepSize);
	}
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * GetTraceLocations - Get start and end socket locations from target mesh
 */
void UAC_CollisionManager::GetTraceLocations_Implementation(FVector& OutTraceStartLocation, FVector& OutTraceEndLocation)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CollisionManager::GetTraceLocations"));

	USkeletalMeshComponent* MeshComp = Cast<USkeletalMeshComponent>(TargetMesh);
	if (IsValid(MeshComp))
	{
		OutTraceStartLocation = MeshComp->GetSocketLocation(TraceSocketStart);
		OutTraceEndLocation = MeshComp->GetSocketLocation(TraceSocketEnd);
	}
	else
	{
		OutTraceStartLocation = FVector::ZeroVector;
		OutTraceEndLocation = FVector::ZeroVector;
	}
}

/**
 * SubsteppedTrace - Perform trace with substepping for accurate hit detection
 *
 * Blueprint Logic:
 * 1. Get current trace locations
 * 2. Calculate distance from last position to current
 * 3. Subdivide into substeps based on StepSize
 * 4. Perform sphere trace at each substep
 * 5. Process any hits
 * 6. Update last positions
 */
void UAC_CollisionManager::SubsteppedTrace_Implementation(double StepSize)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CollisionManager::SubsteppedTrace - StepSize: %.4f"), StepSize);

	// Get current trace locations
	FVector CurrentStartPosition, CurrentEndPosition;
	GetTraceLocations(CurrentStartPosition, CurrentEndPosition);

	// Calculate distance traveled
	double Distance = FVector::Dist(LastStartPosition, CurrentStartPosition);

	// Calculate number of substeps
	int32 Substeps = 1;
	if (StepSize > 0.0 && Distance > 0.0)
	{
		Substeps = FMath::Max(1, FMath::CeilToInt(Distance / (StepSize * 100.0)));
	}

	UE_LOG(LogTemp, Log, TEXT("  Distance: %.2f, Substeps: %d"), Distance, Substeps);

	// Actors to ignore (owner and already traced actors)
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	ActorsToIgnore.Append(TracedActors);

	// Perform substep traces
	for (int32 i = 0; i < Substeps; i++)
	{
		double Alpha = (Substeps > 1) ? (static_cast<double>(i) / static_cast<double>(Substeps - 1)) : 1.0;

		// Interpolate positions
		FVector NewStartPosition = FMath::Lerp(LastStartPosition, CurrentStartPosition, Alpha);
		FVector NewEndPosition = FMath::Lerp(LastEndPosition, CurrentEndPosition, Alpha);

		// Perform sphere trace
		TArray<FHitResult> HitResults;
		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			NewStartPosition,
			NewEndPosition,
			TraceRadius,
			TraceTypes,
			false,
			ActorsToIgnore,
			TraceDebugMode,
			HitResults,
			true
		);

		if (bHit)
		{
			ProcessTrace(HitResults);
		}
	}

	// Update last positions for next frame
	LastStartPosition = CurrentStartPosition;
	LastEndPosition = CurrentEndPosition;
}

/**
 * ProcessTrace - Process hit results and broadcast events for valid hits
 *
 * Blueprint Logic:
 * 1. For each hit result
 * 2. Check if actor is not already in TracedActors
 * 3. Add to TracedActors
 * 4. Broadcast OnActorTraced with hit info and damage multiplier
 */
void UAC_CollisionManager::ProcessTrace_Implementation(const TArray<FHitResult>& Hits)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CollisionManager::ProcessTrace - %d hits"), Hits.Num());

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (IsValid(HitActor) && !TracedActors.Contains(HitActor))
		{
			TracedActors.Add(HitActor);
			UE_LOG(LogTemp, Log, TEXT("  Hit actor: %s"), *HitActor->GetName());
			OnActorTraced.Broadcast(HitActor, Hit, DamageMultiplier);
		}
	}
}

/**
 * GetMultipliers - Get current damage and trace size multipliers
 */
void UAC_CollisionManager::GetMultipliers_Implementation(double& OutDamageMultiplier, double& OutTraceSizeMultiplier)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CollisionManager::GetMultipliers - Damage: %.2f, TraceSize: %.2f"),
		DamageMultiplier, TraceSizeMultiplier);

	OutDamageMultiplier = DamageMultiplier;
	OutTraceSizeMultiplier = TraceSizeMultiplier;
}

/**
 * SetMultipliers - Set damage and trace size multipliers
 */
void UAC_CollisionManager::SetMultipliers_Implementation(double InDamageMultiplier, double InTraceSizeMultiplier)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CollisionManager::SetMultipliers - Damage: %.2f, TraceSize: %.2f"),
		InDamageMultiplier, InTraceSizeMultiplier);

	DamageMultiplier = InDamageMultiplier;
	TraceSizeMultiplier = InTraceSizeMultiplier;
}

/**
 * ToggleTrace - Enable or disable weapon tracing
 */
void UAC_CollisionManager::ToggleTrace_Implementation(bool bEnable)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CollisionManager::ToggleTrace - %s"), bEnable ? TEXT("Enable") : TEXT("Disable"));

	bTraceActive = bEnable;
	SetComponentTickEnabled(bEnable);

	if (bEnable)
	{
		// Initialize trace points when enabling
		InitializeTracePoints();
	}
	else
	{
		// Clear traced actors when disabling
		TracedActors.Empty();
	}
}

/**
 * InitializeTracePoints - Set initial trace positions
 */
void UAC_CollisionManager::InitializeTracePoints_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_CollisionManager::InitializeTracePoints"));

	GetTraceLocations(LastStartPosition, LastEndPosition);
}
