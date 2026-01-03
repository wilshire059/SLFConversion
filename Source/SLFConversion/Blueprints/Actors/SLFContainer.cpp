// SLFContainer.cpp
#include "SLFContainer.h"
#include "Components/BoxComponent.h"

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

		// TODO: Add items to looter's inventory

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
		// TODO: Check if interactor has required key
		return false;
	}
	return true;
}
