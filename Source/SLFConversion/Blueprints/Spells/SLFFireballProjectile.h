// SLFFireballProjectile.h
// Fire spell projectile with flickering orange/red flames and ember particles

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFProjectileBase.h"
#include "SLFFireballProjectile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;

/**
 * Fireball spell projectile
 * Orange/red core with flickering flames and trailing embers
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFFireballProjectile : public ASLFProjectileBase
{
	GENERATED_BODY()

public:
	ASLFFireballProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FireCore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FlameOuter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* FireLight;

	// Visual Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Fire")
	float CoreRadius = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Fire")
	float FlameRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Fire")
	FLinearColor CoreColor = FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);  // Bright yellow-orange

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Fire")
	FLinearColor FlameColor = FLinearColor(1.0f, 0.3f, 0.0f, 1.0f);  // Deep orange-red

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Fire")
	float FlickerSpeed = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Fire")
	float FlickerIntensity = 0.4f;

	// Spell Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 12.0f;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CoreMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* FlameMaterial;

	float AccumulatedTime = 0.0f;

	void SetupFireEffect();
	void UpdateFlickerEffect(float DeltaTime);
};
