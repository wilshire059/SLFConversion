// AC_LadderManager.h
// C++ component for AC_LadderManager
//
// 20-PASS VALIDATION: 2026-01-03 - UPDATED
// Source: BlueprintDNA/Component/AC_LadderManager.json
// Variables: 10 | Functions: 6 | Dispatchers: 0
//
// PASS 6-7: Added TryLoadClimbMontages and input handling

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_LadderManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations


UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_LadderManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_LadderManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (10)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsOnGround;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsClimbing;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsClimbingOffTop;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsClimbingDownFromTop;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsOrientedToLadder;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	ESLFLadderClimbState ClimbAnimState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	AB_Ladder* CurrentLadder;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* LadderAnimset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool ClimbFast;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	double ClimbSprintMultiplier;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (5)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_LadderManager")
	void SetIsOnGround(bool InIsOnGround);
	virtual void SetIsOnGround_Implementation(bool InIsOnGround);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_LadderManager")
	void SetIsClimbing(bool InIsClimbing);
	virtual void SetIsClimbing_Implementation(bool InIsClimbing);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_LadderManager")
	void SetIsClimbingOffTop(bool InIsClimbingOffTop);
	virtual void SetIsClimbingOffTop_Implementation(bool InIsClimbingOffTop);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_LadderManager")
	void SetIsClimbingDownFromTop(bool InIsClimbingDownFromTop);
	virtual void SetIsClimbingDownFromTop_Implementation(bool InIsClimbingDownFromTop);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_LadderManager")
	void SetCurrentLadder(AB_Ladder* InCurrentLadder);
	virtual void SetCurrentLadder_Implementation(AB_Ladder* InCurrentLadder);

	/**
	 * TryLoadClimbMontages - Async load ladder climb montages from animset
	 * Called when entering ladder to ensure animations are ready
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_LadderManager")
	void TryLoadClimbMontages();
	virtual void TryLoadClimbMontages_Implementation();

	/**
	 * HandleClimbInput - Process movement input while on ladder
	 * @param InputValue - 2D movement input vector
	 */
	UFUNCTION(BlueprintCallable, Category = "AC_LadderManager")
	void HandleClimbInput(const FVector2D& InputValue);

protected:
	/**
	 * OnClimbMontagesLoaded - Callback when climb montages finish loading
	 */
	void OnClimbMontagesLoaded();
};
