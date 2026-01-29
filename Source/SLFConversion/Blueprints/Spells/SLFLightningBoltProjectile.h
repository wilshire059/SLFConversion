// SLFLightningBoltProjectile.h
// Lightning spell with bright flashing yellow/white electric effect

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFProjectileBase.h"
#include "SLFLightningBoltProjectile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;

/**
 * Lightning Bolt spell projectile
 * Yellow/white with bright flashing and electric sparks
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFLightningBoltProjectile : public ASLFProjectileBase
{
	GENERATED_BODY()

public:
	ASLFLightningBoltProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Electric core
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BoltCore;

	// Electric arc meshes (multiple for jagged look)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Arc1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Arc2;

	// Bright flash light
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* ElectricLight;

	// Visual Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lightning")
	float CoreRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lightning")
	FLinearColor LightningColor = FLinearColor(1.0f, 1.0f, 0.4f, 1.0f);  // Bright yellow

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lightning")
	FLinearColor ArcColor = FLinearColor(0.8f, 0.9f, 1.0f, 1.0f);  // Blue-white

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lightning")
	float FlashRate = 25.0f;  // Very fast flashing

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lightning")
	float ArcJitterAmount = 10.0f;

	// Spell Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 14.0f;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CoreMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* ArcMaterial1;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* ArcMaterial2;

	float AccumulatedTime = 0.0f;
	float NextFlashTime = 0.0f;
	bool bFlashOn = true;

	void SetupLightningEffect();
	void UpdateElectricEffect(float DeltaTime);
};
