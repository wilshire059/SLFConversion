// SLFSpellProjectile.cpp
// Blackhole spell projectile with dark core and purple swirling glow

#include "SLFSpellProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraComponent.h"

ASLFSpellProjectile::ASLFSpellProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default spell configuration
	SpellElement = ESLFSpellElement::Dark;  // Dark element for blackhole
	FPCost = 15.0f;
	IntelligenceScaling = 0.6f;
	FaithScaling = 0.0f;

	// Blackhole visual settings - LARGER for visibility
	CoreRadius = 25.0f;
	GlowRadius = 50.0f;
	CoreColor = FLinearColor(0.02f, 0.0f, 0.05f, 1.0f);  // Near black with hint of purple
	GlowColor = FLinearColor(0.6f, 0.1f, 1.0f, 1.0f);    // Bright purple
	EmissiveIntensity = 50.0f;  // Brighter
	CoreRotationSpeed = 180.0f;  // Degrees per second
	PulseSpeed = 3.0f;
	PulseIntensity = 0.5f;

	// Faster projectile for spells
	ProjectileSpeed = 3500.0f;

	// Spell damage
	MinDamage = 40.0f;
	MaxDamage = 60.0f;

	AccumulatedTime = 0.0f;

	// Create core mesh component (dark sphere)
	CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
	CoreMesh->SetupAttachment(RootComponent);  // Attach to RootComponent which parent creates
	CoreMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CoreMesh->SetCastShadow(false);
	CoreMesh->SetRelativeScale3D(FVector(0.5f));  // Initial scale

	// Load basic sphere mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshFinder.Succeeded())
	{
		CoreMesh->SetStaticMesh(SphereMeshFinder.Object);
		UE_LOG(LogTemp, Log, TEXT("[SLFSpellProjectile] Loaded sphere mesh for core"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] FAILED to load sphere mesh!"));
	}

	// Create glow mesh component (larger transparent sphere)
	GlowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlowMesh"));
	GlowMesh->SetupAttachment(RootComponent);
	GlowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GlowMesh->SetCastShadow(false);
	GlowMesh->SetRelativeScale3D(FVector(1.0f));  // Larger than core
	if (SphereMeshFinder.Succeeded())
	{
		GlowMesh->SetStaticMesh(SphereMeshFinder.Object);
	}

	// Create point light for purple glow
	CoreLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("CoreLight"));
	CoreLight->SetupAttachment(RootComponent);
	CoreLight->SetIntensity(10000.0f);  // Brighter
	CoreLight->SetAttenuationRadius(300.0f);
	CoreLight->SetLightColor(FLinearColor(0.5f, 0.1f, 1.0f));  // Purple light
	CoreLight->SetCastShadows(false);

	CoreMaterial = nullptr;
	GlowMaterial = nullptr;
}

void ASLFSpellProjectile::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] ========== BLACKHOLE SPELL SPAWNED =========="));
	UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] CoreMesh: %s, GlowMesh: %s, CoreLight: %s"),
		CoreMesh ? TEXT("YES") : TEXT("NO"),
		GlowMesh ? TEXT("YES") : TEXT("NO"),
		CoreLight ? TEXT("YES") : TEXT("NO"));

	// Setup blackhole effect before parent call
	SetupBlackholeEffect();

	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] BeginPlay complete - CoreRadius=%.1f, GlowRadius=%.1f"),
		CoreRadius, GlowRadius);
}

void ASLFSpellProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update visual effects
	UpdatePulseEffect(DeltaTime);

	// Rotate the core for swirling effect
	if (CoreMesh)
	{
		FRotator CurrentRotation = CoreMesh->GetRelativeRotation();
		CurrentRotation.Yaw += CoreRotationSpeed * DeltaTime;
		CurrentRotation.Pitch += CoreRotationSpeed * 0.7f * DeltaTime;
		CoreMesh->SetRelativeRotation(CurrentRotation);
	}

	// Counter-rotate glow for interesting visual
	if (GlowMesh)
	{
		FRotator GlowRotation = GlowMesh->GetRelativeRotation();
		GlowRotation.Yaw -= CoreRotationSpeed * 0.5f * DeltaTime;
		GlowRotation.Roll += CoreRotationSpeed * 0.3f * DeltaTime;
		GlowMesh->SetRelativeRotation(GlowRotation);
	}
}

