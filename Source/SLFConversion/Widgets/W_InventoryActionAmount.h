// W_InventoryActionAmount.h
// C++ Widget class for W_InventoryActionAmount
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_InventoryActionAmount.json
// Parent: UUserWidget
// Variables: 5 | Functions: 0 | Dispatchers: 2

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


#include "W_InventoryActionAmount.generated.h"

// Forward declarations for widget types
class UW_InventorySlot;
class UW_GenericButton;
class UButton;
class UTextBlock;
class UWidgetSwitcher;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_InventoryActionAmount_OnRequestConfirmed, int32, Amount, ESLFInventoryAmountedActionType, Operation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_InventoryActionAmount_OnRequestCanceled, ESLFInventoryAmountedActionType, OperationType);

UCLASS()
class SLFCONVERSION_API UW_InventoryActionAmount : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_InventoryActionAmount(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_InventorySlot* SelectedSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 CurrentAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 MaxAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* CurrentItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFInventoryAmountedActionType OperationType;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_InventoryActionAmount_OnRequestConfirmed OnRequestConfirmed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_InventoryActionAmount_OnRequestCanceled OnRequestCanceled;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (5 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryActionAmount")
	void EventAmountConfirmButtonPressed();
	virtual void EventAmountConfirmButtonPressed_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryActionAmount")
	void EventInitializeForSlot(uint8 InOperationType, UW_InventorySlot* InSlot);
	virtual void EventInitializeForSlot_Implementation(uint8 InOperationType, UW_InventorySlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryActionAmount")
	void EventNavigateAmountDown();
	virtual void EventNavigateAmountDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryActionAmount")
	void EventNavigateAmountUp();
	virtual void EventNavigateAmountUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventoryActionAmount")
	void EventNavigateConfirmButtonsHorizontal();
	virtual void EventNavigateConfirmButtonsHorizontal_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();

	// Bind button events
	void BindButtonEvents();

	// Update the amount display text
	void UpdateAmountDisplay();

	// Handler for BtnUp pressed
	UFUNCTION()
	void OnBtnUpPressed();

	// Handler for BtnDown pressed
	UFUNCTION()
	void OnBtnDownPressed();

	// Handler for YES button pressed
	UFUNCTION()
	void OnYesButtonPressed();

	// Handler for NO button pressed
	UFUNCTION()
	void OnNoButtonPressed();

	// Cached widget references
	UPROPERTY()
	UButton* CachedBtnUp;

	UPROPERTY()
	UButton* CachedBtnDown;

	UPROPERTY()
	UTextBlock* CachedAmountText;

	UPROPERTY()
	UTextBlock* CachedMaxAmountText;

	UPROPERTY()
	UW_GenericButton* CachedYesButton;

	UPROPERTY()
	UW_GenericButton* CachedNoButton;

	// Fallback: inner UButton from W_GenericButton if C++ cast fails
	UPROPERTY()
	UButton* CachedYesButtonInner;

	UPROPERTY()
	UButton* CachedNoButtonInner;

	// Parent W_GenericButton widgets (for accessing Blueprint functionality)
	UPROPERTY()
	UUserWidget* CachedYesWidgetParent;

	UPROPERTY()
	UUserWidget* CachedNoWidgetParent;

	// Navigation state for YES/NO buttons (0 = YES, 1 = NO)
	int32 ConfirmButtonIndex;
};
