// SLFAutomationLibrary.h
// C++ Blueprint automation for Shell Pattern migration

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLFAutomationLibrary.generated.h"

/**
 * Automation library for Blueprint-to-C++ Shell Pattern migration.
 * Provides functions to reparent, remove variables/functions, and clean up Blueprints.
 */
UCLASS()
class SLFCONVERSION_API USLFAutomationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// ========================================================================
	// QUERY FUNCTIONS
	// ========================================================================

	/** Gets all variable names defined in a Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintVariables(UObject* BlueprintAsset);

	/** Gets all function names defined in a Blueprint (user-created only). */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintFunctions(UObject* BlueprintAsset);

	/** Gets all event dispatcher names defined in a Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintEventDispatchers(UObject* BlueprintAsset);

	/** Gets the current parent class name of a Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static FString GetBlueprintParentClass(UObject* BlueprintAsset);

	/** Gets all SCS (SimpleConstructionScript) component names in a Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintSCSComponents(UObject* BlueprintAsset);

	// ========================================================================
	// REPARENTING
	// ========================================================================

	/**
	 * Reparents a Blueprint to a new C++ parent class.
	 * @param BlueprintAsset - The Blueprint to reparent.
	 * @param NewParentClassPath - Full path like "/Script/SLFConversion.StatManagerComponent"
	 * @return True if successful.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ReparentBlueprint(UObject* BlueprintAsset, const FString& NewParentClassPath);

	// ========================================================================
	// VARIABLE OPERATIONS
	// ========================================================================

	/**
	 * Removes a variable from a Blueprint.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @param VariableName - Name of the variable to remove.
	 * @return True if the variable was found and removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveVariable(UObject* BlueprintAsset, const FString& VariableName);

	/**
	 * Removes multiple variables from a Blueprint.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @param VariableNames - Array of variable names to remove.
	 * @return Number of variables successfully removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveVariables(UObject* BlueprintAsset, const TArray<FString>& VariableNames);

	// ========================================================================
	// FUNCTION OPERATIONS
	// ========================================================================

	/**
	 * Removes a function graph from a Blueprint.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @param FunctionName - Name of the function to remove.
	 * @return True if the function was found and removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveFunction(UObject* BlueprintAsset, const FString& FunctionName);

	/**
	 * Removes multiple functions from a Blueprint.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @param FunctionNames - Array of function names to remove.
	 * @return Number of functions successfully removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveFunctions(UObject* BlueprintAsset, const TArray<FString>& FunctionNames);

	// ========================================================================
	// EVENT DISPATCHER OPERATIONS
	// ========================================================================

	/**
	 * Removes an event dispatcher from a Blueprint.
	 * Event dispatchers are stored as variables with delegate types.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @param DispatcherName - Name of the event dispatcher to remove.
	 * @return True if the dispatcher was found and removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveEventDispatcher(UObject* BlueprintAsset, const FString& DispatcherName);

	/**
	 * Removes multiple event dispatchers from a Blueprint.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @param DispatcherNames - Array of dispatcher names to remove.
	 * @return Number of dispatchers successfully removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveEventDispatchers(UObject* BlueprintAsset, const TArray<FString>& DispatcherNames);

	// ========================================================================
	// GRAPH CLEARING
	// ========================================================================

	/**
	 * Clears all nodes from the Event Graph (UbergraphPages).
	 * Removes BeginPlay, Tick, custom events, and all logic nodes.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @return Number of nodes removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ClearEventGraphs(UObject* BlueprintAsset);

	/**
	 * Clears all nodes from all function graphs.
	 * Keeps the function entry point but removes all logic.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @return Number of nodes removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ClearFunctionGraphs(UObject* BlueprintAsset);

	/**
	 * Removes ALL Blueprint logic - clears event graphs, removes all functions,
	 * removes all variables, removes all dispatchers.
	 * This creates a true "shell" Blueprint with only the class identity.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @return True if successful.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearAllBlueprintLogic(UObject* BlueprintAsset);

	/**
	 * Removes ALL Blueprint logic WITHOUT compiling.
	 * Use this for batch operations where compilation order matters.
	 * Call CompileAndSave() separately after all Blueprints are cleared and reparented.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @return True if successful.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearAllBlueprintLogicNoCompile(UObject* BlueprintAsset);

	/**
	 * Clears Blueprint graphs but KEEPS variables.
	 * Use for components referenced by AnimBPs - their variables must remain
	 * so AnimGraph nodes can still find them.
	 * Variables will shadow C++ properties with the same names.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @return True if successful.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearGraphsKeepVariables(UObject* BlueprintAsset);

	/**
	 * Clears Blueprint graphs but KEEPS variables WITHOUT compiling.
	 * Use for batch operations where compilation order matters.
	 * Call CompileAndSave() separately after all Blueprints are processed.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @return True if successful.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearGraphsKeepVariablesNoCompile(UObject* BlueprintAsset);

	// ========================================================================
	// COMPREHENSIVE MIGRATION
	// ========================================================================

	/**
	 * Performs full shell migration: reparent + remove variables/functions/dispatchers.
	 * @param BlueprintAsset - The Blueprint to migrate.
	 * @param NewParentClassPath - C++ parent class path.
	 * @param VariablesToRemove - Variables to delete.
	 * @param FunctionsToRemove - Functions to delete.
	 * @param DispatchersToRemove - Event dispatchers to delete.
	 * @return True if migration completed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool MigrateToShell(
		UObject* BlueprintAsset,
		const FString& NewParentClassPath,
		const TArray<FString>& VariablesToRemove,
		const TArray<FString>& FunctionsToRemove,
		const TArray<FString>& DispatchersToRemove
	);

	/**
	 * Compiles and saves a Blueprint after modifications.
	 * @param BlueprintAsset - The Blueprint to compile and save.
	 * @return True if successful.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool CompileAndSave(UObject* BlueprintAsset);

	/**
	 * Removes all implemented interfaces from a Blueprint.
	 * Use this when reparenting to a C++ class that implements the interfaces.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @return Number of interfaces removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveImplementedInterfaces(UObject* BlueprintAsset);

	// ========================================================================
	// VARIABLE REFERENCE REPLACEMENT (from original library)
	// ========================================================================

	/**
	 * Advanced replacement with wire preservation.
	 * Caches pin connections before ReconstructNode and restores them after.
	 * @param BlueprintAsset - The Blueprint asset.
	 * @param OldVarName - The current variable name.
	 * @param NewVarName - The new variable name (must exist in C++ parent).
	 * @return Number of nodes modified.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ReplaceVariableReferences(UObject* BlueprintAsset, FName OldVarName, FName NewVarName);

#endif // WITH_EDITOR
};
