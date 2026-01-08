// AC_StatusEffectManager.h
// C++ component for AC_StatusEffectManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_StatusEffectManager.json
// Variables: 1 | Functions: 4 | Dispatchers: 3

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_StatusEffectManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_StatusEffectManager_OnStatusEffectAdded, UB_StatusEffect*, StatusEffect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_StatusEffectManager_OnStatusEffectRemoved, UPrimaryDataAsset*, StatusEffect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_StatusEffectManager_OnStatusEffectTriggered, FText, TriggeredText);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_StatusEffectManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_StatusEffectManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// Map of status effect tag -> B_StatusEffect instance
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UB_StatusEffect*> ActiveStatusEffects;

	// Legacy map for backwards compatibility (tag -> tag)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FGameplayTag> ActiveStatusEffectTags;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_StatusEffectManager_OnStatusEffectAdded OnStatusEffectAdded;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_StatusEffectManager_OnStatusEffectRemoved OnStatusEffectRemoved;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_StatusEffectManager_OnStatusEffectTriggered OnStatusEffectTriggered;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatusEffectManager")
	void StartBuildup(UPrimaryDataAsset* StatusEffect, int32 Rank);
	virtual void StartBuildup_Implementation(UPrimaryDataAsset* StatusEffect, int32 Rank);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatusEffectManager")
	void StopBuildup(UPrimaryDataAsset* StatusEffect, bool ApplyDecayDelay);
	virtual void StopBuildup_Implementation(UPrimaryDataAsset* StatusEffect, bool ApplyDecayDelay);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatusEffectManager")
	void AddOneShotBuildup(UPrimaryDataAsset* StatusEffect, int32 EffectRank, double Delta);
	virtual void AddOneShotBuildup_Implementation(UPrimaryDataAsset* StatusEffect, int32 EffectRank, double Delta);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatusEffectManager")
	bool IsStatusEffectActive(const FGameplayTag& StatusEffectTag);
	virtual bool IsStatusEffectActive_Implementation(const FGameplayTag& StatusEffectTag);

	// Get a specific status effect by tag
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatusEffectManager")
	UB_StatusEffect* GetStatusEffect(const FGameplayTag& StatusEffectTag);
	virtual UB_StatusEffect* GetStatusEffect_Implementation(const FGameplayTag& StatusEffectTag);

	// Get all active status effects
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_StatusEffectManager")
	TArray<UB_StatusEffect*> GetAllActiveStatusEffects();
	virtual TArray<UB_StatusEffect*> GetAllActiveStatusEffects_Implementation();

protected:
	// Internal event handlers for status effect events
	UFUNCTION()
	void HandleStatusEffectTriggered(FText TriggeredText);

	UFUNCTION()
	void HandleStatusEffectFinished(FGameplayTag StatusEffectTag);
};
