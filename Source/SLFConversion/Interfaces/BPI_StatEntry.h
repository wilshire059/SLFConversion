// BPI_StatEntry.h
// C++ Interface for BPI_StatEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_StatEntry.json
// Functions: 1

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_StatEntry.generated.h"

// Forward declaration
class UUserWidget;

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_StatEntry : public UInterface
{
	GENERATED_BODY()
};

/**
 * Stat Entry Interface
 * Provides stat entry widget getter
 */
class SLFCONVERSION_API IBPI_StatEntry
{
	GENERATED_BODY()

public:
	// Get the stat entry widget
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Entry Interface")
	void GetStatEntry(UUserWidget*& Entry);
};
