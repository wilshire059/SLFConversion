// W_RestMenu_TimeEntry.h
// C++ Widget class for W_RestMenu_TimeEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_RestMenu_TimeEntry.json
// Parent: UUserWidget
// Variables: 3 | Functions: 1 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_RestMenu_TimeEntry.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_RestMenu_TimeEntry_OnTimeEntryPressed, FSLFDayNightInfo, TimeInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_RestMenu_TimeEntry_OnTimeEntrySelected, UW_RestMenu_TimeEntry*, TimeEntry);

UCLASS()
class SLFCONVERSION_API UW_RestMenu_TimeEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_RestMenu_TimeEntry(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSLFDayNightInfo TimeInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FLinearColor SelectedColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_RestMenu_TimeEntry_OnTimeEntryPressed OnTimeEntryPressed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_RestMenu_TimeEntry_OnTimeEntrySelected OnTimeEntrySelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu_TimeEntry")
	void SetTimeEntrySelected(bool InSelected);
	virtual void SetTimeEntrySelected_Implementation(bool InSelected);


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu_TimeEntry")
	void EventTimeEntryPressed();
	virtual void EventTimeEntryPressed_Implementation();

	// Update display text from TimeInfo
	void UpdateTimeText();

protected:
	// Cache references
	void CacheWidgetReferences();

	// Button event handlers
	UFUNCTION()
	void HandleButtonClicked();

	UFUNCTION()
	void HandleButtonHovered();

	UFUNCTION()
	void HandleButtonUnhovered();

	// Cached widget references (NOT UPROPERTY to avoid conflict with Blueprint widget bindings)
	class UTextBlock* TimeTextWidget;
	class UBorder* BtnBorderWidget;
	class UButton* BtnWidget;
};
