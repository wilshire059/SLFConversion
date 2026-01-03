// W_GenericButton.h
// C++ Widget class for W_GenericButton
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_GenericButton.json
// Parent: UUserWidget
// Variables: 10 | Functions: 1 | Dispatchers: 2

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


#include "W_GenericButton.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_GenericButton_OnButtonSelected, UW_GenericButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_GenericButton_OnButtonPressed);

UCLASS()
class SLFCONVERSION_API UW_GenericButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_GenericButton(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (10)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText Text;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FSlateColor TextColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FMargin TextPadding;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor ButtonColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor OutlineColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double OutlineWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FVector4 OutlineCornerRadii;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor SelectedButtonColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor SelectedOutlineColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GenericButton_OnButtonSelected OnButtonSelected;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GenericButton_OnButtonPressed OnButtonPressed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GenericButton")
	void SetButtonSelected(bool InSelected);
	virtual void SetButtonSelected_Implementation(bool InSelected);


	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GenericButton")
	void EventPressButton();
	virtual void EventPressButton_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GenericButton")
	void EventSetButtonText(const FText& InText);
	virtual void EventSetButtonText_Implementation(const FText& InText);

protected:
	// Cache references
	void CacheWidgetReferences();
};
