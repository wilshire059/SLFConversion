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

#include "W_InventoryAction.generated.h"

// Forward declarations for widget types
class UW_InventorySlot;
class UW_Inventory_ActionButton;

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

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	bool AreAllButtonsDisabled();
	virtual bool AreAllButtonsDisabled_Implementation();


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
	void EventUpdateBackInputIcons(int32 BackInputTexture);
	virtual void EventUpdateBackInputIcons_Implementation(int32 BackInputTexture);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryAction")
	void EventUpdateOkInputIcons(int32 OkInputTexture);
	virtual void EventUpdateOkInputIcons_Implementation(int32 OkInputTexture);

protected:
	// Cache references
	void CacheWidgetReferences();
};
