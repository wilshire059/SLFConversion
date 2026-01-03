// W_GenericError.h
// C++ Widget class for W_GenericError
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_GenericError.json
// Parent: UUserWidget
// Variables: 1 | Functions: 0 | Dispatchers: 1

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


#include "W_GenericError.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_GenericError_OnErrorMessageDismissed);

UCLASS()
class SLFCONVERSION_API UW_GenericError : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_GenericError(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FText Message;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GenericError_OnErrorMessageDismissed OnErrorMessageDismissed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GenericError")
	void EventErrorOkPressed();
	virtual void EventErrorOkPressed_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GenericError")
	void EventSetErrorMessage(const FText& InMessage);
	virtual void EventSetErrorMessage_Implementation(const FText& InMessage);

protected:
	// Cache references
	void CacheWidgetReferences();
};
