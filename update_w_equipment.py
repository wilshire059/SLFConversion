# Script to add all event declarations to W_Equipment.h
path = r'C:/scripts/SLFConversion/Source/SLFConversion/Widgets/W_Equipment.h'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Add forward declaration for PDA_Item
if 'class UPDA_Item;' not in content:
    content = content.replace(
        'class UAC_EquipmentManager;',
        'class UAC_EquipmentManager;\nclass UPDA_Item;'
    )

# New events to add
new_events = '''
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
	void EventOnEquipmentSlotPressed(UW_EquipmentSlot* Slot);
	virtual void EventOnEquipmentSlotPressed_Implementation(UW_EquipmentSlot* Slot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventOnEquipmentSlotSelected(bool bSelected, UW_EquipmentSlot* Slot);
	virtual void EventOnEquipmentSlotSelected_Implementation(bool bSelected, UW_EquipmentSlot* Slot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventOnEquipmentPressed(UW_InventorySlot* Slot);
	virtual void EventOnEquipmentPressed_Implementation(UW_InventorySlot* Slot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventOnEquipmentSelected(bool bSelected, UW_InventorySlot* Slot);
	virtual void EventOnEquipmentSelected_Implementation(bool bSelected, UW_InventorySlot* Slot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventOnItemUnequippedFromSlot(UPDA_Item* Item, const FGameplayTag& TargetSlot);
	virtual void EventOnItemUnequippedFromSlot_Implementation(UPDA_Item* Item, const FGameplayTag& TargetSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Slots")
	void EventToggleSlotName(UW_EquipmentSlot* EquipmentSlot, bool bShow);
	virtual void EventToggleSlotName_Implementation(UW_EquipmentSlot* EquipmentSlot, bool bShow);

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
	void EventShowError(const FText& Message);
	virtual void EventShowError_Implementation(const FText& Message);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Error")
	void EventDismissError();
	virtual void EventDismissError_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment|Error")
	void EventOnWeaponStatCheckFailed();
	virtual void EventOnWeaponStatCheckFailed_Implementation();
'''

# Insert before 'protected:'
insertion_point = content.find('protected:')
if insertion_point != -1:
    content = content[:insertion_point] + new_events + '\n' + content[insertion_point:]

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print('Added event declarations to W_Equipment.h')
