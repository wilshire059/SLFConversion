// SLFBeamSpell.h
// Continuous beam spell effect - a stream of energy from caster forward

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFBeamSpell.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;
class USceneComponent;

/**
 * Beam spell effect
 * Continuous stream of energy that extends forward from spawn point
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFBeamSpell : public AActor
{
	GENERATED_BODY()

public:
	ASLFBeamSpell();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	// Main beam cylinder
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BeamCore;

	// Outer glow cylinder
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BeamGlow;

	// Start point glow
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StartOrb;

	// End point impact
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* EndOrb;

	// Beam lights
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* StartLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* EndLight;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Beam")
	float BeamLength = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Beam")
	float BeamRadius = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Beam")
	float GlowRadius = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Beam")
	FLinearColor BeamColor = FLinearColor(0.2f, 0.8f, 1.0f, 1.0f);  // Cyan

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Beam")
	float PulseSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Beam")
	float Duration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float DamagePerSecond = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 20.0f;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CoreMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* GlowMaterial;

	float AccumulatedTime = 0.0f;
	float LifeTime = 0.0f;

	void SetupBeamEffect();
	void UpdateBeamEffect(float DeltaTime);
};
