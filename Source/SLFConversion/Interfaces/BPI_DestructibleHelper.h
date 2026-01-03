// BPI_DestructibleHelper.h
// C++ Interface for BPI_DestructibleHelper
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_DestructibleHelper.json
// Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_DestructibleHelper.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_DestructibleHelper : public UInterface
{
	GENERATED_BODY()
};

/**
 * Destructible Helper Interface
 * Controls Chaos destruction enable/disable
 */
class SLFCONVERSION_API IBPI_DestructibleHelper
{
	GENERATED_BODY()

public:
	// Enable Chaos destruction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Destructible Helper")
	void EnableChaosDestroy();

	// Disable Chaos destruction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Destructible Helper")
	void DisableChaosDestroy();
};
