// B_Item_AI_Weapon.h
// C++ class for Blueprint B_Item_AI_Weapon
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Item_AI_Weapon.json
// Parent: B_Item_Weapon_C -> AB_Item_Weapon
// Variables: 1 | Functions: 2 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Item_Weapon.h"
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
#include "B_Item_AI_Weapon.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Item_AI_Weapon : public AB_Item_Weapon
{
	GENERATED_BODY()

public:
	AB_Item_AI_Weapon();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IMPORTANT")
	UPrimaryDataAsset* ItemData;

	/** Default rotation offset for socket attachment.
	 * Applied when attaching to owner skeleton. Different skeletons may have
	 * different socket orientations - this compensates for those differences. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Attachment")
	FRotator DefaultAttachmentRotationOffset;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

};
