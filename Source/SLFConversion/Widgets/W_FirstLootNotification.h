// W_FirstLootNotification.h
// C++ Widget class for W_FirstLootNotification
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_FirstLootNotification.json
// Parent: UUserWidget
// Variables: 0 | Functions: 0 | Dispatchers: 0

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


#include "W_FirstLootNotification.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_FirstLootNotification : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_FirstLootNotification(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_FirstLootNotification")
	void EventOnFinish();
	virtual void EventOnFinish_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_FirstLootNotification")
	void EventShowNotification(const FText& ItemName, int32 ItemIcon, double InDuration);
	virtual void EventShowNotification_Implementation(const FText& ItemName, int32 ItemIcon, double InDuration);

protected:
	// Cache references
	void CacheWidgetReferences();
};
