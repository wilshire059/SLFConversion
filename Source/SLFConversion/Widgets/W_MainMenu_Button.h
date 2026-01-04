// W_MainMenu_Button.h
// C++ Widget class for W_MainMenu_Button
//
// 20-PASS VALIDATION: 2026-01-03
// Source: BlueprintDNA/WidgetBlueprint/W_MainMenu_Button.json

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/SLFMainMenuInterface.h"
#include "W_MainMenu_Button.generated.h"

// Forward declarations
class USizeBox;
class UBorder;
class UButton;
class UWidgetAnimation;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_MainMenu_Button_OnSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_MainMenu_Button_OnButtonClicked);

UCLASS()
class SLFCONVERSION_API UW_MainMenu_Button : public UUserWidget, public ISLFMainMenuInterface
{
	GENERATED_BODY()

public:
	UW_MainMenu_Button(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float Width;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
	USizeBox* Sizer;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
	UBorder* SelectedBorder;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
	UButton* Button;

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATIONS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly, Category = "Animations")
	UWidgetAnimation* Loadup;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_MainMenu_Button_OnSelected OnSelected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_MainMenu_Button_OnButtonClicked OnButtonClicked;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE IMPLEMENTATIONS (ISLFMainMenuInterface)
	// ═══════════════════════════════════════════════════════════════════════

	virtual void SetMenuButtonSelected_Implementation(bool Selected) override;
	virtual void PlayButtonInitAnimation_Implementation(double StartTime, int32 Loops, EUMGSequencePlayMode::Type Mode, double PlayRate) override;
	virtual void OnMenuButtonPressed_Implementation() override;

protected:
	// Button event handlers
	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void OnButtonClicked_Internal();
};
