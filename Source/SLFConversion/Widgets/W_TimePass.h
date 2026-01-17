// W_TimePass.h
// C++ Widget class for W_TimePass
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_TimePass.json
// Parent: UUserWidget
// Variables: 5 | Functions: 0 | Dispatchers: 1

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


#include "W_TimePass.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_TimePass_OnTimePassEnd);

UCLASS()
class SLFCONVERSION_API UW_TimePass : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_TimePass(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double FromTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double ToTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTimerHandle Timer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double FromAngle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double ToAngle;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_TimePass_OnTimePassEnd OnTimePassEnd;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_TimePass")
	void EventAnimate();
	virtual void EventAnimate_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_TimePass")
	void EventInitialize(double InFromTime, double InToTime);
	virtual void EventInitialize_Implementation(double InFromTime, double InToTime);

protected:
	// Cache references
	void CacheWidgetReferences();

	// Timer tick for clock animation
	void AnimationTick();

	// Cached widget references (NOT UPROPERTY to avoid Blueprint conflicts)
	class UWidget* ClockInnerWidget;

	// Current angle during animation
	double CurrentAngle;

	// Angle increment per tick
	double AngleIncrement;
};
