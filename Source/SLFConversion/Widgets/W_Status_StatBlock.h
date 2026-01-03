// W_Status_StatBlock.h
// C++ Widget class for W_Status_StatBlock
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Status_StatBlock.json
// Parent: UUserWidget
// Variables: 3 | Functions: 2 | Dispatchers: 0

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


#include "W_Status_StatBlock.generated.h"

// Forward declarations for widget types
class UW_StatEntry_Status;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Status_StatBlock : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Status_StatBlock(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag Category;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_StatEntry_Status*> StatEntries;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 DebugShowCount;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Status_StatBlock")
	void SetupCurrentStats(const TArray<UB_Stat*>& StatObjects, const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& StatClassesAndCategories);
	virtual void SetupCurrentStats_Implementation(const TArray<UB_Stat*>& StatObjects, const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& StatClassesAndCategories);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Status_StatBlock")
	TArray<UW_StatEntry_Status*> GetAllStatsInBlock();
	virtual TArray<UW_StatEntry_Status*> GetAllStatsInBlock_Implementation();


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Status_StatBlock")
	void EventStatInitializationComplete();
	virtual void EventStatInitializationComplete_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
