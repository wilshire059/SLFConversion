// SLFFireballProjectile.cpp
// Fire spell with flickering flames

#include "SLFFireballProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraComponent.h"

ASLFFireballProjectile::ASLFFireballProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Faster fireball
	ProjectileSpeed = 2500.0f;
	MinDamage = 35.0f;
	MaxDamage = 55.0f;

	// Create fire core (bright yellow-orange center)
	FireCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FireCore"));
	FireCore->SetupAttachment(RootComponent);
	FireCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FireCore->SetCastShadow(false);
	FireCore->SetRelativeScale3D(FVector(0.4f));

	// Load sphere mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		FireCore->SetStaticMesh(SphereMesh.Object);
	}

	// Create outer flame layer
	FlameOuter = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameOuter"));
	FlameOuter->SetupAttachment(RootComponent);
	FlameOuter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlameOuter->SetCastShadow(false);
	FlameOuter->SetRelativeScale3D(FVector(0.8f));
	if (SphereMesh.Succeeded())
	{
		FlameOuter->SetStaticMesh(SphereMesh.Object);
	}

	// Fire light (orange glow)
	FireLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FireLight"));
	FireLight->SetupAttachment(RootComponent);
	FireLight->SetIntensity(8000.0f);
	FireLight->SetAttenuationRadius(400.0f);
	FireLight->SetLightColor(FLinearColor(1.0f, 0.5f, 0.1f));
	FireLight->SetCastShadows(false);
}

void ASLFFireballProjectile::BeginPlay()
{
	SetupFireEffect();
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[SLFFireballProjectile] FIREBALL SPAWNED!"));
}

void ASLFFireballProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateFlickerEffect(DeltaTime);

	// Rotate the flame for turbulent look
	if (FlameOuter)
	{
		FRotator Rot = FlameOuter->GetRelativeRotation();
		Rot.Yaw += 200.0f * DeltaTime;
		Rot.Pitch += 150.0f * DeltaTime;
		FlameOuter->SetRelativeRotation(Rot);
	}
}

void ASLFFireballProjectile::SetupFireEffect()
{
	// Scale meshes
	if (FireCore)
	{
		float CoreScale = CoreRadius / 50.0f;
		FireCore->SetRelativeScale3D(FVector(CoreScale));
		FireCore->SetVisibility(true);
	}

	if (FlameOuter)
	{
		float FlameScale = FlameRadius / 50.0f;
		FlameOuter->SetRelativeScale3D(FVector(FlameScale));
		FlameOuter->SetVisibility(true);
	}

	// Core material - bright yellow-orange
	if (FireCore && FireCore->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = FireCore->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			CoreMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			CoreMaterial->SetVectorParameterValue(TEXT("BaseColor"), CoreColor);
			FireCore->SetMaterial(0, CoreMaterial);
		}
	}

	// Flame material - deep orange-red
	if (FlameOuter && FlameOuter->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = FlameOuter->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			FlameMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			FlameMaterial->SetVectorParameterValue(TEXT("BaseColor"), FlameColor);
			FlameOuter->SetMaterial(0, FlameMaterial);
		}
	}

	// Disable inherited Niagara
	if (Effect)
	{
		Effect->Deactivate();
		Effect->SetVisibility(false);
	}
}

void ASLFFireballProjectile::UpdateFlickerEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Random-ish flicker using multiple sine waves
	float Flicker1 = FMath::Sin(AccumulatedTime * FlickerSpeed) * 0.5f;
	float Flicker2 = FMath::Sin(AccumulatedTime * FlickerSpeed * 1.7f) * 0.3f;
	float Flicker3 = FMath::Sin(AccumulatedTime * FlickerSpeed * 2.3f) * 0.2f;
	float FlickerFactor = 1.0f + (Flicker1 + Flicker2 + Flicker3) * FlickerIntensity;

	// Update light intensity
	if (FireLight)
	{
		FireLight->SetIntensity(8000.0f * FlickerFactor);
	}

	// Pulse flame scale slightly
	if (FlameOuter)
	{
		float BaseScale = FlameRadius / 50.0f;
		float PulsedScale = BaseScale * (1.0f + (FlickerFactor - 1.0f) * 0.2f);
		FlameOuter->SetRelativeScale3D(FVector(PulsedScale));
	}

	// Vary flame color intensity
	if (FlameMaterial)
	{
		FLinearColor PulsedColor = FlameColor * FlickerFactor;
		FlameMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), PulsedColor * 30.0f);
	}
}
