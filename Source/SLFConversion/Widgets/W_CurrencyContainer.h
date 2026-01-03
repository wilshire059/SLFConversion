// W_CurrencyContainer.h
// C++ Widget class for W_CurrencyContainer
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_CurrencyContainer.json
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


#include "W_CurrencyContainer.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_CurrencyContainer : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_CurrencyContainer(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 LocalCurrency;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CurrencyContainer")
	void EventOnCurrencyIncreased(int32 IncreaseAmount);
	virtual void EventOnCurrencyIncreased_Implementation(int32 IncreaseAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CurrencyContainer")
	void EventOnCurrencyUpdated(int32 NewCurrency);
	virtual void EventOnCurrencyUpdated_Implementation(int32 NewCurrency);

protected:
	// Cache references
	void CacheWidgetReferences();
};
