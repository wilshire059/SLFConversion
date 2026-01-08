// AC_ProgressManager.h
// C++ component for AC_ProgressManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_ProgressManager.json
// Variables: 4 | Functions: 5 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "Curves/CurveFloat.h"
#include "AC_ProgressManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_ProgressManager_OnPlayTimeUpdated, FTimespan, NewTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAC_ProgressManager_OnSaveRequested, FGameplayTag, SaveDataTag, TArray<FInstancedStruct>, ProgressData);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_ProgressManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_ProgressManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	/** Current progress states - maps progress tag to progress state */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, ESLFProgress> CurrentProgress;

	/** Curve defining level up costs - X=level, Y=currency required */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	UCurveFloat* LevelUpCosts;

	/** Default progress states to initialize with */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, ESLFProgress> DefaultsToProgress;

	/** Total play time for this save */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimespan PlayTime;

	/** Timer handle for play time tracking */
	UPROPERTY()
	FTimerHandle PlayTimeTimer;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_ProgressManager_OnPlayTimeUpdated OnPlayTimeUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_ProgressManager_OnSaveRequested OnSaveRequested;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (5)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ProgressManager")
	void SetProgress(const FGameplayTag& ProgressTag, ESLFProgress State);
	virtual void SetProgress_Implementation(const FGameplayTag& ProgressTag, ESLFProgress State);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ProgressManager", meta=(DisplayName="GetProgress"))
	void GetProgress(const FGameplayTag& ProgressTag, UPARAM(DisplayName="Found?") bool& Found, ESLFProgress& State);
	virtual void GetProgress_Implementation(const FGameplayTag& ProgressTag, bool& Found, ESLFProgress& State);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ProgressManager")
	void ExecuteGameplayEvents(const TArray<FSLFDialogGameplayEvent>& GameplayEvents);
	virtual void ExecuteGameplayEvents_Implementation(const TArray<FSLFDialogGameplayEvent>& GameplayEvents);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ProgressManager")
	int32 GetRequiredCurrencyForLevel(int32 Level);
	virtual int32 GetRequiredCurrencyForLevel_Implementation(int32 Level);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ProgressManager")
	void SerializeProgressData();
	virtual void SerializeProgressData_Implementation();

	/** Initialize progress from loaded save data */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ProgressManager")
	void InitializeLoadedProgress(const TArray<FSLFProgressSaveInfo>& LoadedProgress);
	virtual void InitializeLoadedProgress_Implementation(const TArray<FSLFProgressSaveInfo>& LoadedProgress);

	/** Set play time from loaded save */
	UFUNCTION(BlueprintCallable, Category = "AC_ProgressManager")
	void SetPlayTime(const FTimespan& NewPlayTime);

	/** Get current play time */
	UFUNCTION(BlueprintCallable, Category = "AC_ProgressManager")
	FTimespan GetPlayTime() const { return PlayTime; }

	// Internal helpers
	void StartPlayTimeTimer();
	void UpdatePlayTime();
};
