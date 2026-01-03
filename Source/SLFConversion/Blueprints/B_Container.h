// B_Container.h
// C++ class for Blueprint B_Container
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Container.json
// Parent: B_Interactable_C -> AB_Interactable
// Variables: 5 | Functions: 1 | Dispatchers: 0

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
#include "B_Container.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Container : public AB_Interactable
{
	GENERATED_BODY()

public:
	AB_Container();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Config")
	TSoftObjectPtr<UAnimMontage> OpenMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Config")
	TSoftObjectPtr<UNiagaraSystem> OpenVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Config")
	double MoveDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Config")
	double SpeedMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	double CachedIntensity;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
