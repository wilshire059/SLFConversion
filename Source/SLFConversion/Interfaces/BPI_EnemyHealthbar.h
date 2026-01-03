// BPI_EnemyHealthbar.h
// C++ Interface for BPI_EnemyHealthbar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_EnemyHealthbar.json
// Functions: 1

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_EnemyHealthbar.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_EnemyHealthbar : public UInterface
{
	GENERATED_BODY()
};

/**
 * Enemy Healthbar Interface
 * Updates enemy healthbar display
 */
class SLFCONVERSION_API IBPI_EnemyHealthbar
{
	GENERATED_BODY()

public:
	// Update enemy healthbar with current health values
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Healthbar")
	void UpdateEnemyHealthbar(double CurrentHealth, double MaxHealth, double Change);
};
