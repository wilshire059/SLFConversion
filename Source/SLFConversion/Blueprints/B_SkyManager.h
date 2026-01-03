// B_SkyManager.h
// C++ class for Blueprint B_SkyManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_SkyManager.json
// Parent: Actor -> AActor
// Variables: 6 | Functions: 1 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
#include "B_SkyManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_SkyManager : public AActor
{
	GENERATED_BODY()

public:
	AB_SkyManager();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (6)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Time")
	UPrimaryDataAsset* TimeInfoAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Time")
	double Time;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Time")
	double TimeDilation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	UCurveFloat* FogDensityTrack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FSLFDayNightInfo TimeEntryInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	ESLFLightningMode CurrentLightningMode;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
