// B_DeathCurrency.h
// C++ class for Blueprint B_DeathCurrency
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DeathCurrency.json
// Parent: B_Interactable_C -> AB_Interactable
// Variables: 4 | Functions: 1 | Dispatchers: 0

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
#include "B_DeathCurrency.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_DeathCurrency : public AB_Interactable
{
	GENERATED_BODY()

public:
	AB_DeathCurrency();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int32 CurrencyAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector PlayerLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTransform CacheTransform;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<FInstancedStruct> CacheTest;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
