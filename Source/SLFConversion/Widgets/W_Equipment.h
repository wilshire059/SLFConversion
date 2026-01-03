// W_Equipment.h
// C++ Widget class for W_Equipment
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Equipment.json
// Parent: UW_Navigable_InputReader
// Variables: 10 | Functions: 5 | Dispatchers: 1

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


#include "W_Equipment.generated.h"

// Forward declarations for widget types
class UW_EquipmentSlot;
class UW_InventorySlot;

// Forward declarations for Blueprint types
class UAC_EquipmentManager;
class UPDA_Item;
class UAC_InventoryManager;

// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Equipment_OnEquipmentClosed);

UCLASS()
class SLFCONVERSION_API UW_Equipment : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_Equipment(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (10)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_InventoryManager* InventoryComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_EquipmentSlot* SelectedSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_EquipmentSlot*> EquipmentSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<FVector2D> EquipmentSlotsPositions;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_InventorySlot*> EquipmentInventorySlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_EquipmentSlot* ActiveEquipmentSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_EquipmentManager* EquipmentComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_InventorySlot* ActiveItemSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 EquipmentSlotNavigationIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 ItemNavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Equipment_OnEquipmentClosed OnEquipmentClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (5)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment")
	UW_EquipmentSlot* GetEquipmentSlotByTag(const FGameplayTag& TargetSlotTag);
	virtual UW_EquipmentSlot* GetEquipmentSlotByTag_Implementation(const FGameplayTag& TargetSlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment")
	void SetupInformationPanel(const FSLFItemInfo& ItemInfo, bool CompareStats);
	virtual void SetupInformationPanel_Implementation(const FSLFItemInfo& ItemInfo, bool CompareStats);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment")
	void EquipItemAtSlot(UW_InventorySlot* InSlot);
	virtual void EquipItemAtSlot_Implementation(UW_InventorySlot* InSlot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment")
	void UnequipItemAtSlot(const FGameplayTag& SlotTag);
	virtual void UnequipItemAtSlot_Implementation(const FGameplayTag& SlotTag);


	// Navigation Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Navigation")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Navigation")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Navigation")
	void EventNavigateLeft();
	virtual void EventNavigateLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Navigation")
	void EventNavigateRight();
	virtual void EventNavigateRight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Navigation")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Navigation")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Navigation")
	void EventNavigateDetailedView();
	virtual void EventNavigateDetailedView_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Navigation")
	void EventNavigateUnequip();
	virtual void EventNavigateUnequip_Implementation();

	// Slot Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventOnEquipmentSlotPressed(UW_EquipmentSlot* InSlot);
	virtual void EventOnEquipmentSlotPressed_Implementation(UW_EquipmentSlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventOnEquipmentSlotSelected(bool bSelected, UW_EquipmentSlot* InSlot);
	virtual void EventOnEquipmentSlotSelected_Implementation(bool bSelected, UW_EquipmentSlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventOnEquipmentPressed(UW_InventorySlot* InSlot);
	virtual void EventOnEquipmentPressed_Implementation(UW_InventorySlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventOnEquipmentSelected(bool bSelected, UW_InventorySlot* InSlot);
	virtual void EventOnEquipmentSelected_Implementation(bool bSelected, UW_InventorySlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventOnItemUnequippedFromSlot(UPDA_Item* InItem, const FGameplayTag& TargetSlot);
	virtual void EventOnItemUnequippedFromSlot_Implementation(UPDA_Item* InItem, const FGameplayTag& TargetSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventToggleSlotName(UW_EquipmentSlot* InEquipmentSlot, bool bShow);
	virtual void EventToggleSlotName_Implementation(UW_EquipmentSlot* InEquipmentSlot, bool bShow);

	// UI Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|UI")
	void EventOnVisibilityChanged(ESlateVisibility InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|UI")
	void EventToggleItemInfo(bool bVisible);
	virtual void EventToggleItemInfo_Implementation(bool bVisible);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|UI")
	void EventSetupItemInfoPanel(const FSLFItemInfo& ItemInfo, bool bCompareStats);
	virtual void EventSetupItemInfoPanel_Implementation(const FSLFItemInfo& ItemInfo, bool bCompareStats);

	// Error Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Error")
	void EventShowError(const FText& InMessage);
	virtual void EventShowError_Implementation(const FText& InMessage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Error")
	void EventDismissError();
	virtual void EventDismissError_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Error")
	void EventOnWeaponStatCheckFailed();
	virtual void EventOnWeaponStatCheckFailed_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
