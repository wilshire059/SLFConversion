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
class UProgressBar;
class USizeBox;

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

	// Height settings for thin bars (matching bp_only visual)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Bar Sizing")
	double BaseHeightHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Bar Sizing")
	double BaseHeightFp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Bar Sizing")
	double BaseHeightStamina;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Resources")
	void AdjustBarWidth(FStatInfo Stat);
	virtual void AdjustBarWidth_Implementation(FStatInfo Stat);


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
	// Widget lifecycle - NativePreConstruct for initial bar sizing
	virtual void NativePreConstruct() override;

	// Cache references
	void CacheWidgetReferences();

	// Hide white separator lines between bars
	void HideBarSeparators();

	// Cached widget references for stat bars (no UPROPERTY to avoid Blueprint conflict)
	// NOTE: Widget names from Blueprint use lowercase 'b' (Focusbar, Staminabar)
	UProgressBar* CachedHealthBar_Front;
	UProgressBar* CachedHealthBar_Back;
	UProgressBar* CachedFocusBar_Front;
	UProgressBar* CachedFocusBar_Back;
	UProgressBar* CachedStaminaBar_Front;
	UProgressBar* CachedStaminaBar_Back;

	// SizeBox widgets that control bar widths (from Blueprint hierarchy)
	USizeBox* CachedHealthbarSizer;
	USizeBox* CachedFocusbarSizer;
	USizeBox* CachedStaminabarSizer;

	// Scaling factor for bar width calculation (800.0 from bp_only Blueprint)
	static constexpr double ScalingFactor = 800.0;
};
