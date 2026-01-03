// SLFDeathTrigger.h
// C++ base for B_DeathTrigger - Kill volume / instant death zone
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFDeathTrigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorKilled, AActor*, KilledActor);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFDeathTrigger : public AActor
{
	GENERATED_BODY()

public:
	ASLFDeathTrigger();

	// ============================================================
	// MIGRATION SUMMARY: B_DeathTrigger
	// Variables: 4 | Functions: 2 | Dispatchers: 1
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* DeathTrigger;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death Trigger")
	bool bAffectsPlayers = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death Trigger")
	bool bAffectsAI = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death Trigger")
	float DamageAmount = 99999.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death Trigger")
	TSubclassOf<UDamageType> DamageType;

	// Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Death Trigger|Events")
	FOnActorKilled OnActorKilled;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Death Trigger")
	void OnActorEnter(AActor* OtherActor);
	virtual void OnActorEnter_Implementation(AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Death Trigger")
	void KillActor(AActor* ActorToKill);
	virtual void KillActor_Implementation(AActor* ActorToKill);

protected:
	virtual void BeginPlay() override;
};
