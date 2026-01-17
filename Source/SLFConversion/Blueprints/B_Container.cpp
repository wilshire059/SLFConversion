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

	// Create root scene component
	USceneComponent* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultRoot;

	// Create Lid component
	Lid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lid"));
	Lid->SetupAttachment(RootComponent);

	// Create ItemSpawn billboard
	ItemSpawn = CreateDefaultSubobject<UBillboardComponent>(TEXT("ItemSpawn"));
	ItemSpawn->SetupAttachment(RootComponent);

	// Create PointLight
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(RootComponent);

	// Create LootDropManager component
	AC_LootDropManager = CreateDefaultSubobject<UAC_LootDropManager>(TEXT("AC_LootDropManager"));

	// Create MoveToLocation billboard
	MoveToLocation = CreateDefaultSubobject<UBillboardComponent>(TEXT("MoveToLocation"));
	MoveToLocation->SetupAttachment(RootComponent);

	// Create NiagaraLocation billboard
	NiagaraLocation = CreateDefaultSubobject<UBillboardComponent>(TEXT("NiagaraLocation"));
	NiagaraLocation->SetupAttachment(RootComponent);

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

	// Cache the point light intensity for fade off timeline
	if (PointLight)
	{
		CachedIntensity = PointLight->Intensity;
	}

	// Store initial lid rotation
	if (Lid)
	{
		InitialLidRotation = Lid->GetRelativeRotation();
	}

	// Bind to loot drop manager's delegate
	if (AC_LootDropManager)
	{
		AC_LootDropManager->OnItemReadyForSpawn.AddDynamic(this, &AB_Container::OnItemReadyForSpawn);
	}
}

void AB_Container::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Set MoveToLocation relative position based on MoveDistance
	// Blueprint: Sets X = MoveDistance, preserves existing Y and Z
	if (MoveToLocation)
	{
		FVector CurrentLocation = MoveToLocation->GetRelativeLocation();
		MoveToLocation->SetRelativeLocation(FVector(MoveDistance, CurrentLocation.Y, CurrentLocation.Z));
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
						ISLFGenericCharacterInterface::Execute_OpenContainer(InteractingActor, LoadedMontage, this);
					}
				})
			);
		}

		// Call OpenContainer to start the chest opening animation
		OpenContainer();
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
	if (Lid)
	{
		float TargetPitch = FMath::Lerp(0.0f, 90.0f, OpenChestProgress);
		FRotator NewRotation = InitialLidRotation;
		NewRotation.Pitch = TargetPitch;
		Lid->SetRelativeRotation(NewRotation);
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
	if (PointLight)
	{
		float NewIntensity = FMath::Lerp(CachedIntensity, 0.0f, FadeOffProgress);
		PointLight->SetIntensity(NewIntensity);
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

	if (!OpenVFX.IsNull() && NiagaraLocation)
	{
		// Async load and spawn VFX
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(
			OpenVFX.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this]()
			{
				UNiagaraSystem* LoadedVFX = OpenVFX.Get();
				if (LoadedVFX && NiagaraLocation)
				{
					FVector SpawnLocation = NiagaraLocation->GetComponentLocation();
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
	if (AC_LootDropManager)
	{
		AC_LootDropManager->PickItem();
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

	// Get spawn transform from ItemSpawn component
	FTransform SpawnTransform = FTransform::Identity;
	if (ItemSpawn)
	{
		SpawnTransform.SetLocation(ItemSpawn->GetComponentLocation());
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
		if (Lid)
		{
			FRotator OpenRotation = InitialLidRotation;
			OpenRotation.Pitch = 90.0f;
			Lid->SetRelativeRotation(OpenRotation);
		}

		// Turn off point light
		if (PointLight)
		{
			PointLight->SetIntensity(0.0f);
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

