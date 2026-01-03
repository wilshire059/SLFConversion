// BPI_Executable.h
// C++ Interface for BPI_Executable
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_Executable.json
// Functions: 3

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "BPI_Executable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_Executable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Executable Interface
 * Handles execution/backstab events
 */
class SLFCONVERSION_API IBPI_Executable
{
	GENERATED_BODY()

public:
	// Called when execution starts
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Execute Interface")
	void OnExecutionStarted();

	// Called when executed
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Execute Interface")
	void OnExecuted(FGameplayTag ExecutionTag);

	// Called when backstabbed
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Execute Interface")
	void OnBackstabbed(FGameplayTag ExecutionTag);
};
