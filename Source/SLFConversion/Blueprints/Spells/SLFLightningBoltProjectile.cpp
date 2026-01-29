// SLFLightningBoltProjectile.cpp
// Lightning bolt with electric flashing

#include "SLFLightningBoltProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraComponent.h"

ASLFLightningBoltProjectile::ASLFLightningBoltProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Very fast lightning
	ProjectileSpeed = 5000.0f;
	MinDamage = 40.0f;
	MaxDamage = 70.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Electric core (bright center)
	BoltCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoltCore"));
	BoltCore->SetupAttachment(RootComponent);
	BoltCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoltCore->SetCastShadow(false);
	BoltCore->SetRelativeScale3D(FVector(0.3f));
	if (SphereMesh.Succeeded())
	{
		BoltCore->SetStaticMesh(SphereMesh.Object);
	}

	// Arc 1 - elongated cube for jagged look
	Arc1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arc1"));
	Arc1->SetupAttachment(RootComponent);
	Arc1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Arc1->SetCastShadow(false);
	Arc1->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.6f));
	Arc1->SetRelativeLocation(FVector(20.0f, 10.0f, 0.0f));
	Arc1->SetRelativeRotation(FRotator(0.0f, 45.0f, 30.0f));
	if (CubeMesh.Succeeded())
	{
		Arc1->SetStaticMesh(CubeMesh.Object);
	}

	// Arc 2 - another jagged piece
	Arc2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arc2"));
	Arc2->SetupAttachment(RootComponent);
	Arc2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Arc2->SetCastShadow(false);
	Arc2->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.5f));
	Arc2->SetRelativeLocation(FVector(-15.0f, -10.0f, 5.0f));
	Arc2->SetRelativeRotation(FRotator(0.0f, -30.0f, -45.0f));
	if (CubeMesh.Succeeded())
	{
		Arc2->SetStaticMesh(CubeMesh.Object);
	}

	// Very bright electric light
	ElectricLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ElectricLight"));
	ElectricLight->SetupAttachment(RootComponent);
	ElectricLight->SetIntensity(15000.0f);
	ElectricLight->SetAttenuationRadius(500.0f);
	ElectricLight->SetLightColor(FLinearColor(1.0f, 1.0f, 0.7f));
	ElectricLight->SetCastShadows(false);
}

void ASLFLightningBoltProjectile::BeginPlay()
{
	SetupLightningEffect();
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[SLFLightningBoltProjectile] LIGHTNING SPAWNED!"));
}

void ASLFLightningBoltProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateElectricEffect(DeltaTime);
}

void ASLFLightningBoltProjectile::SetupLightningEffect()
{
	// Scale core
	if (BoltCore)
	{
		float Scale = CoreRadius / 50.0f;
		BoltCore->SetRelativeScale3D(FVector(Scale));
		BoltCore->SetVisibility(true);
	}

	// Core material - bright yellow
	if (BoltCore && BoltCore->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = BoltCore->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			CoreMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			CoreMaterial->SetVectorParameterValue(TEXT("BaseColor"), LightningColor);
			CoreMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), LightningColor * 50.0f);
			BoltCore->SetMaterial(0, CoreMaterial);
		}
	}

	// Arc materials - blue-white
	auto SetupArcMaterial = [this](UStaticMeshComponent* Arc, UMaterialInstanceDynamic*& OutMat)
	{
		if (Arc && Arc->GetStaticMesh())
		{
			UMaterialInterface* BaseMat = Arc->GetMaterial(0);
			if (!BaseMat)
			{
				BaseMat = LoadObject<UMaterialInterface>(nullptr,
					TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
			}
			if (BaseMat)
			{
				OutMat = UMaterialInstanceDynamic::Create(BaseMat, this);
				OutMat->SetVectorParameterValue(TEXT("BaseColor"), ArcColor);
				OutMat->SetVectorParameterValue(TEXT("EmissiveColor"), ArcColor * 40.0f);
				Arc->SetMaterial(0, OutMat);
			}
		}
	};

	SetupArcMaterial(Arc1, ArcMaterial1);
	SetupArcMaterial(Arc2, ArcMaterial2);

	// Disable inherited Niagara
	if (Effect)
	{
		Effect->Deactivate();
		Effect->SetVisibility(false);
	}
}

void ASLFLightningBoltProjectile::UpdateElectricEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Random flash timing
	if (AccumulatedTime >= NextFlashTime)
	{
		bFlashOn = !bFlashOn;
		NextFlashTime = AccumulatedTime + FMath::RandRange(0.02f, 0.08f);

		// Random intensity when on
		float Intensity = bFlashOn ? FMath::RandRange(10000.0f, 20000.0f) : 2000.0f;
		if (ElectricLight)
		{
			ElectricLight->SetIntensity(Intensity);
		}
	}

	// Jitter the arcs randomly
	if (Arc1)
	{
		FVector Jitter(
			FMath::RandRange(-ArcJitterAmount, ArcJitterAmount),
			FMath::RandRange(-ArcJitterAmount, ArcJitterAmount),
			FMath::RandRange(-ArcJitterAmount, ArcJitterAmount)
		);
		Arc1->SetRelativeLocation(FVector(20.0f, 10.0f, 0.0f) + Jitter);

		FRotator RandRot(
			FMath::RandRange(-20.0f, 20.0f),
			FMath::RandRange(-20.0f, 20.0f),
			FMath::RandRange(-20.0f, 20.0f)
		);
		Arc1->SetRelativeRotation(FRotator(0.0f, 45.0f, 30.0f) + RandRot);
	}

	if (Arc2)
	{
		FVector Jitter(
			FMath::RandRange(-ArcJitterAmount, ArcJitterAmount),
			FMath::RandRange(-ArcJitterAmount, ArcJitterAmount),
			FMath::RandRange(-ArcJitterAmount, ArcJitterAmount)
		);
		Arc2->SetRelativeLocation(FVector(-15.0f, -10.0f, 5.0f) + Jitter);
	}

	// Pulse emissive
	float EmissivePulse = bFlashOn ? 60.0f : 20.0f;
	if (CoreMaterial)
	{
		CoreMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), LightningColor * EmissivePulse);
	}
}
