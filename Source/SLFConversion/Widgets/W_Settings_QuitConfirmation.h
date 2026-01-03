// W_Settings_QuitConfirmation.h
// C++ Widget class for W_Settings_QuitConfirmation
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Settings_QuitConfirmation.json
// Parent: UUserWidget
// Variables: 1 | Functions: 0 | Dispatchers: 2

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
#include "Components/Button.h"

#include "W_Settings_QuitConfirmation.generated.h"

// Forward declarations for widget types
class UW_GenericButton;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Settings_QuitConfirmation_OnQuitGameConfirmed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Settings_QuitConfirmation_OnQuitGameCanceled);

UCLASS()
class SLFCONVERSION_API UW_Settings_QuitConfirmation : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Settings_QuitConfirmation(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	// Note: This is NOT a BindWidget - it's a runtime reference set by Blueprint logic
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_GenericButton* ActiveBtn;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_QuitConfirmation_OnQuitGameConfirmed OnQuitGameConfirmed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_QuitConfirmation_OnQuitGameCanceled OnQuitGameCanceled;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════


protected:
	// Cache references
	void CacheWidgetReferences();
};
