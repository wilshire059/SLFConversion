// B_PickupItem.cpp
// C++ implementation for Blueprint B_PickupItem
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_PickupItem.json

#include "Blueprints/B_PickupItem.h"
#include "Interfaces/BPI_Player.h"
#include "Components/SaveLoadManagerComponent.h"
#include "Kismet/GameplayStatics.h"

AB_PickupItem::AB_PickupItem()
{
	// Components (Support Collision, World Niagara) are created via Blueprint SCS
	// and use the custom "Interactable" collision channel (ECC_GameTraceChannel1)
	// AC_InteractionManager must trace for this channel to detect pickup items
}

FSLFItemInfo AB_PickupItem::TryGetItemInfo_Implementation()
{
	// Return the cached ItemInfo if available, otherwise try to get from Item asset
	if (Item)
	{
		// If we have a valid Item asset, try to get its ItemInformation
		if (UPDA_Item* ItemAsset = Cast<UPDA_Item>(Item))
		{
			return ItemAsset->ItemInformation;
		}
	}
	return ItemInfo;
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERACTION - From Blueprint B_PickupItem EventGraph
// ═══════════════════════════════════════════════════════════════════════════════

void AB_PickupItem::OnInteract_Implementation(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Log, TEXT("[B_PickupItem] OnInteract by %s"),
		InteractingActor ? *InteractingActor->GetName() : TEXT("null"));

	if (!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_PickupItem] No item to pick up"));
		return;
	}

	// Call BPI_Player::OnLootItem on the player
	// The player's OnLootItem handles adding item to inventory and playing pickup animation
	if (InteractingActor && InteractingActor->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
	{
		IBPI_Player::Execute_OnLootItem(InteractingActor, this);
		UE_LOG(LogTemp, Log, TEXT("[B_PickupItem] Called BPI_Player::OnLootItem on %s"), *InteractingActor->GetName());
	}

	// Mark this pickup as collected in the save system so it won't respawn on load
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (USaveLoadManagerComponent* SaveMgr = PC->FindComponentByClass<USaveLoadManagerComponent>())
		{
			SaveMgr->MarkPickupCollected(this);
		}
	}

	// Broadcast OnItemLooted dispatcher
	OnItemLooted.Broadcast();

	// Destroy after pickup
	Destroy();
}

void AB_PickupItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// When destroyed (by either C++ OnInteract_Implementation or Blueprint EventGraph),
	// mark as collected so it won't respawn on save/load.
	// This is defensive - if the Blueprint's EventGraph still handles OnInteract instead of
	// the C++ _Implementation, the MarkPickupCollected in OnInteract_Implementation never fires.
	// EndPlay always fires on Destroy() regardless of which code path triggered it.
	if (EndPlayReason == EEndPlayReason::Destroyed && Item)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			if (USaveLoadManagerComponent* SaveMgr = PC->FindComponentByClass<USaveLoadManagerComponent>())
			{
				SaveMgr->MarkPickupCollected(this);
				UE_LOG(LogTemp, Log, TEXT("[B_PickupItem] EndPlay - Marked pickup as collected: %s"), *GetName());
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}
