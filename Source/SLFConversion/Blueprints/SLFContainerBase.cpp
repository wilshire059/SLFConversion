// SLFContainerBase.cpp
// C++ implementation for B_Container
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Container
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         5/5 (initialized in constructor)
// Functions:         1/1 implemented (override only)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFContainerBase.h"
#include "Animation/AnimMontage.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/LootDropManagerComponent.h"
#include "Engine/World.h"

ASLFContainerBase::ASLFContainerBase()
{
	// Enable Tick for lid animation
	PrimaryActorTick.bCanEverTick = true;

	// Initialize open config
	MoveDistance = 100.0;
	SpeedMultiplier = 1.0;

	// Initialize runtime
	CachedIntensity = 1.0f;

	// Update interaction text
	InteractionText = FText::FromString(TEXT("Open"));

	// Set root component scale to match bp_only (0.75)
	if (DefaultSceneRoot)
	{
		DefaultSceneRoot->SetRelativeScale3D(FVector(0.75, 0.75, 0.75));
	}
}

void ASLFContainerBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[Container] BeginPlay: %s"),
		*InteractableDisplayName.ToString());

	// Apply chest box mesh to InteractableSM (base class creates this component)
	if (InteractableSM && !ChestBoxMesh.IsNull())
	{
		UStaticMesh* BoxMesh = ChestBoxMesh.LoadSynchronous();
		if (BoxMesh)
		{
			InteractableSM->SetStaticMesh(BoxMesh);
			UE_LOG(LogTemp, Log, TEXT("[Container] Applied ChestBoxMesh: %s"), *BoxMesh->GetName());
		}
	}

	// Cache components from Blueprint SCS
	TArray<UActorComponent*> AllComponents;
	GetComponents(AllComponents);
	UE_LOG(LogTemp, Warning, TEXT("[Container] BeginPlay - Found %d components"), AllComponents.Num());

	for (UActorComponent* Comp : AllComponents)
	{
		FString CompName = Comp->GetName();
		FString CompClass = Comp->GetClass()->GetName();

		// Log all components for debugging
		UE_LOG(LogTemp, Warning, TEXT("[Container] Component: %s (%s)"), *CompName, *CompClass);

		if (CompName == TEXT("Lid"))
		{
			CachedLid = Cast<UStaticMeshComponent>(Comp);
			UE_LOG(LogTemp, Warning, TEXT("[Container] Cached Lid component"));
		}
		else if (CompName == TEXT("NiagaraLocation"))
		{
			CachedNiagaraLocation = Cast<USceneComponent>(Comp);
			UE_LOG(LogTemp, Warning, TEXT("[Container] Cached NiagaraLocation component"));
		}
		else if (CompName == TEXT("ItemSpawn"))
		{
			CachedItemSpawn = Cast<USceneComponent>(Comp);
			UE_LOG(LogTemp, Warning, TEXT("[Container] Cached ItemSpawn component"));
		}

		// Cache LootDropManager component - check by class name and use correct C++ parent class
		if (CompClass.Contains(TEXT("LootDropManager")))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Container] Found LootDropManager candidate: %s (%s)"), *CompName, *CompClass);

			// Try Cast to C++ class - Blueprint inherits from ULootDropManagerComponent
			if (ULootDropManagerComponent* LootMgr = Cast<ULootDropManagerComponent>(Comp))
			{
				CachedLootManager = LootMgr;
				UE_LOG(LogTemp, Warning, TEXT("[Container] SUCCESS - Cached LootDropManagerComponent via Cast"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Container] Cast to ULootDropManagerComponent FAILED"));
			}
		}
	}

	// Bind to loot manager's OnItemReadyForSpawn event
	if (CachedLootManager)
	{
		CachedLootManager->OnItemReadyForSpawn.AddDynamic(this, &ASLFContainerBase::HandleItemReadyForSpawn);
		UE_LOG(LogTemp, Log, TEXT("[Container] Bound to OnItemReadyForSpawn event"));
	}

	// Debug log components
	UE_LOG(LogTemp, Log, TEXT("[Container] InteractableSM: %s, CachedLid: %s, NiagaraLoc: %s, ItemSpawn: %s, LootMgr: %s"),
		InteractableSM ? TEXT("Valid") : TEXT("NULL"),
		CachedLid ? TEXT("Valid") : TEXT("NULL"),
		CachedNiagaraLocation ? TEXT("Valid") : TEXT("NULL"),
		CachedItemSpawn ? TEXT("Valid") : TEXT("NULL"),
		CachedLootManager ? TEXT("Valid") : TEXT("NULL"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERACTION OVERRIDE [1/1]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFContainerBase::OnInteract_Implementation(AActor* Interactor)
{
	Super::OnInteract_Implementation(Interactor);

	// Don't re-open if already open - check FIRST
	if (bIsOpen)
	{
		UE_LOG(LogTemp, Log, TEXT("[Container] OnInteract - Already open"));
		return;
	}

	// IMMEDIATELY mark as open and disable interaction to prevent double-calls
	bIsOpen = true;
	bCanInteract = false;

	UE_LOG(LogTemp, Log, TEXT("[Container] OnInteract - Opening container"));

	// Load and play open montage on the interactor FIRST
	if (!OpenMontage.IsNull())
	{
		UAnimMontage* LoadedMontage = OpenMontage.LoadSynchronous();
		if (LoadedMontage && Interactor)
		{
			// Play montage on interactor via interface
			if (Interactor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
			{
				IBPI_GenericCharacter::Execute_PlayMontageReplicated(
					Interactor, LoadedMontage, SpeedMultiplier, 0.0, NAME_None);
				UE_LOG(LogTemp, Log, TEXT("[Container] Playing open montage: %s"), *LoadedMontage->GetName());
			}
		}
	}

	// Set timer to open lid and spawn items AFTER delay (syncs with animation)
	GetWorldTimerManager().SetTimer(
		OpenTimerHandle,
		this,
		&ASLFContainerBase::DelayedOpenLidAndSpawnItems,
		OpenDelayTime,
		false  // Don't loop
	);

	UE_LOG(LogTemp, Log, TEXT("[Container] Timer set for %.2f seconds"), OpenDelayTime);
}

void ASLFContainerBase::DelayedOpenLidAndSpawnItems()
{
	UE_LOG(LogTemp, Log, TEXT("[Container] DelayedOpenLidAndSpawnItems - Starting lid animation"));

	// Reset animation state
	bIsOpeningLid = true;
	LidOpenProgress = 0.0f;
	OpenElapsedTime = 0.0f;
	bVFXSpawned = false;
	bItemsSpawned = false;

	// Cache the starting rotation
	if (CachedLid)
	{
		LidStartRotation = CachedLid->GetRelativeRotation();
		UE_LOG(LogTemp, Log, TEXT("[Container] Lid start rotation: %s"), *LidStartRotation.ToString());
	}
}

void ASLFContainerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Simple tick-based lid animation
	if (bIsOpeningLid)
	{
		OpenElapsedTime += DeltaTime;

		// Update progress (0 to 1)
		LidOpenProgress = FMath::Clamp(OpenElapsedTime / LidOpenDuration, 0.0f, 1.0f);

		// Lerp the lid rotation
		if (CachedLid)
		{
			float CurrentAngle = FMath::Lerp(0.0f, LidOpenAngle, LidOpenProgress);
			FRotator NewRotation = FRotator(
				LidStartRotation.Pitch + CurrentAngle,
				LidStartRotation.Yaw,
				LidStartRotation.Roll
			);
			CachedLid->SetRelativeRotation(NewRotation);
		}

		// Spawn VFX at the right time
		if (!bVFXSpawned && OpenElapsedTime >= VFXSpawnTime)
		{
			bVFXSpawned = true;
			if (!OpenVFX.IsNull())
			{
				UNiagaraSystem* LoadedVFX = OpenVFX.LoadSynchronous();
				if (LoadedVFX)
				{
					FVector SpawnLoc = CachedNiagaraLocation ?
						CachedNiagaraLocation->GetComponentLocation() :
						GetActorLocation();
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, LoadedVFX, SpawnLoc);
					UE_LOG(LogTemp, Log, TEXT("[Container] VFX spawned at %.2fs"), OpenElapsedTime);
				}
			}
		}

		// Spawn items at the right time
		if (!bItemsSpawned && OpenElapsedTime >= ItemSpawnTime)
		{
			bItemsSpawned = true;
			if (CachedLootManager)
			{
				CachedLootManager->PickItem();
				bool bHasLootConfig = CachedLootManager->IsOverrideItemValid() || !CachedLootManager->LootTable.IsNull();
				if (!bHasLootConfig)
				{
					SpawnDefaultLootPickup();
				}
			}
			else
			{
				SpawnDefaultLootPickup();
			}
			UE_LOG(LogTemp, Log, TEXT("[Container] Items spawned at %.2fs"), OpenElapsedTime);
		}

		// Check if animation is complete
		if (LidOpenProgress >= 1.0f)
		{
			bIsOpeningLid = false;
			UE_LOG(LogTemp, Log, TEXT("[Container] Lid animation complete"));
		}
	}
}

void ASLFContainerBase::OpenLid()
{
	// Instant lid open (bypasses timeline animation)
	// Use this for debugging or if you need to skip the animation
	if (CachedLid)
	{
		FRotator CurrentRotation = CachedLid->GetRelativeRotation();
		FRotator OpenRotation = FRotator(CurrentRotation.Pitch + LidOpenAngle, CurrentRotation.Yaw, CurrentRotation.Roll);
		CachedLid->SetRelativeRotation(OpenRotation);
		UE_LOG(LogTemp, Log, TEXT("[Container] OpenLid (instant) - Set rotation to: %s"), *OpenRotation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Container] CachedLid is NULL - cannot open!"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// LOOT SPAWNING
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFContainerBase::HandleItemReadyForSpawn(FSLFLootItem Item)
{
	// Calculate actual spawn amount between MinAmount and MaxAmount
	int32 SpawnAmount = FMath::RandRange(Item.MinAmount, Item.MaxAmount);

	UE_LOG(LogTemp, Log, TEXT("[Container] HandleItemReadyForSpawn - Item: %s, Amount: %d (min=%d, max=%d)"),
		Item.Item ? *Item.Item->GetName() : TEXT("NULL"),
		SpawnAmount, Item.MinAmount, Item.MaxAmount);

	SpawnLootPickup(Item, SpawnAmount);
}

void ASLFContainerBase::SpawnLootPickup(const FSLFLootItem& Item, int32 Amount)
{
	// Need a valid item to spawn
	if (!Item.Item && !Item.ItemClass.Get())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Container] SpawnLootPickup - No item to spawn"));
		return;
	}

	// Get spawn location from ItemSpawn component
	FVector SpawnLocation = CachedItemSpawn ?
		CachedItemSpawn->GetComponentLocation() :
		GetActorLocation() + FVector(0, 0, 50);

	FRotator SpawnRotation = FRotator::ZeroRotator;

	UE_LOG(LogTemp, Log, TEXT("[Container] Spawning pickup at: %s"), *SpawnLocation.ToString());

	// Load the pickup class - B_PickupItem
	UClass* PickupClass = Item.ItemClass.Get();
	if (!PickupClass)
	{
		// Default to B_PickupItem if no specific class
		static const FSoftClassPath PickupClassPath(TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem.B_PickupItem_C"));
		PickupClass = PickupClassPath.TryLoadClass<AActor>();
	}

	if (!PickupClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Container] Could not load pickup class"));
		return;
	}

	// Spawn the pickup actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedPickup = GetWorld()->SpawnActor<AActor>(PickupClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedPickup)
	{
		UE_LOG(LogTemp, Log, TEXT("[Container] Spawned pickup: %s"), *SpawnedPickup->GetName());

		// Set the item data on the pickup (if it has the property)
		// The pickup's Item and Count properties need to be set
		if (Item.Item)
		{
			// Try to set Item property via reflection
			FProperty* ItemProp = SpawnedPickup->GetClass()->FindPropertyByName(TEXT("Item"));
			if (ItemProp)
			{
				if (FObjectProperty* ObjProp = CastField<FObjectProperty>(ItemProp))
				{
					ObjProp->SetObjectPropertyValue_InContainer(SpawnedPickup, Item.Item);
					UE_LOG(LogTemp, Log, TEXT("[Container] Set Item property on pickup"));
				}
			}

			// Try to set Count property
			FProperty* CountProp = SpawnedPickup->GetClass()->FindPropertyByName(TEXT("Count"));
			if (CountProp)
			{
				if (FIntProperty* IntProp = CastField<FIntProperty>(CountProp))
				{
					IntProp->SetPropertyValue_InContainer(SpawnedPickup, Amount);
					UE_LOG(LogTemp, Log, TEXT("[Container] Set Count property on pickup: %d"), Amount);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Container] Failed to spawn pickup!"));
	}
}

void ASLFContainerBase::SpawnDefaultLootPickup()
{
	UE_LOG(LogTemp, Warning, TEXT("[Container] SpawnDefaultLootPickup - Spawning default item"));

	// Get spawn location from ItemSpawn component
	FVector SpawnLocation = CachedItemSpawn ?
		CachedItemSpawn->GetComponentLocation() :
		GetActorLocation() + FVector(0, 0, 50);

	FRotator SpawnRotation = FRotator::ZeroRotator;

	UE_LOG(LogTemp, Log, TEXT("[Container] Default spawn location: %s"), *SpawnLocation.ToString());

	// Load B_PickupItem class
	static const FSoftClassPath PickupClassPath(TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem.B_PickupItem_C"));
	UClass* PickupClass = PickupClassPath.TryLoadClass<AActor>();

	if (!PickupClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Container] Could not load B_PickupItem class"));
		return;
	}

	// Spawn the pickup actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedPickup = GetWorld()->SpawnActor<AActor>(PickupClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedPickup)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Container] Spawned default pickup: %s"), *SpawnedPickup->GetName());

		// Try to load a default item - DA_Sword01
		static const FSoftObjectPath DefaultItemPath(TEXT("/Game/SoulslikeFramework/Data/Items/DA_Sword01.DA_Sword01"));
		UObject* DefaultItem = DefaultItemPath.TryLoad();

		if (DefaultItem)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Container] Loaded default item: %s"), *DefaultItem->GetName());

			// Set the Item property on the pickup
			FProperty* ItemProp = SpawnedPickup->GetClass()->FindPropertyByName(TEXT("Item"));
			if (ItemProp)
			{
				if (FObjectProperty* ObjProp = CastField<FObjectProperty>(ItemProp))
				{
					ObjProp->SetObjectPropertyValue_InContainer(SpawnedPickup, DefaultItem);
					UE_LOG(LogTemp, Warning, TEXT("[Container] Set Item property on default pickup"));
				}
			}

			// Set Count to 1
			FProperty* CountProp = SpawnedPickup->GetClass()->FindPropertyByName(TEXT("Count"));
			if (CountProp)
			{
				if (FIntProperty* IntProp = CastField<FIntProperty>(CountProp))
				{
					IntProp->SetPropertyValue_InContainer(SpawnedPickup, 1);
					UE_LOG(LogTemp, Warning, TEXT("[Container] Set Count=1 on default pickup"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Container] Could not load default item DA_Sword01"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Container] Failed to spawn default pickup!"));
	}
}
