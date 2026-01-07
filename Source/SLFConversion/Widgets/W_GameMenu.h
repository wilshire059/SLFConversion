// W_GameMenu.h
// C++ Widget class for W_GameMenu
//
// 20-PASS VALIDATION: 2026-01-05
// Source: BlueprintDNA_v2/WidgetBlueprint/W_GameMenu.json
// Parent: UW_Navigable_InputReader
// Variables: 4 | Functions: 8 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "W_GameMenu.generated.h"

// Forward declarations
class UW_GameMenu_Button;
class UWidgetAnimation;
class UPanelWidget;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_GameMenu_OnGameMenuWidgetRequest, FGameplayTag, Tag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_GameMenu_OnGameMenuClosed);

UCLASS()
class SLFCONVERSION_API UW_GameMenu : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_GameMenu(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	// Input handling for menu navigation
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	// Array of menu buttons - populated from children in Construct
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	TArray<UW_GameMenu_Button*> MenuButtons;

	// Current navigation index
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS (Optional)
	// ═══════════════════════════════════════════════════════════════════════

	// Container for menu buttons (if using BindWidget)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UPanelWidget* ButtonContainer;

	// Animation for showing/hiding menu (named FadeBorders in Blueprint)
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* FadeBorders;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	// Fired when a menu button is pressed with the target widget tag
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_OnGameMenuWidgetRequest OnGameMenuWidgetRequest;

	// Fired when the menu is closed
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_OnGameMenuClosed OnGameMenuClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// NAVIGATION FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Navigate down - increment index and select button
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	// Navigate up - decrement index and select button
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	// Navigate OK - activate current button
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	// Navigate Cancel - close menu
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// BUTTON EVENT HANDLERS
	// ═══════════════════════════════════════════════════════════════════════

	// Called when a menu button is pressed
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventOnMenuButtonPressed(FGameplayTag WidgetTag);
	virtual void EventOnMenuButtonPressed_Implementation(FGameplayTag WidgetTag);

	// Called when a menu button is selected (hovered)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventOnMenuButtonSelected(UW_GameMenu_Button* Button);
	virtual void EventOnMenuButtonSelected_Implementation(UW_GameMenu_Button* Button);

	// ═══════════════════════════════════════════════════════════════════════
	// VISIBILITY HANDLING
	// ═══════════════════════════════════════════════════════════════════════

	// Called when widget visibility changes
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventOnVisibilityChanged(ESlateVisibility InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility);

	// Input device changed handler
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void OnInputDeviceChanged();
	virtual void OnInputDeviceChanged_Implementation();

protected:
	// Populate MenuButtons array from children
	void PopulateMenuButtons();

	// Bind to button events
	void BindButtonEvents();

	// Unbind from button events
	void UnbindButtonEvents();

	// Select button at current NavigationIndex
	void SelectCurrentButton();

	// Apply input mapping contexts
	void ApplyInputMappingContexts(bool bAdd);
};
