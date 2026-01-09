// W_Inventory.h
// C++ Widget class for W_Inventory
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Inventory.json
// Parent: UW_Navigable_InputReader
// Variables: 10 | Functions: 8 | Dispatchers: 2

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


#include "W_Inventory.generated.h"

// Forward declarations for widget types
class UW_InventorySlot;
class UW_Inventory_CategoryEntry;
class UW_InventoryAction;

// Forward declarations for UMG types
class UScrollBox;
class UImage;
class UUniformGridPanel;
class UWidgetSwitcher;

// Forward declarations for Blueprint types
class UInventoryManagerComponent;

// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_Inventory_OnEquipmentRemoved, FGameplayTag, OnSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Inventory_OnInventoryClosed);

UCLASS()
class SLFCONVERSION_API UW_Inventory : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_Inventory(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Input handling (replaces Blueprint EventGraph input bindings)
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ═══════════════════════════════════════════════════════════════════════
	// BIND WIDGETS (Grid panels for slot containers)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UUniformGridPanel* UniformInventoryGrid;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UUniformGridPanel* UniformStorageGrid;

	// ItemInfoBoxSwitcher - Controls which panel is shown on the right side
	// Index 0 = CharacterStatsOverlay, Index 1 = ItemInfo panel
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UWidgetSwitcher* ItemInfoBoxSwitcher;

	// Action menu widgets (from WidgetTree)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_InventoryAction* W_InventoryAction;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_InventoryAction* W_StorageAction;

	// ScrollBox widgets for positioning action menus
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UScrollBox* InventoryScrollBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UScrollBox* StorageScrollBox;

	// ScrollBox for category entries (used for scrolling when navigating categories)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UScrollBox* CategoriesScroll;

	// Input Icon Images (for input prompt display)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets|InputIcons")
	UImage* CategoryLeftInputIcon;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets|InputIcons")
	UImage* CategoryRightInputIcon;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets|InputIcons")
	UImage* OkInputIcon;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets|InputIcons")
	UImage* BackInputIcon;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets|InputIcons")
	UImage* ScrollLeftInputIcon;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets|InputIcons")
	UImage* ScrollRightInputIcon;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets|InputIcons")
	UImage* DetailsInputIcon;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (10)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInventoryManagerComponent* InventoryComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_InventorySlot*> InventorySlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_InventorySlot*> OccupiedInventorySlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_InventorySlot*> StorageSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_InventorySlot* SelectedSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_Inventory_CategoryEntry*> CategoryEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFItemCategory ActiveFilterCategory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool StorageMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 CategoryNavigationIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 ItemNavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Inventory_OnEquipmentRemoved OnEquipmentRemoved;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Inventory_OnInventoryClosed OnInventoryClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (8)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory")
	void AddNewSlots(ESLFInventorySlotType SlotType);
	virtual void AddNewSlots_Implementation(ESLFInventorySlotType SlotType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory")
	FVector2D GetTranslationForActionMenu(UScrollBox* TargetScrollbox);
	virtual FVector2D GetTranslationForActionMenu_Implementation(UScrollBox* TargetScrollbox);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory")
	void SetStorageMode(bool InStorageMode);
	virtual void SetStorageMode_Implementation(bool InStorageMode);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory")
	void ReinitOccupiedInventorySlots();
	virtual void ReinitOccupiedInventorySlots_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory")
	void SetCategorization(ESLFItemCategory ItemCategory);
	virtual void SetCategorization_Implementation(ESLFItemCategory ItemCategory);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory")
	void ResetCategorization();
	virtual void ResetCategorization_Implementation();


	// Navigation Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Navigation")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Navigation")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Navigation")
	void EventNavigateLeft();
	virtual void EventNavigateLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Navigation")
	void EventNavigateRight();
	virtual void EventNavigateRight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Navigation")
	void EventNavigateCategoryLeft();
	virtual void EventNavigateCategoryLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Navigation")
	void EventNavigateCategoryRight();
	virtual void EventNavigateCategoryRight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Navigation")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Navigation")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Navigation")
	void EventNavigateDetailedView();
	virtual void EventNavigateDetailedView_Implementation();

	// Slot Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Slots")
	void EventOnSlotPressed(UW_InventorySlot* InSlot);
	virtual void EventOnSlotPressed_Implementation(UW_InventorySlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Slots")
	void EventOnSlotSelected(bool bSelected, UW_InventorySlot* InSlot);
	virtual void EventOnSlotSelected_Implementation(bool bSelected, UW_InventorySlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Slots")
	void EventOnSlotCleared(UW_InventorySlot* InSlot, bool bTriggerShift);
	virtual void EventOnSlotCleared_Implementation(UW_InventorySlot* InSlot, bool bTriggerShift);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Slots")
	void EventOnInventorySlotAssigned(UW_InventorySlot* InSlot);
	virtual void EventOnInventorySlotAssigned_Implementation(UW_InventorySlot* InSlot);

	// Category Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Category")
	void EventOnCategorySelected(UW_Inventory_CategoryEntry* InCategoryEntry, ESLFItemCategory SelectedCategory);
	virtual void EventOnCategorySelected_Implementation(UW_Inventory_CategoryEntry* InCategoryEntry, ESLFItemCategory SelectedCategory);

	// UI Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|UI")
	void EventOnVisibilityChanged(ESlateVisibility InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|UI")
	void EventOnActionMenuVisibilityChanged(ESlateVisibility InVisibility);
	virtual void EventOnActionMenuVisibilityChanged_Implementation(ESlateVisibility InVisibility);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|UI")
	void EventToggleItemInfo(bool bVisible);
	virtual void EventToggleItemInfo_Implementation(bool bVisible);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|UI")
	void EventSetupItemInfoPanel(UW_InventorySlot* ForSlot);
	virtual void EventSetupItemInfoPanel_Implementation(UW_InventorySlot* ForSlot);

	// Error Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Error")
	void EventOnErrorReceived(const FText& ErrorMessage);
	virtual void EventOnErrorReceived_Implementation(const FText& ErrorMessage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Error")
	void EventDismissError();
	virtual void EventDismissError_Implementation();

	// Input Icon Update
	void UpdateInputIcons();

	// Override from parent to update icons when input device changes
	virtual void EventOnHardwareDeviceDetected_Implementation(FPlatformUserId UserId, FInputDeviceId DeviceId) override;

protected:
	// Cache references
	void CacheWidgetReferences();

	// Create inventory slots dynamically (per Blueprint Construct logic)
	void CreateInventorySlots();

	// Create storage slots dynamically
	void CreateStorageSlots();

	// Create category entry widgets dynamically
	void CreateCategoryEntries();

	// Populate slots with items from InventoryComponent
	void PopulateSlotsWithItems();

	// Refresh display with filtered items (used when category changes)
	void RefreshFilteredDisplay();

	// Default slot widget class for dynamic creation
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UW_InventorySlot> InventorySlotClass;

	// Category entry widget class for dynamic creation
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UW_Inventory_CategoryEntry> CategoryEntryClass;

	// Handler for slot OnSelected event
	UFUNCTION()
	void HandleSlotSelected(bool bSelected, UW_InventorySlot* InSlot);

	// Handler for slot OnPressed event
	UFUNCTION()
	void HandleSlotPressed(UW_InventorySlot* InSlot);

	// Handler for slot OnCleared event
	UFUNCTION()
	void HandleSlotCleared(UW_InventorySlot* InSlot, bool bTriggerShift);

	// Handler for slot OnSlotAssigned event
	UFUNCTION()
	void HandleSlotAssigned(UW_InventorySlot* InSlot);
};
