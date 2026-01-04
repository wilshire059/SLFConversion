// W_GameMenu.h
// C++ Widget class for W_GameMenu
//
// 20-PASS VALIDATION: 2026-01-03
// Source: BlueprintDNA/WidgetBlueprint/W_GameMenu.json

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "W_GameMenu.generated.h"

// Forward declarations
class UW_GameMenu_Button;
class UVerticalBox;
class UWidgetAnimation;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_GameMenu_OnGameMenuWidgetRequest, FGameplayTag, Tag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_GameMenu_OnGameMenuClosed);

UCLASS()
class SLFCONVERSION_API UW_GameMenu : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_GameMenu(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	TArray<UW_GameMenu_Button*> MenuButtons;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UVerticalBox* ButtonsBox;

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATIONS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional), BlueprintReadOnly, Category = "Animations")
	UWidgetAnimation* FadeIn;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_OnGameMenuWidgetRequest OnGameMenuWidgetRequest;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_OnGameMenuClosed OnGameMenuClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT HANDLERS
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventOnMenuButtonPressed(FGameplayTag WidgetTag);
	virtual void EventOnMenuButtonPressed_Implementation(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventOnMenuButtonSelected(UW_GameMenu_Button* Button);
	virtual void EventOnMenuButtonSelected_Implementation(UW_GameMenu_Button* Button);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu")
	void EventOnVisibilityChanged(ESlateVisibility InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility);

protected:
	// Initialize menu buttons from ButtonsBox
	void InitializeMenuButtons();

	// Update button selection state based on NavigationIndex
	void UpdateButtonSelection();
};
