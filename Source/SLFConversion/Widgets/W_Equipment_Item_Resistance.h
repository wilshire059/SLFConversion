// W_Equipment_Item_Resistance.h
// C++ Widget class for W_Equipment_Item_Resistance
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Equipment_Item_Resistance.json
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


#include "W_Equipment_Item_Resistance.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Equipment_Item_Resistance : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Equipment_Item_Resistance(const FObjectInitializer& ObjectInitializer);

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



	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment_Item_Resistance")
	void EventSetupResistanceEntries(const FGameplayTag& TargetResistanceStats, const FGameplayTag& CurrentResistanceStats, bool CanCompare);
	virtual void EventSetupResistanceEntries_Implementation(const FGameplayTag& TargetResistanceStats, const FGameplayTag& CurrentResistanceStats, bool CanCompare);

protected:
	// Cache references
	void CacheWidgetReferences();
};
