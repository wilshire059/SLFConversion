// W_InteractionError.h
// C++ Widget class for W_InteractionError
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_InteractionError.json
// Parent: UUserWidget
// Variables: 1 | Functions: 0 | Dispatchers: 0

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


#include "W_InteractionError.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_InteractionError : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_InteractionError(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool MessageActive;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InteractionError")
	void EventRemoveMessage();
	virtual void EventRemoveMessage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_InteractionError")
	void EventShowMessage(const FText& InMessage, double Time);
	virtual void EventShowMessage_Implementation(const FText& InMessage, double Time);

protected:
	// Cache references
	void CacheWidgetReferences();
};
