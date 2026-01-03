// BPI_MainMenu.h
// C++ Interface for BPI_MainMenu
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_MainMenu.json
// Functions: 3

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/UMGSequencePlayMode.h"
#include "BPI_MainMenu.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_MainMenu : public UInterface
{
	GENERATED_BODY()
};

/**
 * Main Menu Interface
 * Provides menu button functionality
 */
class SLFCONVERSION_API IBPI_MainMenu
{
	GENERATED_BODY()

public:
	// Called when menu button is pressed
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Menu Interface")
	void OnMenuButtonPressed();

	// Play button initialization animation
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Menu Interface")
	void PlayButtonInitAnimation(double StartTime, int32 Loops, EUMGSequencePlayMode::Type Mode, double PlayRate);

	// Set menu button selected state
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Menu Interface")
	void SetMenuButtonSelected(bool bSelected);
};
