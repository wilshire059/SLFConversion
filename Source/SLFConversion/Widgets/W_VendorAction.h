// W_VendorAction.h
// C++ Widget class for W_VendorAction
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_VendorAction.json
// Parent: UUserWidget
// Variables: 7 | Functions: 3 | Dispatchers: 3

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


#include "W_VendorAction.generated.h"

// Forward declarations for widget types
class UW_VendorSlot;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_VendorAction_OnVendorActionClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_VendorAction_OnVendorItemPurchased, UW_VendorSlot*, TargetSlot, int32, PurchasedAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_VendorAction_OnVendorItemSold, UPrimaryDataAsset*, Item, int32, Amount);

UCLASS()
class SLFCONVERSION_API UW_VendorAction : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_VendorAction(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_VendorSlot* SelectedSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 AssignedItemAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 MaxPossibleAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 DesiredAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* AssignedItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 AssignedItemPrice;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFVendorType ActionType;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_VendorAction_OnVendorActionClosed OnVendorActionClosed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_VendorAction_OnVendorItemPurchased OnVendorItemPurchased;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_VendorAction_OnVendorItemSold OnVendorItemSold;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_VendorAction")
	void GetMaxPossibleAmount(int32& OutMaxPossible, int32& OutMaxPossible1);
	virtual void GetMaxPossibleAmount_Implementation(int32& OutMaxPossible, int32& OutMaxPossible1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_VendorAction")
	void BuyItem();
	virtual void BuyItem_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_VendorAction")
	void SellItem();
	virtual void SellItem_Implementation();


	// Event Handlers (5 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_VendorAction")
	void EventNavigateConfirmButtonsHorizontal();
	virtual void EventNavigateConfirmButtonsHorizontal_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_VendorAction")
	void EventNavigateVendorActionDown();
	virtual void EventNavigateVendorActionDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_VendorAction")
	void EventNavigateVendorActionUp();
	virtual void EventNavigateVendorActionUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_VendorAction")
	void EventSetupVendorAction(UW_VendorSlot* InSelectedSlot, uint8 InType);
	virtual void EventSetupVendorAction_Implementation(UW_VendorSlot* InSelectedSlot, uint8 InType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_VendorAction")
	void EventVendorActionBtnPressed();
	virtual void EventVendorActionBtnPressed_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
