// B_Container.cpp
// C++ implementation for Blueprint B_Container
//
// 20-PASS VALIDATION: 2026-01-14
// Source: BlueprintDNA/Blueprint/B_Container.json
//
// Blueprint Logic:
// - OnInteract: Opens chest via OpenContainer
// - OpenContainer: Starts OpenChest timeline, async loads and plays montage
// - OpenChest timeline: Rotates lid (90 degrees on Y over 1s), triggers SpawnVFX at 0.3, ItemSpawn at 0.6
// - FadeOffTimeline: Fades point light from cached intensity to 0

#include "Blueprints/B_Container.h"
#include "Blueprints/B_PickupItem.h"
#include "Components/AC_LootDropManager.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/AssetManager.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "Interfaces/SLFGenericCharacterInterface.h"

AB_Container::AB_Container()
{
	PrimaryActorTick.bCanEverTick = false;

	// NOTE: Components are NOT created here - they come from Blueprint SCS
	// C++ finds and caches references in BeginPlay
	// This pattern allows Blueprint SCS to own components with mesh assignments

	// Initialize component pointers to nullptr
	CachedLid = nullptr;
	CachedItemSpawn = nullptr;
	CachedPointLight = nullptr;
	CachedLootDropManager = nullptr;
	CachedMoveToLocation = nullptr;
	CachedNiagaraLocation = nullptr;

	// Initialize variables
	MoveDistance = 100.0;
	SpeedMultiplier = 1.0;
	CachedIntensity = 0.0f;
	OpenChestProgress = 0.0f;
	FadeOffProgress = 0.0f;
	InitialLidRotation = FRotator::ZeroRotator;
	bHasSpawnedVFX = false;
	bHasSpawnedItems = false;
}

void AB_Container::BeginPlay()
{
	Super::BeginPlay();

	// Find component references from Blueprint SCS
	// Blueprint SCS owns the components; C++ caches references for runtime access
	const TSet<UActorComponent*>& AllComponents = GetComponents();
	for (UActorComponent* Component : AllComponents)
	{
		FString CompName = Component->GetName();

		if (CompName == TEXT("Lid") && !CachedLid)
		{
			CachedLid = Cast<UStaticMeshComponent>(Component);
		}
		else if (CompName == TEXT("ItemSpawn") && !CachedItemSpawn)
		{
			CachedItemSpawn = Cast<UBillboardComponent>(Component);
		}
		else if (CompName == TEXT("PointLight") && !CachedPointLight)
		{
			CachedPointLight = Cast<UPointLightComponent>(Component);
		}
		else if (CompName == TEXT("AC_LootDropManager") && !CachedLootDropManager)
		{
			CachedLootDropManager = Cast<UAC_LootDropManager>(Component);
		}
		else if (CompName == TEXT("MoveToLocation") && !CachedMoveToLocation)
		{
			CachedMoveToLocation = Cast<UBillboardComponent>(Component);
		}
		else if (CompName == TEXT("NiagaraLocation") && !CachedNiagaraLocation)
		{
			CachedNiagaraLocation = Cast<UBillboardComponent>(Component);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Container] BeginPlay - Lid: %s, PointLight: %s, LootManager: %s"),
		CachedLid ? TEXT("Found") : TEXT("NOT FOUND"),
		CachedPointLight ? TEXT("Found") : TEXT("NOT FOUND"),
		CachedLootDropManager ? TEXT("Found") : TEXT("NOT FOUND"));

	// Cache the point light intensity for fade off timeline
	if (CachedPointLight)
	{
		CachedIntensity = CachedPointLight->Intensity;
	}

	// Store initial lid rotation
	if (CachedLid)
	{
		InitialLidRotation = CachedLid->GetRelativeRotation();
	}

	// Bind to loot drop manager's delegate
	if (CachedLootDropManager)
	{
		CachedLootDropManager->OnItemReadyForSpawn.AddDynamic(this, &AB_Container::OnItemReadyForSpawn);
	}
}

void AB_Container::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Set CachedMoveToLocation relative position based on MoveDistance
	// Blueprint: Sets X = MoveDistance, preserves existing Y and Z
	if (CachedMoveToLocation)
	{
		FVector CurrentLocation = CachedMoveToLocation->GetRelativeLocation();
		CachedMoveToLocation->SetRelativeLocation(FVector(MoveDistance, CurrentLocation.Y, CurrentLocation.Z));
	}
}

