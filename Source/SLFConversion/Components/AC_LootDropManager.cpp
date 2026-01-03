// AC_LootDropManager.cpp
// C++ component implementation for AC_LootDropManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_LootDropManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_LootDropManager.h"
#include "Engine/DataTable.h"

UAC_LootDropManager::UAC_LootDropManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAC_LootDropManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_LootDropManager::BeginPlay"));
}

void UAC_LootDropManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * GetRandomItemFromTable - Select weighted random item from loot table
 *
 * Blueprint Logic:
 * 1. Get all row names from DataTable
 * 2. For each row, get FSLFWeightedLoot struct
 * 3. Accumulate weight scores in map
 * 4. Generate random value 0-TotalWeight
 * 5. Return matching item
 */
FSLFLootItem UAC_LootDropManager::GetRandomItemFromTable_Implementation(UDataTable* InLootTable)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_LootDropManager::GetRandomItemFromTable"));

	FSLFLootItem Result;

	if (!IsValid(InLootTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid loot table"));
		return Result;
	}

	// Get all row names
	TArray<FName> RowNames = InLootTable->GetRowNames();
	if (RowNames.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Loot table has no rows"));
		return Result;
	}

	// Build weighted list
	TMap<FName, double> WeightedItems;
	double TotalWeight = 0.0;

	for (const FName& RowName : RowNames)
	{
		FSLFWeightedLoot* LootRow = InLootTable->FindRow<FSLFWeightedLoot>(RowName, TEXT("GetRandomItemFromTable"));
		if (LootRow)
		{
			TotalWeight += LootRow->Weight;
			WeightedItems.Add(RowName, TotalWeight);
		}
	}

	if (TotalWeight <= 0.0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Total weight is 0"));
		return Result;
	}

	// Generate random value
	double RandomValue = FMath::RandRange(0.0, TotalWeight);

	// Find matching item
	for (const auto& Pair : WeightedItems)
	{
		if (RandomValue <= Pair.Value)
		{
			FSLFWeightedLoot* LootRow = InLootTable->FindRow<FSLFWeightedLoot>(Pair.Key, TEXT("GetRandomItemFromTable"));
			if (LootRow)
			{
				Result = LootRow->Item;
				UE_LOG(LogTemp, Log, TEXT("  Selected item from row: %s"), *Pair.Key.ToString());
			}
			break;
		}
	}

	return Result;
}

/**
 * IsOverrideItemValid - Check if override item is set and valid
 */
bool UAC_LootDropManager::IsOverrideItemValid_Implementation()
{
	bool bValid = (OverrideItem.Item != nullptr) || (OverrideItem.ItemClass.Get() != nullptr);

	UE_LOG(LogTemp, Log, TEXT("UAC_LootDropManager::IsOverrideItemValid - %s"), bValid ? TEXT("true") : TEXT("false"));

	return bValid;
}

/**
 * PickItem - Select an item (override or random) and broadcast event
 */
void UAC_LootDropManager::PickItem_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_LootDropManager::PickItem"));

	FSLFLootItem SelectedItem;

	if (IsOverrideItemValid())
	{
		// Use override item
		SelectedItem = OverrideItem;
		UE_LOG(LogTemp, Log, TEXT("  Using override item"));
	}
	else
	{
		// Get from loot table
		UDataTable* LoadedTable = LootTable.LoadSynchronous();
		if (IsValid(LoadedTable))
		{
			SelectedItem = GetRandomItemFromTable(LoadedTable);
		}
	}

	// Broadcast the selected item
	OnItemReadyForSpawn.Broadcast(SelectedItem);
}
