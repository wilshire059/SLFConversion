// SLFStatusEffectOneShot.h
// C++ base for B_StatusEffectOneShot - Instant status effect application
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFStatusEffectOneShot.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFStatusEffectOneShot : public AActor
{
	GENERATED_BODY()

public:
	ASLFStatusEffectOneShot();

	// ============================================================
	// MIGRATION SUMMARY: B_StatusEffectOneShot
	// Variables: 5 | Functions: 2 | Dispatchers: 0
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* TriggerSphere;

	// Status Effect Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	FGameplayTag StatusEffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	float InstantBuildupAmount = 100.0f;  // Usually enough to trigger

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	float TriggerRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	bool bDestroyOnTrigger = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	bool bHasTriggered = false;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect")
	void OnActorOverlap(AActor* OtherActor);
	virtual void OnActorOverlap_Implementation(AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect")
	void TriggerEffect(AActor* Target);
	virtual void TriggerEffect_Implementation(AActor* Target);

protected:
	virtual void BeginPlay() override;
};
