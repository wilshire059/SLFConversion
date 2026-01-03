// W_LevelCurrencyBlock_LevelUp.h
// C++ Widget class for W_LevelCurrencyBlock_LevelUp
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_LevelCurrencyBlock_LevelUp.json
// Parent: UUserWidget
// Variables: 2 | Functions: 0 | Dispatchers: 1

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


#include "W_LevelCurrencyBlock_LevelUp.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_LevelCurrencyBlock_LevelUp_OnLevelUpCostChanged, bool, CanLevelUp);

UCLASS()
class SLFCONVERSION_API UW_LevelCurrencyBlock_LevelUp : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_LevelCurrencyBlock_LevelUp(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 LocalPlayerLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 LocalCurrency;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_LevelCurrencyBlock_LevelUp_OnLevelUpCostChanged OnLevelUpCostChanged;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (4 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelCurrencyBlock_LevelUp")
	void EventInitializeLevelUpCost(int32 NewLevel, int32 CurrentCurrency);
	virtual void EventInitializeLevelUpCost_Implementation(int32 NewLevel, int32 CurrentCurrency);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelCurrencyBlock_LevelUp")
	void EventOnCurrencyUpdated(int32 NewCurrency);
	virtual void EventOnCurrencyUpdated_Implementation(int32 NewCurrency);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelCurrencyBlock_LevelUp")
	void EventOnLevelUpdated(int32 NewLevel);
	virtual void EventOnLevelUpdated_Implementation(int32 NewLevel);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelCurrencyBlock_LevelUp")
	void EventSetCurrentLevel(int32 Level);
	virtual void EventSetCurrentLevel_Implementation(int32 Level);

protected:
	// Cache references
	void CacheWidgetReferences();
};
