// BPI_Item.h
// C++ Interface for BPI_Item
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_Item.json
// Functions: 3

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_Item.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_Item : public UInterface
{
	GENERATED_BODY()
};

/**
 * Item Interface
 * Provides item equip/unequip/use functionality
 */
class SLFCONVERSION_API IBPI_Item
{
	GENERATED_BODY()

public:
	// Called when weapon is unequipped
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item Interface")
	void OnWeaponUnequip();

	// Called when weapon is equipped
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item Interface")
	void OnWeaponEquip(bool bRightHand);

	// Called when item is used
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item Interface")
	void OnUse();
};
