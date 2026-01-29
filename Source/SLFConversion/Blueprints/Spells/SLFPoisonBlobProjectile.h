// SLFPoisonBlobProjectile.h
// Poison spell with bubbling green toxic effect

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFProjectileBase.h"
#include "SLFPoisonBlobProjectile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;

/**
 * Poison Blob spell projectile
 * Green bubbling toxic mass with dripping effect
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFPoisonBlobProjectile : public ASLFProjectileBase
{
	GENERATED_BODY()

public:
	ASLFPoisonBlobProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Main toxic blob
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ToxicCore;

	// Bubble meshes (multiple small spheres)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Bubble1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Bubble2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Bubble3;

	// Toxic glow
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* ToxicLight;

	// Visual Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Poison")
	float BlobRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Poison")
	FLinearColor PoisonGreen = FLinearColor(0.2f, 0.9f, 0.1f, 1.0f);  // Toxic green

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Poison")
	FLinearColor DarkGreen = FLinearColor(0.1f, 0.4f, 0.05f, 1.0f);  // Dark sickly green

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Poison")
	float BubbleSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Poison")
	float WobbleAmount = 0.15f;

	// Spell Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 8.0f;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CoreMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* BubbleMaterial;

	float AccumulatedTime = 0.0f;
	FVector Bubble1BasePos;
	FVector Bubble2BasePos;
	FVector Bubble3BasePos;

	void SetupPoisonEffect();
	void UpdateBubblingEffect(float DeltaTime);
};
