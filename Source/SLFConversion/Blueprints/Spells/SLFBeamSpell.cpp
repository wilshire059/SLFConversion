// SLFBeamSpell.cpp
// Continuous beam effect

#include "SLFBeamSpell.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASLFBeamSpell::ASLFBeamSpell()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Main beam core (cylinder pointing forward)
	BeamCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeamCore"));
	BeamCore->SetupAttachment(Root);
	BeamCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BeamCore->SetCastShadow(false);
	BeamCore->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));  // Point forward (X axis)
	if (CylinderMesh.Succeeded())
	{
		BeamCore->SetStaticMesh(CylinderMesh.Object);
	}

	// Outer glow
	BeamGlow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeamGlow"));
	BeamGlow->SetupAttachment(Root);
	BeamGlow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BeamGlow->SetCastShadow(false);
	BeamGlow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	if (CylinderMesh.Succeeded())
	{
		BeamGlow->SetStaticMesh(CylinderMesh.Object);
	}

	// Start orb (at caster)
	StartOrb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartOrb"));
	StartOrb->SetupAttachment(Root);
	StartOrb->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StartOrb->SetCastShadow(false);
	StartOrb->SetRelativeScale3D(FVector(0.25f));
	if (SphereMesh.Succeeded())
	{
		StartOrb->SetStaticMesh(SphereMesh.Object);
	}

	// End orb (at impact)
	EndOrb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EndOrb"));
	EndOrb->SetupAttachment(Root);
	EndOrb->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EndOrb->SetCastShadow(false);
	EndOrb->SetRelativeScale3D(FVector(0.3f));
	if (SphereMesh.Succeeded())
	{
		EndOrb->SetStaticMesh(SphereMesh.Object);
	}

	// Lights
	StartLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("StartLight"));
	StartLight->SetupAttachment(Root);
	StartLight->SetIntensity(8000.0f);
	StartLight->SetAttenuationRadius(200.0f);
	StartLight->SetCastShadows(false);

	EndLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("EndLight"));
	EndLight->SetupAttachment(Root);
	EndLight->SetIntensity(10000.0f);
	EndLight->SetAttenuationRadius(250.0f);
	EndLight->SetCastShadows(false);
}

void ASLFBeamSpell::BeginPlay()
{
	Super::BeginPlay();
	SetupBeamEffect();

	UE_LOG(LogTemp, Warning, TEXT("[SLFBeamSpell] BEAM SPELL SPAWNED!"));
}

void ASLFBeamSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LifeTime += DeltaTime;
	if (LifeTime >= Duration)
	{
		Destroy();
		return;
	}

	UpdateBeamEffect(DeltaTime);
}

void ASLFBeamSpell::SetupBeamEffect()
{
	// Calculate beam cylinder scale
	// Default cylinder is 100 units radius, 100 units height
	float RadiusScale = BeamRadius / 50.0f;
	float LengthScale = BeamLength / 100.0f;

	// Core beam
	if (BeamCore)
	{
		BeamCore->SetRelativeScale3D(FVector(RadiusScale, RadiusScale, LengthScale));
		BeamCore->SetRelativeLocation(FVector(BeamLength / 2.0f, 0.0f, 0.0f));  // Center of beam
		BeamCore->SetVisibility(true);
	}

	// Glow beam (larger)
	if (BeamGlow)
	{
		float GlowRadiusScale = GlowRadius / 50.0f;
		BeamGlow->SetRelativeScale3D(FVector(GlowRadiusScale, GlowRadiusScale, LengthScale));
		BeamGlow->SetRelativeLocation(FVector(BeamLength / 2.0f, 0.0f, 0.0f));
		BeamGlow->SetVisibility(true);
	}

	// Position orbs
	if (StartOrb)
	{
		StartOrb->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		StartOrb->SetVisibility(true);
	}

	if (EndOrb)
	{
		EndOrb->SetRelativeLocation(FVector(BeamLength, 0.0f, 0.0f));
		EndOrb->SetVisibility(true);
	}

	// Position lights
	if (StartLight)
	{
		StartLight->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		StartLight->SetLightColor(BeamColor);
	}

	if (EndLight)
	{
		EndLight->SetRelativeLocation(FVector(BeamLength, 0.0f, 0.0f));
		EndLight->SetLightColor(BeamColor);
	}

	// Setup materials
	auto SetupMaterial = [this](UStaticMeshComponent* Mesh, float EmissiveMult) -> UMaterialInstanceDynamic*
	{
		if (Mesh && Mesh->GetStaticMesh())
		{
			UMaterialInterface* BaseMat = Mesh->GetMaterial(0);
			if (!BaseMat)
			{
				BaseMat = LoadObject<UMaterialInterface>(nullptr,
					TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
			}
			if (BaseMat)
			{
				UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(BaseMat, this);
				Mat->SetVectorParameterValue(TEXT("BaseColor"), BeamColor);
				Mat->SetVectorParameterValue(TEXT("EmissiveColor"), BeamColor * EmissiveMult);
				Mesh->SetMaterial(0, Mat);
				return Mat;
			}
		}
		return nullptr;
	};

	CoreMaterial = SetupMaterial(BeamCore, 40.0f);
	GlowMaterial = SetupMaterial(BeamGlow, 15.0f);
	SetupMaterial(StartOrb, 50.0f);
	SetupMaterial(EndOrb, 60.0f);
}

void ASLFBeamSpell::UpdateBeamEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Pulsing intensity along the beam
	float Pulse = FMath::Sin(AccumulatedTime * PulseSpeed) * 0.3f + 0.7f;

	// Update light intensities
	if (StartLight)
	{
		StartLight->SetIntensity(6000.0f + 4000.0f * Pulse);
	}

	if (EndLight)
	{
		EndLight->SetIntensity(8000.0f + 5000.0f * Pulse);
	}

	// Pulse emissive
	if (CoreMaterial)
	{
		CoreMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), BeamColor * (35.0f + 20.0f * Pulse));
	}

	// Slight wobble on glow
	if (BeamGlow)
	{
		float GlowRadiusScale = GlowRadius / 50.0f;
		float WobbleScale = GlowRadiusScale * (1.0f + FMath::Sin(AccumulatedTime * PulseSpeed * 2.0f) * 0.1f);
		float LengthScale = BeamLength / 100.0f;
		BeamGlow->SetRelativeScale3D(FVector(WobbleScale, WobbleScale, LengthScale));
	}

	// Fade out near end of life
	float FadeStart = Duration * 0.7f;
	if (LifeTime > FadeStart)
	{
		float FadeAlpha = 1.0f - (LifeTime - FadeStart) / (Duration - FadeStart);

		if (StartLight) StartLight->SetIntensity(StartLight->Intensity * FadeAlpha);
		if (EndLight) EndLight->SetIntensity(EndLight->Intensity * FadeAlpha);
	}
}
