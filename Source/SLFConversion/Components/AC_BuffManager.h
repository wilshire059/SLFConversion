// AC_BuffManager.h
// C++ component for AC_BuffManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_BuffManager.json
// Variables: 2 | Functions: 5 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_BuffManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAC_BuffManager_OnBuffDetected, UPrimaryDataAsset*, Buff, bool, Added);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_BuffManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_BuffManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<UB_Buff*> ActiveBuffs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UB_Buff* Cache_Buff;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_BuffManager_OnBuffDetected OnBuffDetected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (7)
	// ═══════════════════════════════════════════════════════════════════════

	// TryAddBuff - Async loads and constructs a buff, grants it to the owner
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_BuffManager")
	void TryAddBuff(UPrimaryDataAsset* Buff, int32 Rank, bool IsLoading);
	virtual void TryAddBuff_Implementation(UPrimaryDataAsset* Buff, int32 Rank, bool IsLoading);

	// DelayedRemoveBuff - Removes a buff after a delay (for async loading race conditions)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_BuffManager")
	void DelayedRemoveBuff(float DelayDuration, UPrimaryDataAsset* Buff);
	virtual void DelayedRemoveBuff_Implementation(float DelayDuration, UPrimaryDataAsset* Buff);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_BuffManager")
	void RemoveBuffOfType(UPrimaryDataAsset* Buff);
	virtual void RemoveBuffOfType_Implementation(UPrimaryDataAsset* Buff);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_BuffManager")
	void RemoveAllBuffsOfType(UPrimaryDataAsset* Buff);
	virtual void RemoveAllBuffsOfType_Implementation(UPrimaryDataAsset* Buff);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_BuffManager")
	void RemoveBuffWithTag(const FGameplayTag& Tag);
	virtual void RemoveBuffWithTag_Implementation(const FGameplayTag& Tag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_BuffManager")
	void RemoveAllBuffsWithTag(const FGameplayTag& Tag);
	virtual void RemoveAllBuffsWithTag_Implementation(const FGameplayTag& Tag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_BuffManager")
	TArray<UB_Buff*> GetBuffsWithTag(const FGameplayTag& Tag);
	virtual TArray<UB_Buff*> GetBuffsWithTag_Implementation(const FGameplayTag& Tag);

private:
	// Internal helper for delayed removal
	void ExecuteDelayedRemove(UPrimaryDataAsset* Buff);
};
