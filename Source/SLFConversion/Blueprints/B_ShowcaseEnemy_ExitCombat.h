// B_ShowcaseEnemy_ExitCombat.h
// C++ class for Blueprint B_ShowcaseEnemy_ExitCombat
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_ShowcaseEnemy_ExitCombat.json
// Parent: Actor -> AActor
// Variables: 3 | Functions: 1 | Dispatchers: 0

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
#include "B_ShowcaseEnemy_ExitCombat.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class AB_Soulslike_Enemy;

// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_ShowcaseEnemy_ExitCombat : public AActor
{
	GENERATED_BODY()

public:
	AB_ShowcaseEnemy_ExitCombat();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	AB_Soulslike_Enemy* MyEnemyActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTransform DefaultTransform;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTransform CacheTransform;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
