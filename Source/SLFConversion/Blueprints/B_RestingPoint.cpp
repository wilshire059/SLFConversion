// B_RestingPoint.cpp
// C++ implementation for Blueprint B_RestingPoint
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/Blueprint/B_RestingPoint.json

#include "Blueprints/B_RestingPoint.h"

AB_RestingPoint::AB_RestingPoint()
{
	// Create components matching Blueprint SCS hierarchy

	// SittingZone - Billboard component for sitting position
	SittingZone = CreateDefaultSubobject<UBillboardComponent>(TEXT("SittingZone"));

	// Scene - for spawn position (attached to DefaultSceneRoot)
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	Scene->SetupAttachment(RootComponent);

	// CameraArm - Spring arm for rest view camera (attached to DefaultSceneRoot)
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);

	// LookatCamera - Camera for rest view
	LookatCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("LookatCamera"));
	// Note: In Blueprint this was not attached to CameraArm - separate component

	// Effect components would be attached to "Interactable SM" from parent
	// But we create them here - they'll be attached in Blueprint
	UnlockEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UnlockEffect"));
	Effect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));
	EffectLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("EffectLight"));
	EnvironmentLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("EnvironmentLight"));

	// Default values
	SittingAngle = 0.0;
	ForceFaceSittingActor = false;
	CameraDistance = 300.0;
	CameraOffset = FVector::ZeroVector;
	CameraRotation = FRotator::ZeroRotator;
	SittingActor = nullptr;
}

void AB_RestingPoint::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::BeginPlay - %s"), *GetName());
}

void AB_RestingPoint::GetReplenishData_Implementation(TArray<UPrimaryDataAsset*>& OutItemsToReplenish, TArray<FGameplayTag>& OutStatsToReplenish)
{
	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::GetReplenishData"));

	// Convert ItemsToReplenish TSet to TArray
	// Blueprint: ItemsToReplenish (TSet) -> To Array -> OutItemsToReplenish
	OutItemsToReplenish = ItemsToReplenish.Array();

	UE_LOG(LogTemp, Log, TEXT("  Items to replenish: %d"), OutItemsToReplenish.Num());

	// Convert StatsToReplenish GameplayTagContainer to TArray<FGameplayTag>
	// Blueprint: StatsToReplenish -> Break Gameplay Tag Container -> OutStatsToReplenish
	OutStatsToReplenish.Empty();
	StatsToReplenish.GetGameplayTagArray(OutStatsToReplenish);

	UE_LOG(LogTemp, Log, TEXT("  Stats to replenish: %d"), OutStatsToReplenish.Num());
}

void AB_RestingPoint::GetRestingPointSpawnPosition_Implementation(bool& OutSuccess, FVector& OutLocation, FRotator& OutRotation)
{
	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::GetRestingPointSpawnPosition"));

	// Blueprint Logic:
	// 1. Check if Scene component is valid
	// 2. Get Scene component world transform
	// 3. Break transform to get Location and Rotation
	// 4. Return Success = true if valid

	if (IsValid(Scene))
	{
		FTransform SceneTransform = Scene->GetComponentTransform();
		OutLocation = SceneTransform.GetLocation();
		OutRotation = SceneTransform.GetRotation().Rotator();
		OutSuccess = true;

		UE_LOG(LogTemp, Log, TEXT("  Success - Location: %s, Rotation: %s"),
			*OutLocation.ToString(), *OutRotation.ToString());
	}
	else
	{
		// Fallback to actor location if Scene component not available
		OutLocation = GetActorLocation();
		OutRotation = GetActorRotation();
		OutSuccess = false;

		UE_LOG(LogTemp, Warning, TEXT("  Scene component not valid, using actor transform"));
	}
}
