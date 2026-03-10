// SLFShortcutGate.cpp
#include "SLFShortcutGate.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ASLFShortcutGate::ASLFShortcutGate()
{
	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	GateMesh->SetupAttachment(RootComponent);

	BlockingVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingVolume"));
	BlockingVolume->SetupAttachment(GateMesh);
	BlockingVolume->SetBoxExtent(FVector(50.0f, 200.0f, 200.0f));
	BlockingVolume->SetCollisionProfileName(TEXT("BlockAll"));
}

void ASLFShortcutGate::BeginPlay()
{
	Super::BeginPlay();

	if (bIsOpened)
	{
		OpenGate();
	}
}

bool ASLFShortcutGate::IsOnOpenSide(AActor* Actor) const
{
	if (!Actor) return false;

	FVector ToActor = (Actor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	float Dot = FVector::DotProduct(GetActorForwardVector(), ToActor);

	return bCanOpenFromFront ? (Dot > 0.0f) : (Dot < 0.0f);
}

void ASLFShortcutGate::OpenGate()
{
	bIsOpened = true;

	if (BlockingVolume)
	{
		BlockingVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (GateMesh)
	{
		// Rotate gate open (90 degrees)
		GateMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	}

	UE_LOG(LogTemp, Log, TEXT("[ShortcutGate] %s opened"), *GetName());
}

void ASLFShortcutGate::OnInteract_Implementation(AActor* InteractingActor)
{
	if (bIsOpened)
	{
		UE_LOG(LogTemp, Log, TEXT("[ShortcutGate] %s already open"), *GetName());
		return;
	}

	if (!IsOnOpenSide(InteractingActor))
	{
		UE_LOG(LogTemp, Log, TEXT("[ShortcutGate] %s - wrong side"), *GetName());
		return;
	}

	OpenGate();
}
