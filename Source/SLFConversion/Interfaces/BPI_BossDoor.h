// BPI_BossDoor.h
// C++ Interface for BPI_BossDoor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_BossDoor.json
// Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SceneComponent.h"
#include "BPI_BossDoor.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_BossDoor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Boss Door Interface
 * Controls boss door unlocking and death currency spawn points
 */
class SLFCONVERSION_API IBPI_BossDoor
{
	GENERATED_BODY()

public:
	// Unlock the boss door
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door Interface")
	void UnlockBossDoor();

	// Get the spawn point for death currency
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door Interface")
	void GetDeathCurrencySpawnPoint(USceneComponent*& SpawnPoint);
};
