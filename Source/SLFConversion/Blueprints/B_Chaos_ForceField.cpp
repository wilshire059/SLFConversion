// B_Chaos_ForceField.cpp
// C++ implementation for Blueprint B_Chaos_ForceField
//
// 20-PASS VALIDATION: 2026-01-17
// Source: BlueprintDNA_v2/Blueprint/B_Chaos_ForceField.json

#include "Blueprints/B_Chaos_ForceField.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Field/FieldSystemComponent.h"

AB_Chaos_ForceField::AB_Chaos_ForceField()
{
	// Tick is disabled by default, enabled via EnableChaosDestroy interface call
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Initialize pointers
	OperatorFIeld_Input = nullptr;
	FieldVolume = nullptr;
	skel = nullptr;
	CachedRadialFalloff = nullptr;
	CachedSphere = nullptr;
	CachedRadialForce = nullptr;
	CachedRadialVector = nullptr;
	CachedFieldSystemMetaDataFilter = nullptr;

	// NOTE: Components are created in Blueprint SCS, not in C++ constructor
	// This avoids name collisions and preserves Blueprint component setup
}

void AB_Chaos_ForceField::BeginPlay()
{
	Super::BeginPlay();

	// Cache component references from Blueprint SCS
	const TSet<UActorComponent*>& AllComponents = GetComponents();
	for (UActorComponent* Component : AllComponents)
	{
		if (!Component) continue;
		FString CompName = Component->GetName();

		if (CompName.Contains(TEXT("RadialFalloff")) && !CachedRadialFalloff)
		{
			CachedRadialFalloff = Cast<URadialFalloff>(Component);
		}
		else if (CompName.Contains(TEXT("Sphere")) && !CachedSphere)
		{
			CachedSphere = Cast<USphereComponent>(Component);
		}
		else if (CompName.Contains(TEXT("RadialForce")) && !CachedRadialForce)
		{
			CachedRadialForce = Cast<URadialForceComponent>(Component);
		}
		else if (CompName.Contains(TEXT("RadialVector")) && !CachedRadialVector)
		{
			CachedRadialVector = Cast<URadialVector>(Component);
		}
		else if (CompName.Contains(TEXT("FieldSystemMetaDataFilter")) && !CachedFieldSystemMetaDataFilter)
		{
			CachedFieldSystemMetaDataFilter = Cast<UFieldSystemMetaDataFilter>(Component);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Chaos_ForceField] BeginPlay - Components cached: RadialFalloff=%s, Sphere=%s, RadialForce=%s"),
		CachedRadialFalloff ? TEXT("Yes") : TEXT("No"),
		CachedSphere ? TEXT("Yes") : TEXT("No"),
		CachedRadialForce ? TEXT("Yes") : TEXT("No"));
}

void AB_Chaos_ForceField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Replicate Blueprint Tick logic:
	// 1. Get sphere position and radius
	// 2. Setup RadialFalloff with huge magnitude (99999999)
	// 3. Apply physics field targeting Field_ExternalClusterStrain

	if (!CachedSphere || !CachedRadialFalloff)
	{
		return;
	}

	// Get sphere's world location and radius
	FVector SphereLocation = CachedSphere->GetComponentLocation();
	float SphereRadius = CachedSphere->GetScaledSphereRadius();

	// Setup RadialFalloff field:
	// Magnitude = 99999999.0 (huge strain to trigger destruction)
	// MinRange = 0.0
	// MaxRange = 1.0
	// Default = 0.0
	// Radius = sphere radius
	// Position = sphere world location
	// Falloff = None
	CachedRadialFalloff->SetRadialFalloff(
		99999999.0f,  // Magnitude - must be huge to exceed strain threshold
		0.0f,         // MinRange
		1.0f,         // MaxRange
		0.0f,         // Default
		SphereRadius, // Radius
		SphereLocation, // Position
		EFieldFalloffType::Field_FallOff_None  // Falloff type
	);

	// Apply the physics field to the Field System Component
	// Target = Field_ExternalClusterStrain triggers Chaos GeometryCollection destruction
	if (UFieldSystemComponent* FSC = GetFieldSystemComponent())
	{
		FSC->ApplyPhysicsField(
			true,  // Enabled
			EFieldPhysicsType::Field_ExternalClusterStrain,  // Target - THIS triggers Chaos destruction!
			nullptr,  // MetaData
			CachedRadialFalloff  // Field
		);

		UE_LOG(LogTemp, Verbose, TEXT("[B_Chaos_ForceField] Tick - Applied Field_ExternalClusterStrain at %s radius %.1f"),
			*SphereLocation.ToString(), SphereRadius);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_Chaos_ForceField] Tick - No FieldSystemComponent!"));
	}
}

void AB_Chaos_ForceField::EnableChaosDestroy_Implementation()
{
	// Enable tick to activate the force field effects
	SetActorTickEnabled(true);
	UE_LOG(LogTemp, Warning, TEXT("[B_Chaos_ForceField] EnableChaosDestroy_Implementation - Tick enabled for %s"), *GetName());
}

void AB_Chaos_ForceField::DisableChaosDestroy_Implementation()
{
	// Disable tick to deactivate the force field effects
	SetActorTickEnabled(false);
	UE_LOG(LogTemp, Warning, TEXT("[B_Chaos_ForceField] DisableChaosDestroy_Implementation - Tick disabled for %s"), *GetName());
}
