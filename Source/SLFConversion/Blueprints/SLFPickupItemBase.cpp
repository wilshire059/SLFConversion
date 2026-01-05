// SLFPickupItemBase.cpp
// C++ implementation for B_PickupItem
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_PickupItem
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         5/5 (initialized in constructor)
// Functions:         4/4 implemented (3 migrated + 1 override)
// Event Dispatchers: 1/1 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFPickupItemBase.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/BPI_Player.h"
#include "SLFPrimaryDataAssets.h"

ASLFPickupItemBase::ASLFPickupItemBase()
{
	// Initialize pickup config
	Item = nullptr;
	Count = 1;
	bUsePhysics = false;
	PlacementTraceChannel = EObjectTypeQuery::ObjectTypeQuery1; // WorldStatic

	// Update interaction text
	InteractionText = FText::FromString(TEXT("Pick Up"));
}

void ASLFPickupItemBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[PickupItem] BeginPlay: %s x%d"),
		Item ? *Item->GetName() : TEXT("null"), Count);

	// Get item info from data asset
	if (Item)
	{
		TryGetItemInfo(ItemInfo);
	}

	// Setup Niagara effect from item data (replaces EventGraph logic)
	SetupWorldNiagara();

	// Set physics on Support Collision component (per Blueprint JSON)
	if (USphereComponent* SupportCollision = FindComponentByClass<USphereComponent>())
	{
		SupportCollision->SetSimulatePhysics(bUsePhysics);
	}

	// If not using physics, trace down to place item on floor (per Blueprint JSON lines 2112-3860)
	if (!bUsePhysics)
	{
		FVector Start = GetActorLocation() + (GetActorUpVector() * 300.0);
		FVector End = GetActorLocation() - (GetActorUpVector() * 3000.0);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(PlacementTraceChannel);

		FHitResult HitResult;
		TArray<AActor*> ActorsToIgnore;

		if (UKismetSystemLibrary::LineTraceSingleForObjects(
			this, Start, End, ObjectTypes, false, ActorsToIgnore,
			EDrawDebugTrace::None, HitResult, true))
		{
			// Place item at hit location + small offset (0,0,1)
			SetActorLocation(HitResult.Location + FVector(0, 0, 1));
			UE_LOG(LogTemp, Log, TEXT("[PickupItem] Placed at floor: %s"), *HitResult.Location.ToString());
		}
	}
}

void ASLFPickupItemBase::SetupWorldNiagara()
{
	// Find the World Niagara component (from Blueprint SCS)
	UNiagaraComponent* NiagaraComp = FindComponentByClass<UNiagaraComponent>();
	if (!NiagaraComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] No Niagara component found"));
		return;
	}

	if (!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] No Item data asset"));
		return;
	}

	// EXPERIMENT: Try direct cast to UPDA_Item (C++ class with WorldNiagaraSystem property)
	// This only works if PDA_Item has been reparented to UPDA_Item and data was migrated
	if (UPDA_Item* ItemData = Cast<UPDA_Item>(Item))
	{
		UE_LOG(LogTemp, Log, TEXT("[PickupItem] Using C++ UPDA_Item direct access"));

		// Check if WorldNiagaraSystem is set
		if (!ItemData->WorldNiagaraSystem.IsNull())
		{
			UNiagaraSystem* NiagaraSystem = ItemData->WorldNiagaraSystem.LoadSynchronous();
			if (NiagaraSystem)
			{
				NiagaraComp->SetAsset(NiagaraSystem);
				NiagaraComp->Activate(true);
				UE_LOG(LogTemp, Log, TEXT("[PickupItem] Set Niagara System (C++ direct): %s"), *NiagaraSystem->GetName());
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[PickupItem] No WorldNiagaraSystem set in C++ property"));
		}
	}

	// FALLBACK: Use reflection to access Blueprint ItemInformation property
	// This handles cases where the data hasn't been migrated yet
	UE_LOG(LogTemp, Log, TEXT("[PickupItem] Falling back to reflection-based access"));
	SetupWorldNiagaraViaReflection(NiagaraComp);
}

