// SLFHolyOrbProjectile.h
// Holy spell with golden white radiant glow and soft divine light

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFProjectileBase.h"
#include "SLFHolyOrbProjectile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;

/**
 * Holy Orb spell projectile
 * Golden white with radiant beams and soft divine glow
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFHolyOrbProjectile : public ASLFProjectileBase
{
	GENERATED_BODY()

public:
	ASLFHolyOrbProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Divine core
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* HolyCore;

	// Radiant halo
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Halo;

	// Divine light beams (cross pattern)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BeamVertical;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BeamHorizontal;

	// Warm divine light
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* DivineLight;

	// Visual Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Holy")
	float CoreRadius = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Holy")
	float HaloRadius = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Holy")
	FLinearColor GoldColor = FLinearColor(1.0f, 0.9f, 0.5f, 1.0f);  // Warm gold

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Holy")
	FLinearColor WhiteColor = FLinearColor(1.0f, 1.0f, 0.95f, 1.0f);  // Divine white

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Holy")
	float RadianceSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Holy")
	float BeamRotationSpeed = 45.0f;

	// Spell Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 18.0f;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CoreMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* HaloMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* BeamMaterial;

	float AccumulatedTime = 0.0f;

	void SetupHolyEffect();
	void UpdateRadianceEffect(float DeltaTime);
};
