// SLFInteractableInterface.h
// C++ interface for BPI_Interactable
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_Interactable.json
// Functions: 4

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
#include "SLFInteractableInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_Interactable
 * Generated from JSON with 4 functions
 */
class SLFCONVERSION_API ISLFInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnTraced(AActor* TracedBy);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnInteract(AActor* InteractingActor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnSpawnedFromSave(const FGuid& Id, const FInstancedStruct& CustomData);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	FSLFItemInfo TryGetItemInfo();
};
