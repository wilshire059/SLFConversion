// SLFSummoningCircle.cpp
// Summoning circle with rotating runes and energy pillar

#include "SLFSummoningCircle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASLFSummoningCircle::ASLFSummoningCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Outer ring (flat cylinder)
	OuterRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OuterRing"));
	OuterRing->SetupAttachment(Root);
	OuterRing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OuterRing->SetCastShadow(false);
	OuterRing->SetRelativeScale3D(FVector(4.0f, 4.0f, 0.02f));
	OuterRing->SetRelativeLocation(FVector(0.0f, 0.0f, 1.0f));  // Slightly above ground
	if (CylinderMesh.Succeeded())
	{
		OuterRing->SetStaticMesh(CylinderMesh.Object);
	}

	// Inner ring
	InnerRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InnerRing"));
	InnerRing->SetupAttachment(Root);
	InnerRing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InnerRing->SetCastShadow(false);
	InnerRing->SetRelativeScale3D(FVector(2.5f, 2.5f, 0.025f));
	InnerRing->SetRelativeLocation(FVector(0.0f, 0.0f, 2.0f));
	if (CylinderMesh.Succeeded())
	{
		InnerRing->SetStaticMesh(CylinderMesh.Object);
	}

	// Center symbol (flat square)
	CenterSymbol = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CenterSymbol"));
	CenterSymbol->SetupAttachment(Root);
	CenterSymbol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CenterSymbol->SetCastShadow(false);
	CenterSymbol->SetRelativeScale3D(FVector(1.5f, 1.5f, 0.03f));
	CenterSymbol->SetRelativeLocation(FVector(0.0f, 0.0f, 3.0f));
	CenterSymbol->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));  // Diamond orientation
	if (CubeMesh.Succeeded())
	{
		CenterSymbol->SetStaticMesh(CubeMesh.Object);
	}

	// Rune markers at cardinal points
	UStaticMesh* CubeMeshPtr = CubeMesh.Succeeded() ? CubeMesh.Object : nullptr;
	auto CreateRune = [this, CubeMeshPtr](const TCHAR* Name, float Angle) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* Rune = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		Rune->SetupAttachment(Root);
		Rune->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Rune->SetCastShadow(false);
		Rune->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.5f));
		float Rad = FMath::DegreesToRadians(Angle);
		float Dist = 150.0f;  // Distance from center
		Rune->SetRelativeLocation(FVector(FMath::Cos(Rad) * Dist, FMath::Sin(Rad) * Dist, 25.0f));
		if (CubeMeshPtr)
		{
			Rune->SetStaticMesh(CubeMeshPtr);
		}
		return Rune;
	};

	Rune1 = CreateRune(TEXT("Rune1"), 0.0f);
	Rune2 = CreateRune(TEXT("Rune2"), 90.0f);
	Rune3 = CreateRune(TEXT("Rune3"), 180.0f);
	Rune4 = CreateRune(TEXT("Rune4"), 270.0f);

	// Energy pillar (tall cylinder in center)
	EnergyPillar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnergyPillar"));
	EnergyPillar->SetupAttachment(Root);
	EnergyPillar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnergyPillar->SetCastShadow(false);
	EnergyPillar->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.0f));  // Start at 0 height
	EnergyPillar->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	if (CylinderMesh.Succeeded())
	{
		EnergyPillar->SetStaticMesh(CylinderMesh.Object);
	}

	// Circle ground light
	CircleLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("CircleLight"));
	CircleLight->SetupAttachment(Root);
	CircleLight->SetIntensity(10000.0f);
	CircleLight->SetAttenuationRadius(400.0f);
	CircleLight->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	CircleLight->SetCastShadows(false);

	// Pillar top light
	PillarLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PillarLight"));
	PillarLight->SetupAttachment(Root);
	PillarLight->SetIntensity(15000.0f);
	PillarLight->SetAttenuationRadius(500.0f);
	PillarLight->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	PillarLight->SetCastShadows(false);
}

void ASLFSummoningCircle::BeginPlay()
{
	Super::BeginPlay();
	SetupCircleEffect();

	UE_LOG(LogTemp, Warning, TEXT("[SLFSummoningCircle] SUMMONING CIRCLE SPAWNED!"));
}

void ASLFSummoningCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LifeTime += DeltaTime;
	if (LifeTime >= Duration)
	{
		Destroy();
		return;
	}

	UpdateCircleEffect(DeltaTime);
}

