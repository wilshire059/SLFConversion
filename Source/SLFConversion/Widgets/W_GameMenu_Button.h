// W_GameMenu_Button.h
// C++ Widget class for W_GameMenu_Button
//
// 20-PASS VALIDATION: 2026-01-03
// Source: BlueprintDNA/WidgetBlueprint/W_GameMenu_Button.json

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "W_GameMenu_Button.generated.h"

// Forward declarations
class UButton;
class UBorder;
class UImage;
class UTextBlock;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_GameMenu_Button_OnGameMenuButtonPressed, FGameplayTag, WidgetTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_GameMenu_Button_OnGameMenuButtonSelected, UW_GameMenu_Button*, Button);

UCLASS()
class SLFCONVERSION_API UW_GameMenu_Button : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_GameMenu_Button(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// CONFIGURATION VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag TargetWidgetTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText ButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UTexture2D> ButtonImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor ImageColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor SelectedImageColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor ImageBorderColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor SelectedImageBorderColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor HighlightColor;

	// ═══════════════════════════════════════════════════════════════════════
	// STATE VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
	UButton* EscMenuButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
	UBorder* SelectedBorder;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UImage* ButtonIcon;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UTextBlock* ButtonLabel;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_Button_OnGameMenuButtonPressed OnGameMenuButtonPressed;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_Button_OnGameMenuButtonSelected OnGameMenuButtonSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu_Button")
	void SetGameMenuButtonSelected(bool InSelected);
	virtual void SetGameMenuButtonSelected_Implementation(bool InSelected);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu_Button")
	void EventOnGameMenuButtonPressed();
	virtual void EventOnGameMenuButtonPressed_Implementation();

protected:
	// Button event handlers
	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void OnButtonPressed();

	// Apply visual configuration
	void ApplyVisualConfig();
};
