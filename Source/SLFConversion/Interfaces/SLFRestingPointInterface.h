// SLFRestingPointInterface.h
// C++ interface for BPI_RestingPoint
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_RestingPoint.json
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
#include "SLFRestingPointInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFRestingPointInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_RestingPoint
 * Generated from JSON with 1 functions
 */
class SLFCONVERSION_API ISLFRestingPointInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RestingPoint")
	void GetRestingPointSpawnPosition(bool& OutSuccess, FVector& OutLocation, FRotator& OutRotation);
};
