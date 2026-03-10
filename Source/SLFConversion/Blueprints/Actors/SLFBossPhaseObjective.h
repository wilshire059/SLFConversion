// SLFBossPhaseObjective.h
// Destructible/interactable objective for boss immunity phases
// When all objectives in a phase are completed, boss immunity drops

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "SLFBossPhaseObjective.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveDestroyed, ASLFBossPhaseObjective*, Objective);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFBossPhaseObjective : public AActor
{
	GENERATED_BODY()

public:
	ASLFBossPhaseObjective();

	/** Visual mesh (crystal, pillar, orb, etc.) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ObjectiveMesh;

	/** Interaction/damage sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* InteractionSphere;

	/** HP of this objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	float Health = 100.0f;

	/** Max HP */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	float MaxHealth = 100.0f;

	/** Whether this objective is destroyed */
	UPROPERTY(BlueprintReadOnly, Category = "Boss Phase")
	bool bIsDestroyed = false;

	/** Tag linking to boss encounter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	FName BossEncounterTag;

	/** Phase this objective belongs to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	int32 PhaseIndex = 0;

	/** Fired when this objective is destroyed */
	UPROPERTY(BlueprintAssignable, Category = "Boss Phase|Events")
	FOnObjectiveDestroyed OnObjectiveDestroyed;

	/** Apply damage to the objective */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	/** Destroy the objective */
	UFUNCTION(BlueprintCallable, Category = "Boss Phase")
	void DestroyObjective();

protected:
	virtual void BeginPlay() override;
};
