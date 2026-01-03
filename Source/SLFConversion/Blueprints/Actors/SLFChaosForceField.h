// SLFChaosForceField.h
// C++ base for B_Chaos_ForceField - Chaos magic force field effect
#pragma once

#include "CoreMinimal.h"
#include "SLFAbilityEffectBase.h"
#include "SLFChaosForceField.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFChaosForceField : public ASLFAbilityEffectBase
{
	GENERATED_BODY()

public:
	ASLFChaosForceField();

	// ============================================================
	// MIGRATION SUMMARY: B_Chaos_ForceField
	// Variables: 6 | Functions: 3 | Dispatchers: 0
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* ForceFieldCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ForceFieldMesh;

	// Force Field Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force Field")
	float Radius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force Field")
	float PushForce = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force Field")
	float DamagePerSecond = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force Field")
	bool bBlocksProjectiles = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force Field")
	float RotationSpeed = 30.0f;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Force Field")
	void OnActorEnterField(AActor* OtherActor);
	virtual void OnActorEnterField_Implementation(AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Force Field")
	void PushActorAway(AActor* ActorToPush);
	virtual void PushActorAway_Implementation(AActor* ActorToPush);

	virtual void OnEffectTick_Implementation(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<AActor*> ActorsInField;
};
