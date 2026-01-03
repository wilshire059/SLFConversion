// W_GameMenu.h
// C++ Widget class for W_GameMenu
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_GameMenu.json
// Parent: UW_Navigable_InputReader
// Variables: 2 | Functions: 1 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"
#include "Components/Button.h"

#include "W_GameMenu.generated.h"

// Forward declarations for widget types
class UW_GameMenu_Button;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


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
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_GameMenu_Button*> MenuButtons;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_OnGameMenuWidgetRequest OnGameMenuWidgetRequest;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_OnGameMenuClosed OnGameMenuClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (7 events)
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
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

protected:
	// Cache references
	void CacheWidgetReferences();
};
