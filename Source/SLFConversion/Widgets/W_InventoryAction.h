// W_InventoryAction.h
// C++ Widget class for W_InventoryAction
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_InventoryAction.json
// Parent: UUserWidget
// Variables: 3 | Functions: 1 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"
#include "Components/Button.h"
#include "Engine/Texture2D.h"

#include "W_InventoryAction.generated.h"

// Forward declarations for widget types
class UW_InventorySlot;
class UW_Inventory_ActionButton;
class UW_InventoryActionAmount;
class UWidgetSwitcher;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_InventoryAction : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_InventoryAction(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_InventorySlot* SelectedSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_Inventory_ActionButton*> ButtonEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// True when the action menu is for a storage slot (set by EventSetupForStorage)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool bIsStorageSlot;

	// True when the parent inventory widget is in storage mode
	// Set by W_Inventory before opening action menu
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool bInStorageMode;

	// The index of the selected slot in the inventory/storage array
	// Set by W_Inventory when showing the action menu
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 SlotIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	bool AreAllButtonsDisabled();
	virtual bool AreAllButtonsDisabled_Implementation();

	// Getter for amount selection mode (used by W_Inventory to block navigation)
	FORCEINLINE bool IsInAmountSelectionMode() const { return bInAmountSelectionMode; }

	// Reset the amount selection mode (used when hiding/switching action widgets)
	FORCEINLINE void ResetAmountSelectionMode() { bInAmountSelectionMode = false; }

	// Event Handlers (10 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventActionButtonPressed();
	virtual void EventActionButtonPressed_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventNavigateActionConfirmHorizontal();
	virtual void EventNavigateActionConfirmHorizontal_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventNavigateActionDown();
	virtual void EventNavigateActionDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventNavigateActionUp();
	virtual void EventNavigateActionUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventOnButtonSelected(UW_Inventory_ActionButton* Button);
	virtual void EventOnButtonSelected_Implementation(UW_Inventory_ActionButton* Button);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventSetupForInventory(UW_InventorySlot* InSlot);
	virtual void EventSetupForInventory_Implementation(UW_InventorySlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventSetupForStorage(UW_InventorySlot* InSelectedSlot);
	virtual void EventSetupForStorage_Implementation(UW_InventorySlot* InSelectedSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventUpdateBackInputIcons(const TSoftObjectPtr<UTexture2D>& BackInputTexture);
	virtual void EventUpdateBackInputIcons_Implementation(const TSoftObjectPtr<UTexture2D>& BackInputTexture);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventUpdateOkInputIcons(const TSoftObjectPtr<UTexture2D>& OkInputTexture);
	virtual void EventUpdateOkInputIcons_Implementation(const TSoftObjectPtr<UTexture2D>& OkInputTexture);

protected:
	// Cache references
	void CacheWidgetReferences();

	// Handler for button's OnActionButtonPressed delegate
	UFUNCTION()
	void OnActionButtonPressedHandler();

	// Handler for button's OnActionButtonSelected delegate
	UFUNCTION()
	void OnActionButtonSelectedHandler(UW_Inventory_ActionButton* Button);

	// Handler for AmountActionPanel's OnRequestConfirmed delegate
	UFUNCTION()
	void OnAmountConfirmedHandler(int32 Amount, ESLFInventoryAmountedActionType Operation);

	// Handler for AmountActionPanel's OnRequestCanceled delegate
	UFUNCTION()
	void OnAmountCanceledHandler(ESLFInventoryAmountedActionType Operation);

	// Show the amount selector panel for the specified operation
	void ShowAmountSelector(ESLFInventoryAmountedActionType Operation);

	// Cached widget references
	UPROPERTY()
	UWidgetSwitcher* CachedSwitcher;

	UPROPERTY()
	UW_InventoryActionAmount* CachedAmountPanel;

	// Flag to track if we're currently in amount selection mode
	bool bInAmountSelectionMode;
};
