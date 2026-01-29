// SLFShieldSpell.h
// Protective shield/barrier spell - sphere around caster

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFShieldSpell.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;
class USceneComponent;

/**
 * Shield spell effect
 * Protective barrier sphere that surrounds the caster
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFShieldSpell : public AActor
{
	GENERATED_BODY()

public:
	ASLFShieldSpell();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	// Main shield sphere
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ShieldSphere;

	// Inner hexagon pattern layer
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* InnerShield;

	// Rotating rune ring
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RuneRing;

	// Shield glow light
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* ShieldLight;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Shield")
	float ShieldRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Shield")
	FLinearColor ShieldColor = FLinearColor(0.3f, 0.6f, 1.0f, 1.0f);  // Blue shield

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Shield")
	float PulseSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Shield")
	float RotationSpeed = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Shield")
	float Duration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float DamageReduction = 0.5f;  // 50% damage reduction

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 30.0f;

	// Owner to follow
	UPROPERTY(BlueprintReadWrite, Category = "Spell")
	AActor* ShieldOwner;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* ShieldMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* InnerMaterial;

	float AccumulatedTime = 0.0f;
	float LifeTime = 0.0f;

	void SetupShieldEffect();
	void UpdateShieldEffect(float DeltaTime);
};
