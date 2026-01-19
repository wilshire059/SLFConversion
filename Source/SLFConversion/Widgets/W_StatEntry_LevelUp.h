// W_StatEntry_LevelUp.h
// C++ Widget class for W_StatEntry_LevelUp
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_StatEntry_LevelUp.json
// Parent: UUserWidget
// Variables: 9 | Functions: 2 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"  // For FAffectedStat, FStatInfo
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"

#include "Interfaces/SLFStatEntryInterface.h"
#include "W_StatEntry_LevelUp.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types
class USLFStatBase;

// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_StatEntry_LevelUp_OnStatChangeRequest, USLFStatBase*, StatObject, bool, Increase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_StatEntry_LevelUp_OnStatEntrySelected, TArray<FGameplayTag>, AffectedStats, bool, Selected);

UCLASS()
class SLFCONVERSION_API UW_StatEntry_LevelUp : public UUserWidget, public ISLFStatEntryInterface
{
	GENERATED_BODY()

public:
	UW_StatEntry_LevelUp(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (9)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	USLFStatBase* Stat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FGameplayTag StatCategory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UObject* Tooltip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool ShowAdjustButtons;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double OldValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FLinearColor Color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FLinearColor SelectedColor;
	// GOLD color for text when there's a pending stat change (R=0.858824, G=0.745098, B=0.619608)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FLinearColor PendingChangeTextColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool CanLevelUp;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_StatEntry_LevelUp_OnStatChangeRequest OnStatChangeRequest;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_StatEntry_LevelUp_OnStatEntrySelected OnStatEntrySelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_LevelUp")
	void InitStatEntry();
	virtual void InitStatEntry_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_LevelUp")
	void SetLevelUpStatEntrySelected(bool InSelected);
	virtual void SetLevelUpStatEntrySelected_Implementation(bool InSelected);


	// Event Handlers (7 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_LevelUp")
	void EventDecreaseStat();
	virtual void EventDecreaseStat_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_LevelUp")
	void EventHighlight();
	virtual void EventHighlight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_LevelUp")
	void EventIncreaseStat();
	virtual void EventIncreaseStat_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_LevelUp")
	void EventOnStatUpdated(USLFStatBase* UpdatedStat, double Change, bool UpdateAffectedStats, uint8 ValueType);
	virtual void EventOnStatUpdated_Implementation(USLFStatBase* UpdatedStat, double Change, bool UpdateAffectedStats, uint8 ValueType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_LevelUp")
	void EventRemoveHighlight();
	virtual void EventRemoveHighlight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_LevelUp")
	void EventSetInitialValue(double InOldValue);
	virtual void EventSetInitialValue_Implementation(double InOldValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_LevelUp")
	void EventToggleLevelUpEnabled(bool Enabled);
	virtual void EventToggleLevelUpEnabled_Implementation(bool Enabled);

protected:
	// Cache references
	void CacheWidgetReferences();

	// Button click handlers (bound to UButton::OnClicked)
	UFUNCTION()
	void OnButtonIncreaseClicked();
	UFUNCTION()
	void OnButtonDecreaseClicked();

	// Handler for Stat->OnStatUpdated delegate binding
	// Signature must match FOnStatValueUpdated exactly for dynamic delegate binding
	UFUNCTION()
	void OnStatValueUpdatedHandler(USLFStatBase* UpdatedStat, double Change, bool UpdateAffectedStats, ESLFValueType ValueType);
};