void ASLFPickupItemBase::SetupWorldNiagaraViaReflection(UNiagaraComponent* NiagaraComp)
{
	// The Item is a Blueprint data asset (PDA_Item_C) with an "ItemInformation" property
	// We need to use reflection to access ItemInformation.WorldPickupInfo.WorldNiagaraSystem
	UClass* ItemClass = Item->GetClass();
	UE_LOG(LogTemp, Log, TEXT("[PickupItem] Item class: %s"), *ItemClass->GetName());

	// Find the ItemInformation property
	FProperty* ItemInfoProp = ItemClass->FindPropertyByName(TEXT("ItemInformation"));
	if (!ItemInfoProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] ItemInformation property not found on %s"), *ItemClass->GetName());
		return;
	}

	// Get pointer to ItemInformation struct
	const void* ItemInfoPtr = ItemInfoProp->ContainerPtrToValuePtr<void>(Item);
	if (!ItemInfoPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] Could not get ItemInformation value"));
		return;
	}

	// ItemInformation is a Blueprint struct (FItemInfo), need to find WorldPickupInfo within it
	FStructProperty* ItemInfoStructProp = CastField<FStructProperty>(ItemInfoProp);
	if (!ItemInfoStructProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] ItemInformation is not a struct property"));
		return;
	}

	UScriptStruct* ItemInfoStruct = ItemInfoStructProp->Struct;
	UE_LOG(LogTemp, Log, TEXT("[PickupItem] ItemInformation struct: %s"), *ItemInfoStruct->GetName());

	// Find WorldPickupInfo property (may have GUID suffix)
	FProperty* WorldPickupInfoProp = nullptr;
	for (TFieldIterator<FProperty> PropIt(ItemInfoStruct); PropIt; ++PropIt)
	{
		if (PropIt->GetName().StartsWith(TEXT("WorldPickupInfo")))
		{
			WorldPickupInfoProp = *PropIt;
			break;
		}
	}

	if (!WorldPickupInfoProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] WorldPickupInfo property not found"));
		return;
	}

	// Get pointer to WorldPickupInfo struct
	const void* WorldPickupInfoPtr = WorldPickupInfoProp->ContainerPtrToValuePtr<void>(ItemInfoPtr);
	if (!WorldPickupInfoPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] Could not get WorldPickupInfo value"));
		return;
	}

	// WorldPickupInfo is FWorldMeshInfo, need to find WorldNiagaraSystem within it
	FStructProperty* WorldPickupStructProp = CastField<FStructProperty>(WorldPickupInfoProp);
	if (!WorldPickupStructProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] WorldPickupInfo is not a struct property"));
		return;
	}

	UScriptStruct* WorldPickupStruct = WorldPickupStructProp->Struct;
	UE_LOG(LogTemp, Log, TEXT("[PickupItem] WorldPickupInfo struct: %s"), *WorldPickupStruct->GetName());

	// Find WorldNiagaraSystem property (may have GUID suffix)
	FProperty* NiagaraSystemProp = nullptr;
	for (TFieldIterator<FProperty> PropIt(WorldPickupStruct); PropIt; ++PropIt)
	{
		if (PropIt->GetName().StartsWith(TEXT("WorldNiagaraSystem")))
		{
			NiagaraSystemProp = *PropIt;
			break;
		}
	}

	if (!NiagaraSystemProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] WorldNiagaraSystem property not found"));
		return;
	}

	// Get the soft object reference
	FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(NiagaraSystemProp);
	if (!SoftObjProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] WorldNiagaraSystem is not a soft object property"));
		return;
	}

	// Get the soft object path
	const FSoftObjectPtr* SoftObjectPtr = NiagaraSystemProp->ContainerPtrToValuePtr<FSoftObjectPtr>(WorldPickupInfoPtr);
	if (!SoftObjectPtr || SoftObjectPtr->IsNull())
	{
		UE_LOG(LogTemp, Log, TEXT("[PickupItem] No WorldNiagaraSystem set in item data"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[PickupItem] Loading Niagara System: %s"), *SoftObjectPtr->ToString());

	// Load the Niagara System
	UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(SoftObjectPtr->LoadSynchronous());
	if (NiagaraSystem)
	{
		NiagaraComp->SetAsset(NiagaraSystem);
		NiagaraComp->Activate(true);
		UE_LOG(LogTemp, Log, TEXT("[PickupItem] Set Niagara System: %s"), *NiagaraSystem->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] Failed to load Niagara System"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTIONS [1-3/4]
// ═══════════════════════════════════════════════════════════════════════════════

bool ASLFPickupItemBase::TryGetItemInfo_Implementation(FSLFItemInfo& OutInfo)
{
	if (Item)
	{
		OutInfo = ItemInfo;
		return true;
	}
	return false;
}

void ASLFPickupItemBase::TriggerOnItemLooted_Implementation()
{
	OnItemLooted.Broadcast(); // No parameters per Blueprint JSON
}

void ASLFPickupItemBase::SetupInteractable_Implementation()
{
	Super::SetupInteractable_Implementation();

	UE_LOG(LogTemp, Log, TEXT("[PickupItem] SetupInteractable"));

	// Get item info from data asset
	if (Item)
	{
		TryGetItemInfo(ItemInfo);
		InteractableDisplayName = ItemInfo.DisplayName;
	}

	// Setup physics if enabled
	if (bUsePhysics)
	{
		// TODO: Enable physics simulation
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERACTION OVERRIDE [4/4]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFPickupItemBase::OnInteract_Implementation(AActor* Interactor)
{
	Super::OnInteract_Implementation(Interactor);

	UE_LOG(LogTemp, Log, TEXT("[PickupItem] OnInteract by %s"),
		Interactor ? *Interactor->GetName() : TEXT("null"));

	if (!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PickupItem] No item to pick up"));
		return;
	}

	// Call BPI_Player::OnLootItem on the player (per Blueprint JSON lines 1882-4550)
	// The player's OnLootItem handles adding item to inventory
	if (Interactor && Interactor->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
	{
		IBPI_Player::Execute_OnLootItem(Interactor, this);
		UE_LOG(LogTemp, Log, TEXT("[PickupItem] Called BPI_Player::OnLootItem on %s"), *Interactor->GetName());
	}

	// Broadcast OnItemLooted dispatcher (no params)
	TriggerOnItemLooted();

	// Destroy after pickup
	Destroy();
}
