// W_MainMenu_Button.h
// C++ Widget class for W_MainMenu_Button
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_MainMenu_Button.json
// Parent: UUserWidget
// Variables: 2 | Functions: 0 | Dispatchers: 2

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

#include "Interfaces/SLFMainMenuInterface.h"
#include "W_MainMenu_Button.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_MainMenu_Button_OnSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_MainMenu_Button_OnButtonClicked);

UCLASS()
class SLFCONVERSION_API UW_MainMenu_Button : public UUserWidget, public ISLFMainMenuInterface
{
	GENERATED_BODY()

public:
	UW_MainMenu_Button(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double Width;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_MainMenu_Button_OnSelected OnSelected;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_MainMenu_Button_OnButtonClicked OnButtonClicked;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════


protected:
	// Cache references
	void CacheWidgetReferences();
};
