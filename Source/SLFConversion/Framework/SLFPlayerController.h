// SLFPlayerController.h
// C++ base for PC_SoulslikeFramework - Player Controller
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_Back;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_Ok;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_Up;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_Down;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_Left_Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_Right_Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_Unequip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_DetailedView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Navigation")
	UInputAction* IA_NavigableMenu_ResetToDefaults;

	/** Currently active widget tag for navigation routing */
	UPROPERTY(BlueprintReadWrite, Category = "Navigation")
	FGameplayTag ActiveWidgetTag;

	// ═══════════════════════════════════════════════════════════════════
	// INPUT MAPPING CONTEXTS (for SwitchInputContext)
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Contexts")
	UInputMappingContext* IMC_Gameplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Contexts")
	UInputMappingContext* IMC_NavigableMenu;

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
	// WIDGET PRELOADING (AAA pattern - eliminates first-use freeze)
	// ═══════════════════════════════════════════════════════════════════

	/** Pre-load commonly used widget classes to eliminate first-use hitches */
	void PreloadWidgetClasses();

	/** Cached widget classes to prevent garbage collection */
	UPROPERTY()
	TSubclassOf<UUserWidget> CachedInventorySlotClass;
	UPROPERTY()
	TSubclassOf<UUserWidget> CachedCategoryEntryClass;
	UPROPERTY()
	TSubclassOf<UUserWidget> CachedLootNotificationClass;

	// ═══════════════════════════════════════════════════════════════════
	// INPUT HANDLERS
	// ═══════════════════════════════════════════════════════════════════

	/** Handle IA_GameMenu input - toggle game menu via W_HUD */
	void HandleGameMenuInput(const FInputActionValue& Value);

	/** Navigation input handlers - route to active widget via W_HUD */
	void HandleNavigateBack(const FInputActionValue& Value);
	void HandleNavigateOk(const FInputActionValue& Value);
	void HandleNavigateUp(const FInputActionValue& Value);
	void HandleNavigateDown(const FInputActionValue& Value);
	void HandleNavigateLeft(const FInputActionValue& Value);
	void HandleNavigateRight(const FInputActionValue& Value);
	void HandleNavigateCategoryLeft(const FInputActionValue& Value);
	void HandleNavigateCategoryRight(const FInputActionValue& Value);
	void HandleNavigateUnequip(const FInputActionValue& Value);
	void HandleNavigateDetailedView(const FInputActionValue& Value);
	void HandleNavigateResetToDefaults(const FInputActionValue& Value);

public:
	// ═══════════════════════════════════════════════════════════════════
	// INPUT CONTEXT SWITCHING (from Blueprint SwitchInputContext)
	// ═══════════════════════════════════════════════════════════════════

	/**
	 * Switch between input mapping contexts.
	 * From Blueprint: Enables ContextsToEnable and disables ContextsToDisable.
	 * Used when opening/closing menus to switch between gameplay and menu input.
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SwitchInputContext(const TArray<UInputMappingContext*>& ContextsToEnable, const TArray<UInputMappingContext*>& ContextsToDisable);

protected:

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
