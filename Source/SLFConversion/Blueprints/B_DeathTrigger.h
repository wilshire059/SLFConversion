// B_DeathTrigger.h
// C++ class for Blueprint B_DeathTrigger
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DeathTrigger.json
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
#include "B_DeathTrigger.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_DeathTrigger : public AActor
{
	GENERATED_BODY()

public:
	AB_DeathTrigger();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool ShowBillboard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double BillboardProportionalScaleMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FName> RelevantActorTags;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
