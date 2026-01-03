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

#include "W_RestMenu.generated.h"

// Forward declarations for widget types
class UW_RestMenu_Button;
class UW_RestMenu_TimeEntry;

// Forward declarations for Blueprint types
class AB_RestingPoint;

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

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (6)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	AB_RestingPoint* CurrentRestingPoint;
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
	void EventSetupRestingPoint(AB_RestingPoint* CurrentCampfire);
	virtual void EventSetupRestingPoint_Implementation(AB_RestingPoint* CurrentCampfire);

protected:
	// Cache references
	void CacheWidgetReferences();
};
