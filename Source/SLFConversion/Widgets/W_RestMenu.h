// W_RestMenu.h
// C++ Widget class for W_RestMenu
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_RestMenu.json
// Parent: UW_Navigable_InputReader
// Variables: 6 | Functions: 4 | Dispatchers: 1

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
#include "Components/VerticalBox.h"

#include "W_RestMenu.generated.h"

// Forward declarations for widget types
class UW_RestMenu_Button;
class UW_RestMenu_TimeEntry;
class UW_TimePass;
class UW_LevelUp;

// Forward declarations for Blueprint types
class AB_RestingPoint;
class ASLFRestingPointBase;

// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_RestMenu_OnStorageRequested);

UCLASS()
class SLFCONVERSION_API UW_RestMenu : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_RestMenu(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Input handling - intercepts keyboard/gamepad input for navigation
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (6)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	AActor* CurrentRestingPoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* LocalTimeInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_RestMenu_Button*> MainButtonEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 MainNavigationIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_RestMenu_TimeEntry*> TimeEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 TimeEntryNavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_RestMenu_OnStorageRequested OnStorageRequested;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void InitializeTimeEntries();
	virtual void InitializeTimeEntries_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void InitializeTimePassWidget(const FSLFDayNightInfo& TargetTime);
	virtual void InitializeTimePassWidget_Implementation(const FSLFDayNightInfo& TargetTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	bool GetLevelUpMenuVisibility();
	virtual bool GetLevelUpMenuVisibility_Implementation();


	// Event Handlers (11 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventBackToMain();
	virtual void EventBackToMain_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventCloseRestMenu();
	virtual void EventCloseRestMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventFadeInRestMenu();
	virtual void EventFadeInRestMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventOnMainRestMenuButtonSelected(UW_RestMenu_Button* Button);
	virtual void EventOnMainRestMenuButtonSelected_Implementation(UW_RestMenu_Button* Button);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventOnTimeEntryPressed(FSLFDayNightInfo TimeInfo);
	virtual void EventOnTimeEntryPressed_Implementation(FSLFDayNightInfo TimeInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventOnTimeEntrySelected(UW_RestMenu_TimeEntry* TimeEntry);
	virtual void EventOnTimeEntrySelected_Implementation(UW_RestMenu_TimeEntry* TimeEntry);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu")
	void EventSetupRestingPoint(AActor* CurrentCampfire);
	virtual void EventSetupRestingPoint_Implementation(AActor* CurrentCampfire);

protected:
	// Cache references
	void CacheWidgetReferences();
	void BindButtonEvents();

	// Individual button press handlers (matches Blueprint pattern - each button has its own handler)
	UFUNCTION()
	void HandleRestButtonPressed();
	UFUNCTION()
	void HandleLevelUpButtonPressed();
	UFUNCTION()
	void HandleStorageButtonPressed();
	UFUNCTION()
	void HandleLeaveButtonPressed();

	// Button selection handler (generic - updates navigation index)
	UFUNCTION()
	void HandleButtonSelected(UW_RestMenu_Button* Button);

	// Time entry event handler
	UFUNCTION()
	void HandleTimeEntryPressed(FSLFDayNightInfo TimeInfo);

	UFUNCTION()
	void HandleTimeEntrySelected(UW_RestMenu_TimeEntry* TimeEntry);

	// Time pass completion handler
	UFUNCTION()
	void HandleTimePassEnd();

	// Respawn all enemies in the level (called when resting)
	void RespawnAllEnemies();

	// Cached widget references (named differently from Blueprint's BindWidgets to avoid conflict)
	UPROPERTY()
	class UWidgetSwitcher* CachedSwitcher;
	UPROPERTY()
	class UTextBlock* CachedLocationText;
	// These are NOT UPROPERTY to avoid conflict with Blueprint widget bindings
	class UVerticalBox* TimeEntriesBoxWidget;
	UW_TimePass* TimePassWidgetRef;
	class UTextBlock* TimeOfDayTextWidget;
	UW_LevelUp* LevelUpWidgetRef;

	// Class for creating time entry widgets
	UPROPERTY()
	TSubclassOf<UW_RestMenu_TimeEntry> TimeEntryWidgetClass;

	// Individual button references for binding
	UPROPERTY()
	UW_RestMenu_Button* RestButton;
	UPROPERTY()
	UW_RestMenu_Button* LevelUpButton;
	UPROPERTY()
	UW_RestMenu_Button* StorageButton;
	UPROPERTY()
	UW_RestMenu_Button* LeaveButton;

	// Current view index (0 = main buttons, 1 = time selection)
	int32 CurrentViewIndex;
};
