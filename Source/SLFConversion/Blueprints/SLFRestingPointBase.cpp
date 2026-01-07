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
#include "Components/StatManagerComponent.h"
#include "Components/InventoryManagerComponent.h"

ASLFRestingPointBase::ASLFRestingPointBase()
{
	// DefaultSceneRoot is created by parent SLFInteractableBase

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

	// Replenish stats via StatManager - restore to max for each stat tag
	if (UStatManagerComponent* StatManager = Interactor->FindComponentByClass<UStatManagerComponent>())
	{
		for (const FGameplayTag& StatTag : ReplenishData.StatsToReplenish)
		{
			// Fully restore the stat (set current = max)
			StatManager->ResetStat(StatTag);
			UE_LOG(LogTemp, Verbose, TEXT("[RestingPoint] Replenished stat: %s"), *StatTag.ToString());
		}
	}

	// Replenish items via InventoryManager
	if (UInventoryManagerComponent* InventoryManager = Interactor->FindComponentByClass<UInventoryManagerComponent>())
	{
		for (UDataAsset* Item : ReplenishData.ItemsToReplenish)
		{
			if (Item)
			{
				// Replenish rechargeable items to max count
				InventoryManager->ReplenishItem(Item, -1); // -1 means replenish to max
				UE_LOG(LogTemp, Verbose, TEXT("[RestingPoint] Replenished item: %s"), *Item->GetName());
			}
		}
	}

	// Camera setup and menu opening are handled by player controller response to OnReady
	// The player controller should bind to OnReady and handle UI/camera transitions

	OnReady.Broadcast();
}
