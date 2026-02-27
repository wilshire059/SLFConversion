// W_MainMenu.h
// C++ Widget class for W_MainMenu
//
// 20-PASS VALIDATION: 2026-01-03
// Source: BlueprintDNA/WidgetBlueprint/W_MainMenu.json
// Parent: UW_Navigable
// Variables: 6 | Functions: 5 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable.h"
#include "GameplayTagContainer.h"
#include "Animation/WidgetAnimation.h"
#include "Components/VerticalBox.h"
#include "MediaPlayer.h"

#include "W_MainMenu.generated.h"

// Forward declarations
class UW_LoadingScreen;
class UW_MainMenu_Button;
class UW_CharacterSelection;
class UW_LoadGame;
class UW_Settings;
class UW_Credits;
class UW_WorldMap;
class UPrimaryDataAsset;
class UButton;
class UTextBlock;

UCLASS()
class SLFCONVERSION_API UW_MainMenu : public UW_Navigable
{
	GENERATED_BODY()

public:
	UW_MainMenu(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (6)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UMediaPlayer* MediaPlayer;

	// Buttons array - populated from ButtonsBox children at construct
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	TArray<UWidget*> Buttons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool CanContinueGame;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_LoadingScreen* LoadingScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* MainMenuAsset;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// BIND WIDGETS (UMG Designer References)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
	UVerticalBox* ButtonsBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_MainMenu_Button* BtnContinue;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_MainMenu_Button* BtnNewGame;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_MainMenu_Button* BtnLoadGame;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_MainMenu_Button* BtnSettings;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_MainMenu_Button* BtnCredits;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_MainMenu_Button* BtnMap;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_MainMenu_Button* BtnQuitGame;

	// Embedded overlay widgets (children in UMG designer hierarchy)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_CharacterSelection* W_CharacterSelection;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_LoadGame* W_LoadGame;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_Settings* W_Settings;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_Credits* W_Credits;

	// World map overlay (created dynamically in NativeConstruct)
	UPROPERTY()
	UW_WorldMap* WorldMapOverlay;

	// Dynamic map button fallback (created if BtnMap is null)
	UPROPERTY()
	UButton* DynamicMapButton;

	// Track which overlay is currently active (nullptr = main menu)
	UPROPERTY()
	UWidget* ActiveOverlay;

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATIONS (UMG Designer References)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly, Category = "Animations")
	UWidgetAnimation* Fade;

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly, Category = "Animations")
	UWidgetAnimation* FadeMenuOnly;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════


	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Called when a button is selected - updates all button states
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void OnButtonSelected(UW_MainMenu_Button* Button);
	virtual void OnButtonSelected_Implementation(UW_MainMenu_Button* Button);

	// Override CanNavigate from W_Navigable - check if navigation is allowed (not loading, visible, etc.)
	// Note: No UFUNCTION needed - inherited from parent
	virtual bool CanNavigate_Implementation() override;

	// Event Handlers (4 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventFadeInMenu(float PlaybackSpeed = 1.0f);
	virtual void EventFadeInMenu_Implementation(float PlaybackSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventNavigateLeft();
	virtual void EventNavigateLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventNavigateRight();
	virtual void EventNavigateRight_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS
	// ═══════════════════════════════════════════════════════════════════════

	// Broadcast when a main menu button is clicked (with button name for routing)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMainMenuButtonClicked, FName, ButtonName);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMainMenuButtonClicked OnMenuButtonClicked;

protected:
	// Initialize buttons from ButtonsBox
	void InitializeButtons();

	// Update button selection based on NavigationIndex
	void UpdateButtonSelection();

	// Bind button events
	void BindButtonEvents();

	// Unbind button events
	void UnbindButtonEvents();

	// Button click handlers
	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnLoadGameClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnCreditsClicked();

	UFUNCTION()
	void OnMapClicked();

	UFUNCTION()
	void OnQuitGameClicked();

	// Overlay handlers
	UFUNCTION()
	void OnCharacterClassSelected(UPrimaryDataAsset* ClassAsset);

	UFUNCTION()
	void OnCharacterSelectionClosed();

	UFUNCTION()
	void OnLoadGameClosed();

	// Called when a save slot is confirmed in W_LoadGame (slot already set as active)
	UFUNCTION()
	void OnSaveSlotConfirmed();

	UFUNCTION()
	void OnSettingsClosed();

	UFUNCTION()
	void OnCreditsClosed();

	UFUNCTION()
	void OnWorldMapClosed();

	// Dynamic map button click handler
	UFUNCTION()
	void OnDynamicMapButtonClicked();

	// Animation finish callback
	UFUNCTION()
	void OnFadeMenuOnlyFinished();

	// Show an overlay widget with fade animation
	void ShowOverlay(UWidget* Overlay);

	// Return from overlay to main menu
	void ReturnFromOverlay();

	// Generate a unique save slot name for a new game: "SLF_{CharacterName}_{index}"
	FString GenerateUniqueSlotName(UPrimaryDataAsset* CharacterClass);

	// Open the game level via loading screen (or direct OpenLevel as fallback)
	void LoadGameLevel();
};
