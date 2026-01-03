// SLFExecutionIndicatorInterface.h
// C++ interface for BPI_ExecutionIndicator
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_ExecutionIndicator.json
// Functions: 1

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/UMGSequencePlayMode.h"
#include "SLFExecutionIndicatorInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFExecutionIndicatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_ExecutionIndicator
 * Generated from JSON with 1 functions
 */
class SLFCONVERSION_API ISLFExecutionIndicatorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ExecutionIndicator")
	void ToggleExecutionIcon(bool Visible);
};
