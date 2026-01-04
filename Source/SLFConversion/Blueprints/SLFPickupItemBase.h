// SLFPickupItemBase.h
// C++ base class for B_PickupItem
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_PickupItem
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         5/5 migrated (excluding 1 dispatcher counted separately)
// Functions:         3/3 migrated (excluding UserConstructionScript)
// Event Dispatchers: 1/1 migrated
// Graphs:            4 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_PickupItem
//
// PURPOSE: Pickup item actor - lootable items in world
// PARENT: B_Interactable

#pragma once

#include "CoreMinimal.h"
#include "SLFInteractableBase.h"
#include "SLFItemBase.h"
#include "SLFPickupItemBase.generated.h"

// Forward declarations
class UDataAsset;

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 1/1 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/1] Called when item is looted by player (no parameters per Blueprint JSON) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickupItemLooted);

/**
 * Pickup item actor - lootable item in world
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFPickupItemBase : public ASLFInteractableBase
{
	GENERATED_BODY()

public:
	ASLFPickupItemBase();

protected:
	virtual void BeginPlay() override;

	/** Setup Niagara effect from item data - replaces EventGraph logic */
	void SetupWorldNiagara();

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 5/5 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/5] Item info struct */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FSLFItemInfo ItemInfo;

	/** [2/5] Item data asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	UDataAsset* Item;

	/** [3/5] Stack count for this pickup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	int32 Count;

	/** [4/5] Whether to use physics simulation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bUsePhysics;

	/** [5/5] Trace channel for placement (EObjectTypeQuery per Blueprint JSON) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	TEnumAsByte<EObjectTypeQuery> PlacementTraceChannel;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPickupItemLooted OnItemLooted;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 3/3 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/3] Try to get item info from data asset
	 * @param OutInfo - Retrieved item info
	 * @return True if successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	bool TryGetItemInfo(FSLFItemInfo& OutInfo);
	virtual bool TryGetItemInfo_Implementation(FSLFItemInfo& OutInfo);

	/** [2/3] Trigger OnItemLooted dispatcher */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup|Events")
	void TriggerOnItemLooted();
	virtual void TriggerOnItemLooted_Implementation();

	/** [3/3] Setup pickup visuals */
	virtual void SetupInteractable_Implementation() override;

	// --- Interaction Override ---

	virtual void OnInteract_Implementation(AActor* Interactor) override;
};
