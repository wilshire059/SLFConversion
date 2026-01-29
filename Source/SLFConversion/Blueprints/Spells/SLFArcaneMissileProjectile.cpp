// SLFArcaneMissileProjectile.cpp
// Arcane missile with sparkles and runes

#include "SLFArcaneMissileProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraComponent.h"

ASLFArcaneMissileProjectile::ASLFArcaneMissileProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileSpeed = 2800.0f;
	MinDamage = 30.0f;
	MaxDamage = 50.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Arcane core (purple center)
	ArcaneCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcaneCore"));
	ArcaneCore->SetupAttachment(RootComponent);
	ArcaneCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArcaneCore->SetCastShadow(false);
	ArcaneCore->SetRelativeScale3D(FVector(0.3f));
	if (SphereMesh.Succeeded())
	{
		ArcaneCore->SetStaticMesh(SphereMesh.Object);
	}

	// Magic aura (larger transparent sphere)
	MagicAura = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagicAura"));
	MagicAura->SetupAttachment(RootComponent);
	MagicAura->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MagicAura->SetCastShadow(false);
	MagicAura->SetRelativeScale3D(FVector(0.6f));
	if (SphereMesh.Succeeded())
	{
		MagicAura->SetStaticMesh(SphereMesh.Object);
	}

	// Rune rings (flat cubes rotating around)
	RuneRing1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RuneRing1"));
	RuneRing1->SetupAttachment(RootComponent);
	RuneRing1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RuneRing1->SetCastShadow(false);
	RuneRing1->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.02f));  // Flat disc shape
	RuneRing1->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
	if (CubeMesh.Succeeded())
	{
		RuneRing1->SetStaticMesh(CubeMesh.Object);
	}

	RuneRing2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RuneRing2"));
	RuneRing2->SetupAttachment(RootComponent);
	RuneRing2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RuneRing2->SetCastShadow(false);
	RuneRing2->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.02f));
	RuneRing2->SetRelativeLocation(FVector(0.0f, 0.0f, -20.0f));
	RuneRing2->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));
	if (CubeMesh.Succeeded())
	{
		RuneRing2->SetStaticMesh(CubeMesh.Object);
	}

	// Sparkle points (tiny bright spheres)
	UStaticMesh* SphereMeshPtr = SphereMesh.Succeeded() ? SphereMesh.Object : nullptr;
	auto CreateSparkle = [this, SphereMeshPtr](const TCHAR* Name, FVector Location) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* Sparkle = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		Sparkle->SetupAttachment(RootComponent);
		Sparkle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sparkle->SetCastShadow(false);
		Sparkle->SetRelativeScale3D(FVector(0.05f));
		Sparkle->SetRelativeLocation(Location);
		if (SphereMeshPtr)
		{
			Sparkle->SetStaticMesh(SphereMeshPtr);
		}
		return Sparkle;
	};

	Sparkle1 = CreateSparkle(TEXT("Sparkle1"), FVector(20.0f, 0.0f, 0.0f));
	Sparkle2 = CreateSparkle(TEXT("Sparkle2"), FVector(-10.0f, 17.0f, 0.0f));
	Sparkle3 = CreateSparkle(TEXT("Sparkle3"), FVector(-10.0f, -17.0f, 0.0f));

	// Magical purple light
	ArcaneLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ArcaneLight"));
	ArcaneLight->SetupAttachment(RootComponent);
	ArcaneLight->SetIntensity(8000.0f);
	ArcaneLight->SetAttenuationRadius(350.0f);
	ArcaneLight->SetLightColor(FLinearColor(0.7f, 0.3f, 1.0f));
	ArcaneLight->SetCastShadows(false);
}

void ASLFArcaneMissileProjectile::BeginPlay()
{
	SetupArcaneEffect();
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[SLFArcaneMissileProjectile] ARCANE MISSILE SPAWNED!"));
}

void ASLFArcaneMissileProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMagicalEffect(DeltaTime);
}

