// B_Action_DrinkFlask_HP.h
// C++ class for Blueprint B_Action_DrinkFlask_HP
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Action_DrinkFlask_HP.json
// Parent: B_Action_C -> UB_Action
// Variables: 0 | Functions: 1 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Action.h"
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
#include "B_Action_DrinkFlask_HP.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UB_Action_DrinkFlask_HP : public UB_Action
{
	GENERATED_BODY()

public:
	UB_Action_DrinkFlask_HP();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	// Override ExecuteAction to implement flask drinking logic
	virtual void ExecuteAction_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action_DrinkFlask_HP")
	double GetChangeAmountFromPercent(const FGameplayTag& Stat, double PercentChange);
	virtual double GetChangeAmountFromPercent_Implementation(const FGameplayTag& Stat, double PercentChange);
};
