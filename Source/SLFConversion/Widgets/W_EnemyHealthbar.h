// W_EnemyHealthbar.h
// C++ Widget class for W_EnemyHealthbar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_EnemyHealthbar.json
// Parent: UUserWidget
// Variables: 4 | Functions: 0 | Dispatchers: 0

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

#include "Interfaces/BPI_EnemyHealthbar.h"
#include "W_EnemyHealthbar.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_EnemyHealthbar : public UUserWidget, public IBPI_EnemyHealthbar
{
	GENERATED_BODY()

public:
	UW_EnemyHealthbar(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTimerHandle TickTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double HealthPercentCache;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTimerHandle DamageTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double TotalDamage;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (3 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_EnemyHealthbar")
	void EventHideDamage();
	virtual void EventHideDamage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_EnemyHealthbar")
	void EventLerpHealthPositive();
	virtual void EventLerpHealthPositive_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_EnemyHealthbar")
	void EventTimerTick();
	virtual void EventTimerTick_Implementation();

	// ISLFEnemyHealthbarInterface implementation
	virtual void UpdateEnemyHealthbar_Implementation(double CurrentHealth, double MaxHealth, double Change) override;

protected:
	// Cache references
	void CacheWidgetReferences();

	// Widget references (found via GetWidgetFromName, not UPROPERTY to avoid conflict with Blueprint)
	class UProgressBar* CachedHealthBar_Front;
	class UProgressBar* CachedHealthBar_Back;
};
