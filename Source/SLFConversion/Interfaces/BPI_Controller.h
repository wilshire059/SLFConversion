// BPI_Controller.h
// C++ Interface for BPI_Controller
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_Controller.json
// Functions: 20

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "InputMappingContext.h"
#include "SLFEnums.h"
#include "BPI_Controller.generated.h"

// Forward declarations
class UActorComponent;
class UUserWidget;
class UMaterialInterface;

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_Controller : public UInterface
{
	GENERATED_BODY()
};

/**
 * Player Controller Interface
 * Provides player controller functionality for save/load, UI, input, and getters
 */
class SLFCONVERSION_API IBPI_Controller
{
	GENERATED_BODY()

public:
	// === Save Functions ===

	// Request add data to save
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Save")
	void RequestAddToSaveData(FGameplayTag SaveTag, const FInstancedStruct& Data);

	// Request update save data
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Save")
	void RequestUpdateSaveData(FGameplayTag SaveTag, TArray<FInstancedStruct>& Data);

	// Serialize data for saving
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Save")
	void SerializeDataForSaving(ESLFSaveBehavior Behavior, FGameplayTag SaveTag);

	// Serialize all data for saving
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Save")
	void SerializeAllDataForSaving(ESLFSaveBehavior Behavior);

	// === UI Functions ===

	// Set active widget for navigation
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|UI")
	void SetActiveWidgetForNavigation(FGameplayTag NavigableWidgetTag);

	// Send big screen message
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|UI")
	void SendBigScreenMessage(const FText& Text, UMaterialInterface* GradientMaterial, bool bBackdrop, double PlayRate);

	// Show saving visual
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|UI")
	void ShowSavingVisual(double Length);

	// Toggle radar update state
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|UI")
	void ToggleRadarUpdateState(bool bUpdateEnabled);

	// Get player HUD widget
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|UI")
	void GetPlayerHUD(UUserWidget*& HUD);

	// === Input Functions ===

	// Switch input context (returns contexts to enable/disable)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Input")
	void SwitchInputContext(TArray<UInputMappingContext*>& ContextsToEnable, TArray<UInputMappingContext*>& ContextsToDisable);

	// Toggle input enabled
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Input")
	void ToggleInput(bool bEnabled);

	// === Getter Functions ===

	// Get currency amount
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Getters")
	void GetCurrency(int32& Currency);

	// Get progress manager component
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Getters")
	void GetProgressManager(UActorComponent*& ProgressManager);

	// Get soulslike controller
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Getters")
	void GetSoulslikeController(APlayerController*& Controller);

	// Get pawn from controller
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Getters")
	void GetPawnFromController(APawn*& Pawn);

	// Get inventory component
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Getters")
	void GetInventoryComponent(UActorComponent*& Inventory);

	// === Loot Functions ===

	// Adjust currency by delta
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Loot")
	void AdjustCurrency(int32 Delta);

	// Loot item to inventory
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface|Loot")
	void LootItemToInventory(AActor* Item);

	// === Other Functions ===

	// Start respawn with fade delay
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface")
	void StartRespawn(double FadeDelay);

	// Blend view to target actor
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Controller Interface")
	void BlendViewTarget(AActor* TargetActor, double BlendTime, double BlendExp, bool bLockOutgoing);
};
