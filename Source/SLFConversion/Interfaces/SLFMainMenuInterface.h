// SLFMainMenuInterface.h
// C++ interface for BPI_MainMenu
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_MainMenu.json
// Functions: 3

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
#include "SLFMainMenuInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFMainMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_MainMenu
 * Generated from JSON with 3 functions
 */
class SLFCONVERSION_API ISLFMainMenuInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MainMenu")
	void SetMenuButtonSelected(bool Selected);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MainMenu")
	void PlayButtonInitAnimation(double StartTime, int32 Loops, EUMGSequencePlayMode::Type Mode, double PlayRate);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MainMenu")
	void OnMenuButtonPressed();
};
