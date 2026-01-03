// SLFExecutableInterface.h
// C++ interface for BPI_Executable
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_Executable.json
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
#include "SLFExecutableInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFExecutableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_Executable
 * Generated from JSON with 3 functions
 */
class SLFCONVERSION_API ISLFExecutableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Executable")
	void OnBackstabbed(const FGameplayTag& ExecutionTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Executable")
	void OnExecuted(const FGameplayTag& ExecutionTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Executable")
	void OnExecutionStarted();
};
