// BPI_ExecutionIndicator.h
// C++ Interface for BPI_ExecutionIndicator
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_ExecutionIndicator.json
// Functions: 1

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_ExecutionIndicator.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_ExecutionIndicator : public UInterface
{
	GENERATED_BODY()
};

/**
 * Execution Indicator Interface
 * Controls execution icon visibility
 */
class SLFCONVERSION_API IBPI_ExecutionIndicator
{
	GENERATED_BODY()

public:
	// Toggle execution icon visibility
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Execution Widget Interface")
	void ToggleExecutionIcon(bool bVisible);
};
