// W_LevelUp.h
// C++ Widget class for W_LevelUp
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_LevelUp.json
// Parent: UW_Navigable_InputReader
// Variables: 8 | Functions: 3 | Dispatchers: 2

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


#include "W_LevelUp.generated.h"

// Forward declarations for widget types
class UW_StatEntry_LevelUp;

// Forward declarations for Blueprint types
class UAC_InventoryManager;
class UAC_StatManager;
class UB_Stat;

// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_LevelUp_OnLevelUpMenuClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_LevelUp_OnPreviewDisregarded);

UCLASS()
class SLFCONVERSION_API UW_LevelUp : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_LevelUp(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (8)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_InventoryManager* InventoryComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_StatManager* StatManagerComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 CurrentPlayerCurrency;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 CurrentPlayerLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TMap<FGameplayTag, UB_Stat*> CurrentChanges;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TMap<FGameplayTag, UB_Stat*> DefaultValues;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_StatEntry_LevelUp*> AllStatEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_LevelUp_OnLevelUpMenuClosed OnLevelUpMenuClosed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_LevelUp_OnPreviewDisregarded OnPreviewDisregarded;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void HandleStatChanges(UB_Stat* StatObject, bool Increase);
	virtual void HandleStatChanges_Implementation(UB_Stat* StatObject, bool Increase);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void SetAllStatEntries();
	virtual void SetAllStatEntries_Implementation();


	// Event Handlers (17 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventApplyNewStats();
	virtual void EventApplyNewStats_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventConfirmLevelUp();
	virtual void EventConfirmLevelUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventDisregardPreview();
	virtual void EventDisregardPreview_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventExit();
	virtual void EventExit_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventNavigateLeft();
	virtual void EventNavigateLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventNavigateRight();
	virtual void EventNavigateRight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventOnBackRequested();
	virtual void EventOnBackRequested_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventOnPlayerCurrencyUpdated(int32 NewCurrency);
	virtual void EventOnPlayerCurrencyUpdated_Implementation(int32 NewCurrency);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventOnPlayerLevelUpdated(int32 NewLevel);
	virtual void EventOnPlayerLevelUpdated_Implementation(int32 NewLevel);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventOnStatChangeRequested(UB_Stat* StatObject, bool Increase);
	virtual void EventOnStatChangeRequested_Implementation(UB_Stat* StatObject, bool Increase);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventOnStatEntrySelected(const TArray<FGameplayTag>& AffectedStats, bool Selected);
	virtual void EventOnStatEntrySelected_Implementation(const TArray<FGameplayTag>& AffectedStats, bool Selected);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelUp")
	void EventUpdateStatChanges();
	virtual void EventUpdateStatChanges_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
