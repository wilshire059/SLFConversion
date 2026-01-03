// SLFGameInstanceInterface.h
// C++ interface for BPI_GameInstance
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_GameInstance.json
// Functions: 10

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
#include "SLFGameInstanceInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFGameInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_GameInstance
 * Generated from JSON with 10 functions
 */
class SLFCONVERSION_API ISLFGameInstanceInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	UPrimaryDataAsset* GetCustomGameSettings();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	UGI_SoulslikeFramework* GetSoulslikeGameInstance();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	void SetSelectedClass(UPrimaryDataAsset* BaseCharacterClass);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	UPrimaryDataAsset* GetSelectedClass();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	void SetActiveSlot(const FString& ActiveSlotName);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	FString GetActiveSlotName();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	void AddAndSaveSlots(const FString& NewSlotName);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	bool DoesAnySaveExist();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	void SetLastSlotNameToActive();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstance")
	TArray<FString> GetAllSaveSlots();
};
