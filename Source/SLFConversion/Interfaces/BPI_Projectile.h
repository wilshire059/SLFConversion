// BPI_Projectile.h
// C++ Interface for BPI_Projectile
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_Projectile.json
// Functions: 1

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_Projectile.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_Projectile : public UInterface
{
	GENERATED_BODY()
};

/**
 * Projectile Interface
 * Provides projectile initialization functionality
 */
class SLFCONVERSION_API IBPI_Projectile
{
	GENERATED_BODY()

public:
	// Initialize projectile with target actor
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile Interface")
	void InitializeProjectile(AActor* TargetActor);
};
