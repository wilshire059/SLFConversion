// W_StatEntry.h
// C++ Widget class for W_StatEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_StatEntry.json
// Parent: UUserWidget
// Variables: 4 | Functions: 1 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "SLFStatTypes.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"

#include "Interfaces/SLFStatEntryInterface.h"
#include "W_StatEntry.generated.h"

// Forward declarations for widget types


// Forward declarations for stat types
class USLFStatBase;

// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_StatEntry_OnStatChangeRequest, USLFStatBase*, StatObject, bool, Increase);

UCLASS()
class SLFCONVERSION_API UW_StatEntry : public UUserWidget, public ISLFStatEntryInterface
{
	GENERATED_BODY()

public:
	UW_StatEntry(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	USLFStatBase* Stat;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UObject* Tooltip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool ShowAdjustButtons;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double OldValue;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_StatEntry_OnStatChangeRequest OnStatChangeRequest;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry")
	void InitStatEntry();
	virtual void InitStatEntry_Implementation();


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry")
	void EventOnStatUpdated(USLFStatBase* UpdatedStat, double Change, bool UpdateAffectedStats, ESLFValueType ValueType);
	virtual void EventOnStatUpdated_Implementation(USLFStatBase* UpdatedStat, double Change, bool UpdateAffectedStats, ESLFValueType ValueType);

protected:
	// Cache references
	void CacheWidgetReferences();
};
