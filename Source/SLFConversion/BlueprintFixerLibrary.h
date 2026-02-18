// BlueprintFixerLibrary.h
// C++ utilities for fixing Blueprint function bindings after migration

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintFixerLibrary.generated.h"

/**
 * UBlueprintFixerLibrary
 *
 * Provides C++ functions callable from Python to fix Blueprint nodes
 * after reparenting components to C++ base classes.
 */
UCLASS()
class SLFCONVERSION_API UBlueprintFixerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	/**
	 * Refresh all nodes in a Blueprint that call functions on a specific class.
	 * This rebinds function calls to inherited implementations after reparenting.
	 *
	 * @param Blueprint The Blueprint to fix
	 * @param OldClassName The old class name (e.g., "AC_InputBuffer_C")
	 * @param NewClassName The new parent class name (e.g., "InputBufferComponent")
	 * @return Number of nodes fixed
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static int32 RefreshFunctionCallsForClass(UBlueprint* Blueprint, FString OldClassName, FString NewClassName);

	/**
	 * Reconstruct all nodes in a Blueprint.
	 * Forces all nodes to refresh their pins and bindings.
	 *
	 * @param Blueprint The Blueprint to refresh
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static bool ReconstructAllNodes(UBlueprint* Blueprint);

	/**
	 * Get information about function call nodes in a Blueprint.
	 * Useful for debugging which nodes need fixing.
	 *
	 * @param Blueprint The Blueprint to analyze
	 * @return Array of strings describing function call nodes
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static TArray<FString> GetFunctionCallInfo(UBlueprint* Blueprint);

	/**
	 * Fix event dispatcher bindings after reparenting.
	 * Updates component-bound events to use inherited dispatchers.
	 *
	 * @param Blueprint The Blueprint to fix
	 * @param ComponentName The component name (e.g., "AC_InputBuffer")
	 * @return Number of events fixed
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static int32 FixEventDispatcherBindings(UBlueprint* Blueprint, FString ComponentName);

	//////////////////////////////////////////////////////////////////////////
	// Blueprint Cleanup Functions
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Delete a function from a Blueprint by name.
	 *
	 * @param Blueprint The Blueprint to modify
	 * @param FunctionName The name of the function to delete
	 * @return True if deleted successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static bool DeleteBlueprintFunction(UBlueprint* Blueprint, FString FunctionName);

	/**
	 * Delete a variable from a Blueprint by name.
	 *
	 * @param Blueprint The Blueprint to modify
	 * @param VariableName The name of the variable to delete
	 * @return True if deleted successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static bool DeleteBlueprintVariable(UBlueprint* Blueprint, FString VariableName);

	/**
	 * Clear all nodes from the Event Graph.
	 *
	 * @param Blueprint The Blueprint to modify
	 * @return Number of nodes removed
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static int32 ClearEventGraph(UBlueprint* Blueprint);

	/**
	 * Delete an event dispatcher from a Blueprint.
	 *
	 * @param Blueprint The Blueprint to modify
	 * @param DispatcherName The name of the event dispatcher to delete
	 * @return True if deleted successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static bool DeleteEventDispatcher(UBlueprint* Blueprint, FString DispatcherName);

	//////////////////////////////////////////////////////////////////////////
	// Diagnostic/Logging Functions
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Get detailed diagnostic information about a Blueprint.
	 * Returns JSON-like formatted string for comparison with exports.
	 *
	 * @param Blueprint The Blueprint to analyze
	 * @return Detailed diagnostic string
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static FString GetBlueprintDiagnostics(UBlueprint* Blueprint);

	/**
	 * List all functions defined in a Blueprint.
	 *
	 * @param Blueprint The Blueprint to analyze
	 * @return Array of function names
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static TArray<FString> ListBlueprintFunctions(UBlueprint* Blueprint);

	/**
	 * List all variables defined in a Blueprint.
	 *
	 * @param Blueprint The Blueprint to analyze
	 * @return Array of variable names with types
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static TArray<FString> ListBlueprintVariables(UBlueprint* Blueprint);

	/**
	 * Get detailed node information for a specific graph.
	 * Outputs NodeGuid, NodeClass, connections - matches JSON export format.
	 *
	 * @param Blueprint The Blueprint to analyze
	 * @param GraphName Name of graph (empty = all graphs)
	 * @return Array of node info strings
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static TArray<FString> GetDetailedNodeInfo(UBlueprint* Blueprint, FString GraphName);

	//////////////////////////////////////////////////////////////////////////
	// Enum Migration Functions
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Fix enum pin default values in a Blueprint.
	 * Used for migrating Blueprint enums to C++ enums with different value names.
	 *
	 * @param Blueprint The Blueprint to fix
	 * @param EnumPathSubstring Substring to match in enum path (e.g., "E_ValueType")
	 * @param OldValue The old enum value name (e.g., "NewEnumerator0")
	 * @param NewValue The new enum value name (e.g., "CurrentValue")
	 * @return Number of pins fixed
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static int32 FixEnumPinValues(UBlueprint* Blueprint, FString EnumPathSubstring, FString OldValue, FString NewValue);

	/**
	 * Get all enum pins in a Blueprint for diagnostic purposes.
	 *
	 * @param Blueprint The Blueprint to analyze
	 * @param EnumPathSubstring Substring to match in enum path (e.g., "E_ValueType")
	 * @return Array of strings describing enum pins found
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static TArray<FString> GetEnumPinInfo(UBlueprint* Blueprint, FString EnumPathSubstring);

	/**
	 * Fix pin values by searching for pins that contain a specific value string.
	 * This works regardless of pin type - searches ALL pins in the Blueprint.
	 *
	 * @param Blueprint The Blueprint to fix
	 * @param OldValue The value to search for (exact match on DefaultValue)
	 * @param NewValue The value to replace with
	 * @return Number of pins fixed
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static int32 FixPinsByValue(UBlueprint* Blueprint, FString OldValue, FString NewValue);

	/**
	 * Get diagnostic info about ALL pins in a Blueprint - shows pins with non-empty DefaultValue.
	 *
	 * @param Blueprint The Blueprint to analyze
	 * @return Array of strings describing pins with values
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static TArray<FString> GetAllPinValues(UBlueprint* Blueprint);

	/**
	 * Fix pin values by searching for pins with a specific NAME and VALUE.
	 * More targeted than FixPinsByValue - won't affect other pins with same value but different name.
	 *
	 * @param Blueprint The Blueprint to fix
	 * @param PinName The exact pin name to match (e.g., "ValueType")
	 * @param OldValue The value to search for
	 * @param NewValue The value to replace with
	 * @return Number of pins fixed
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static int32 FixPinsByNameAndValue(UBlueprint* Blueprint, FString PinName, FString OldValue, FString NewValue);

	//////////////////////////////////////////////////////////////////////////
	// Variable Type Fixing Functions
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Fix a variable's struct type by replacing broken Blueprint struct reference with C++ struct.
	 * Used when Blueprint struct files were deleted before variables were migrated.
	 *
	 * @param Blueprint The Blueprint to modify
	 * @param VariableName The name of the variable to fix
	 * @param NewStructPath The C++ struct path (e.g., "/Script/SLFConversion.FStatusEffectTick")
	 * @return True if fixed successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static bool FixVariableStructType(UBlueprint* Blueprint, FString VariableName, FString NewStructPath);

	/**
	 * Get detailed information about all variables in a Blueprint, including type paths.
	 *
	 * @param Blueprint The Blueprint to analyze
	 * @return Array of strings with variable info
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static TArray<FString> GetVariableTypeInfo(UBlueprint* Blueprint);

	//////////////////////////////////////////////////////////////////////////
	// Node Management Functions
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Delete a node from a Blueprint by its GUID.
	 *
	 * @param Blueprint The Blueprint to modify
	 * @param NodeGuidString The GUID of the node to delete (e.g., "92E4BA3E4F98F0ACCB45A9BD485233DD")
	 * @return True if node was found and deleted
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static bool DeleteNodeByGuid(UBlueprint* Blueprint, FString NodeGuidString);

	/**
	 * Get list of broken nodes (unknown struct, bad enum, etc.) with their GUIDs.
	 *
	 * @param Blueprint The Blueprint to analyze
	 * @return Array of strings with broken node info including GUIDs
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static TArray<FString> GetBrokenNodeGuids(UBlueprint* Blueprint);

	/**
	 * Delete multiple nodes by their GUIDs.
	 *
	 * @param Blueprint The Blueprint to modify
	 * @param NodeGuids Array of GUID strings
	 * @return Number of nodes deleted
	 */
	UFUNCTION(BlueprintCallable, Category = "Blueprint Fixer", meta = (DevelopmentOnly))
	static int32 DeleteNodesByGuids(UBlueprint* Blueprint, TArray<FString> NodeGuids);
#endif // WITH_EDITOR
};
