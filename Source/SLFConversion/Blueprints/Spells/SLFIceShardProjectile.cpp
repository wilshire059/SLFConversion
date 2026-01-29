// SLFIceShardProjectile.cpp
// Ice shard with crystalline shimmer

#include "SLFIceShardProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraComponent.h"

ASLFIceShardProjectile::ASLFIceShardProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Fast ice shard
	ProjectileSpeed = 3200.0f;
	MinDamage = 30.0f;
	MaxDamage = 45.0f;

	// Create elongated ice crystal using cylinder
	IceCrystal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IceCrystal"));
	IceCrystal->SetupAttachment(RootComponent);
	IceCrystal->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IceCrystal->SetCastShadow(false);
	IceCrystal->SetRelativeScale3D(FVector(0.3f, 0.3f, 1.0f));  // Elongated
	IceCrystal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));  // Point forward

	// Load cylinder for crystal shape
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		IceCrystal->SetStaticMesh(CylinderMesh.Object);
	}

	// Inner frost core (sphere)
	FrostCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrostCore"));
	FrostCore->SetupAttachment(RootComponent);
	FrostCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FrostCore->SetCastShadow(false);
	FrostCore->SetRelativeScale3D(FVector(0.25f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		FrostCore->SetStaticMesh(SphereMesh.Object);
	}

	// Ice light (cold blue)
	IceLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("IceLight"));
	IceLight->SetupAttachment(RootComponent);
	IceLight->SetIntensity(6000.0f);
	IceLight->SetAttenuationRadius(350.0f);
	IceLight->SetLightColor(FLinearColor(0.5f, 0.8f, 1.0f));
	IceLight->SetCastShadows(false);
}

void ASLFIceShardProjectile::BeginPlay()
{
	SetupIceEffect();
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[SLFIceShardProjectile] ICE SHARD SPAWNED!"));
}

void ASLFIceShardProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateShimmerEffect(DeltaTime);

	// Spin the crystal as it flies
	if (IceCrystal)
	{
		FRotator Rot = IceCrystal->GetRelativeRotation();
		Rot.Roll += 300.0f * DeltaTime;  // Spin around length axis
		IceCrystal->SetRelativeRotation(Rot);
	}
}

void ASLFIceShardProjectile::SetupIceEffect()
{
	// Scale crystal
	if (IceCrystal)
	{
		float WidthScale = CrystalWidth / 50.0f;
		float LengthScale = CrystalLength / 50.0f;
		IceCrystal->SetRelativeScale3D(FVector(WidthScale, WidthScale, LengthScale));
		IceCrystal->SetVisibility(true);
	}

	if (FrostCore)
	{
		FrostCore->SetRelativeScale3D(FVector(CrystalWidth / 100.0f));
		FrostCore->SetVisibility(true);
	}

	// Crystal material - translucent blue
	if (IceCrystal && IceCrystal->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = IceCrystal->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			CrystalMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			CrystalMaterial->SetVectorParameterValue(TEXT("BaseColor"), IceColor);
			IceCrystal->SetMaterial(0, CrystalMaterial);
		}
	}

	// Frost core - bright white
	if (FrostCore && FrostCore->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = FrostCore->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			FrostMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			FrostMaterial->SetVectorParameterValue(TEXT("BaseColor"), FrostColor);
			FrostCore->SetMaterial(0, FrostMaterial);
		}
	}

	// Disable inherited Niagara
	if (Effect)
	{
		Effect->Deactivate();
		Effect->SetVisibility(false);
	}
}

void ASLFIceShardProjectile::UpdateShimmerEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Crystalline shimmer effect
	float Shimmer = FMath::Sin(AccumulatedTime * ShimmerSpeed) * 0.5f + 0.5f;

	// Pulse light
	if (IceLight)
	{
		IceLight->SetIntensity(4000.0f + 4000.0f * Shimmer);
	}

	// Shimmer the crystal color
	if (CrystalMaterial)
	{
		FLinearColor ShimmerColor = FMath::Lerp(IceColor, FrostColor, Shimmer * 0.3f);
		CrystalMaterial->SetVectorParameterValue(TEXT("BaseColor"), ShimmerColor);
		CrystalMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ShimmerColor * 20.0f * Shimmer);
	}
}
