// SLFItemBase.h
// C++ base class for B_Item_Weapon and other specialized items
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Item (extended)
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         Inherited from AB_Item (ItemInfo)
// Functions:         1/1 migrated (SetupItem)
// Components:        0/0 (DefaultSceneRoot comes from Blueprint SCS to avoid collision)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════
//
// Hierarchy: AActor → AB_Item → ASLFItemBase → ASLFWeaponBase
//
// PURPOSE: Extended item functionality for specialized items
// CHILDREN: ASLFWeaponBase (B_Item_Weapon), etc.

#pragma once

#include "CoreMinimal.h"
#include "B_Item.h"
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
#include "SLFItemBase.generated.h"

// Forward declarations
class UDataAsset;

/**
 * Extended item actor - base for weapons and specialized equipment
 * Inherits ItemInfo and ISLFItemInterface from AB_Item
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFItemBase : public AB_Item
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

	// ItemInfo is inherited from AB_Item

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
