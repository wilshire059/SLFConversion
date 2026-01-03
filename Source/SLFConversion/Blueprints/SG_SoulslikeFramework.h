// SG_SoulslikeFramework.h
// C++ class for Blueprint SG_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/SG_SoulslikeFramework.json
// Parent: SaveGame -> UObject
// Variables: 1 | Functions: 2 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "CoreMinimal.h"
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
#include "SG_SoulslikeFramework.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USG_SoulslikeFramework : public UObject
{
	GENERATED_BODY()

public:
	USG_SoulslikeFramework();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FSLFSaveGameInfo SavedData;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SG_SoulslikeFramework")
	void SetSavedData(const FSLFSaveGameInfo& InSavedData);
	virtual void SetSavedData_Implementation(const FSLFSaveGameInfo& InSavedData);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SG_SoulslikeFramework")
	FSLFSaveGameInfo GetSavedData();
	virtual FSLFSaveGameInfo GetSavedData_Implementation();
};
