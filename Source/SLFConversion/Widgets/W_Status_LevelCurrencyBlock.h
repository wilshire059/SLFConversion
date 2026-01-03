// W_Status_LevelCurrencyBlock.h
// C++ Widget class for W_Status_LevelCurrencyBlock
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Status_LevelCurrencyBlock.json
// Parent: UUserWidget
// Variables: 3 | Functions: 0 | Dispatchers: 0

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


#include "W_Status_LevelCurrencyBlock.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Status_LevelCurrencyBlock : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Status_LevelCurrencyBlock(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 LocalPlayerLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 LocalCurrency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UCurveFloat* LevelRequirementCurve;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Status_LevelCurrencyBlock")
	void EventInitializeLevelAndCurrency(int32 CurrentLevel, int32 CurrentCurrency);
	virtual void EventInitializeLevelAndCurrency_Implementation(int32 CurrentLevel, int32 CurrentCurrency);

protected:
	// Cache references
	void CacheWidgetReferences();
};
