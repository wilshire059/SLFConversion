// W_RestMenu_Button.h
// C++ Widget class for W_RestMenu_Button
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_RestMenu_Button.json
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


#include "W_RestMenu_Button.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_RestMenu_Button_OnRestMenuButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_RestMenu_Button_OnRestMenuButtonSelected, UW_RestMenu_Button*, Button);

UCLASS()
class SLFCONVERSION_API UW_RestMenu_Button : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_RestMenu_Button(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor SelectedColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText ButtonText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_RestMenu_Button_OnRestMenuButtonPressed OnRestMenuButtonPressed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_RestMenu_Button_OnRestMenuButtonSelected OnRestMenuButtonSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu_Button")
	void SetRestMenuButtonSelected(bool InSelected);
	virtual void SetRestMenuButtonSelected_Implementation(bool InSelected);


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_RestMenu_Button")
	void EventRestMenuButtonPressed();
	virtual void EventRestMenuButtonPressed_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();

	// Cached widget references (use "Cached" prefix to avoid Blueprint BindWidget conflict)
	UPROPERTY()
	class UBorder* CachedBtnBorder;
	UPROPERTY()
	class UTextBlock* CachedTxt;
	UPROPERTY()
	class UButton* CachedBtn;

	// Button click handler
	UFUNCTION()
	void OnBtnClicked();
};
