// SLFItemBase.h
// C++ base class for B_Item
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Item
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         1/1 migrated
// Functions:         1/1 migrated (UserConstructionScript)
// Components:        0/0 (DefaultSceneRoot comes from Blueprint SCS to avoid collision)
// Event Dispatchers: 0/0
// Graphs:            2 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_Item
//
// PURPOSE: Base item actor - spawnable item in world (weapon, armor, tool)
// CHILDREN: B_Item_Weapon, B_Item_Lantern, etc.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
#include "SLFItemBase.generated.h"

// Forward declarations
class UDataAsset;

// Types used from SLFGameTypes.h:
// - FSLFItemInfo

/**
 * Base item actor - spawnable equipment/consumable in world
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFItemBase : public AActor
{
	GENERATED_BODY()

public:
	ASLFItemBase();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// NOTE: DefaultSceneRoot is defined in B_Item's Blueprint SCS, not in C++
	// This avoids name collision when B_Item inherits from SLFItemBase

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Item info struct with all item data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FSLFItemInfo ItemInfo;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Construction script - setup item visuals */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void SetupItem();
	virtual void SetupItem_Implementation();

	// --- Getters ---

	UFUNCTION(BlueprintPure, Category = "Item|Getters")
	FSLFItemInfo GetItemInfo() const { return ItemInfo; }

	UFUNCTION(BlueprintPure, Category = "Item|Getters")
	FGameplayTag GetItemTag() const { return ItemInfo.ItemTag; }
};
