// SLFIceShardProjectile.h
// Ice spell projectile with crystalline blue/white visuals and frost trail

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFProjectileBase.h"
#include "SLFIceShardProjectile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;

/**
 * Ice Shard spell projectile
 * Blue/white crystalline shard with frost particles
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFIceShardProjectile : public ASLFProjectileBase
{
	GENERATED_BODY()

public:
	ASLFIceShardProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Main ice crystal (elongated)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* IceCrystal;

	// Inner glow core
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FrostCore;

	// Frost light
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* IceLight;

	// Visual Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Ice")
	float CrystalLength = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Ice")
	float CrystalWidth = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Ice")
	FLinearColor IceColor = FLinearColor(0.3f, 0.7f, 1.0f, 1.0f);  // Light blue

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Ice")
	FLinearColor FrostColor = FLinearColor(0.9f, 0.95f, 1.0f, 1.0f);  // Near white

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Ice")
	float ShimmerSpeed = 8.0f;

	// Spell Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 10.0f;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CrystalMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* FrostMaterial;

	float AccumulatedTime = 0.0f;

	void SetupIceEffect();
	void UpdateShimmerEffect(float DeltaTime);
};
