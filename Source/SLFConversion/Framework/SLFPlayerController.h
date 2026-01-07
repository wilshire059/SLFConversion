// SLFPlayerController.h
// C++ base for PC_SoulslikeFramework - Player Controller
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SLFPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UW_HUD;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASLFPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// ═══════════════════════════════════════════════════════════════════
	// INPUT ACTIONS
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_GameMenu;

	// ═══════════════════════════════════════════════════════════════════
	// HUD REFERENCE
	// ═══════════════════════════════════════════════════════════════════
	// Note: The Blueprint PC_SoulslikeFramework also has a W_HUD variable.
	// This C++ version uses a different property name to avoid collision.
	// The Blueprint can set this reference after creating the HUD.

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UW_HUD* HUDWidgetRef;

	/** Widget class to spawn for HUD - loaded from Blueprint */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	/** Widget class for Game Menu - loaded from Blueprint */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameMenuWidgetClass;

	/** Reference to the Game Menu widget instance */
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* GameMenuWidgetRef;

protected:
	// ═══════════════════════════════════════════════════════════════════
	// INPUT HANDLERS
	// ═══════════════════════════════════════════════════════════════════

	/** Handle IA_GameMenu input - opens/closes game menu */
	void HandleGameMenuInput(const FInputActionValue& Value);

	/** Debounce timer for menu input (prevents double-fire from Blueprint + C++ bindings) */
	double LastMenuInputTime = 0.0;

	// ═══════════════════════════════════════════════════════════════════
	// HUD INITIALIZATION (from Blueprint CLI_InitializeHUD)
	// ═══════════════════════════════════════════════════════════════════

	/**
	 * Native_InitializeHUD - C++ implementation of HUD initialization
	 * Called from BeginPlay to create and display the W_HUD widget.
	 * From Blueprint DNA: SetInputMode_GameOnly → bShowMouseCursor=false → CreateWidget<W_HUD_C> → AddToViewport
	 * Note: Renamed from CLI_InitializeHUD to avoid Blueprint name conflict
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void Native_InitializeHUD();
};
