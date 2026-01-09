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
#include "Components/StaticMeshComponent.h"
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

	// Get item info from data asset via interface function
	if (Item)
	{
		ItemInfo = ISLFInteractableInterface::Execute_TryGetItemInfo(this);
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
		return;
	}

	if (!Item)
	{
		return;
	}

	// Direct C++ property access - Item must be reparented to UPDA_Item
	if (UPDA_Item* ItemData = Cast<UPDA_Item>(Item))
	{
		if (!ItemData->WorldNiagaraSystem.IsNull())
		{
			UNiagaraSystem* NiagaraSystem = ItemData->WorldNiagaraSystem.LoadSynchronous();
			if (NiagaraSystem)
			{
				NiagaraComp->SetAsset(NiagaraSystem);
				NiagaraComp->Activate(true);
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTIONS [1-3/4]
// ═══════════════════════════════════════════════════════════════════════════════

FSLFItemInfo ASLFPickupItemBase::TryGetItemInfo_Implementation()
{
	// Read ItemInformation directly from the UPDA_Item data asset
	if (UPDA_Item* ItemData = Cast<UPDA_Item>(Item))
	{
		const FSLFItemInfo& Info = ItemData->ItemInformation;
		UE_LOG(LogTemp, Log, TEXT("[PickupItem] TryGetItemInfo - Got ItemInfo: DisplayName='%s', Tag='%s'"),
			*Info.DisplayName.ToString(), *Info.ItemTag.ToString());
		return Info;
	}
	UE_LOG(LogTemp, Warning, TEXT("[PickupItem] TryGetItemInfo - Item is not UPDA_Item: %s"),
		Item ? *Item->GetName() : TEXT("null"));
	return FSLFItemInfo();
}

void ASLFPickupItemBase::TriggerOnItemLooted_Implementation()
{
	OnItemLooted.Broadcast(); // No parameters per Blueprint JSON
}

void ASLFPickupItemBase::SetupInteractable_Implementation()
{
	Super::SetupInteractable_Implementation();

	UE_LOG(LogTemp, Log, TEXT("[PickupItem] SetupInteractable"));

	// Get item info from data asset via interface function
	if (Item)
	{
		ItemInfo = ISLFInteractableInterface::Execute_TryGetItemInfo(this);
		InteractableDisplayName = ItemInfo.DisplayName;
	}

	// Setup physics if enabled
	// Physics simulation is applied to the mesh component defined in the Blueprint's SCS (Simple Construction Script)
	// The actual mesh component doesn't exist in this C++ class - it's added in the Blueprint editor
	if (bUsePhysics)
	{
		// Find the first static mesh component and enable physics
		if (UStaticMeshComponent* MeshComp = FindComponentByClass<UStaticMeshComponent>())
		{
			MeshComp->SetSimulatePhysics(true);
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
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
