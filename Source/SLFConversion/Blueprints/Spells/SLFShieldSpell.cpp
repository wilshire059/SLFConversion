// SLFShieldSpell.cpp
// Protective shield barrier

#include "SLFShieldSpell.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASLFShieldSpell::ASLFShieldSpell()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	// Main shield sphere (outer)
	ShieldSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldSphere"));
	ShieldSphere->SetupAttachment(Root);
	ShieldSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldSphere->SetCastShadow(false);
	ShieldSphere->SetRelativeScale3D(FVector(3.0f));
	if (SphereMesh.Succeeded())
	{
		ShieldSphere->SetStaticMesh(SphereMesh.Object);
	}

	// Inner shield layer
	InnerShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InnerShield"));
	InnerShield->SetupAttachment(Root);
	InnerShield->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InnerShield->SetCastShadow(false);
	InnerShield->SetRelativeScale3D(FVector(2.8f));
	if (SphereMesh.Succeeded())
	{
		InnerShield->SetStaticMesh(SphereMesh.Object);
	}

	// Rotating rune ring at equator
	RuneRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RuneRing"));
	RuneRing->SetupAttachment(Root);
	RuneRing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RuneRing->SetCastShadow(false);
	RuneRing->SetRelativeScale3D(FVector(3.2f, 3.2f, 0.05f));  // Flat ring
	if (CylinderMesh.Succeeded())
	{
		RuneRing->SetStaticMesh(CylinderMesh.Object);
	}

	// Inner glow light
	ShieldLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ShieldLight"));
	ShieldLight->SetupAttachment(Root);
	ShieldLight->SetIntensity(5000.0f);
	ShieldLight->SetAttenuationRadius(300.0f);
	ShieldLight->SetCastShadows(false);

	ShieldOwner = nullptr;
}

void ASLFShieldSpell::BeginPlay()
{
	Super::BeginPlay();
	SetupShieldEffect();

	UE_LOG(LogTemp, Warning, TEXT("[SLFShieldSpell] SHIELD SPAWNED!"));
}

void ASLFShieldSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LifeTime += DeltaTime;
	if (LifeTime >= Duration)
	{
		Destroy();
		return;
	}

	// Follow owner if set
	if (ShieldOwner)
	{
		SetActorLocation(ShieldOwner->GetActorLocation());
	}

	UpdateShieldEffect(DeltaTime);
}

void ASLFShieldSpell::SetupShieldEffect()
{
	// Scale based on shield radius
	float Scale = ShieldRadius / 50.0f;

	if (ShieldSphere)
	{
		ShieldSphere->SetRelativeScale3D(FVector(Scale));
		ShieldSphere->SetVisibility(true);
	}

	if (InnerShield)
	{
		InnerShield->SetRelativeScale3D(FVector(Scale * 0.95f));
		InnerShield->SetVisibility(true);
	}

	if (RuneRing)
	{
		RuneRing->SetRelativeScale3D(FVector(Scale * 1.05f, Scale * 1.05f, 0.02f));
		RuneRing->SetVisibility(true);
	}

	// Shield material - translucent blue
	if (ShieldSphere && ShieldSphere->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = ShieldSphere->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			ShieldMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			ShieldMaterial->SetVectorParameterValue(TEXT("BaseColor"), ShieldColor * 0.3f);
			ShieldMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ShieldColor * 5.0f);
			ShieldSphere->SetMaterial(0, ShieldMaterial);
		}
	}

	// Inner layer - slightly brighter
	if (InnerShield && InnerShield->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = InnerShield->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			InnerMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			InnerMaterial->SetVectorParameterValue(TEXT("BaseColor"), ShieldColor * 0.2f);
			InnerMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ShieldColor * 8.0f);
			InnerShield->SetMaterial(0, InnerMaterial);
		}
	}

	// Rune ring material
	if (RuneRing && RuneRing->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = RuneRing->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			UMaterialInstanceDynamic* RuneMat = UMaterialInstanceDynamic::Create(BaseMat, this);
			RuneMat->SetVectorParameterValue(TEXT("BaseColor"), ShieldColor);
			RuneMat->SetVectorParameterValue(TEXT("EmissiveColor"), ShieldColor * 20.0f);
			RuneRing->SetMaterial(0, RuneMat);
		}
	}

	// Set light color
	if (ShieldLight)
	{
		ShieldLight->SetLightColor(ShieldColor);
	}
}

void ASLFShieldSpell::UpdateShieldEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Pulse effect
	float Pulse = FMath::Sin(AccumulatedTime * PulseSpeed * PI) * 0.2f + 0.8f;

	// Rotate rune ring
	if (RuneRing)
	{
		FRotator Rot = RuneRing->GetRelativeRotation();
		Rot.Yaw += RotationSpeed * DeltaTime;
		RuneRing->SetRelativeRotation(Rot);
	}

	// Pulse shield opacity/emissive
	if (ShieldMaterial)
	{
		ShieldMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ShieldColor * (4.0f + 3.0f * Pulse));
	}

	if (InnerMaterial)
	{
		InnerMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ShieldColor * (6.0f + 4.0f * Pulse));
	}

	// Pulse light
	if (ShieldLight)
	{
		ShieldLight->SetIntensity(4000.0f + 2000.0f * Pulse);
	}

	// Fade out near end
	float FadeStart = Duration * 0.8f;
	if (LifeTime > FadeStart)
	{
		float FadeAlpha = 1.0f - (LifeTime - FadeStart) / (Duration - FadeStart);

		if (ShieldMaterial)
		{
			ShieldMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ShieldColor * 5.0f * FadeAlpha);
		}
		if (InnerMaterial)
		{
			InnerMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ShieldColor * 8.0f * FadeAlpha);
		}
		if (ShieldLight)
		{
			ShieldLight->SetIntensity(5000.0f * FadeAlpha);
		}
	}
}
