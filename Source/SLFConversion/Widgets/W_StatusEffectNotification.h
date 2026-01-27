// W_StatusEffectNotification.h
// C++ Widget class for W_StatusEffectNotification
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_StatusEffectNotification.json
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


#include "W_StatusEffectNotification.generated.h"

// Forward declarations for widget types
class UTextBlock;
class UAC_StatusEffectManager;
class UStatusEffectManagerComponent;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_StatusEffectNotification : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_StatusEffectNotification(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double NotificationDuration;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET REFERENCES - from UMG hierarchy
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true), Category = "Widgets")
	UTextBlock* TxtStatusEffect;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatusEffectNotification")
	void EventFinishNotification();
	virtual void EventFinishNotification_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_StatusEffectNotification")
	void EventOnStatusEffectTriggered(const FText& TriggeredText);
	virtual void EventOnStatusEffectTriggered_Implementation(const FText& TriggeredText);

protected:
	// Cache references
	void CacheWidgetReferences();

	// Bind to status effect manager events (matches bp_only Construct graph)
	void BindToStatusEffectManager();

	// Cached status effect manager reference
	UPROPERTY(Transient)
	UAC_StatusEffectManager* CachedStatusEffectManager;

	UPROPERTY(Transient)
	UStatusEffectManagerComponent* CachedStatusEffectManagerComponent;

	// Handler called when status effect is triggered
	UFUNCTION()
	void OnStatusEffectTriggeredHandler(FText TriggeredText);

	// Timer handle for notification duration
	FTimerHandle NotificationTimerHandle;
};
