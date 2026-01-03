// W_NPC_Window_Vendor.h
// C++ Widget class for W_NPC_Window_Vendor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_NPC_Window_Vendor.json
// Parent: UW_Navigable_InputReader
// Variables: 9 | Functions: 5 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_NPC_Window_Vendor.generated.h"

// Forward declarations for widget types
class UW_Inventory_CategoryEntry;
class UW_VendorSlot;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_NPC_Window_Vendor_OnVendorWindowClosed);

UCLASS()
class SLFCONVERSION_API UW_NPC_Window_Vendor : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_NPC_Window_Vendor(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (9)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_VendorSlot*> VendorSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_VendorSlot*> OccupiedVendorSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_VendorSlot* SelectedSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_Inventory_CategoryEntry*> CategoryEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFItemCategory ActiveFilterCategory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* CurrentVendorAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FText CurrentNpcName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFVendorType VendorType;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_NPC_Window_Vendor_OnVendorWindowClosed OnVendorWindowClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (5)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	int32 GetPlayerCurrency();
	virtual int32 GetPlayerCurrency_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	UW_VendorSlot* GetEmptySlot();
	virtual UW_VendorSlot* GetEmptySlot_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void AddNewSlots();
	virtual void AddNewSlots_Implementation();


	// Event Handlers (15 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventCloseVendorAction();
	virtual void EventCloseVendorAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventInitializeVendor(const FText& NpcName, UPDA_Vendor* InVendorAsset, uint8 InVendorType);
	virtual void EventInitializeVendor_Implementation(const FText& NpcName, UPDA_Vendor* InVendorAsset, uint8 InVendorType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventNavigateLeft();
	virtual void EventNavigateLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventNavigateRight();
	virtual void EventNavigateRight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventOnActionMenuVisibilityChanged(uint8 InVisibility);
	virtual void EventOnActionMenuVisibilityChanged_Implementation(uint8 InVisibility);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventOnSellSlotCleared(UW_VendorSlot* InSlot, bool TriggerShift);
	virtual void EventOnSellSlotCleared_Implementation(UW_VendorSlot* InSlot, bool TriggerShift);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventOnSlotPressed(UW_VendorSlot* InSlot);
	virtual void EventOnSlotPressed_Implementation(UW_VendorSlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventOnSlotSelected(bool Selected, UW_VendorSlot* InSlot);
	virtual void EventOnSlotSelected_Implementation(bool Selected, UW_VendorSlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventOnVendorSlotAssigned(UW_VendorSlot* InSlot);
	virtual void EventOnVendorSlotAssigned_Implementation(UW_VendorSlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventSetupItemInfoPanel(UW_VendorSlot* ForSlot);
	virtual void EventSetupItemInfoPanel_Implementation(UW_VendorSlot* ForSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window_Vendor")
	void EventToggleItemInfo(bool Visible);
	virtual void EventToggleItemInfo_Implementation(bool Visible);

protected:
	// Cache references
	void CacheWidgetReferences();
};
