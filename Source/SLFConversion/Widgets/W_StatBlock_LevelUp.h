// W_StatBlock_LevelUp.h
// C++ Widget class for W_StatBlock_LevelUp
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_StatBlock_LevelUp.json
// Parent: UUserWidget
// Variables: 5 | Functions: 2 | Dispatchers: 0

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


#include "W_StatBlock_LevelUp.generated.h"

// Forward declarations for widget types
class UW_StatEntry_LevelUp;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_StatBlock_LevelUp : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_StatBlock_LevelUp(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag Category;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool ShowUpgradeButtons;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<UW_StatEntry_LevelUp*> StatEntries;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor Color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor SelectedColor;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatBlock_LevelUp")
	void SetupStatsByCategory(const TArray<UB_Stat*>& StatObjects, const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& StatClassesAndCategories);
	virtual void SetupStatsByCategory_Implementation(const TArray<UB_Stat*>& StatObjects, const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& StatClassesAndCategories);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatBlock_LevelUp")
	TArray<UW_StatEntry_LevelUp*> GetAllStatsInBlock();
	virtual TArray<UW_StatEntry_LevelUp*> GetAllStatsInBlock_Implementation();


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatBlock_LevelUp")
	void EventStatInitializationComplete();
	virtual void EventStatInitializationComplete_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
