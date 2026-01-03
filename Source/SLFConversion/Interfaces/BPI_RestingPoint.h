// BPI_RestingPoint.h
// C++ Interface for BPI_RestingPoint
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_RestingPoint.json
// Functions: 1

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_RestingPoint.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_RestingPoint : public UInterface
{
	GENERATED_BODY()
};

/**
 * Resting Point Interface
 * Provides spawn position functionality for resting points
 */
class SLFCONVERSION_API IBPI_RestingPoint
{
	GENERATED_BODY()

public:
	// Get the spawn position for this resting point
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resting Point Interface")
	void GetRestingPointSpawnPosition(bool& bSuccess, FVector& Location, FRotator& Rotation);
};
