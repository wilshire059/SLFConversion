// SLFArcaneMissileProjectile.h
// Arcane spell with purple/pink magical sparkles and mystical runes

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFProjectileBase.h"
#include "SLFArcaneMissileProjectile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;

/**
 * Arcane Missile spell projectile
 * Purple/pink with sparkles and rotating magical runes
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFArcaneMissileProjectile : public ASLFProjectileBase
{
	GENERATED_BODY()

public:
	ASLFArcaneMissileProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Arcane core
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ArcaneCore;

	// Outer magical aura
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MagicAura;

	// Rotating rune rings (flat cubes as placeholders for runes)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RuneRing1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RuneRing2;

	// Sparkle points (small spheres)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Sparkle1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Sparkle2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Sparkle3;

	// Magical light
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* ArcaneLight;

	// Visual Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Arcane")
	float CoreRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Arcane")
	FLinearColor ArcaneColor = FLinearColor(0.6f, 0.2f, 1.0f, 1.0f);  // Deep purple

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Arcane")
	FLinearColor SparkleColor = FLinearColor(1.0f, 0.5f, 1.0f, 1.0f);  // Pink

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Arcane")
	float RuneRotationSpeed = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Arcane")
	float SparkleSpeed = 10.0f;

	// Spell Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 10.0f;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CoreMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* AuraMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* RuneMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* SparkleMaterial;

	float AccumulatedTime = 0.0f;

	void SetupArcaneEffect();
	void UpdateMagicalEffect(float DeltaTime);
};
