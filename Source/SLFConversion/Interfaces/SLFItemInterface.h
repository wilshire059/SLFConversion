// SLFItemInterface.h
// C++ interface for BPI_Item
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_Item.json
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
#include "SLFItemInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_Item
 * Generated from JSON with 3 functions
 */
class SLFCONVERSION_API ISLFItemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void OnUse();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void OnWeaponEquip(bool RightHand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void OnWeaponUnequip();
};
