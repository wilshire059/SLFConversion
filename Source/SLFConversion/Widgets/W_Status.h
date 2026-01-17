// W_Status.h
// C++ Widget class for W_Status
//
// 20-PASS VALIDATION: 2026-01-05
// Source: BlueprintDNA_v2/WidgetBlueprint/W_Status.json
// Parent: UW_Navigable_InputReader
// Variables: 4 | Functions: 1 | Dispatchers: 1
//
// NO REFLECTION - all widgets accessed via BindWidgetOptional

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"

#include "W_Status.generated.h"

// Forward declarations for widget types
class UW_Status_LevelCurrencyBlock;
class UW_Status_StatBlock;

// Forward declarations for Blueprint types
class UInventoryManagerComponent;
class UStatManagerComponent;
class UB_Stat;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Status_OnStatusClosed);

UCLASS()
class SLFCONVERSION_API UW_Status : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_Status(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Input handling (replaces Blueprint EventGraph input bindings)
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ═══════════════════════════════════════════════════════════════════════
	// BIND WIDGETS - Direct access via BindWidgetOptional
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_Status_LevelCurrencyBlock* W_Status_LevelCurrencyBlock;

	// Stat block widgets (6 blocks for different stat categories)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_Status_StatBlock* W_StatBlock_Status;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_Status_StatBlock* W_StatBlock_Status_1;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_Status_StatBlock* W_StatBlock_Status_3;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_Status_StatBlock* W_StatBlock_Status_5;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_Status_StatBlock* W_StatBlock_Status_7;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UW_Status_StatBlock* W_StatBlock_Status_8;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	UInventoryManagerComponent* InventoryComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	int32 CurrentPlayerCurrency;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	UStatManagerComponent* StatManagerComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	int32 CurrentPlayerLevel;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Status_OnStatusClosed OnStatusClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (4 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Status")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Status")
	void EventOnCurrencyUpdated(int32 NewCurrency);
	virtual void EventOnCurrencyUpdated_Implementation(int32 NewCurrency);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Status")
	void EventOnLevelUpdated(int32 NewLevel);
	virtual void EventOnLevelUpdated_Implementation(int32 NewLevel);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Status")
	void EventOnVisibilityChanged(ESlateVisibility InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility);

protected:
	// Initialize child widgets with current values
	void InitializeLevelCurrencyBlock();

	// Called when stats are initialized - populates all stat blocks
	UFUNCTION()
	void OnStatsInitializedHandler();

	// Populates all stat blocks with current stat data
	void PopulateStatBlocks();
};
