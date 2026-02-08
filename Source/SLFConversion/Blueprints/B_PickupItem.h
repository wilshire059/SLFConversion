// B_PickupItem.h
// C++ class for Blueprint B_PickupItem
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_PickupItem.json
// Parent: B_Interactable_C -> AB_Interactable
// Variables: 5 | Functions: 2 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Interactable.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "SkeletalMergingLibrary.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Field/FieldSystemObjects.h"
#include "B_PickupItem.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_PickupItem_OnItemLooted);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_PickupItem : public AB_Interactable
{
	GENERATED_BODY()

public:
	AB_PickupItem();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FSLFItemInfo ItemInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 Count;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool UsePhysics;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TEnumAsByte<EObjectTypeQuery> PlacementTraceChannel;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_PickupItem_OnItemLooted OnItemLooted;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_PickupItem")
	FSLFItemInfo TryGetItemInfo();
	virtual FSLFItemInfo TryGetItemInfo_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE OVERRIDES (ISLFInteractableInterface)
	// ═══════════════════════════════════════════════════════════════════════

	/** OnInteract - Called when player interacts with this pickup item */
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;

protected:
	/** Mark pickup as collected when destroyed (works even if Blueprint EventGraph handles OnInteract) */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
