// SLFAOESpell.cpp
// Expanding AOE explosion effect

#include "SLFAOESpell.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASLFAOESpell::ASLFAOESpell()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	// Main expanding ring (flat cylinder)
	ExpansionRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExpansionRing"));
	ExpansionRing->SetupAttachment(Root);
	ExpansionRing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExpansionRing->SetCastShadow(false);
	ExpansionRing->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.02f));  // Flat ring
	if (CylinderMesh.Succeeded())
	{
		ExpansionRing->SetStaticMesh(CylinderMesh.Object);
	}

	// Explosion core sphere
	ExplosionCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosionCore"));
	ExplosionCore->SetupAttachment(Root);
	ExplosionCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExplosionCore->SetCastShadow(false);
	ExplosionCore->SetRelativeScale3D(FVector(0.1f));
	if (SphereMesh.Succeeded())
	{
		ExplosionCore->SetStaticMesh(SphereMesh.Object);
	}

	// Second ring (delayed)
	SecondRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondRing"));
	SecondRing->SetupAttachment(Root);
	SecondRing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SecondRing->SetCastShadow(false);
	SecondRing->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.015f));
	SecondRing->SetRelativeLocation(FVector(0.0f, 0.0f, 5.0f));  // Slightly elevated
	if (CylinderMesh.Succeeded())
	{
		SecondRing->SetStaticMesh(CylinderMesh.Object);
	}

	// Bright explosion light
	ExplosionLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ExplosionLight"));
	ExplosionLight->SetupAttachment(Root);
	ExplosionLight->SetIntensity(30000.0f);
	ExplosionLight->SetAttenuationRadius(800.0f);
	ExplosionLight->SetCastShadows(false);
}

void ASLFAOESpell::BeginPlay()
{
	Super::BeginPlay();
	SetupAOEEffect();

	CurrentRadius = StartRadius;

	UE_LOG(LogTemp, Warning, TEXT("[SLFAOESpell] AOE EXPLOSION SPAWNED!"));
}

void ASLFAOESpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LifeTime += DeltaTime;
	float TotalDuration = ExpansionTime + FadeTime;

	if (LifeTime >= TotalDuration)
	{
		Destroy();
		return;
	}

	UpdateExpansion(DeltaTime);
}

void ASLFAOESpell::SetupAOEEffect()
{
	// Setup materials
	auto SetupMaterial = [this](UStaticMeshComponent* Mesh) -> UMaterialInstanceDynamic*
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
				Mat->SetVectorParameterValue(TEXT("BaseColor"), AOEColor);
				Mat->SetVectorParameterValue(TEXT("EmissiveColor"), AOEColor * 50.0f);
				Mesh->SetMaterial(0, Mat);
				return Mat;
			}
		}
		return nullptr;
	};

	RingMaterial = SetupMaterial(ExpansionRing);
	CoreMaterial = SetupMaterial(ExplosionCore);
	SetupMaterial(SecondRing);

	// Set light color
	if (ExplosionLight)
	{
		ExplosionLight->SetLightColor(AOEColor);
	}
}

void ASLFAOESpell::UpdateExpansion(float DeltaTime)
{
	// Calculate expansion progress (0 to 1)
	float ExpansionProgress = FMath::Clamp(LifeTime / ExpansionTime, 0.0f, 1.0f);

	// Ease out for natural explosion feel
	float EasedProgress = 1.0f - FMath::Pow(1.0f - ExpansionProgress, 3.0f);

	// Calculate current radius
	CurrentRadius = FMath::Lerp(StartRadius, MaxRadius, EasedProgress);
	float RadiusScale = CurrentRadius / 50.0f;  // 50 = half of default cylinder size

	// Update main ring
	if (ExpansionRing)
	{
		ExpansionRing->SetRelativeScale3D(FVector(RadiusScale, RadiusScale, 0.02f));
	}

	// Update second ring (delayed by 0.1 seconds)
	float SecondRingProgress = FMath::Clamp((LifeTime - 0.1f) / ExpansionTime, 0.0f, 1.0f);
	float SecondEased = 1.0f - FMath::Pow(1.0f - SecondRingProgress, 3.0f);
	float SecondRadius = FMath::Lerp(StartRadius, MaxRadius * 0.8f, SecondEased);
	if (SecondRing)
	{
		SecondRing->SetRelativeScale3D(FVector(SecondRadius / 50.0f, SecondRadius / 50.0f, 0.015f));
	}

	// Core expands then shrinks
	if (ExplosionCore)
	{
		float CoreScale;
		if (ExpansionProgress < 0.3f)
		{
			// Rapid expansion
			CoreScale = FMath::Lerp(0.1f, MaxRadius / 100.0f, ExpansionProgress / 0.3f);
		}
		else
		{
			// Shrink away
			CoreScale = FMath::Lerp(MaxRadius / 100.0f, 0.0f, (ExpansionProgress - 0.3f) / 0.7f);
		}
		ExplosionCore->SetRelativeScale3D(FVector(CoreScale));
	}

	// Calculate fade (starts after expansion)
	float FadeProgress = 0.0f;
	if (LifeTime > ExpansionTime)
	{
		FadeProgress = (LifeTime - ExpansionTime) / FadeTime;
	}
	float Alpha = 1.0f - FadeProgress;

	// Update light intensity (bright flash then fade)
	if (ExplosionLight)
	{
		float LightIntensity;
		if (LifeTime < 0.1f)
		{
			// Initial bright flash
			LightIntensity = 50000.0f;
		}
		else
		{
			// Fade out
			LightIntensity = 30000.0f * Alpha;
		}
		ExplosionLight->SetIntensity(LightIntensity);
		ExplosionLight->SetAttenuationRadius(CurrentRadius * 1.5f);
	}

	// Update material opacity/emissive
	if (RingMaterial)
	{
		RingMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), AOEColor * 50.0f * Alpha);
	}

	if (CoreMaterial)
	{
		CoreMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), AOEColor * 80.0f * Alpha);
	}
}
