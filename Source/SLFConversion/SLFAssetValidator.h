// SLFAssetValidator.h
// C++ utility to export and validate all asset properties between bp_only and SLFConversion

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLFAssetValidator.generated.h"

/**
 * Asset validation utility for comparing bp_only vs SLFConversion assets
 * Exports ALL properties, components, and settings for line-by-line comparison
 */
UCLASS()
class SLFCONVERSION_API USLFAssetValidator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Export all properties of a Blueprint asset to a text file
	 * Includes: Variables, Components, Default values, References
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Validation")
	static bool ExportBlueprintProperties(const FString& BlueprintPath, const FString& OutputPath);

	/**
	 * Export all properties of a Widget Blueprint
	 * Includes: Widget tree, bindings, animations, variables
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Validation")
	static bool ExportWidgetProperties(const FString& WidgetPath, const FString& OutputPath);

	/**
	 * Export all properties of a Data Asset
	 * Includes: All UPROPERTY values
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Validation")
	static bool ExportDataAssetProperties(const FString& AssetPath, const FString& OutputPath);

	/**
	 * Export all properties of a DataTable
	 * Includes: Row struct, all rows with all column values
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Validation")
	static bool ExportDataTableProperties(const FString& TablePath, const FString& OutputPath);

	/**
	 * Compare two export files and generate a diff report
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Validation")
	static bool CompareExports(const FString& File1Path, const FString& File2Path, const FString& DiffOutputPath);

	/**
	 * Batch export all dialog-related assets
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Validation")
	static int32 ExportAllDialogAssets(const FString& OutputFolder);

	/**
	 * Export a single object's properties recursively
	 */
	static void ExportObjectProperties(UObject* Object, FString& OutText, int32 IndentLevel = 0);

	/**
	 * Export struct properties
	 */
	static void ExportStructProperties(const void* StructData, const UStruct* StructType, FString& OutText, int32 IndentLevel);
};
