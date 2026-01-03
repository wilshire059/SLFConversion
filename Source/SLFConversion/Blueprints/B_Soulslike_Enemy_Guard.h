// B_Soulslike_Enemy_Guard.h
// C++ class for Blueprint B_Soulslike_Enemy_Guard
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Soulslike_Enemy_Guard.json
// Parent: B_Soulslike_Enemy_C -> AB_Soulslike_Enemy
// Variables: 0 | Functions: 1 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Soulslike_Enemy.h"
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
#include "B_Soulslike_Enemy_Guard.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Soulslike_Enemy_Guard : public AB_Soulslike_Enemy
{
	GENERATED_BODY()

public:
	AB_Soulslike_Enemy_Guard();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
