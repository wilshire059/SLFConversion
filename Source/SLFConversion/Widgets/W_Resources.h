// W_Resources.h
// C++ Widget class for W_Resources
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Resources.json
// Parent: UUserWidget
// Variables: 11 | Functions: 1 | Dispatchers: 0

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


#include "W_Resources.generated.h"

// Forward declarations for widget types
class UW_Buff;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Resources : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Resources(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (11)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	double HealthPercentCache;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	double FocusPercentCache;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	double StaminaPercentCache;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTimerHandle TickTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double BackBarPositiveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double BackBarNegativeSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	double BackBarDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_Buff*> BuffEntries;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Bar Sizing")
	double BaseWidthHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Bar Sizing")
	double BaseWidthFp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Bar Sizing")
	double BaseWidthStamina;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Resources")
	void AdjustBarWidth(const FStatInfo& InStat);
	virtual void AdjustBarWidth_Implementation(const FStatInfo& InStat);


	// Event Handlers (7 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Resources")
	void EventAddBuff(UPDA_Buff* Buff);
	virtual void EventAddBuff_Implementation(UPDA_Buff* Buff);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Resources")
	void EventLerpFocusPositive();
	virtual void EventLerpFocusPositive_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Resources")
	void EventLerpHealthPositive();
	virtual void EventLerpHealthPositive_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Resources")
	void EventLerpStaminaPositive();
	virtual void EventLerpStaminaPositive_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Resources")
	void EventRemoveBuff(UPDA_Buff* Buff);
	virtual void EventRemoveBuff_Implementation(UPDA_Buff* Buff);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Resources")
	void EventTimerTick();
	virtual void EventTimerTick_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Resources")
	void EventUpdateStat(FStatInfo InStat);
	virtual void EventUpdateStat_Implementation(FStatInfo InStat);

protected:
	// Cache references
	void CacheWidgetReferences();
};
