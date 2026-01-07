// SLFContainer.cpp
#include "SLFContainer.h"
#include "Components/BoxComponent.h"
#include "Components/InventoryManagerComponent.h"

ASLFContainer::ASLFContainer()
{
	PrimaryActorTick.bCanEverTick = false;




	UE_LOG(LogTemp, Log, TEXT("[Container] Created"));
}

void ASLFContainer::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[Container] BeginPlay - LootEntries: %d, Currency: %d"), LootTable.Num(), DeathCurrencyAmount);
}

void ASLFContainer::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("[Container] Interact by: %s"), Interactor ? *Interactor->GetName() : TEXT("None"));

	if (bIsLooted)
	{
		UE_LOG(LogTemp, Log, TEXT("[Container] Already looted"));
		return;
	}

	if (!bIsOpen)
	{
		if (CanOpen(Interactor))
		{
			OpenContainer(Interactor);
		}
	}
	else
	{
		LootContainer(Interactor);
	}
}

void ASLFContainer::OpenContainer_Implementation(AActor* Opener)
{
	if (!bIsOpen)
	{
		bIsOpen = true;
		OnContainerOpened.Broadcast(Opener);
		UE_LOG(LogTemp, Log, TEXT("[Container] Opened by: %s"), Opener ? *Opener->GetName() : TEXT("None"));

		// Auto-loot if no animation
		if (!OpenAnimation)
		{
			LootContainer(Opener);
		}
	}
}

void ASLFContainer::LootContainer_Implementation(AActor* Looter)
{
	if (!bIsLooted)
	{
		bIsLooted = true;

		TArray<FSLFLootEntry> Loot = GenerateLoot();
		UE_LOG(LogTemp, Log, TEXT("[Container] Looted - Items: %d"), Loot.Num());

		// Add items to looter's inventory
		if (UInventoryManagerComponent* InventoryManager = Looter->FindComponentByClass<UInventoryManagerComponent>())
		{
			for (const FSLFLootEntry& Entry : Loot)
			{
				if (Entry.ItemClass)
				{
					// Create item instance and add to inventory
					UObject* ItemAsset = Entry.ItemClass->GetDefaultObject();
					if (UDataAsset* Item = Cast<UDataAsset>(ItemAsset))
					{
						InventoryManager->AddItem(Item, Entry.Quantity, true); // bTriggerLootUI = true
						UE_LOG(LogTemp, Log, TEXT("[Container] Added %s x%d to inventory"), *Item->GetName(), Entry.Quantity);
					}
				}
			}

			// Add currency if any
			if (DeathCurrencyAmount > 0)
			{
				InventoryManager->AdjustCurrency(DeathCurrencyAmount);
				UE_LOG(LogTemp, Log, TEXT("[Container] Added %d currency"), DeathCurrencyAmount);
			}
		}

		OnContainerLooted.Broadcast();
	}
}

TArray<FSLFLootEntry> ASLFContainer::GenerateLoot_Implementation()
{
	TArray<FSLFLootEntry> GeneratedLoot;

	for (const FSLFLootEntry& Entry : LootTable)
	{
		if (FMath::FRand() <= Entry.DropChance)
		{
			GeneratedLoot.Add(Entry);
			UE_LOG(LogTemp, Log, TEXT("[Container] Generated loot: %s x%d"),
				Entry.ItemClass ? *Entry.ItemClass->GetName() : TEXT("None"), Entry.Quantity);
		}
	}

	return GeneratedLoot;
}

bool ASLFContainer::CanOpen(AActor* Interactor) const
{
	if (bRequiresKey)
	{
		// Check if interactor has required key in inventory
		// Note: Full key check requires iterating inventory to find item with matching tag
		// For now, log and check for any key-type items
		UE_LOG(LogTemp, Log, TEXT("[Container] Requires key with tag: %s"), *RequiredKeyTag.ToString());
		if (UInventoryManagerComponent* InventoryManager = Interactor->FindComponentByClass<UInventoryManagerComponent>())
		{
			// Inventory manager would need to iterate items to find one with matching tag
			// This is a placeholder - full implementation would check item tags
			UE_LOG(LogTemp, Warning, TEXT("[Container] Key check requires tag iteration - not fully implemented"));
		}
		return false;
	}
	return true;
}

void ASLFContainer::EventOpenLidAndSpawnItem(bool bSpawnItem, bool bSpawnVFX)
{
	UE_LOG(LogTemp, Log, TEXT("[Container] EventOpenLidAndSpawnItem - SpawnItem: %s, SpawnVFX: %s"),
		bSpawnItem ? TEXT("true") : TEXT("false"),
		bSpawnVFX ? TEXT("true") : TEXT("false"));

	// Open the lid (set state)
	bIsOpen = true;

	// Spawn VFX if requested
	if (bSpawnVFX)
	{
		// VFX spawning would be done via Niagara system
		// The specific system would be referenced in Blueprint data
		UE_LOG(LogTemp, Log, TEXT("[Container] VFX spawn requested"));
	}

	// Spawn pickup item if requested
	if (bSpawnItem)
	{
		// Spawn pickup items from loot table
		TArray<FSLFLootEntry> Loot = GenerateLoot();
		for (const FSLFLootEntry& Entry : Loot)
		{
			if (Entry.ItemClass)
			{
				// Spawn pickup actor at container location
				// The actual spawning depends on B_PickupItem class
				UE_LOG(LogTemp, Log, TEXT("[Container] Would spawn pickup: %s x%d"),
					*Entry.ItemClass->GetName(), Entry.Quantity);
			}
		}
	}
}

void ASLFContainer::EventOnSpawnedItemLooted()
{
	UE_LOG(LogTemp, Log, TEXT("[Container] EventOnSpawnedItemLooted"));

	// Called when the spawned pickup item is looted by player
	// Mark container as looted
	bIsLooted = true;

	// Broadcast looted event
	OnContainerLooted.Broadcast();
}

void ASLFContainer::EventInitializeLoadedStates()
{
	UE_LOG(LogTemp, Log, TEXT("[Container] EventInitializeLoadedStates - IsOpen: %s, IsLooted: %s"),
		bIsOpen ? TEXT("true") : TEXT("false"),
		bIsLooted ? TEXT("true") : TEXT("false"));

	// Restore visual state based on loaded data
	// If already open/looted, update mesh/animation state to match
	if (bIsOpen)
	{
		// Would set mesh to open position
		UE_LOG(LogTemp, Log, TEXT("[Container] Restoring open state"));
	}

	if (bIsLooted)
	{
		// Would disable interaction
		UE_LOG(LogTemp, Log, TEXT("[Container] Container was already looted"));
	}
}
