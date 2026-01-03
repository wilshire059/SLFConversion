// W_Settings.h
// C++ Widget class for W_Settings
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Settings.json
// Parent: UW_Navigable_InputReader
// Variables: 7 | Functions: 4 | Dispatchers: 2

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

#include "W_Settings.generated.h"

// Forward declarations for widget types
class UW_GenericButton;
class UW_Settings_CategoryEntry;
class UW_Settings_Entry;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_Settings_OnQuitRequested, bool, Desktop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Settings_OnSettingsClosed);

UCLASS()
class SLFCONVERSION_API UW_Settings : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_Settings(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_Settings_CategoryEntry*> CategoryEntries;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FGameplayTag> CategoriesToHide;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_Settings_Entry*> SettingEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 CategoryNavigationIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 EntryNavigationIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool QuitToDesktop;
	// Note: This is NOT a BindWidget - it's a runtime reference set by Blueprint logic
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_GenericButton* ActiveQuitButton;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_OnQuitRequested OnQuitRequested;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_OnSettingsClosed OnSettingsClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void InitializeCategories();
	virtual void InitializeCategories_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void InitializeEntries();
	virtual void InitializeEntries_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	int32 GetEntryCountForActiveCategory();
	virtual int32 GetEntryCountForActiveCategory_Implementation();


	// Event Handlers (13 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventCancelConfirmation();
	virtual void EventCancelConfirmation_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventNavigateCategoryLeft();
	virtual void EventNavigateCategoryLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventNavigateCategoryRight();
	virtual void EventNavigateCategoryRight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventNavigateLeft();
	virtual void EventNavigateLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventNavigateResetToDefault();
	virtual void EventNavigateResetToDefault_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventNavigateRight();
	virtual void EventNavigateRight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventOnCategorySelected(UW_Settings_CategoryEntry* CategoryEntry, int32 InIndex);
	virtual void EventOnCategorySelected_Implementation(UW_Settings_CategoryEntry* CategoryEntry, int32 InIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventOnEntrySelected(UW_Settings_Entry* InEntry);
	virtual void EventOnEntrySelected_Implementation(UW_Settings_Entry* InEntry);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

protected:
	// Cache references
	void CacheWidgetReferences();
};
