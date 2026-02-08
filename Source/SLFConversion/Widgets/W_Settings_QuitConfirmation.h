// W_Settings_QuitConfirmation.h
// C++ Widget class for W_Settings_QuitConfirmation
//
// Source: BlueprintDNA_v2/WidgetBlueprint/W_Settings_QuitConfirmation.json
// Parent: UUserWidget
// Variables: 1 | Functions: 0 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SlateWrapperTypes.h"

#include "W_Settings_QuitConfirmation.generated.h"

// Forward declarations
class UW_GenericButton;
class UTextBlock;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Settings_QuitConfirmation_OnQuitGameConfirmed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Settings_QuitConfirmation_OnQuitGameCanceled);

UCLASS()
class SLFCONVERSION_API UW_Settings_QuitConfirmation : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Settings_QuitConfirmation(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Gamepad/keyboard input handling
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_GenericButton* ActiveBtn;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FW_Settings_QuitConfirmation_OnQuitGameConfirmed OnQuitGameConfirmed;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FW_Settings_QuitConfirmation_OnQuitGameCanceled OnQuitGameCanceled;

protected:
	void CacheWidgetReferences();

	// Cached widget refs - NOT UPROPERTY to avoid conflict with Blueprint WidgetTree IsVariable=true
	UW_GenericButton* CachedYesBtn;
	UW_GenericButton* CachedNoBtn;

	// Button event handlers
	UFUNCTION()
	void OnYesButtonPressed();

	UFUNCTION()
	void OnNoButtonPressed();

	UFUNCTION()
	void OnYesButtonSelected(UW_GenericButton* Button);

	UFUNCTION()
	void OnNoButtonSelected(UW_GenericButton* Button);
};
