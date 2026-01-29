// SLFPoisonBlobProjectile.cpp
// Poison blob with bubbling effect

#include "SLFPoisonBlobProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraComponent.h"

ASLFPoisonBlobProjectile::ASLFPoisonBlobProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Slower poison blob
	ProjectileSpeed = 1800.0f;
	MinDamage = 20.0f;
	MaxDamage = 35.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Main toxic blob
	ToxicCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToxicCore"));
	ToxicCore->SetupAttachment(RootComponent);
	ToxicCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ToxicCore->SetCastShadow(false);
	ToxicCore->SetRelativeScale3D(FVector(0.5f));
	if (SphereMesh.Succeeded())
	{
		ToxicCore->SetStaticMesh(SphereMesh.Object);
	}

	// Bubbles orbiting/rising around the blob
	Bubble1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bubble1"));
	Bubble1->SetupAttachment(RootComponent);
	Bubble1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Bubble1->SetCastShadow(false);
	Bubble1->SetRelativeScale3D(FVector(0.1f));
	Bubble1->SetRelativeLocation(FVector(15.0f, 0.0f, 10.0f));
	if (SphereMesh.Succeeded())
	{
		Bubble1->SetStaticMesh(SphereMesh.Object);
	}

	Bubble2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bubble2"));
	Bubble2->SetupAttachment(RootComponent);
	Bubble2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Bubble2->SetCastShadow(false);
	Bubble2->SetRelativeScale3D(FVector(0.08f));
	Bubble2->SetRelativeLocation(FVector(-10.0f, 12.0f, -5.0f));
	if (SphereMesh.Succeeded())
	{
		Bubble2->SetStaticMesh(SphereMesh.Object);
	}

	Bubble3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bubble3"));
	Bubble3->SetupAttachment(RootComponent);
	Bubble3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Bubble3->SetCastShadow(false);
	Bubble3->SetRelativeScale3D(FVector(0.12f));
	Bubble3->SetRelativeLocation(FVector(5.0f, -15.0f, 8.0f));
	if (SphereMesh.Succeeded())
	{
		Bubble3->SetStaticMesh(SphereMesh.Object);
	}

	// Sickly green light
	ToxicLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ToxicLight"));
	ToxicLight->SetupAttachment(RootComponent);
	ToxicLight->SetIntensity(5000.0f);
	ToxicLight->SetAttenuationRadius(300.0f);
	ToxicLight->SetLightColor(FLinearColor(0.3f, 1.0f, 0.2f));
	ToxicLight->SetCastShadows(false);

	// Store base positions
	Bubble1BasePos = FVector(15.0f, 0.0f, 10.0f);
	Bubble2BasePos = FVector(-10.0f, 12.0f, -5.0f);
	Bubble3BasePos = FVector(5.0f, -15.0f, 8.0f);
}

void ASLFPoisonBlobProjectile::BeginPlay()
{
	SetupPoisonEffect();
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[SLFPoisonBlobProjectile] POISON BLOB SPAWNED!"));
}

void ASLFPoisonBlobProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateBubblingEffect(DeltaTime);
}

void ASLFPoisonBlobProjectile::SetupPoisonEffect()
{
	// Scale blob
	if (ToxicCore)
	{
		float Scale = BlobRadius / 50.0f;
		ToxicCore->SetRelativeScale3D(FVector(Scale));
		ToxicCore->SetVisibility(true);
	}

	// Core material - dark sickly green
	if (ToxicCore && ToxicCore->GetStaticMesh())
	{
		UMaterialInterface* BaseMat = ToxicCore->GetMaterial(0);
		if (!BaseMat)
		{
			BaseMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		}
		if (BaseMat)
		{
			CoreMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			CoreMaterial->SetVectorParameterValue(TEXT("BaseColor"), DarkGreen);
			CoreMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), PoisonGreen * 10.0f);
			ToxicCore->SetMaterial(0, CoreMaterial);
		}
	}

	// Bubble material - bright toxic green
	auto SetupBubble = [this](UStaticMeshComponent* Bubble)
	{
		if (Bubble && Bubble->GetStaticMesh())
		{
			UMaterialInterface* BaseMat = Bubble->GetMaterial(0);
			if (!BaseMat)
			{
				BaseMat = LoadObject<UMaterialInterface>(nullptr,
					TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
			}
			if (BaseMat)
			{
				BubbleMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
				BubbleMaterial->SetVectorParameterValue(TEXT("BaseColor"), PoisonGreen);
				BubbleMaterial->SetVectorParameterValue(TEXT("EmissiveColor"), PoisonGreen * 15.0f);
				Bubble->SetMaterial(0, BubbleMaterial);
			}
		}
	};

	SetupBubble(Bubble1);
	SetupBubble(Bubble2);
	SetupBubble(Bubble3);

	// Disable inherited Niagara
	if (Effect)
	{
		Effect->Deactivate();
		Effect->SetVisibility(false);
	}
}

void ASLFPoisonBlobProjectile::UpdateBubblingEffect(float DeltaTime)
{
	AccumulatedTime += DeltaTime;

	// Wobble the main blob
	if (ToxicCore)
	{
		float WobbleX = FMath::Sin(AccumulatedTime * BubbleSpeed * 2.0f) * WobbleAmount;
		float WobbleY = FMath::Sin(AccumulatedTime * BubbleSpeed * 2.3f) * WobbleAmount;
		float WobbleZ = FMath::Sin(AccumulatedTime * BubbleSpeed * 1.7f) * WobbleAmount;

		float BaseScale = BlobRadius / 50.0f;
		ToxicCore->SetRelativeScale3D(FVector(
			BaseScale * (1.0f + WobbleX),
			BaseScale * (1.0f + WobbleY),
			BaseScale * (1.0f + WobbleZ)
		));
	}

	// Animate bubbles rising and orbiting
	auto AnimateBubble = [this](UStaticMeshComponent* Bubble, FVector BasePos, float PhaseOffset)
	{
		if (Bubble)
		{
			float T = AccumulatedTime * BubbleSpeed + PhaseOffset;
			FVector Offset(
				FMath::Sin(T * 1.5f) * 5.0f,
				FMath::Cos(T * 1.2f) * 5.0f,
				FMath::Sin(T * 2.0f) * 8.0f + FMath::Abs(FMath::Sin(T)) * 5.0f  // Rising
			);
			Bubble->SetRelativeLocation(BasePos + Offset);

			// Pulse bubble size
			float SizePhase = FMath::Sin(T * 3.0f) * 0.3f + 1.0f;
			float BaseScale = Bubble == Bubble1 ? 0.1f : (Bubble == Bubble2 ? 0.08f : 0.12f);
			Bubble->SetRelativeScale3D(FVector(BaseScale * SizePhase));
		}
	};

	AnimateBubble(Bubble1, Bubble1BasePos, 0.0f);
	AnimateBubble(Bubble2, Bubble2BasePos, 2.1f);
	AnimateBubble(Bubble3, Bubble3BasePos, 4.2f);

	// Pulse light
	float LightPulse = FMath::Sin(AccumulatedTime * BubbleSpeed * 1.5f) * 0.3f + 0.7f;
	if (ToxicLight)
	{
		ToxicLight->SetIntensity(4000.0f + 3000.0f * LightPulse);
	}
}
