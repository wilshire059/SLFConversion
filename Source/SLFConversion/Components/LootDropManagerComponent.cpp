// LootDropManagerComponent.cpp
// C++ implementation for AC_LootDropManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_LootDropManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 (initialized in constructor)
// Functions:         4/4 implemented
// Event Dispatchers: 1/1 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "LootDropManagerComponent.h"
#include "Engine/DataTable.h"

ULootDropManagerComponent::ULootDropManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config
	LootTable = nullptr;
	OverrideItem = FSLFLootItem();
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUERY FUNCTIONS [1/4]
// ═══════════════════════════════════════════════════════════════════════════════

bool ULootDropManagerComponent::IsOverrideItemValid() const
{
	return OverrideItem.ItemClass != nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SELECTION FUNCTIONS [2/4]
// ═══════════════════════════════════════════════════════════════════════════════

void ULootDropManagerComponent::GetRandomItemFromTable_Implementation(const UDataTable* InLootTable, FSLFLootItem& OutSelectedItem)
{
	if (!InLootTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LootDropManager] GetRandomItemFromTable: Invalid table"));
		OutSelectedItem = FSLFLootItem();
		return;
	}

	// Get all rows from the table
	TArray<FSLFWeightedLoot*> AllRows;
	InLootTable->GetAllRows<FSLFWeightedLoot>(TEXT("GetRandomItemFromTable"), AllRows);

	if (AllRows.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LootDropManager] GetRandomItemFromTable: Empty table"));
		OutSelectedItem = FSLFLootItem();
		return;
	}

	// Calculate total weight
	double TotalWeight = 0.0;
	for (const FSLFWeightedLoot* Row : AllRows)
	{
		if (Row)
		{
			TotalWeight += Row->Weight;
		}
	}

	// Generate random value
	double RandomValue = FMath::FRandRange(0.0, TotalWeight);

	// Find the selected item using weighted random
	double CurrentWeight = 0.0;
	for (const FSLFWeightedLoot* Row : AllRows)
	{
		if (Row)
		{
			CurrentWeight += Row->Weight;
			if (RandomValue <= CurrentWeight)
			{
				OutSelectedItem = Row->Item;
				UE_LOG(LogTemp, Log, TEXT("[LootDropManager] Selected item with weight %.2f"), Row->Weight);
				return;
			}
		}
	}

	// Fallback to last item
	if (AllRows.Num() > 0 && AllRows.Last())
	{
		OutSelectedItem = AllRows.Last()->Item;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// MAIN FUNCTION [3/4]
// ═══════════════════════════════════════════════════════════════════════════════

void ULootDropManagerComponent::PickItem_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LootDropManager] PickItem"));

	FSLFLootItem SelectedItem;

	// Check for override item first
	if (IsOverrideItemValid())
	{
		SelectedItem = OverrideItem;
		UE_LOG(LogTemp, Log, TEXT("[LootDropManager] Using override item"));
	}
	else if (!LootTable.IsNull()) // TSoftObjectPtr has IsNull()
	{
		// Load the data table synchronously (or async if needed)
		UDataTable* LoadedTable = LootTable.LoadSynchronous();
		if (LoadedTable)
		{
			GetRandomItemFromTable(LoadedTable, SelectedItem);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[LootDropManager] Failed to load loot table"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[LootDropManager] No loot table or override set"));
		return;
	}

	// Broadcast the selected item
	if (SelectedItem.ItemClass != nullptr)
	{
		OnItemReadyForSpawn.Broadcast(SelectedItem);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERNAL CALLBACKS [4/4]
// ═══════════════════════════════════════════════════════════════════════════════

void ULootDropManagerComponent::OnLoaded_3F8DEA424F264C1F3CEA99818FC7D680(UObject* Loaded)
{
	// Async load callback - item class has been loaded
	UE_LOG(LogTemp, Log, TEXT("[LootDropManager] Async load completed: %s"),
		Loaded ? *Loaded->GetName() : TEXT("null"));
}
