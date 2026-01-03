// W_Inventory_ActionButton.h
// C++ Widget class for W_Inventory_ActionButton
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Inventory_ActionButton.json
// Parent: UUserWidget
// Variables: 4 | Functions: 1 | Dispatchers: 2

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


#include "W_Inventory_ActionButton.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Inventory_ActionButton_OnActionButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_Inventory_ActionButton_OnActionButtonSelected, UW_Inventory_ActionButton*, Button);

UCLASS()
class SLFCONVERSION_API UW_Inventory_ActionButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Inventory_ActionButton(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText ActionText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor Color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor SelectedColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Inventory_ActionButton_OnActionButtonPressed OnActionButtonPressed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Inventory_ActionButton_OnActionButtonSelected OnActionButtonSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory_ActionButton")
	void SetActionButtonSelected(bool InSelected);
	virtual void SetActionButtonSelected_Implementation(bool InSelected);


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory_ActionButton")
	void EventActionPressed();
	virtual void EventActionPressed_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
