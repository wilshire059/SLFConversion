// W_InventorySlot.h
// C++ Widget class for W_InventorySlot
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_InventorySlot.json
// Parent: UUserWidget
// Variables: 8 | Functions: 1 | Dispatchers: 4

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


#include "W_InventorySlot.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_InventorySlot_OnSelected, bool, Selected, UW_InventorySlot*, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_InventorySlot_OnPressed, UW_InventorySlot*, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_InventorySlot_OnSlotCleared, UW_InventorySlot*, Slot, bool, TriggerShift);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_InventorySlot_OnSlotAssigned, UW_InventorySlot*, Slot);

UCLASS()
class SLFCONVERSION_API UW_InventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_InventorySlot(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (8)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* AssignedItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool IsOccupied;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 Count;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool CraftingRelated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool StorageRelated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool EquipmentRelated;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool CraftingSlotEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FLinearColor SlotColor;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_InventorySlot_OnSelected OnSelected;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_InventorySlot_OnPressed OnPressed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_InventorySlot_OnSlotCleared OnSlotCleared;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_InventorySlot_OnSlotAssigned OnSlotAssigned;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventorySlot")
	void AdjustAmountAndIconOpacity(double Alpha);
	virtual void AdjustAmountAndIconOpacity_Implementation(double Alpha);


	// Event Handlers (8 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventorySlot")
	void EventChangeAmount(int32 NewCount);
	virtual void EventChangeAmount_Implementation(int32 NewCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventorySlot")
	void EventClearSlot(bool TriggerShift);
	virtual void EventClearSlot_Implementation(bool TriggerShift);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventorySlot")
	void EventOccupySlot(UPDA_Item* AssignedItemAsset, int32 InCount);
	virtual void EventOccupySlot_Implementation(UPDA_Item* AssignedItemAsset, int32 InCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventorySlot")
	void EventOnSelected(bool Selected);
	virtual void EventOnSelected_Implementation(bool Selected);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventorySlot")
	void EventSetHighlighted(bool Highlighted);
	virtual void EventSetHighlighted_Implementation(bool Highlighted);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventorySlot")
	void EventSlotPressed();
	virtual void EventSlotPressed_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventorySlot")
	void EventToggleEquippedVisual(bool Show);
	virtual void EventToggleEquippedVisual_Implementation(bool Show);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InventorySlot")
	void EventToggleSlot(bool IsEnabled);
	virtual void EventToggleSlot_Implementation(bool IsEnabled);

protected:
	// Cache references
	void CacheWidgetReferences();
};
