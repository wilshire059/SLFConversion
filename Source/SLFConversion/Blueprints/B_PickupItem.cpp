// B_PickupItem.cpp
// C++ implementation for Blueprint B_PickupItem
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_PickupItem.json

#include "Blueprints/B_PickupItem.h"
#include "Interfaces/BPI_Player.h"

AB_PickupItem::AB_PickupItem()
{
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

	// Broadcast OnItemLooted dispatcher
	OnItemLooted.Broadcast();

	// Destroy after pickup
	Destroy();
}
