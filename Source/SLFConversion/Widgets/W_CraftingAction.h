// W_CraftingAction.h
// C++ Widget class for W_CraftingAction
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_CraftingAction.json
// Parent: UUserWidget
// Variables: 4 | Functions: 3 | Dispatchers: 1

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


#include "W_CraftingAction.generated.h"

// Forward declarations for widget types
class UW_InventorySlot;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_CraftingAction_OnCraftingActionClosed);

UCLASS()
class SLFCONVERSION_API UW_CraftingAction : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_CraftingAction(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_InventorySlot* SelectedSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 CurrentAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 MaxPossibleAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* AssignedItem;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_CraftingAction_OnCraftingActionClosed OnCraftingActionClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingAction")
	int32 GetMaxPossibleAmount();
	virtual int32 GetMaxPossibleAmount_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingAction")
	void SetupRequiredItems();
	virtual void SetupRequiredItems_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingAction")
	void CraftItem();
	virtual void CraftItem_Implementation();


	// Event Handlers (5 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingAction")
	void EventCraftButtonPressed();
	virtual void EventCraftButtonPressed_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingAction")
	void EventNavigateConfirmButtonsHorizontal();
	virtual void EventNavigateConfirmButtonsHorizontal_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingAction")
	void EventNavigateCraftingActionDown();
	virtual void EventNavigateCraftingActionDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingAction")
	void EventNavigateCraftingActionUp();
	virtual void EventNavigateCraftingActionUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingAction")
	void EventSetupCraftingAction(UW_InventorySlot* InSelectedSlot);
	virtual void EventSetupCraftingAction_Implementation(UW_InventorySlot* InSelectedSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingAction")
	void EventCancelButtonPressed();
	virtual void EventCancelButtonPressed_Implementation();

protected:
	// Cache references and bind button events
	void CacheWidgetReferences();
	void BindButtonEvents();

	// Button click handlers
	UFUNCTION()
	void HandleOKButtonPressed();
	UFUNCTION()
	void HandleCancelButtonPressed();

	// Cached widget references
	UPROPERTY(Transient)
	class UW_GenericButton* CachedOKButton;
	UPROPERTY(Transient)
	class UW_GenericButton* CachedCancelButton;
	UPROPERTY(Transient)
	class UTextBlock* CachedItemNameText;
	UPROPERTY(Transient)
	class UImage* CachedItemIcon;
};
