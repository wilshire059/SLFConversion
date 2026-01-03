// SLFStatusEffectBuildup.h
// C++ base for B_StatusEffectBuildup - Single-target status effect applicator
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFStatusEffectBuildup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuildupApplied, AActor*, Target, float, BuildupAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusTriggered, AActor*, Target);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFStatusEffectBuildup : public AActor
{
	GENERATED_BODY()

public:
	ASLFStatusEffectBuildup();

	// ============================================================
	// MIGRATION SUMMARY: B_StatusEffectBuildup
	// Variables: 6 | Functions: 3 | Dispatchers: 2
	// ============================================================

	// Status Effect Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	FGameplayTag StatusEffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	float BuildupAmount = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	AActor* SourceActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	AActor* TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	bool bApplyOnSpawn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	bool bDestroyAfterApply = true;

	// Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Status Effect|Events")
	FOnBuildupApplied OnBuildupApplied;

	UPROPERTY(BlueprintAssignable, Category = "Status Effect|Events")
	FOnStatusTriggered OnStatusTriggered;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect")
	void ApplyBuildup(AActor* Target);
	virtual void ApplyBuildup_Implementation(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect")
	void SetTarget(AActor* NewTarget);
	virtual void SetTarget_Implementation(AActor* NewTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect")
	void OnStatusEffectTriggered(AActor* AffectedActor);
	virtual void OnStatusEffectTriggered_Implementation(AActor* AffectedActor);

protected:
	virtual void BeginPlay() override;
};
