// W_Status.h
// C++ Widget class for W_Status
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Status.json
// Parent: UW_Navigable_InputReader
// Variables: 4 | Functions: 1 | Dispatchers: 1

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


// Forward declarations for Blueprint types
class UAC_InventoryManager;
class UAC_StatManager;

// Forward declarations for SaveGame types


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

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_InventoryManager* InventoryComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 CurrentPlayerCurrency;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_StatManager* StatManagerComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
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
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

protected:
	// Cache references
	void CacheWidgetReferences();
};
