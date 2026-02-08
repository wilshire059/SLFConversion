// AC_AI_InteractionManager.h
// C++ component for AC_AI_InteractionManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_AI_InteractionManager.json
// Variables: 7 | Functions: 1 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_AI_InteractionManager.generated.h"

// Forward declarations
class UAC_ProgressManager;
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class UW_HUD;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAC_AI_InteractionManager_OnDialogFinished);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_AI_InteractionManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_AI_InteractionManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* DialogAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_ProgressManager* ProgressManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	int32 CurrentIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	int32 MaxIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TSoftObjectPtr<UDataTable> ActiveTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* VendorAsset;

	// Cached HUD reference for dialog callbacks
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Runtime")
	UW_HUD* CachedHUD;

	/** Guard flag to prevent re-entry during dialog finish sequence */
	UPROPERTY(Transient)
	bool bIsFinishingDialog;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_AI_InteractionManager_OnDialogFinished OnDialogFinished;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_InteractionManager")
	FSLFDialogEntry GetCurrentDialogEntry(UDataTable* DataTable, const TArray<FName>& Rows);
	virtual FSLFDialogEntry GetCurrentDialogEntry_Implementation(UDataTable* DataTable, const TArray<FName>& Rows);

	/**
	 * Begin dialog with NPC
	 * @param InProgressManager - Progress manager for determining dialog table
	 * @param InHUD - HUD widget to display dialog
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_InteractionManager|Dialog")
	void EventBeginDialog(UAC_ProgressManager* InProgressManager, UW_HUD* InHUD);
	virtual void EventBeginDialog_Implementation(UAC_ProgressManager* InProgressManager, UW_HUD* InHUD);

	/**
	 * Advance dialog to next entry or finish dialog
	 * Called when player presses OK/interact during dialog
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_InteractionManager|Dialog")
	void EventAdjustIndexForExit();
	virtual void EventAdjustIndexForExit_Implementation();

	/**
	 * Reset dialog index to beginning
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_InteractionManager|Dialog")
	void EventResetDialogIndex();
	virtual void EventResetDialogIndex_Implementation();
};
