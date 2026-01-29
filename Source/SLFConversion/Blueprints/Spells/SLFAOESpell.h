// SLFAOESpell.h
// Area of Effect spell - expanding ring/explosion effect

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFAOESpell.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;
class USceneComponent;

/**
 * AOE spell effect
 * Expanding ring/explosion that damages enemies in radius
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFAOESpell : public AActor
{
	GENERATED_BODY()

public:
	ASLFAOESpell();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	// Expanding ring
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ExpansionRing;

	// Inner explosion sphere
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ExplosionCore;

	// Second ring (slightly delayed)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SecondRing;

	// Center light
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* ExplosionLight;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|AOE")
	float StartRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|AOE")
	float MaxRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|AOE")
	float ExpansionTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|AOE")
	float FadeTime = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|AOE")
	FLinearColor AOEColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f);  // Orange explosion

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float Damage = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost = 25.0f;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* RingMaterial;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CoreMaterial;

	float LifeTime = 0.0f;
	float CurrentRadius = 0.0f;
	bool bHasDealtDamage = false;

	void SetupAOEEffect();
	void UpdateExpansion(float DeltaTime);
};
