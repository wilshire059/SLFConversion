// W_Boss_Healthbar.h
// C++ Widget class for W_Boss_Healthbar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Boss_Healthbar.json
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


#include "W_Boss_Healthbar.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Boss_Healthbar : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Boss_Healthbar(const FObjectInitializer& ObjectInitializer);

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



	// Event Handlers (6 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Boss_Healthbar")
	void EventHideBossBar();
	virtual void EventHideBossBar_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Boss_Healthbar")
	void EventHideDamage();
	virtual void EventHideDamage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Boss_Healthbar")
	void EventInitializeBossBar(const FText& InName, AActor* BossActor);
	virtual void EventInitializeBossBar_Implementation(const FText& InName, AActor* BossActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Boss_Healthbar")
	void EventLerpHealthPositive();
	virtual void EventLerpHealthPositive_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Boss_Healthbar")
	void EventOnBossHealthUpdated(UB_Stat* UpdatedStat, double Change, bool UpdateAffectedStats, uint8 ValueType);
	virtual void EventOnBossHealthUpdated_Implementation(UB_Stat* UpdatedStat, double Change, bool UpdateAffectedStats, uint8 ValueType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Boss_Healthbar")
	void EventTimerTick();
	virtual void EventTimerTick_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
