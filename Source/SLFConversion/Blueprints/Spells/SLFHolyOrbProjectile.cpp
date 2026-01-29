// SLFHolyOrbProjectile.cpp
// Holy orb with divine radiance

#include "SLFHolyOrbProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraComponent.h"

ASLFHolyOrbProjectile::ASLFHolyOrbProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Moderate speed holy projectile
	ProjectileSpeed = 2200.0f;
	MinDamage = 45.0f;
	MaxDamage = 65.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Divine core (golden center)
	HolyCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HolyCore"));
	HolyCore->SetupAttachment(RootComponent);
	HolyCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HolyCore->SetCastShadow(false);
	HolyCore->SetRelativeScale3D(FVector(0.36f));
	if (SphereMesh.Succeeded())
	{
		HolyCore->SetStaticMesh(SphereMesh.Object);
	}

	// Halo ring (larger transparent sphere)
	Halo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Halo"));
	Halo->SetupAttachment(RootComponent);
	Halo->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Halo->SetCastShadow(false);
	Halo->SetRelativeScale3D(FVector(0.7f));
	if (SphereMesh.Succeeded())
	{
		Halo->SetStaticMesh(SphereMesh.Object);
	}

	// Vertical beam (cross)
	BeamVertical = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeamVertical"));
	BeamVertical->SetupAttachment(RootComponent);
	BeamVertical->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BeamVertical->SetCastShadow(false);
	BeamVertical->SetRelativeScale3D(FVector(0.02f, 0.02f, 0.8f));
	if (CubeMesh.Succeeded())
	{
		BeamVertical->SetStaticMesh(CubeMesh.Object);
	}

	// Horizontal beam (cross)
	BeamHorizontal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeamHorizontal"));
	BeamHorizontal->SetupAttachment(RootComponent);
	BeamHorizontal->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BeamHorizontal->SetCastShadow(false);
	BeamHorizontal->SetRelativeScale3D(FVector(0.8f, 0.02f, 0.02f));
	if (CubeMesh.Succeeded())
	{
		BeamHorizontal->SetStaticMesh(CubeMesh.Object);
	}

	// Warm divine light
	DivineLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("DivineLight"));
	DivineLight->SetupAttachment(RootComponent);
	DivineLight->SetIntensity(12000.0f);
	DivineLight->SetAttenuationRadius(450.0f);
	DivineLight->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f));
	DivineLight->SetCastShadows(false);
}

void ASLFHolyOrbProjectile::BeginPlay()
{
	SetupHolyEffect();
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[SLFHolyOrbProjectile] HOLY ORB SPAWNED!"));
}

void ASLFHolyOrbProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateRadianceEffect(DeltaTime);

	// Slowly rotate the cross beams
	if (BeamVertical && BeamHorizontal)
	{
		FRotator Rot = BeamVertical->GetRelativeRotation();
		Rot.Yaw += BeamRotationSpeed * DeltaTime;
		BeamVertical->SetRelativeRotation(Rot);
		BeamHorizontal->SetRelativeRotation(Rot);
	}
}

void ASLFHolyOrbProjectile::SetupHolyEffect()
{
	// Scale components
	if (HolyCore)
	{
		float Scale = CoreRadius / 50.0f;
		HolyCore->SetRelativeScale3D(FVector(Scale));
		HolyCore->SetVisibility(true);
	}

	if (Halo)
	{
		float Scale = HaloRadius / 50.0f;
		Halo->SetRelativeScale3D(FVector(Scale));
		Halo->SetVisibility(true);
	}

	// Core material - warm gold
	if (HolyCore && HolyCore->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = HolyCore->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			CoreMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			CoreMaterial->SetVectorParameterValue(TEXT("BaseColor"), GoldColor);
			CoreMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), GoldColor * 25.0f);
			HolyCore->SetMaterial(0, CoreMaterial);
		}
	}

	// Halo material - soft white
	if (Halo && Halo->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = Halo->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			HaloMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			HaloMaterial->SetVectorParameterValue(TEXT("BaseColor"), WhiteColor * 0.5f);
			Halo->SetMaterial(0, HaloMaterial);
		}
	}

	// Beam material - bright white
	auto SetupBeam = [this](UStaticMeshComponent* Beam)
	{
		if (Beam && Beam->GetStaticMesh())
		{
			UMaterialInterface* BaseMat = Beam->GetMaterial(0);
			if (!BaseMat)
			{
				BaseMat = LoadObject<UMaterialInterface>(nullptr,
					TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
			}
			if (BaseMat)
			{
				BeamMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
				BeamMaterial->SetVectorParameterValue(TEXT("BaseColor"), WhiteColor);
				BeamMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), WhiteColor * 30.0f);
				Beam->SetMaterial(0, BeamMaterial);
			}
		}
	};

	SetupBeam(BeamVertical);
	SetupBeam(BeamHorizontal);

	// Disable inherited Niagara
	if (Effect)
	{
		Effect->Deactivate();
		Effect->SetVisibility(false);
	}
}

void ASLFHolyOrbProjectile::UpdateRadianceEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Soft pulsing radiance
	float Radiance = FMath::Sin(AccumulatedTime * RadianceSpeed * PI) * 0.3f + 0.7f;

	// Pulse light
	if (DivineLight)
	{
		DivineLight->SetIntensity(8000.0f + 8000.0f * Radiance);
	}

	// Pulse halo size
	if (Halo)
	{
		float BaseScale = HaloRadius / 50.0f;
		float PulsedScale = BaseScale * (0.9f + 0.2f * Radiance);
		Halo->SetRelativeScale3D(FVector(PulsedScale));
	}

	// Pulse core emissive
	if (CoreMaterial)
	{
		CoreMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), GoldColor * (20.0f + 15.0f * Radiance));
	}
}