void ASLFSpellProjectile::SetupBlackholeEffect()
{
	UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] Setting up blackhole visual effect"));

	// Scale the meshes - make them LARGE and visible
	if (CoreMesh)
	{
		float CoreScale = CoreRadius / 50.0f;  // Default sphere is 100 units diameter
		CoreMesh->SetRelativeScale3D(FVector(CoreScale));
		CoreMesh->SetVisibility(true);
		CoreMesh->SetHiddenInGame(false);

		UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] CoreMesh scale=%.2f, HasMesh=%s"),
			CoreScale, CoreMesh->GetStaticMesh() ? TEXT("YES") : TEXT("NO"));
	}

	if (GlowMesh)
	{
		float GlowScale = GlowRadius / 50.0f;
		GlowMesh->SetRelativeScale3D(FVector(GlowScale));
		GlowMesh->SetVisibility(true);
		GlowMesh->SetHiddenInGame(false);

		UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] GlowMesh scale=%.2f, HasMesh=%s"),
			GlowScale, GlowMesh->GetStaticMesh() ? TEXT("YES") : TEXT("NO"));
	}

	// Create dynamic material for core - PURPLE COLOR (visible)
	if (CoreMesh && CoreMesh->GetStaticMesh())
	{
		// Use the mesh's existing material as base
		UMaterialInterface* BaseMaterial = CoreMesh->GetMaterial(0);
		if (!BaseMaterial)
		{
			BaseMaterial = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}

		if (BaseMaterial)
		{
			CoreMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			if (CoreMaterial)
			{
				// Set to dark purple (not pure black so it's visible)
				CoreMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.1f, 0.0f, 0.2f, 1.0f));
				CoreMesh->SetMaterial(0, CoreMaterial);
				UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] Created DARK PURPLE core material"));
			}
		}
	}

	// Create dynamic material for glow - BRIGHT PURPLE
	if (GlowMesh && GlowMesh->GetStaticMesh())
	{
		UMaterialInterface* BaseMaterial = GlowMesh->GetMaterial(0);
		if (!BaseMaterial)
		{
			BaseMaterial = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}

		if (BaseMaterial)
		{
			GlowMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			if (GlowMaterial)
			{
				// Bright purple
				GlowMaterial->SetVectorParameterValue(TEXT("BaseColor"), GlowColor);
				GlowMesh->SetMaterial(0, GlowMaterial);
				UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] Created BRIGHT PURPLE glow material"));
			}
		}
	}

	// Configure light - BRIGHT
	if (CoreLight)
	{
		CoreLight->SetLightColor(GlowColor);
		CoreLight->SetIntensity(EmissiveIntensity * 500.0f);  // Very bright
		CoreLight->SetVisibility(true);
		UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] CoreLight intensity=%.0f"), EmissiveIntensity * 500.0f);
	}

	// Disable the inherited Niagara effect - we're using mesh-based visuals
	if (Effect)
	{
		Effect->Deactivate();
		Effect->SetVisibility(false);
		UE_LOG(LogTemp, Warning, TEXT("[SLFSpellProjectile] Disabled Niagara Effect component"));
	}
}

void ASLFSpellProjectile::UpdatePulseEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Calculate pulse factor (oscillates between 1-PulseIntensity and 1+PulseIntensity)
	float PulseFactor = 1.0f + FMath::Sin(AccumulatedTime * PulseSpeed * PI * 2.0f) * PulseIntensity;

	// Update glow material emissive
	if (GlowMaterial)
	{
		FLinearColor PulsedEmissive = GlowColor * EmissiveIntensity * PulseFactor;
		GlowMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), PulsedEmissive);
	}

	// Update light intensity
	if (CoreLight)
	{
		CoreLight->SetIntensity(EmissiveIntensity * 200.0f * PulseFactor);
	}

	// Slight scale pulse on glow mesh
	if (GlowMesh)
	{
		float BaseGlowScale = GlowRadius / 50.0f;
		float PulsedScale = BaseGlowScale * (1.0f + (PulseFactor - 1.0f) * 0.3f);
		GlowMesh->SetRelativeScale3D(FVector(PulsedScale));
	}
}

FLinearColor ASLFSpellProjectile::GetElementColor(ESLFSpellElement Element)
{
	switch (Element)
	{
	case ESLFSpellElement::Fire:
		return FLinearColor(1.0f, 0.3f, 0.0f, 1.0f);  // Orange-red

	case ESLFSpellElement::Ice:
		return FLinearColor(0.3f, 0.7f, 1.0f, 1.0f);  // Light blue

	case ESLFSpellElement::Lightning:
		return FLinearColor(1.0f, 1.0f, 0.3f, 1.0f);  // Yellow

	case ESLFSpellElement::Holy:
		return FLinearColor(1.0f, 0.95f, 0.7f, 1.0f);  // Golden white

	case ESLFSpellElement::Dark:
		return FLinearColor(0.5f, 0.1f, 1.0f, 1.0f);  // Purple (for blackhole)

	case ESLFSpellElement::Arcane:
	default:
		return FLinearColor(0.6f, 0.2f, 1.0f, 1.0f);  // Bright purple
	}
}
