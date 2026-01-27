// B_Destructible.cpp
// C++ implementation for Blueprint B_Destructible
//
// 20-PASS VALIDATION: 2026-01-14
// Source: BlueprintDNA/Blueprint/B_Destructible.json
//
// Blueprint Logic:
// - OnConstruction: Sets GeometryCollection on the GC component
// - OnChaosBreakEvent: Async loads destruction sound and plays at location (once)

#include "Blueprints/B_Destructible.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

AB_Destructible::AB_Destructible()
{
	PrimaryActorTick.bCanEverTick = false;

	// Initialize pointers to nullptr - components come from Blueprint SCS
	CachedGC_DestructibleMesh = nullptr;
	CachedBillboard = nullptr;

	// Initialize state
	bHasPlayedDestructionSound = false;

	// NOTE: Components (GC_DestructibleMesh, Billboard) are created in Blueprint SCS,
	// not in C++ constructor. This avoids name collisions.
}

void AB_Destructible::BeginPlay()
{
	Super::BeginPlay();

	// Cache component references from Blueprint SCS
	const TSet<UActorComponent*>& AllComponents = GetComponents();
	for (UActorComponent* Component : AllComponents)
	{
		if (!Component) continue;
		FString CompName = Component->GetName();

		if (CompName.Contains(TEXT("GC_Destructible")) && !CachedGC_DestructibleMesh)
		{
			CachedGC_DestructibleMesh = Cast<UGeometryCollectionComponent>(Component);
		}
		else if (CompName.Contains(TEXT("Billboard")) && !CachedBillboard)
		{
			CachedBillboard = Cast<UBillboardComponent>(Component);
		}
	}

	// Bind to chaos break event and ensure physics is active
	if (CachedGC_DestructibleMesh)
	{
		CachedGC_DestructibleMesh->OnChaosBreakEvent.AddDynamic(this, &AB_Destructible::OnChaosBreakEvent);

		// CRITICAL: Enable physics simulation
		// The Blueprint SCS has simulate_physics=True set on the component.
		// SetRestCollection with bApplyAssetDefaults=true does NOT set SimulatePhysics.
		// We must explicitly enable it for Chaos destruction to work.
		CachedGC_DestructibleMesh->SetSimulatePhysics(true);

		// Ensure the component is activated for Chaos physics to work
		if (!CachedGC_DestructibleMesh->IsActive())
		{
			CachedGC_DestructibleMesh->Activate(true);
		}

		// Log physics state for debugging
		UE_LOG(LogTemp, Log, TEXT("[B_Destructible] BeginPlay - Physics state: IsActive=%s, IsSimulatingPhysics=%s, SimulatePhysics=%s"),
			CachedGC_DestructibleMesh->IsActive() ? TEXT("Yes") : TEXT("No"),
			CachedGC_DestructibleMesh->IsSimulatingPhysics() ? TEXT("Yes") : TEXT("No"),
			CachedGC_DestructibleMesh->BodyInstance.bSimulatePhysics ? TEXT("Yes") : TEXT("No"));

		// Log collision settings for debugging
		ECollisionChannel ObjectType = CachedGC_DestructibleMesh->GetCollisionObjectType();
		ECollisionEnabled::Type CollisionEnabled = CachedGC_DestructibleMesh->GetCollisionEnabled();
		UE_LOG(LogTemp, Log, TEXT("[B_Destructible] Collision - ObjectType=%d, CollisionEnabled=%d"),
			(int32)ObjectType, (int32)CollisionEnabled);
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Destructible] BeginPlay - GC=%s, Billboard=%s"),
		CachedGC_DestructibleMesh ? TEXT("Yes") : TEXT("No"),
		CachedBillboard ? TEXT("Yes") : TEXT("No"));
}

void AB_Destructible::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// OnConstruction runs before BeginPlay, so we need to find the component directly
	// Set the GeometryCollection on the component (from Blueprint UserConstructionScript)
	if (GeometryCollection)
	{
		// Find GC component directly since CachedGC_DestructibleMesh isn't populated yet
		UGeometryCollectionComponent* GC = FindComponentByClass<UGeometryCollectionComponent>();
		if (GC)
		{
			// CRITICAL: bApplyAssetDefaults=true is required to apply physics settings from the GC asset
			// This enables physics simulation, collision, and break thresholds defined in the asset
			GC->SetRestCollection(GeometryCollection, true);
		}
	}
}

void AB_Destructible::OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent)
{
	// DoOnce pattern - only play sound once
	if (bHasPlayedDestructionSound)
	{
		return;
	}
	bHasPlayedDestructionSound = true;

	// Async load and play destruction sound
	if (!DestructionSound.IsNull())
	{
		// Use async loading for soft reference
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(
			DestructionSound.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this, Location = BreakEvent.Location]()
			{
				USoundBase* LoadedSound = DestructionSound.Get();
				if (LoadedSound)
				{
					UGameplayStatics::PlaySoundAtLocation(
						this,
						LoadedSound,
						Location,
						1.0f,  // Volume
						1.0f   // Pitch
					);
				}
			})
		);
	}
}

