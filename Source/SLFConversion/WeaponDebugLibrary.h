// WeaponDebugLibrary.h
// Debug functions to compare weapon transforms - SLFConversion version

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponDebugLibrary.generated.h"

UCLASS()
class SLFCONVERSION_API UWeaponDebugLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Log all weapons in PIE world and save to JSON automatically
	UFUNCTION(BlueprintCallable, Category = "Debug|Weapon")
	static void LogAllWeaponsInPIE();

	// Save weapon data to specific JSON path
	UFUNCTION(BlueprintCallable, Category = "Debug|Weapon")
	static void SaveWeaponDataToJSON(const FString& OutputPath);
};
