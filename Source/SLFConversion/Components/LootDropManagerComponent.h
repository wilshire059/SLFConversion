// LootDropManagerComponent.h
// C++ base class for AC_LootDropManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_LootDropManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 migrated (LootTable, OverrideItem)
// Functions:         4/4 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 1/1 migrated
// Graphs:            4 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_LootDropManager
//
// PURPOSE: Handles loot drops from enemies using weighted random selection

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "SLFGameTypes.h"
#include "LootDropManagerComponent.generated.h"

// Types used from SLFGameTypes.h:
// - FSLFLootItem
// - FSLFWeightedLoot

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 1/1 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/1] Called when a loot item is ready to be spawned */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemReadyForSpawn, FSLFLootItem, Item);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API ULootDropManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULootDropManagerComponent();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] Soft reference to loot data table */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UDataTable> LootTable;

	/** [2/2] Override item (bypasses table if valid) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FSLFLootItem OverrideItem;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Broadcast when a loot item is selected and ready for spawning */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemReadyForSpawn OnItemReadyForSpawn;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 4/4 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Query Functions (1) ---

	/** [1/4] Check if override item is valid
	 * @return True if OverrideItem has a valid class reference
	 */
	UFUNCTION(BlueprintPure, Category = "Loot Drop Manager|Query")
	bool IsOverrideItemValid() const;

	// --- Selection Functions (1) ---

	/** [2/4] Get a random item from the loot table using weighted selection
	 * @param InLootTable - Data table to select from
	 * @param OutSelectedItem - The randomly selected item
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Loot Drop Manager")
	void GetRandomItemFromTable(const UDataTable* InLootTable, FSLFLootItem& OutSelectedItem);
	virtual void GetRandomItemFromTable_Implementation(const UDataTable* InLootTable, FSLFLootItem& OutSelectedItem);

	// --- Main Function (1) ---

	/** [3/4] Pick an item from table or override and broadcast for spawning */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Loot Drop Manager")
	void PickItem();
	virtual void PickItem_Implementation();

	// --- Internal Callbacks (1) ---

	/** [4/4] Callback when async object load completes */
	UFUNCTION()
	void OnLoaded_3F8DEA424F264C1F3CEA99818FC7D680(UObject* Loaded);
};
