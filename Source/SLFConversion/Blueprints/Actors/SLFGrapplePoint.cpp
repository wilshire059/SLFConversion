// SLFGrapplePoint.cpp
#include "SLFGrapplePoint.h"

ASLFGrapplePoint::ASLFGrapplePoint()
{
	PrimaryActorTick.bCanEverTick = false;

	AnchorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnchorMesh"));
	RootComponent = AnchorMesh;

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetSphereRadius(GrappleRange);
	DetectionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	DetectionSphere->SetGenerateOverlapEvents(true);
}

void ASLFGrapplePoint::BeginPlay()
{
	Super::BeginPlay();
	DetectionSphere->SetSphereRadius(GrappleRange);
}

FVector ASLFGrapplePoint::GetLandingLocation() const
{
	return GetActorLocation() + GetActorTransform().TransformVector(LandingOffset);
}

bool ASLFGrapplePoint::IsInRange(AActor* Actor) const
{
	if (!Actor || !bIsActive) return false;
	return FVector::Dist(GetActorLocation(), Actor->GetActorLocation()) <= GrappleRange;
}
