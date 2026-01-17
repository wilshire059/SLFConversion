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

	// Create GeometryCollection component (root)
	GC_DestructibleMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GC_DestructibleMesh"));
	RootComponent = GC_DestructibleMesh;

	// Create Billboard component for editor visualization
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);

	// Initialize state
	bHasPlayedDestructionSound = false;
}

void AB_Destructible::BeginPlay()
{
	Super::BeginPlay();

	// Bind to chaos break event
	if (GC_DestructibleMesh)
	{
		GC_DestructibleMesh->OnChaosBreakEvent.AddDynamic(this, &AB_Destructible::OnChaosBreakEvent);
	}
}

void AB_Destructible::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Set the GeometryCollection on the component (from Blueprint UserConstructionScript)
	if (GC_DestructibleMesh && GeometryCollection)
	{
		GC_DestructibleMesh->SetRestCollection(GeometryCollection);
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

