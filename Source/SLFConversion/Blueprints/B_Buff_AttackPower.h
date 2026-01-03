// B_Buff_AttackPower.h
// C++ class for Blueprint B_Buff_AttackPower
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Buff_AttackPower.json
// Parent: B_Buff_C -> UB_Buff
// Variables: 0 | Functions: 0 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Buff.h"
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
#include "B_Buff_AttackPower.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UB_Buff_AttackPower : public UB_Buff
{
	GENERATED_BODY()

public:
	UB_Buff_AttackPower();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════

};
