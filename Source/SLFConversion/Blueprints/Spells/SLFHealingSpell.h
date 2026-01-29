// SLFHealingSpell.h
// Healing spell effect - rising particles and soft green glow

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFHealingSpell.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;
class USceneComponent;

/**
 * Healing spell effect
 * Rising particles with soft green/gold glow
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFHealingSpell : public AActor
{
	GENERATED_BODY()

public:
	ASLFHealingSpell();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	// Rising particles (multiple spheres)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Particle1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Particle2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Particle3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Particle4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Particle5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Particle6;

	// Central glow orb
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GlowOrb;

	// Healing light
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* HealingLight;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Healing")
	float EffectRadius = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Healing")
	float RiseSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Healing")
	float RiseHeight = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Healing")
	FLinearColor HealColor = FLinearColor(0.3f, 1.0f, 0.4f, 1.0f);  // Soft green

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Healing")
	FLinearColor AccentColor = FLinearColor(1.0f, 0.95f, 0.6f, 1.0f);  // Gold accent

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Healing")
	float Duration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float HealAmount = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 20.0f;

	// Owner to follow
	UPROPERTY(BlueprintReadWrite, Category = "Spell")
	AActor* HealOwner;

protected:
	UPROPERTY(Transient)
	TArray<UMaterialInstanceDynamic*> ParticleMaterials;

	UPROPERTY(Transient)
	TArray<UStaticMeshComponent*> Particles;

	UPROPERTY(Transient)
	TArray<float> ParticlePhases;

	float AccumulatedTime = 0.0f;
	float LifeTime = 0.0f;

	void SetupHealingEffect();
	void UpdateHealingEffect(float DeltaTime);
};
