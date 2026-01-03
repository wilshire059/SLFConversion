// BPI_GameInstance.h
// C++ Interface for BPI_GameInstance
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_GameInstance.json
// Functions: 10

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_GameInstance.generated.h"

// Forward declarations
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_GameInstance : public UInterface
{
	GENERATED_BODY()
};

/**
 * Game Instance Interface
 * Provides save/load and game settings functionality
 */
class SLFCONVERSION_API IBPI_GameInstance
{
	GENERATED_BODY()

public:
	// Get all save slot names
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance|SaveLoad")
	void GetAllSaveSlots(TArray<FString>& SaveSlots);

	// Set the last slot name as active
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance|SaveLoad")
	void SetLastSlotNameToActive();

	// Check if any save exists
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance|SaveLoad")
	void DoesAnySaveExist(bool& ReturnValue);

	// Add and save new slot
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance|SaveLoad")
	void AddAndSaveSlots(const FString& NewSlotName);

	// Get active slot name
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance|SaveLoad")
	void GetActiveSlotName(FString& ActiveSlotName);

	// Set active slot
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance|SaveLoad")
	void SetActiveSlot(const FString& ActiveSlotName);

	// Get selected character class
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance")
	void GetSelectedClass(UPrimaryDataAsset*& SelectedBaseCharacterClass);

	// Set selected character class
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance")
	void SetSelectedClass(UPrimaryDataAsset* BaseCharacterClass);

	// Get the soulslike game instance
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance")
	void GetSoulslikeGameInstance(UGameInstance*& ReturnValue);

	// Get custom game settings asset
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Soulslike Game Instance|Custom Game Settings")
	void GetCustomGameSettings(UPrimaryDataAsset*& CustomSettingsAsset);
};
