// W_LoadGame.h
// C++ Widget class for W_LoadGame
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/WidgetBlueprint/W_LoadGame.json
// Parent: UW_Navigable_InputReader
// Variables: 2 | Functions: 2 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "Components/ScrollBox.h"
#include "W_LoadGame.generated.h"

// Forward declarations
class UW_LoadGame_Entry;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_LoadGame_OnLoadGameClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_LoadGame_OnSaveSlotConfirmed);

UCLASS()
class SLFCONVERSION_API UW_LoadGame : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_LoadGame(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// BIND WIDGETS (from UMG hierarchy)
	// ═══════════════════════════════════════════════════════════════════════

	/** Scroll box container for save slot entries */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* SaveSlotsScrollBox;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	/** Array of save slot entry widgets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_LoadGame_Entry*> LoadSlotEntries;

	/** Current navigation index for keyboard/gamepad navigation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_LoadGame_OnLoadGameClosed OnLoadGameClosed;

	/** Fired when a save slot is selected and confirmed (active slot already set) */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_LoadGame_OnSaveSlotConfirmed OnSaveSlotConfirmed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	/** Initialize and populate save slot list from game instance */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void InitializeSaveSlots();
	virtual void InitializeSaveSlots_Implementation();

	/** Handle cancel navigation - close the load game menu */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	/** Handle down navigation - move to next save slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	/** Handle OK/confirm navigation - load selected save slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	/** Handle up navigation - move to previous save slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	/** Handle save slot selection from entry widget */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventOnSaveSlotSelected(UW_LoadGame_Entry* Card);
	virtual void EventOnSaveSlotSelected_Implementation(UW_LoadGame_Entry* Card);

	/** Handle visibility changes */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

protected:
	/** Update the visual selection state of all entries */
	void UpdateSelectionVisuals();

	/** Load entry widget class for creating entries */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UW_LoadGame_Entry> LoadGameEntryClass;
};
