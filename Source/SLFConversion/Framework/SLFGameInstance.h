// SLFGameInstance.h
// C++ base for GI_SoulslikeFramework - Game Instance
//
// IMPLEMENTATION SUMMARY - GI_SoulslikeFramework
// Variables:         7/7 (settings, save slots, active slot)
// Functions:         10/10 (BPI_GameInstance interface)
// Event Dispatchers: 1/1 (OnSelectedClassChanged)
//
// 20-PASS VALIDATION: 2026-01-06 - Fixed missing interface implementation

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "Interfaces/BPI_GameInstance.h"
#include "SLFGameInstance.generated.h"

// Forward declarations
class USG_SaveSlots;
class UPrimaryDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedClassChanged);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFGameInstance : public UGameInstance, public IBPI_GameInstance
{
	GENERATED_BODY()

public:
	USLFGameInstance();

	virtual void Init() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES [7/7]
	// ═══════════════════════════════════════════════════════════════════════

	/** Custom game settings data asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* CustomGameSettings;

	/** Selected base character class for new game */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UPrimaryDataAsset* SelectedBaseClass;

	/** Save slots object - stores all slot names */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	USG_SaveSlots* SGO_Slots;

	/** Name used to save/load the slots save game */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FString SlotsSaveName = TEXT("SaveSlots");

	/** Currently active save slot name */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FString ActiveSlot;

	/** Whether this is the first time on the demo level
	 * Note: DisplayName with ? suffix matches original Blueprint variable name */
	UPROPERTY(BlueprintReadWrite, Category = "Demo Level", meta = (DisplayName = "First Time on Demo Level?"))
	bool FirstTimeOnDemoLevel = true;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS [1/1]
	// ═══════════════════════════════════════════════════════════════════════

	/** Broadcast when selected character class changes */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSelectedClassChanged OnSelectedClassChanged;

	// ═══════════════════════════════════════════════════════════════════════
	// BPI_GameInstance INTERFACE FUNCTIONS [10/10]
	// ═══════════════════════════════════════════════════════════════════════

	/** Get all save slot names from SGO_Slots */
	virtual void GetAllSaveSlots_Implementation(TArray<FString>& SaveSlots) override;

	/** Set the last saved slot name as the active slot */
	virtual void SetLastSlotNameToActive_Implementation() override;

	/** Check if the slots save game file exists */
	virtual void DoesAnySaveExist_Implementation(bool& ReturnValue) override;

	/** Add a new slot and save the slots file */
	virtual void AddAndSaveSlots_Implementation(const FString& NewSlotName) override;

	/** Get the currently active slot name */
	virtual void GetActiveSlotName_Implementation(FString& ActiveSlotName) override;

	/** Set the active slot name */
	virtual void SetActiveSlot_Implementation(const FString& ActiveSlotName) override;

	/** Get the selected base character class */
	virtual void GetSelectedClass_Implementation(UPrimaryDataAsset*& SelectedBaseCharacterClass) override;

	/** Set the selected base character class and broadcast change */
	virtual void SetSelectedClass_Implementation(UPrimaryDataAsset* BaseCharacterClass) override;

	/** Get this game instance (self reference) */
	virtual void GetSoulslikeGameInstance_Implementation(UGameInstance*& ReturnValue) override;

	/** Get the custom game settings asset */
	virtual void GetCustomGameSettings_Implementation(UPrimaryDataAsset*& CustomSettingsAsset) override;
};
