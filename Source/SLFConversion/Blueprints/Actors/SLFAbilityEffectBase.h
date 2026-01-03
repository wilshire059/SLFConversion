// SLFAbilityEffectBase.h
// C++ base for B_AbilityEffectBase - Spawnable ability visual effects
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFAbilityEffectBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEffectActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEffectDeactivated);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFAbilityEffectBase : public AActor
{
	GENERATED_BODY()

public:
	ASLFAbilityEffectBase();

	// ============================================================
	// MIGRATION SUMMARY: B_AbilityEffectBase
	// Variables: 8 | Functions: 5 | Dispatchers: 2
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* EffectParticles;

	// Effect Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	AActor* OwnerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float Duration = 0.0f;  // 0 = infinite

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bAutoActivate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bAttachToOwner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName AttachSocketName;

	// Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Sound")
	USoundBase* ActivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Sound")
	USoundBase* DeactivateSound;

	// Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Effect|Events")
	FOnEffectActivated OnEffectActivated;

	UPROPERTY(BlueprintAssignable, Category = "Effect|Events")
	FOnEffectDeactivated OnEffectDeactivated;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Effect")
	void ActivateEffect();
	virtual void ActivateEffect_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Effect")
	void DeactivateEffect();
	virtual void DeactivateEffect_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Effect")
	void SetOwnerActor(AActor* NewOwner);
	virtual void SetOwnerActor_Implementation(AActor* NewOwner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Effect")
	void OnEffectTick(float DeltaTime);
	virtual void OnEffectTick_Implementation(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Effect")
	bool IsEffectActive() const { return bIsActive; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	bool bIsActive = false;
	float ElapsedTime = 0.0f;
};
