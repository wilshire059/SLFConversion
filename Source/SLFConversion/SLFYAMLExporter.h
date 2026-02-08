// SLFYAMLExporter.h
// Comprehensive YAML exporter for complete asset documentation
// Exports EVERY property, component, setting, variable, and reference

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLFYAMLExporter.generated.h"

/**
 * YAML Exporter for complete asset documentation
 * Outputs machine-readable YAML with every single property
 */
UCLASS()
class SLFCONVERSION_API USLFYAMLExporter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════
	// MAIN EXPORT FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════

	/** Export a Blueprint to YAML with ALL properties, variables, components */
	UFUNCTION(BlueprintCallable, Category = "SLF|YAML")
	static bool ExportBlueprintToYAML(const FString& BlueprintPath, const FString& OutputPath);

	/** Export a Widget Blueprint to YAML with complete widget tree */
	UFUNCTION(BlueprintCallable, Category = "SLF|YAML")
	static bool ExportWidgetToYAML(const FString& WidgetPath, const FString& OutputPath);

	/** Export a DataTable to YAML with all rows and columns */
	UFUNCTION(BlueprintCallable, Category = "SLF|YAML")
	static bool ExportDataTableToYAML(const FString& TablePath, const FString& OutputPath);

	/** Export a Data Asset to YAML with all properties */
	UFUNCTION(BlueprintCallable, Category = "SLF|YAML")
	static bool ExportDataAssetToYAML(const FString& AssetPath, const FString& OutputPath);

	/** Export a struct definition to YAML */
	UFUNCTION(BlueprintCallable, Category = "SLF|YAML")
	static bool ExportStructToYAML(const FString& StructPath, const FString& OutputPath);

	// ═══════════════════════════════════════════════════════════════════
	// BATCH EXPORT FOR NPC SYSTEM
	// ═══════════════════════════════════════════════════════════════════

	/** Export all NPC-related assets to YAML files */
	UFUNCTION(BlueprintCallable, Category = "SLF|YAML")
	static int32 ExportNPCSystemToYAML(const FString& OutputFolder);

	// ═══════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════

	/** Convert any UObject property to YAML string */
	static void ObjectToYAML(UObject* Object, FString& OutYAML, int32 IndentLevel = 0);

	/** Convert struct data to YAML string */
	static void StructToYAML(const void* StructData, const UStruct* StructType, FString& OutYAML, int32 IndentLevel);

	/** Convert a single property to YAML string */
	static void PropertyToYAML(FProperty* Property, const void* ValuePtr, FString& OutYAML, int32 IndentLevel);

	/** Get YAML-safe indent string */
	static FString GetIndent(int32 Level);

	/** Escape YAML special characters in string */
	static FString EscapeYAMLString(const FString& Input);

	/** Convert FText to readable string */
	static FString FTextToString(const FText& Text);
};