void ASLFSummoningCircle::SetupCircleEffect()
{
	// Scale based on circle radius
	float Scale = CircleRadius / 50.0f;

	if (OuterRing)
	{
		OuterRing->SetRelativeScale3D(FVector(Scale, Scale, 0.02f));
		OuterRing->SetVisibility(true);
	}

	if (InnerRing)
	{
		InnerRing->SetRelativeScale3D(FVector(Scale * 0.6f, Scale * 0.6f, 0.025f));
		InnerRing->SetVisibility(true);
	}

	if (CenterSymbol)
	{
		CenterSymbol->SetRelativeScale3D(FVector(Scale * 0.3f, Scale * 0.3f, 0.03f));
		CenterSymbol->SetVisibility(true);
	}

	// Position runes at proper radius
	float RuneDist = CircleRadius * 0.75f;
	auto PositionRune = [RuneDist](UStaticMeshComponent* Rune, float Angle)
	{
		if (Rune)
		{
			float Rad = FMath::DegreesToRadians(Angle);
			Rune->SetRelativeLocation(FVector(FMath::Cos(Rad) * RuneDist, FMath::Sin(Rad) * RuneDist, 25.0f));
		}
	};

	PositionRune(Rune1, 0.0f);
	PositionRune(Rune2, 90.0f);
	PositionRune(Rune3, 180.0f);
	PositionRune(Rune4, 270.0f);

	// Setup materials
	auto SetupMaterial = [this](UStaticMeshComponent* Mesh, FLinearColor Color, float EmissiveMult) -> UMaterialInstanceDynamic*
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
				Mat->SetVectorParameterValue(TEXT("BaseColor"), Color);
				Mat->SetVectorParameterValue(TEXT("EmissiveColor"), Color * EmissiveMult);
				Mesh->SetMaterial(0, Mat);
				return Mat;
			}
		}
		return nullptr;
	};

	OuterMaterial = SetupMaterial(OuterRing, CircleColor, 25.0f);
	InnerMaterial = SetupMaterial(InnerRing, CircleColor, 35.0f);
	SetupMaterial(CenterSymbol, CircleColor, 40.0f);
	SetupMaterial(Rune1, CircleColor, 50.0f);
	SetupMaterial(Rune2, CircleColor, 50.0f);
	SetupMaterial(Rune3, CircleColor, 50.0f);
	SetupMaterial(Rune4, CircleColor, 50.0f);
	EnergyMaterial = SetupMaterial(EnergyPillar, EnergyColor, 60.0f);

	// Set light colors
	if (CircleLight)
	{
		CircleLight->SetLightColor(CircleColor);
	}
	if (PillarLight)
	{
		PillarLight->SetLightColor(EnergyColor);
	}
}

void ASLFSummoningCircle::UpdateCircleEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Rotate center symbol and runes
	float RotAngle = AccumulatedTime * RotationSpeed;

	if (CenterSymbol)
	{
		CenterSymbol->SetRelativeRotation(FRotator(0.0f, 45.0f + RotAngle, 0.0f));
	}

	// Runes orbit around
	float RuneDist = CircleRadius * 0.75f;
	auto RotateRune = [this, RuneDist, RotAngle](UStaticMeshComponent* Rune, float BaseAngle)
	{
		if (Rune)
		{
			float Angle = BaseAngle + RotAngle * 0.5f;  // Slower orbit
			float Rad = FMath::DegreesToRadians(Angle);
			Rune->SetRelativeLocation(FVector(FMath::Cos(Rad) * RuneDist, FMath::Sin(Rad) * RuneDist, 25.0f));
			// Spin the rune itself
			Rune->SetRelativeRotation(FRotator(0.0f, RotAngle * 2.0f, 0.0f));
		}
	};

	RotateRune(Rune1, 0.0f);
	RotateRune(Rune2, 90.0f);
	RotateRune(Rune3, 180.0f);
	RotateRune(Rune4, 270.0f);

	// Energy pillar grows over time
	float PillarProgress = FMath::Clamp(LifeTime / (Duration * 0.5f), 0.0f, 1.0f);
	float PillarHeight = 300.0f * PillarProgress;
	if (EnergyPillar)
	{
		float HScale = 0.5f * (1.0f + FMath::Sin(AccumulatedTime * PulseSpeed * PI) * 0.2f);
		EnergyPillar->SetRelativeScale3D(FVector(HScale, HScale, PillarHeight / 100.0f));
		EnergyPillar->SetRelativeLocation(FVector(0.0f, 0.0f, PillarHeight / 2.0f));
	}

	if (PillarLight)
	{
		PillarLight->SetRelativeLocation(FVector(0.0f, 0.0f, PillarHeight));
	}

	// Pulse effect
	float Pulse = FMath::Sin(AccumulatedTime * PulseSpeed * PI) * 0.3f + 0.7f;

	if (OuterMaterial)
	{
		OuterMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), CircleColor * (20.0f + 15.0f * Pulse));
	}

	if (InnerMaterial)
	{
		InnerMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), CircleColor * (30.0f + 20.0f * Pulse));
	}

	if (CircleLight)
	{
		CircleLight->SetIntensity(8000.0f + 5000.0f * Pulse);
	}

	if (PillarLight)
	{
		PillarLight->SetIntensity(10000.0f * PillarProgress + 8000.0f * Pulse);
	}

	// Fade out at end
	float FadeStart = Duration * 0.75f;
	if (LifeTime > FadeStart)
	{
		float FadeAlpha = 1.0f - (LifeTime - FadeStart) / (Duration - FadeStart);

		if (OuterMaterial)
		{
			OuterMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), CircleColor * 25.0f * FadeAlpha);
		}
		if (InnerMaterial)
		{
			InnerMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), CircleColor * 35.0f * FadeAlpha);
		}
		if (EnergyMaterial)
		{
			EnergyMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), EnergyColor * 60.0f * FadeAlpha);
		}
		if (CircleLight)
		{
			CircleLight->SetIntensity(10000.0f * FadeAlpha);
		}
		if (PillarLight)
		{
			PillarLight->SetIntensity(15000.0f * FadeAlpha);
		}
	}
}