void ASLFArcaneMissileProjectile::SetupArcaneEffect()
{
	// Scale core
	if (ArcaneCore)
	{
		float Scale = CoreRadius / 50.0f;
		ArcaneCore->SetRelativeScale3D(FVector(Scale));
		ArcaneCore->SetVisibility(true);
	}

	// Core material - deep purple
	if (ArcaneCore && ArcaneCore->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = ArcaneCore->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			CoreMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			CoreMaterial->SetVectorParameterValue(TEXT("BaseColor"), ArcaneColor);
			CoreMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ArcaneColor * 30.0f);
			ArcaneCore->SetMaterial(0, CoreMaterial);
		}
	}

	// Aura material - translucent purple
	if (MagicAura && MagicAura->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = MagicAura->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			AuraMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			AuraMaterial->SetVectorParameterValue(TEXT("BaseColor"), ArcaneColor * 0.3f);
			MagicAura->SetMaterial(0, AuraMaterial);
		}
	}

	// Rune material - bright purple
	auto SetupRune = [this](UStaticMeshComponent* Rune)
	{
		if (Rune && Rune->GetStaticMesh())
		{
			UMaterialInterface* BaseMat = Rune->GetMaterial(0);
			if (!BaseMat)
			{
				BaseMat = LoadObject<UMaterialInterface>(nullptr,
					TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
			}
			if (BaseMat)
			{
				RuneMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
				RuneMaterial->SetVectorParameterValue(TEXT("BaseColor"), ArcaneColor);
				RuneMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ArcaneColor * 25.0f);
				Rune->SetMaterial(0, RuneMaterial);
			}
		}
	};

	SetupRune(RuneRing1);
	SetupRune(RuneRing2);

	// Sparkle material - bright pink
	auto SetupSparkle = [this](UStaticMeshComponent* Sparkle)
	{
		if (Sparkle && Sparkle->GetStaticMesh())
		{
			UMaterialInterface* BaseMat = Sparkle->GetMaterial(0);
			if (!BaseMat)
			{
				BaseMat = LoadObject<UMaterialInterface>(nullptr,
					TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
			}
			if (BaseMat)
			{
				SparkleMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
				SparkleMaterial->SetVectorParameterValue(TEXT("BaseColor"), SparkleColor);
				SparkleMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), SparkleColor * 50.0f);
				Sparkle->SetMaterial(0, SparkleMaterial);
			}
		}
	};

	SetupSparkle(Sparkle1);
	SetupSparkle(Sparkle2);
	SetupSparkle(Sparkle3);

	// Disable inherited Niagara
	if (Effect)
	{
		Effect->Deactivate();
		Effect->SetVisibility(false);
	}
}

void ASLFArcaneMissileProjectile::UpdateMagicalEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Rotate rune rings
	if (RuneRing1)
	{
		FRotator Rot = RuneRing1->GetRelativeRotation();
		Rot.Yaw += RuneRotationSpeed * DeltaTime;
		RuneRing1->SetRelativeRotation(Rot);
	}

	if (RuneRing2)
	{
		FRotator Rot = RuneRing2->GetRelativeRotation();
		Rot.Yaw -= RuneRotationSpeed * 0.7f * DeltaTime;  // Counter-rotate
		RuneRing2->SetRelativeRotation(Rot);
	}

	// Orbit sparkles around the core
	float Angle1 = AccumulatedTime * SparkleSpeed;
	float Angle2 = AccumulatedTime * SparkleSpeed + 2.094f;  // 120 degrees offset
	float Angle3 = AccumulatedTime * SparkleSpeed + 4.189f;  // 240 degrees offset

	float OrbitRadius = 25.0f;
	float VerticalOscillation = FMath::Sin(AccumulatedTime * SparkleSpeed * 0.5f) * 10.0f;

	if (Sparkle1)
	{
		Sparkle1->SetRelativeLocation(FVector(
			FMath::Cos(Angle1) * OrbitRadius,
			FMath::Sin(Angle1) * OrbitRadius,
			VerticalOscillation
		));
	}

	if (Sparkle2)
	{
		Sparkle2->SetRelativeLocation(FVector(
			FMath::Cos(Angle2) * OrbitRadius,
			FMath::Sin(Angle2) * OrbitRadius,
			-VerticalOscillation
		));
	}

	if (Sparkle3)
	{
		Sparkle3->SetRelativeLocation(FVector(
			FMath::Cos(Angle3) * OrbitRadius,
			FMath::Sin(Angle3) * OrbitRadius,
			VerticalOscillation * 0.5f
		));
	}

	// Pulse light and emissive
	float Pulse = FMath::Sin(AccumulatedTime * SparkleSpeed * 0.3f) * 0.3f + 0.7f;
	if (ArcaneLight)
	{
		ArcaneLight->SetIntensity(6000.0f + 4000.0f * Pulse);
	}

	if (CoreMaterial)
	{
		CoreMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), ArcaneColor * (25.0f + 15.0f * Pulse));
	}
}
