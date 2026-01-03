# Script to add all event declarations to W_Inventory.h
path = r'C:/scripts/SLFConversion/Source/SLFConversion/Widgets/W_Inventory.h'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Add missing includes and forward declarations
if 'class UScrollBox;' not in content:
    content = content.replace(
        '// Forward declarations for Blueprint types',
        '// Forward declarations for UMG types\nclass UScrollBox;\n\n// Forward declarations for Blueprint types'
    )

# New events to add
new_events = '''
	// Additional Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory")
	void OnInputDeviceChanged(ECommonInputType InputType);
	virtual void OnInputDeviceChanged_Implementation(ECommonInputType InputType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory")
	FEventReply OnMouseWheel(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	virtual FEventReply OnMouseWheel_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

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
	void EventOnSlotPressed(UW_InventorySlot* Slot);
	virtual void EventOnSlotPressed_Implementation(UW_InventorySlot* Slot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Slots")
	void EventOnSlotSelected(bool bSelected, UW_InventorySlot* Slot);
	virtual void EventOnSlotSelected_Implementation(bool bSelected, UW_InventorySlot* Slot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Slots")
	void EventOnSlotCleared(UW_InventorySlot* Slot, bool bTriggerShift);
	virtual void EventOnSlotCleared_Implementation(UW_InventorySlot* Slot, bool bTriggerShift);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Slots")
	void EventOnInventorySlotAssigned(UW_InventorySlot* Slot);
	virtual void EventOnInventorySlotAssigned_Implementation(UW_InventorySlot* Slot);

	// Category Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory|Category")
	void EventOnCategorySelected(UW_Inventory_CategoryEntry* CategoryEntry, ESLFItemCategory SelectedCategory);
	virtual void EventOnCategorySelected_Implementation(UW_Inventory_CategoryEntry* CategoryEntry, ESLFItemCategory SelectedCategory);

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
'''

# Insert before 'protected:'
insertion_point = content.find('protected:')
if insertion_point != -1:
    content = content[:insertion_point] + new_events + '\n' + content[insertion_point:]

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print('Added event declarations to W_Inventory.h')
