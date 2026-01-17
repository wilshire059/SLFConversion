// W_StatusEffectBar.h
// C++ Widget class for W_StatusEffectBar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_StatusEffectBar.json
// Parent: UUserWidget
// Variables: 1 | Functions: 0 | Dispatchers: 0

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


#include "W_StatusEffectBar.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types
class UB_StatusEffect;
class UProgressBar;

// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_StatusEffectBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_StatusEffectBar(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UB_StatusEffect* Effect;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET REFERENCES - from UMG hierarchy
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true), Category = "Widgets")
	UProgressBar* Bar;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatusEffectBar")
	void EventOnBuildupPercentChanged();
	virtual void EventOnBuildupPercentChanged_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatusEffectBar")
	void EventOnStatusEffectRemoved(FGameplayTag StatusEffectTag);
	virtual void EventOnStatusEffectRemoved_Implementation(FGameplayTag StatusEffectTag);

protected:
	// Cache references
	void CacheWidgetReferences();
};
