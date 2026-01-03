// B_StatusEffectOneShot.h
// C++ class for Blueprint B_StatusEffectOneShot
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_StatusEffectOneShot.json
// Parent: Actor -> AActor
// Variables: 4 | Functions: 1 | Dispatchers: 0

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
#include "B_StatusEffectOneShot.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class AB_BaseCharacter;

// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_StatusEffectOneShot : public AActor
{
	GENERATED_BODY()

public:
	AB_StatusEffectOneShot();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* StatusEffectToApply;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 Rank;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double Amount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	AB_BaseCharacter* CurrentTarget;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
