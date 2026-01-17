// W_StatEntry_Status.h
// C++ Widget class for W_StatEntry_Status
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_StatEntry_Status.json
// Parent: UUserWidget
// Variables: 2 | Functions: 1 | Dispatchers: 1

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

#include "Interfaces/SLFStatEntryInterface.h"
#include "W_StatEntry_Status.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types
class UB_Stat;
class USLFStatBase;

// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_StatEntry_Status_OnStatChangeRequest, UB_Stat*, StatObject, bool, Increase);

UCLASS()
class SLFCONVERSION_API UW_StatEntry_Status : public UUserWidget, public ISLFStatEntryInterface
{
	GENERATED_BODY()

public:
	UW_StatEntry_Status(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	// Legacy stat reference (UB_Stat hierarchy)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UB_Stat* Stat;

	// New stat reference (USLFStatBase hierarchy - preferred)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	USLFStatBase* StatBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UObject* Tooltip;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_StatEntry_Status_OnStatChangeRequest OnStatChangeRequest;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_Status")
	void InitStatEntry();
	virtual void InitStatEntry_Implementation();


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatEntry_Status")
	void EventOnStatUpdated(UB_Stat* UpdatedStat, double Change, bool UpdateAffectedStats, ESLFValueType ValueType);
	virtual void EventOnStatUpdated_Implementation(UB_Stat* UpdatedStat, double Change, bool UpdateAffectedStats, ESLFValueType ValueType);

protected:
	// Cache references
	void CacheWidgetReferences();
};
