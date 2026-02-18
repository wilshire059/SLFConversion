// PythonBridge.h - Comprehensive Blueprint Export and Migration Utilities
// Reconstructed with enhanced TMap key type support

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#if WITH_EDITOR
#include "WidgetBlueprint.h"
#endif
#include "PythonBridge.generated.h"

class UBlueprint;
class UAnimBlueprint;
class UUserDefinedEnum;
class UUserDefinedStruct;
class UDataAsset;

/**
 * Python Bridge - Exposes Blueprint internals to Python for migration workflows.
 *
 * Key Features:
 * - Complete Blueprint DNA export (variables, functions, graphs, dispatchers)
 * - TMap key type extraction (fixes the key type gap in standard exports)
 * - Blueprint manipulation (reparent, delete variables/functions, etc.)
 * - Asset dependency analysis
 *
 * Usage from Python:
 *   import unreal
 *   bp = unreal.load_asset("/Game/Path/To/Blueprint")
 *   json_str = unreal.PythonBridge.export_blueprint_dna(bp)
 */
UCLASS()
class SLFCONVERSION_API UPythonBridge : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	// ============================================================================
	// EXPORT FUNCTIONS - Convert assets to JSON
	// ============================================================================

	/**
	 * Export complete Blueprint "DNA" to JSON string.
	 * Includes: variables (with TMap key types!), functions, graphs, event dispatchers,
	 * interfaces, components, and all node/pin data.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Export")
	static FString ExportBlueprintDNA(UBlueprint* Blueprint);

	/**
	 * Export Widget Blueprint to JSON string.
	 * Includes widget hierarchy, bindings, and animations.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Export")
	static FString ExportWidgetBlueprintDNA(UObject* WidgetBlueprintObj);

	/**
	 * Export Animation Blueprint to JSON string.
	 * Includes state machines, blend spaces, and anim graph nodes.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Export")
	static FString ExportAnimBlueprintDNA(UAnimBlueprint* AnimBlueprint);

	/**
	 * Export UserDefinedEnum to JSON string.
	 * Includes all enum values and display names.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Export")
	static FString ExportUserDefinedEnum(UUserDefinedEnum* Enum);

	/**
	 * Export UserDefinedStruct to JSON string.
	 * Includes all properties with complete type info (including TMap key types).
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Export")
	static FString ExportUserDefinedStruct(UUserDefinedStruct* Struct);

	/**
	 * Export DataAsset to JSON string.
	 * Includes all UPROPERTY values.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Export")
	static FString ExportDataAsset(UDataAsset* DataAsset);

	/**
	 * Universal asset export - auto-detects type and calls appropriate export function.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Export")
	static FString ExportAssetDNA(UObject* Asset);

	// ============================================================================
	// BLUEPRINT MANIPULATION - Modify blueprints programmatically
	// ============================================================================

	/**
	 * Reparent a Blueprint to a new parent class.
	 * Use this to change AC_StatManager's parent from ActorComponent to USoulslikeStatComponent.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Manipulation")
	static bool ReparentBlueprint(UBlueprint* Blueprint, UClass* NewParentClass);

	/**
	 * Delete a Blueprint variable by name.
	 * Use after migrating variable to C++ base class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Manipulation")
	static bool DeleteBlueprintVariable(UBlueprint* Blueprint, FName VariableName);

	/**
	 * Delete a Blueprint function by name.
	 * Use after migrating function to C++ base class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Manipulation")
	static bool DeleteBlueprintFunction(UBlueprint* Blueprint, FName FunctionName);

	/**
	 * Delete a Blueprint component by name.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Manipulation")
	static bool DeleteBlueprintComponent(UBlueprint* Blueprint, FName ComponentName);

	/**
	 * Rename a Blueprint variable.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Manipulation")
	static bool RenameBlueprintVariable(UBlueprint* Blueprint, FName OldName, FName NewName);

	/**
	 * Rename a Blueprint component.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Manipulation")
	static bool RenameBlueprintComponent(UBlueprint* Blueprint, FName OldName, FName NewName);

	/**
	 * Remove an implemented interface from a Blueprint.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Manipulation")
	static bool RemoveBlueprintInterface(UBlueprint* Blueprint, UClass* InterfaceClass);

	/**
	 * Clear all nodes from the Event Graph (useful before migration).
	 * Returns number of nodes removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Manipulation")
	static int32 ClearEventGraph(UBlueprint* Blueprint);

	/**
	 * Prepare a Blueprint for reparenting by renaming conflicting variables.
	 * Returns array of variables that were renamed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Manipulation")
	static TArray<FString> PrepareBlueprintForReparenting(UBlueprint* Blueprint, UClass* NewParentClass);

	// ============================================================================
	// DEPENDENCY ANALYSIS - Find relationships between assets
	// ============================================================================

	/**
	 * Get all Blueprints that inherit from a given class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Dependencies")
	static TArray<UBlueprint*> GetChildBlueprints(UClass* ParentClass);

	/**
	 * Get all assets that this asset depends on.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Dependencies")
	static TArray<FString> GetAssetDependencies(UObject* Asset);

	/**
	 * Get all assets that reference this asset.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Dependencies")
	static TArray<FString> GetAssetReferencers(UObject* Asset);

	/**
	 * Get a comprehensive dependency report as JSON.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Dependencies")
	static FString GetAssetDependencyReport(UObject* Asset);

	// ============================================================================
	// WIDGET BLUEPRINT HELPERS
	// ============================================================================

	/**
	 * Get the widget tree from a Widget Blueprint.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Widget")
	static UObject* GetWidgetTreeFromBP(UObject* WidgetBlueprintObj);

	/**
	 * Get the root widget from a Widget Blueprint.
	 */
	UFUNCTION(BlueprintCallable, Category = "Python Bridge|Widget")
	static UObject* GetRootWidgetFromBP(UObject* WidgetBlueprintObj);

private:
	// Helper functions for JSON serialization
	static TSharedPtr<FJsonObject> SerializePropertyType(FProperty* Property);
	static TSharedPtr<FJsonObject> SerializeVariable(const FBPVariableDescription& Variable, UBlueprint* Blueprint);
	static TSharedPtr<FJsonObject> SerializeFunction(UFunction* Function, UBlueprint* Blueprint);
	static TSharedPtr<FJsonObject> SerializeGraph(UEdGraph* Graph);
	static TSharedPtr<FJsonObject> SerializeNode(UEdGraphNode* Node);
	static TSharedPtr<FJsonObject> SerializePin(UEdGraphPin* Pin);
	static TSharedPtr<FJsonObject> SerializeEventDispatcher(const FMulticastDelegateProperty* DelegateProperty);
	static FString GetPropertyCPPType(FProperty* Property);

	// TMap key type extraction helper
	static TSharedPtr<FJsonObject> ExtractMapKeyValueTypes(FMapProperty* MapProperty);
#endif
};
