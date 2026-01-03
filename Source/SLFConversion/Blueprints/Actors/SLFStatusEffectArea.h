// SLFStatusEffectArea.h
// C++ base for B_StatusEffectArea - Area that applies status effects over time
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFStatusEffectArea.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFStatusEffectArea : public AActor
{
	GENERATED_BODY()

public:
	ASLFStatusEffectArea();

	// ============================================================
	// MIGRATION SUMMARY: B_StatusEffectArea
	// Variables: 7 | Functions: 3 | Dispatchers: 0
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* EffectVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* AreaEffect;

	// Status Effect Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	FGameplayTag StatusEffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	float BuildupPerSecond = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	bool bAffectsPlayers = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	bool bAffectsAI = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	float DamagePerSecond = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	bool bIsActive = true;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect")
	void OnActorEnter(AActor* OtherActor);
	virtual void OnActorEnter_Implementation(AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect")
	void OnActorExit(AActor* OtherActor);
	virtual void OnActorExit_Implementation(AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect")
	void ApplyEffectTick(AActor* TargetActor, float DeltaTime);
	virtual void ApplyEffectTick_Implementation(AActor* TargetActor, float DeltaTime);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TArray<AActor*> ActorsInArea;
};
