// BPI_AIC.h
// C++ Interface for BPI_AIC
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_AIC.json
// Functions: 1

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BPI_AIC.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_AIC : public UInterface
{
	GENERATED_BODY()
};

/**
 * AI Controller Interface
 * Provides behavior tree initialization functionality
 */
class SLFCONVERSION_API IBPI_AIC
{
	GENERATED_BODY()

public:
	// Initialize behavior tree for AI controller
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Controller Interface")
	void InitializeBehavior(UBehaviorTree* BehaviorTree);
};
