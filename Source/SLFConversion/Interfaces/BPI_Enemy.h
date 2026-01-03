// BPI_Enemy.h
// C++ Interface for BPI_Enemy
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_Enemy.json
// Functions: 8

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_Enemy.generated.h"

// Forward declaration
class AB_PatrolPath;

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_Enemy : public UInterface
{
	GENERATED_BODY()
};

/**
 * Enemy Interface
 * Provides enemy-specific functionality for AI characters
 */
class SLFCONVERSION_API IBPI_Enemy
{
	GENERATED_BODY()

public:
	// Get the execution move-to transform (location and rotation)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Interface")
	void GetExecutionMoveToTransform(FVector& MoveToLocation, FRotator& Rotation);

	// Stop rotating towards target
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Interface|Rotation")
	void StopRotateTowardsTarget();

	// Rotate towards target over duration
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Interface|Rotation")
	void RotateTowardsTarget(double Duration);

	// Pick and spawn loot on death
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Interface")
	void PickAndSpawnLoot();

	// Get the patrol path for this enemy
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Interface")
	void GetPatrolPath(AActor*& PatrolPath);

	// Display death VFX at attractor position
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Interface")
	void DisplayDeathVfx(FVector AttractorPosition);

	// Toggle healthbar visibility
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Interface|UI")
	void ToggleHealthbarVisual(bool bVisible);

	// Update enemy health display
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Interface|UI")
	void UpdateEnemyHealth(double CurrentValue, double MaxValue, double Change);
};
