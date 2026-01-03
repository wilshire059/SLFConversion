// SLFControllerInterface.h
// C++ interface for BPI_Controller
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_Controller.json
// Functions: 20

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
#include "SLFControllerInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_Controller
 * Generated from JSON with 20 functions
 */
class SLFCONVERSION_API ISLFControllerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	UW_HUD* GetPlayerHUD();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	UAC_InventoryManager* GetInventoryComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void LootItemToInventory(AB_PickupItem* Item);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	APawn* GetPawnFromController();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void ToggleInput(bool Enabled);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	APC_SoulslikeFramework* GetSoulslikeController();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void SerializeAllDataForSaving(ESLFSaveBehavior Behavior);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void ToggleRadarUpdateState(bool UpdateEnabled);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void BlendViewTarget(AActor* TargetActor, double BlendTime, double BlendExp, bool LockOutgoing);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	UAC_ProgressManager* GetProgressManager();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void SwitchInputContext(const TArray<UInputMappingContext*>& ContextsToEnable, const TArray<UInputMappingContext*>& ContextsToDisable);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void ShowSavingVisual(double Length);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void SerializeDataForSaving(ESLFSaveBehavior Behavior, const FGameplayTag& SaveTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void AdjustCurrency(int32 Delta);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	int32 GetCurrency();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void SendBigScreenMessage(const FText& Text, UMaterialInterface* GradientMaterial, bool Backdrop, double PlayRate);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void StartRespawn(double FadeDelay);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void RequestUpdateSaveData(const FGameplayTag& SaveTag, const TArray<FInstancedStruct>& Data);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void SetActiveWidgetForNavigation(const FGameplayTag& NavigableWidgetTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void RequestAddToSaveData(const FGameplayTag& SaveTag, const FInstancedStruct& Data);
};
