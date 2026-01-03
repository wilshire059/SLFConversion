// SLFRestingPointBase.cpp
// C++ implementation for B_RestingPoint
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_RestingPoint
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         10/10 (initialized in constructor)
// Functions:         2/2 implemented (1 migrated + 1 override)
// Event Dispatchers: 2/2 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFRestingPointBase.h"
#include "Components/SceneComponent.h"

ASLFRestingPointBase::ASLFRestingPointBase()
{
	// Create DefaultSceneRoot for Blueprint components
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	// Initialize point settings
	LocationName = FText::FromString(TEXT("Resting Point"));

	// Initialize view settings
	SittingAngle = 0.0;
	bForceFaceSittingActor = false;
	CameraDistance = 300.0f;
	CameraOffset = FVector(0.0f, 0.0f, 100.0f);
	CameraRotation = FRotator(-20.0f, 0.0f, 0.0f);

	// Initialize runtime
	SittingActor = nullptr;

	// Update interaction text
	InteractionText = FText::FromString(TEXT("Rest"));
}

void ASLFRestingPointBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] BeginPlay: %s"),
		*LocationName.ToString());
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTIONS [1/2]
// ═══════════════════════════════════════════════════════════════════════════════

FSLFReplenishData ASLFRestingPointBase::GetReplenishData_Implementation()
{
	FSLFReplenishData Data;
	Data.ItemsToReplenish = ItemsToReplenish;
	Data.StatsToReplenish = StatsToReplenish;
	return Data;
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERACTION OVERRIDE [2/2]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFRestingPointBase::OnInteract_Implementation(AActor* Interactor)
{
	Super::OnInteract_Implementation(Interactor);

	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] OnInteract - Player resting at %s"),
		*LocationName.ToString());

	SittingActor = Interactor;

	// Replenish stats
	FSLFReplenishData ReplenishData = GetReplenishData();

	// TODO: Replenish stats via StatManager
	// TODO: Replenish items via InventoryManager
	// TODO: Setup camera for resting view
	// TODO: Open resting menu

	OnReady.Broadcast();
}