void AB_Container::OnInteract_Implementation(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Container] OnInteract by %s"), InteractingActor ? *InteractingActor->GetName() : TEXT("None"));

	// Check if already activated (DoOnce pattern from Blueprint)
	if (IsActivated)
	{
		UE_LOG(LogTemp, Log, TEXT("[B_Container] Already activated, ignoring interaction"));
		return;
	}

	// Check if we implement BPI_Generic_Character interface
	if (InteractingActor && InteractingActor->GetClass()->ImplementsInterface(USLFGenericCharacterInterface::StaticClass()))
	{
		// Set states as per Blueprint logic
		CanBeTraced = false;
		IsActivated = true;

		// Save the interactable state
		AddInteractableStateToSaveData();

		// Load montage and call interface to play animation on character
		// The character's OpenContainer implementation will play the montage and then
		// call back to this container's OpenContainer() to start the timeline
		if (!OpenMontage.IsNull())
		{
			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			StreamableManager.RequestAsyncLoad(
				OpenMontage.ToSoftObjectPath(),
				FStreamableDelegate::CreateLambda([this, InteractingActor]()
				{
					UAnimMontage* LoadedMontage = OpenMontage.Get();
					if (LoadedMontage && InteractingActor)
					{
						// Call the interface function to play montage on character
						// The character will call back to OpenContainer() after starting the montage
						ISLFGenericCharacterInterface::Execute_OpenContainer(InteractingActor, LoadedMontage, this);
					}
				})
			);
		}
		else
		{
			// No montage, start timeline immediately
			OpenContainer();
		}
	}
}

void AB_Container::OpenContainer_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[B_Container] OpenContainer"));

	// Set play rate based on speed multiplier
	float PlayRate = FMath::Max(0.1f, static_cast<float>(SpeedMultiplier));

	// Start OpenChest timeline (simulated with timer)
	OpenChestProgress = 0.0f;
	GetWorld()->GetTimerManager().SetTimer(
		OpenChestTimerHandle,
		this,
		&AB_Container::UpdateOpenChestTimeline,
		0.016f, // ~60fps
		true
	);

	// Note: Montage loading and playback is handled in OnInteract via the BPI_GenericCharacter interface
}

void AB_Container::UpdateOpenChestTimeline()
{
	float DeltaTime = 0.016f;
	float PlayRate = FMath::Max(0.1f, static_cast<float>(SpeedMultiplier));

	OpenChestProgress += DeltaTime * PlayRate;

	// Clamp progress
	if (OpenChestProgress > 1.0f)
	{
		OpenChestProgress = 1.0f;
	}

	// Update lid rotation (0 to 90 degrees on pitch/Y axis)
	if (CachedLid)
	{
		float TargetPitch = FMath::Lerp(0.0f, 90.0f, OpenChestProgress);
		FRotator NewRotation = InitialLidRotation;
		NewRotation.Pitch = TargetPitch;
		CachedLid->SetRelativeRotation(NewRotation);
	}

	// Fire events at specific times:
	// SpawnVFX event at ~0.3 progress
	if (!bHasSpawnedVFX && OpenChestProgress >= 0.3f)
	{
		bHasSpawnedVFX = true;
		SpawnOpenVFX();
	}

	// ItemSpawn event at ~0.6 progress
	if (!bHasSpawnedItems && OpenChestProgress >= 0.6f)
	{
		bHasSpawnedItems = true;
		SpawnItems();
	}

	// Timeline finished
	if (OpenChestProgress >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(OpenChestTimerHandle);
		bHasSpawnedVFX = false;
		bHasSpawnedItems = false;

		UE_LOG(LogTemp, Log, TEXT("[B_Container] OpenChest timeline finished"));

		// Start fade off timeline
		FadeOffProgress = 0.0f;
		GetWorld()->GetTimerManager().SetTimer(
			FadeOffTimerHandle,
			this,
			&AB_Container::UpdateFadeOffTimeline,
			0.016f,
			true
		);
	}
}

void AB_Container::UpdateFadeOffTimeline()
{
	float DeltaTime = 0.016f;
	FadeOffProgress += DeltaTime;

	// Fade over 1 second
	if (FadeOffProgress > 1.0f)
	{
		FadeOffProgress = 1.0f;
	}

	// Lerp point light intensity from cached to 0
	if (CachedPointLight)
	{
		float NewIntensity = FMath::Lerp(CachedIntensity, 0.0f, FadeOffProgress);
		CachedPointLight->SetIntensity(NewIntensity);
	}

	// Timeline finished
	if (FadeOffProgress >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeOffTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("[B_Container] FadeOff timeline finished"));
	}
}

void AB_Container::SpawnOpenVFX()
{
	UE_LOG(LogTemp, Log, TEXT("[B_Container] SpawnOpenVFX"));

	if (!OpenVFX.IsNull() && CachedNiagaraLocation)
	{
		// Async load and spawn VFX
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(
			OpenVFX.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this]()
			{
				UNiagaraSystem* LoadedVFX = OpenVFX.Get();
				if (LoadedVFX && CachedNiagaraLocation)
				{
					FVector SpawnLocation = CachedNiagaraLocation->GetComponentLocation();
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						this,
						LoadedVFX,
						SpawnLocation,
						FRotator::ZeroRotator,
						FVector(1.0f),
						true,
						true
					);
					UE_LOG(LogTemp, Log, TEXT("[B_Container] Spawned VFX at %s"), *SpawnLocation.ToString());
				}
			})
		);
	}
}

