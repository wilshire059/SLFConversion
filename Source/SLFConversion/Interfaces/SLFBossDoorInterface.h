// SLFBossDoorInterface.h
// C++ interface for BPI_BossDoor
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_BossDoor.json
// Functions: 2

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
#include "SLFBossDoorInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFBossDoorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_BossDoor
 * Generated from JSON with 2 functions
 */
class SLFCONVERSION_API ISLFBossDoorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BossDoor")
	USceneComponent* GetDeathCurrencySpawnPoint();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BossDoor")
	void UnlockBossDoor();
};
