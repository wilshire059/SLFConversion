// SLFHealingSpell.cpp
// Healing spell with rising particles

#include "SLFHealingSpell.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASLFHealingSpell::ASLFHealingSpell()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Create particles at various positions
	UStaticMesh* SphereMeshPtr = SphereMesh.Succeeded() ? SphereMesh.Object : nullptr;
	auto CreateParticle = [this, SphereMeshPtr](const TCHAR* Name, float Angle, float Radius) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* P = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		P->SetupAttachment(Root);
		P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		P->SetCastShadow(false);
		P->SetRelativeScale3D(FVector(0.1f));
		// Position in a circle
		float X = FMath::Cos(FMath::DegreesToRadians(Angle)) * Radius;
		float Y = FMath::Sin(FMath::DegreesToRadians(Angle)) * Radius;
		P->SetRelativeLocation(FVector(X, Y, 0.0f));
		if (SphereMeshPtr)
		{
			P->SetStaticMesh(SphereMeshPtr);
		}
		return P;
	};

	Particle1 = CreateParticle(TEXT("Particle1"), 0.0f, 40.0f);
	Particle2 = CreateParticle(TEXT("Particle2"), 60.0f, 50.0f);
	Particle3 = CreateParticle(TEXT("Particle3"), 120.0f, 35.0f);
	Particle4 = CreateParticle(TEXT("Particle4"), 180.0f, 45.0f);
	Particle5 = CreateParticle(TEXT("Particle5"), 240.0f, 55.0f);
	Particle6 = CreateParticle(TEXT("Particle6"), 300.0f, 40.0f);

	// Central glow orb
	GlowOrb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlowOrb"));
	GlowOrb->SetupAttachment(Root);
	GlowOrb->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GlowOrb->SetCastShadow(false);
	GlowOrb->SetRelativeScale3D(FVector(0.4f));
	GlowOrb->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	if (SphereMesh.Succeeded())
	{
		GlowOrb->SetStaticMesh(SphereMesh.Object);
	}

	// Soft healing light
	HealingLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("HealingLight"));
	HealingLight->SetupAttachment(Root);
	HealingLight->SetIntensity(8000.0f);
	HealingLight->SetAttenuationRadius(300.0f);
	HealingLight->SetCastShadows(false);
	HealingLight->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

	HealOwner = nullptr;
}

void ASLFHealingSpell::BeginPlay()
{
	Super::BeginPlay();

	// Build particle array
	Particles.Add(Particle1);
	Particles.Add(Particle2);
	Particles.Add(Particle3);
	Particles.Add(Particle4);
	Particles.Add(Particle5);
	Particles.Add(Particle6);

	// Random starting phases for each particle
	for (int32 i = 0; i < Particles.Num(); i++)
	{
		ParticlePhases.Add(FMath::RandRange(0.0f, 1.0f));
	}

	SetupHealingEffect();

	UE_LOG(LogTemp, Warning, TEXT("[SLFHealingSpell] HEALING SPELL SPAWNED!"));
}

void ASLFHealingSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LifeTime += DeltaTime;
	if (LifeTime >= Duration)
	{
		Destroy();
		return;
	}

	// Follow owner if set
	if (HealOwner)
	{
		SetActorLocation(HealOwner->GetActorLocation());
	}

	UpdateHealingEffect(DeltaTime);
}

void ASLFHealingSpell::SetupHealingEffect()
{
	// Setup particle materials
	for (UStaticMeshComponent* P : Particles)
	{
		if (P && P->GetStaticMesh())
		{
			UMaterialInterface* BaseMat = P->GetMaterial(0);
			if (!BaseMat)
			{
				BaseMat = LoadObject<UMaterialInterface>(nullptr,
					TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
			}
			if (BaseMat)
			{
				UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(BaseMat, this);
				// Alternate between heal color and accent color
				FLinearColor Color = (ParticleMaterials.Num() % 2 == 0) ? HealColor : AccentColor;
				Mat->SetVectorParameterValue(TEXT("BaseColor"), Color);
				Mat->SetVectorParameterValue(TEXT("EmissiveColor"), Color * 30.0f);
				P->SetMaterial(0, Mat);
				ParticleMaterials.Add(Mat);
			}
		}
	}

	// Glow orb material
	if (GlowOrb && GlowOrb->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = GlowOrb->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(BaseMat, this);
			Mat->SetVectorParameterValue(TEXT("BaseColor"), HealColor);
			Mat->SetVectorParameterValue(TEXT("EmissiveColor"), HealColor * 20.0f);
			GlowOrb->SetMaterial(0, Mat);
		}
	}

	// Light color
	if (HealingLight)
	{
		HealingLight->SetLightColor(HealColor);
	}
}

void ASLFHealingSpell::UpdateHealingEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Animate each particle rising
	for (int32 i = 0; i < Particles.Num(); i++)
	{
		UStaticMeshComponent* P = Particles[i];
		if (!P) continue;

		// Calculate particle's current phase in rise cycle
		float Phase = FMath::Fmod(ParticlePhases[i] + AccumulatedTime * 0.5f, 1.0f);

		// Base position (circular)
		float Angle = (i * 60.0f) + AccumulatedTime * 30.0f;  // Slow rotation
		float Radius = EffectRadius * (0.5f + 0.3f * FMath::Sin(Phase * PI * 2.0f));
		float X = FMath::Cos(FMath::DegreesToRadians(Angle)) * Radius;
		float Y = FMath::Sin(FMath::DegreesToRadians(Angle)) * Radius;

		// Rising height
		float Z = Phase * RiseHeight;

		P->SetRelativeLocation(FVector(X, Y, Z));

		// Scale - grow then shrink
		float ScaleFactor = FMath::Sin(Phase * PI);  // 0 at start/end, 1 at middle
		P->SetRelativeScale3D(FVector(0.08f + 0.08f * ScaleFactor));

		// Fade material at top of rise
		if (i < ParticleMaterials.Num() && ParticleMaterials[i])
		{
			FLinearColor Color = (i % 2 == 0) ? HealColor : AccentColor;
			float Intensity = 30.0f * FMath::Sin(Phase * PI);  // Fade at ends
			ParticleMaterials[i]->SetVectorParameterValue(TEXT("EmissiveColor"), Color * Intensity);
		}
	}

	// Pulse central glow
	float GlowPulse = FMath::Sin(AccumulatedTime * 3.0f) * 0.3f + 0.7f;
	if (GlowOrb)
	{
		float BaseScale = 0.4f;
		GlowOrb->SetRelativeScale3D(FVector(BaseScale * GlowPulse));
	}

	// Pulse light
	if (HealingLight)
	{
		HealingLight->SetIntensity(6000.0f + 4000.0f * GlowPulse);
	}

	// Fade out at end of duration
	float FadeStart = Duration * 0.75f;
	if (LifeTime > FadeStart)
	{
		float FadeAlpha = 1.0f - (LifeTime - FadeStart) / (Duration - FadeStart);

		for (UMaterialInstanceDynamic* Mat : ParticleMaterials)
		{
			if (Mat)
			{
				FLinearColor CurrentEmissive;
				Mat->GetVectorParameterValue(TEXT("EmissiveColor"), CurrentEmissive);
				Mat->SetVectorParameterValue(TEXT("EmissiveColor"), CurrentEmissive * FadeAlpha);
			}
		}

		if (HealingLight)
		{
			HealingLight->SetIntensity(HealingLight->Intensity * FadeAlpha);
		}
	}
}