void AB_Container::SpawnItems()
{
	UE_LOG(LogTemp, Log, TEXT("[B_Container] SpawnItems"));

	// Tell loot drop manager to pick an item
	if (CachedLootDropManager)
	{
		CachedLootDropManager->PickItem();
	}
}

void AB_Container::OnItemReadyForSpawn(FSLFLootItem LootItem)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Container] OnItemReadyForSpawn"));

	// Validate item
	if (!LootItem.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_Container] Invalid loot item"));
		return;
	}

	// Get spawn transform from CachedItemSpawn component
	FTransform SpawnTransform = FTransform::Identity;
	if (CachedItemSpawn)
	{
		SpawnTransform.SetLocation(CachedItemSpawn->GetComponentLocation());
	}

	// Calculate random amount
	int32 Amount = FMath::RandRange(LootItem.MinAmount, LootItem.MaxAmount);

	// Load and spawn B_PickupItem class (Blueprint spawns B_PickupItem_C directly, not ItemClass)
	UClass* PickupItemClass = LoadClass<AB_PickupItem>(nullptr, TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem.B_PickupItem_C"));
	if (!PickupItemClass)
	{
		// Fallback to C++ class if Blueprint not found
		PickupItemClass = AB_PickupItem::StaticClass();
	}

	// Spawn with exposed properties (Item, Count, UsePhysics)
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	AB_PickupItem* SpawnedItem = GetWorld()->SpawnActorDeferred<AB_PickupItem>(
		PickupItemClass,
		SpawnTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	if (SpawnedItem)
	{
		// Set exposed properties before finishing spawn
		SpawnedItem->Item = Cast<UPrimaryDataAsset>(LootItem.Item);
		SpawnedItem->Count = Amount;
		SpawnedItem->UsePhysics = false;

		// Bind to OnItemLooted dispatcher (Blueprint binds this to handle post-loot logic)
		SpawnedItem->OnItemLooted.AddDynamic(this, &AB_Container::OnSpawnedItemLooted);

		SpawnedItem->FinishSpawning(SpawnTransform);

		UE_LOG(LogTemp, Log, TEXT("[B_Container] Spawned pickup item: %s x%d"),
			LootItem.Item ? *LootItem.Item->GetName() : TEXT("None"), Amount);
	}
}

void AB_Container::OpenLidAndSpawnItem_Implementation(bool bSpawnItem, bool bSpawnVFX)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Container] OpenLidAndSpawnItem - SpawnItem: %s, SpawnVFX: %s"),
		bSpawnItem ? TEXT("true") : TEXT("false"),
		bSpawnVFX ? TEXT("true") : TEXT("false"));

	// Start open chest animation
	OpenChestProgress = 0.0f;
	GetWorld()->GetTimerManager().SetTimer(
		OpenChestTimerHandle,
		this,
		&AB_Container::UpdateOpenChestTimeline,
		0.016f,
		true
	);

	// Conditionally spawn VFX
	if (bSpawnVFX)
	{
		SpawnOpenVFX();
	}

	// Conditionally spawn items
	if (bSpawnItem)
	{
		SpawnItems();
	}
}

void AB_Container::InitializeLoadedStates_Implementation(bool bCanBeTracedState, bool bIsActivatedState)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Container] InitializeLoadedStates - CanBeTraced: %s, IsActivated: %s"),
		bCanBeTracedState ? TEXT("true") : TEXT("false"),
		bIsActivatedState ? TEXT("true") : TEXT("false"));

	// Set inherited states from B_Interactable
	CanBeTraced = bCanBeTracedState;
	IsActivated = bIsActivatedState;

	// If already activated (from save data), open the chest without spawning items/vfx
	if (bIsActivatedState)
	{
		// Directly set lid to open position
		if (CachedLid)
		{
			FRotator OpenRotation = InitialLidRotation;
			OpenRotation.Pitch = 90.0f;
			CachedLid->SetRelativeRotation(OpenRotation);
		}

		// Turn off point light
		if (CachedPointLight)
		{
			CachedPointLight->SetIntensity(0.0f);
		}
	}

	// If not traceable, disable interaction
	if (!bCanBeTracedState)
	{
		// Disable interaction tracing
		IsActivated = false;
	}
}

void AB_Container::OnSpawnedItemLooted()
{
	UE_LOG(LogTemp, Log, TEXT("[B_Container] OnSpawnedItemLooted - Item was looted from chest"));
	// Blueprint binds to this to handle post-loot logic
	// Currently no additional logic needed - the container stays open
}

