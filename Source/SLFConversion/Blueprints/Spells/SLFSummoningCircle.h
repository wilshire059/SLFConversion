// SLFSummoningCircle.h
// Summoning circle spell - ground decal with rising magical energy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFSummoningCircle.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;
class USceneComponent;

/**
 * Summoning Circle spell effect
 * Ground-based magical circle with runes and rising energy
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFSummoningCircle : public AActor
{
	GENERATED_BODY()

public:
	ASLFSummoningCircle();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	// Outer circle ring
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* OuterRing;

	// Inner circle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* InnerRing;

	// Center pentagram/symbol (flat cube)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CenterSymbol;

	// Rotating rune markers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Rune1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Rune2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Rune3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Rune4;

	// Rising energy pillar
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* EnergyPillar;

	// Circle lights
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* CircleLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* PillarLight;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Summon")
	float CircleRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Summon")
	FLinearColor CircleColor = FLinearColor(0.8f, 0.2f, 0.9f, 1.0f);  // Magenta/purple

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Summon")
	FLinearColor EnergyColor = FLinearColor(1.0f, 0.5f, 1.0f, 1.0f);  // Bright pink

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Summon")
	float RotationSpeed = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Summon")
	float PulseSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Summon")
	float Duration = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 35.0f;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* OuterMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* InnerMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* EnergyMaterial;

	float AccumulatedTime = 0.0f;
	float LifeTime = 0.0f;

	void SetupCircleEffect();
	void UpdateCircleEffect(float DeltaTime);
};
