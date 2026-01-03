// B_SequenceActor.h
// C++ class for Blueprint B_SequenceActor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_SequenceActor.json
// Parent: Actor -> AActor
// Variables: 4 | Functions: 1 | Dispatchers: 1

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
#include "B_SequenceActor.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_SequenceActor_OnSequenceFinished);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_SequenceActor : public AActor
{
	GENERATED_BODY()

public:
	AB_SequenceActor();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	ULevelSequence* SequenceToPlay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FMovieSceneSequencePlaybackSettings Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ULevelSequencePlayer* ActiveSequencePlayer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool CanBeSkipped;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_SequenceActor_OnSequenceFinished OnSequenceFinished;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
