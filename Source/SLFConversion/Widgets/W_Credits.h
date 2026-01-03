// W_Credits.h
// C++ Widget class for W_Credits
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Credits.json
// Parent: UW_Navigable
// Variables: 5 | Functions: 3 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_Credits.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Credits_OnCreditsClosed);

UCLASS()
class SLFCONVERSION_API UW_Credits : public UW_Navigable
{
	GENERATED_BODY()

public:
	UW_Credits(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* CreditsAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double ScrollSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool AutoScroll;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double AutoScrollResetDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double DelayAfterFinish;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Credits_OnCreditsClosed OnCreditsClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void SetupEntries(const TArray<FSLFCreditsEntry>& EntryArray);
	virtual void SetupEntries_Implementation(const TArray<FSLFCreditsEntry>& EntryArray);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void SetupExtraData(const TArray<FSLFCreditsExtra>& ExtraDataArray);
	virtual void SetupExtraData_Implementation(const TArray<FSLFCreditsExtra>& ExtraDataArray);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	TArray<FSLFCreditsExtra> SortExtraDataByScore(const TArray<FSLFCreditsExtra>& TargetArray);
	virtual TArray<FSLFCreditsExtra> SortExtraDataByScore_Implementation(const TArray<FSLFCreditsExtra>& TargetArray);


	// Event Handlers (8 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void EventInitializeCredits();
	virtual void EventInitializeCredits_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void EventOnCreditsFinished();
	virtual void EventOnCreditsFinished_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void EventResetAndExit();
	virtual void EventResetAndExit_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void EventResetAutoFinish();
	virtual void EventResetAutoFinish_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Credits")
	void EventRestartAutoScroll();
	virtual void EventRestartAutoScroll_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
