// PC_Menu_SoulslikeFramework.h
// C++ Game Framework class for PC_Menu_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-03
// Source: BlueprintDNA/GameFramework/PC_Menu_SoulslikeFramework.json
// Parent: APlayerController
// Variables: 3 | Functions: BeginPlay + Interface implementations

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "Interfaces/SLFControllerInterface.h"
#include "PC_Menu_SoulslikeFramework.generated.h"

// Forward declarations
class UW_MainMenu;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class SLFCONVERSION_API APC_Menu_SoulslikeFramework : public APlayerController, public ISLFControllerInterface
{
	GENERATED_BODY()

public:
	APC_Menu_SoulslikeFramework();

	virtual void BeginPlay() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	// Reference to the main menu widget
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	UW_MainMenu* W_MainMenu;

	// Whether a save game exists (can continue)
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	bool SaveGameExists;

	// Tag of the currently active navigable widget
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	FGameplayTag NavigableWidgetTag;

	// ═══════════════════════════════════════════════════════════════════════
	// CONFIGURATION
	// ═══════════════════════════════════════════════════════════════════════

	// Widget class to create for main menu
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TSubclassOf<UW_MainMenu> MainMenuWidgetClass;

	// Input mapping context for menu navigation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	UInputMappingContext* MenuInputMappingContext;

	// ═══════════════════════════════════════════════════════════════════════
	// INPUT ACTIONS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_NavigableMenu_Up;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_NavigableMenu_Down;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_NavigableMenu_Ok;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_NavigableMenu_Back;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE IMPLEMENTATIONS (ISLFControllerInterface)
	// ═══════════════════════════════════════════════════════════════════════

	// SetActiveWidgetForNavigation - sets the navigable widget tag
	virtual void SetActiveWidgetForNavigation_Implementation(const FGameplayTag& NavigableWidgetTag) override;

	virtual void SetupInputComponent() override;

protected:
	// Create and display the main menu widget
	void CreateMainMenuWidget();

	// Handle menu button clicks from W_MainMenu
	UFUNCTION()
	void HandleMenuButtonClicked(FName ButtonName);

	// Setup input for menu navigation
	void SetupMenuInput();

	// Check if any save game exists
	bool CheckSaveGameExists();

	// Load input actions
	void LoadInputActions();

	// Bind input actions after loading them
	void BindInputActions();

	// Input handlers
	void OnNavigateUp(const FInputActionValue& Value);
	void OnNavigateDown(const FInputActionValue& Value);
	void OnNavigateOk(const FInputActionValue& Value);
	void OnNavigateBack(const FInputActionValue& Value);
};
