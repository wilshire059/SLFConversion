// SLFAutomationLibrary.h
// C++ Blueprint automation for Shell Pattern migration

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLFAutomationLibrary.generated.h"

UCLASS()
class SLFCONVERSION_API USLFAutomationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// QUERY FUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintVariables(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintFunctions(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintEventDispatchers(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static FString GetBlueprintParentClass(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintSCSComponents(UObject* BlueprintAsset);

	// REPARENTING
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ReparentBlueprint(UObject* BlueprintAsset, const FString& NewParentClassPath);

	// VARIABLE OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveVariable(UObject* BlueprintAsset, const FString& VariableName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveVariables(UObject* BlueprintAsset, const TArray<FString>& VariableNames);

	// FUNCTION OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveFunction(UObject* BlueprintAsset, const FString& FunctionName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveFunctions(UObject* BlueprintAsset, const TArray<FString>& FunctionNames);

	// EVENT DISPATCHER OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveEventDispatcher(UObject* BlueprintAsset, const FString& DispatcherName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveEventDispatchers(UObject* BlueprintAsset, const TArray<FString>& DispatcherNames);

	// GRAPH CLEARING
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ClearEventGraphs(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ClearFunctionGraphs(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearAllBlueprintLogic(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearAllBlueprintLogicNoCompile(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearGraphsKeepVariables(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearGraphsKeepVariablesNoCompile(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearConstructionScript(UObject* BlueprintAsset);

	// COMPREHENSIVE MIGRATION
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool MigrateToShell(UObject* BlueprintAsset, const FString& NewParentClassPath, const TArray<FString>& VariablesToRemove, const TArray<FString>& FunctionsToRemove, const TArray<FString>& DispatchersToRemove);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool CompileAndSave(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveImplementedInterfaces(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ReplaceVariableReferences(UObject* BlueprintAsset, FName OldVarName, FName NewVarName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RenameVariable(UObject* BlueprintAsset, const FString& OldVarName, const FString& NewVarName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RenameVariables(UObject* BlueprintAsset, const TArray<FString>& OldNames, const TArray<FString>& NewNames);

	// DATA ASSET OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplyEquipSlotsToItem(const FString& ItemAssetPath, const TArray<FString>& SlotTags);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplySkeletalMeshInfoToItem(const FString& ItemAssetPath, const TArray<FString>& CharacterAssetNames, const TArray<FString>& MeshPaths);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static FString GetWeaponOverlayTag(const FString& ItemAssetPath);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplyWeaponOverlayTag(const FString& ItemAssetPath, const FString& OverlayTagString);

	// ANIMBP FIX FUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixPropertyAccessPaths(UObject* AnimBlueprintAsset, const FString& OldPropertyName, const FString& NewPropertyName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixLinkedAnimLayers(UObject* AnimBlueprintAsset, const FString& InterfaceName, const FString& LinkedAnimGraphPath);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString DiagnoseAnimGraphNodes(UObject* AnimBlueprintAsset);

	// AAA-Quality LinkedAnimLayer functions using proper UE5 APIs
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString DiagnoseLinkedAnimLayerNodes(UObject* AnimBlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixLinkedAnimLayerGuid(UObject* AnimBlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString GetBlueprintCompileErrors(UObject* BlueprintAsset);

	// BLUEPRINT STATE EXPORT/DIFF
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString ExportBlueprintState(UObject* BlueprintAsset, const FString& OutputFilePath = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString ExportAnimGraphState(UObject* AnimBlueprintAsset, const FString& OutputFilePath = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString DiffBlueprintStates(const FString& BeforeJsonPath, const FString& AfterJsonPath);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixAnimBPFromReference(UObject* AnimBlueprintAsset, const FString& ReferenceJsonPath);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString GetNodeDetails(UObject* BlueprintAsset, const FString& NodeName);

	// AnimBP Diagnostic - finds all broken nodes, state machine states/transitions
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString DiagnoseAnimBP(UObject* AnimBlueprintAsset);

	// Get state machine structure (states and transitions)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString GetStateMachineStructure(UObject* AnimBlueprintAsset, const FString& StateMachineName);

	// Inspect transition graphs in detail (show all nodes for transitions matching keyword)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString InspectTransitionGraph(UObject* AnimBlueprintAsset, const FString& Keyword);

	// Fix broken transition boolean inputs by connecting to named variable
	// Example: FixTransitionBooleanInput(bp, "Transition_2", "A", "bIsAccelerating")
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixTransitionBooleanInput(UObject* AnimBlueprintAsset, const FString& TransitionGraphNameContains, const FString& BoolPinName, const FString& VariableName);

	// Auto-fix all broken transition inputs based on variable mapping
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixAllBrokenTransitions(UObject* AnimBlueprintAsset);

	// Fix BlendListByEnum nodes by binding their ActiveEnumValue to a variable
	// NodeIdentifier can be partial match on node name, position, or index
	// VariableName is the Blueprint variable to bind to (e.g., "LeftHandOverlayState_0")
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixBlendListByEnumBinding(UObject* AnimBlueprintAsset, int32 NodeIndex, const FString& VariableName);

	// Auto-fix all BlendListByEnum nodes for overlay states
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixAllBlendListByEnumBindings(UObject* AnimBlueprintAsset);

	// Fix ANS_RegisterAttackSequence notify QueuedSection values on a montage
	// Converts invalid enum values to FName section names
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Montage")
	static int32 FixMontageComboNotifies(UObject* MontageAsset);

	// Fix all montages with ANS_RegisterAttackSequence notifies
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Montage")
	static int32 FixAllMontageComboNotifies();

	// BEHAVIOR TREE DIAGNOSTICS
	// Export behavior tree structure including decorator IntValue configuration
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|BehaviorTree")
	static FString ExportBehaviorTreeStructure(const FString& BehaviorTreePath);

	// Export all behavior tree subtrees and their decorator config
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|BehaviorTree")
	static FString ExportAllBTSubtrees();

	// Fix blackboard enum key to use C++ enum instead of Blueprint enum
	// Returns true if the fix was applied, false otherwise
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|BehaviorTree")
	static bool FixBlackboardEnumType(const FString& BlackboardPath, const FString& KeyName, const FString& CppEnumPath);

	// Diagnose blackboard keys - show enum types for all keys
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|BehaviorTree")
	static FString DiagnoseBlackboardKeys(const FString& BlackboardPath);


	// Diagnose and fix BT decorator IntValues for enum comparisons
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|BehaviorTree")
	static FString DiagnoseAndFixBTDecoratorIntValues(const FString& BehaviorTreePath);

	// Diagnose BT task BlackboardKeySelector configurations
	// Returns detailed info about each task's key selectors (TargetKey, RadiusKey, etc.)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|BehaviorTree")
	static FString DiagnoseBTTaskKeySelectors(const FString& BehaviorTreePath);

	// Fix BT State decorator to use specific IntValue (for C++ ESLFAIStates enum migration)
	// ExpectedStateValue: 0=Idle, 1=RandomRoam, 2=Patrolling, 3=Investigating, 4=Combat, 5=PoiseBroken, 6=Uninterruptable, 7=OutOfBounds
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|BehaviorTree")
	static FString FixBTStateDecoratorToValue(const FString& BehaviorTreePath, int32 ExpectedStateValue);

#endif // WITH_EDITOR
};
