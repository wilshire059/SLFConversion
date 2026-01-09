// SLFAutomationLibrary.cpp
// C++ Blueprint automation for Shell Pattern migration

#include "SLFAutomationLibrary.h"

#if WITH_EDITOR
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/DynamicBlueprintBinding.h"
#include "Animation/AnimBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_Variable.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "K2Node_Composite.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_Event.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_ComponentBoundEvent.h"
#include "EdGraphSchema_K2.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/SavePackage.h"
#include "GameplayTagContainer.h"

DEFINE_LOG_CATEGORY_STATIC(LogSLFAutomation, Log, All);

// ============================================================================
// HELPER: Get Blueprint from asset
// ============================================================================
static UBlueprint* GetBlueprintFromAsset(UObject* AssetObject)
{
	if (!AssetObject)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("AssetObject is null"));
		return nullptr;
	}

	UBlueprint* Blueprint = Cast<UBlueprint>(AssetObject);
	if (!Blueprint)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("AssetObject is not a Blueprint: %s"), *AssetObject->GetClass()->GetName());
		return nullptr;
	}

	return Blueprint;
}

// ============================================================================
// QUERY FUNCTIONS
// ============================================================================

TArray<FString> USLFAutomationLibrary::GetBlueprintVariables(UObject* BlueprintAsset)
{
	TArray<FString> VarNames;
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return VarNames;

	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		VarNames.Add(Var.VarName.ToString());
	}

	UE_LOG(LogSLFAutomation, Log, TEXT("Found %d variables in %s"), VarNames.Num(), *Blueprint->GetName());
	return VarNames;
}

TArray<FString> USLFAutomationLibrary::GetBlueprintFunctions(UObject* BlueprintAsset)
{
	TArray<FString> FuncNames;
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return FuncNames;

	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
	{
		if (Graph)
		{
			FuncNames.Add(Graph->GetName());
		}
	}

	UE_LOG(LogSLFAutomation, Log, TEXT("Found %d functions in %s"), FuncNames.Num(), *Blueprint->GetName());
	return FuncNames;
}

TArray<FString> USLFAutomationLibrary::GetBlueprintEventDispatchers(UObject* BlueprintAsset)
{
	TArray<FString> DispatcherNames;
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return DispatcherNames;

	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		// Event dispatchers have delegate pin category
		if (Var.VarType.PinCategory == UEdGraphSchema_K2::PC_MCDelegate)
		{
			DispatcherNames.Add(Var.VarName.ToString());
		}
	}

	UE_LOG(LogSLFAutomation, Log, TEXT("Found %d event dispatchers in %s"), DispatcherNames.Num(), *Blueprint->GetName());
	return DispatcherNames;
}

FString USLFAutomationLibrary::GetBlueprintParentClass(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return TEXT("");

	if (Blueprint->ParentClass)
	{
		return Blueprint->ParentClass->GetPathName();
	}
	return TEXT("");
}

TArray<FString> USLFAutomationLibrary::GetBlueprintSCSComponents(UObject* BlueprintAsset)
{
	TArray<FString> ComponentNames;

	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return ComponentNames;

	USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
	if (!SCS)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Blueprint %s has no SimpleConstructionScript"), *Blueprint->GetName());
		return ComponentNames;
	}

	TArray<USCS_Node*> AllNodes = SCS->GetAllNodes();
	for (USCS_Node* Node : AllNodes)
	{
		if (Node && Node->ComponentTemplate)
		{
			FString Entry = FString::Printf(TEXT("%s (%s)"),
				*Node->ComponentTemplate->GetName(),
				*Node->ComponentTemplate->GetClass()->GetName());
			ComponentNames.Add(Entry);
			UE_LOG(LogSLFAutomation, Warning, TEXT("  SCS Component: %s"), *Entry);
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("Blueprint %s has %d SCS components"), *Blueprint->GetName(), ComponentNames.Num());
	return ComponentNames;
}

// ============================================================================
// REPARENTING
// ============================================================================

bool USLFAutomationLibrary::ReparentBlueprint(UObject* BlueprintAsset, const FString& NewParentClassPath)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint)
	{
		return false;
	}

	// Load the new parent class
	UClass* NewParentClass = LoadClass<UObject>(nullptr, *NewParentClassPath);
	if (!NewParentClass)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("Could not find class: %s"), *NewParentClassPath);
		return false;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("Reparenting %s: %s -> %s"),
		*Blueprint->GetName(),
		Blueprint->ParentClass ? *Blueprint->ParentClass->GetName() : TEXT("null"),
		*NewParentClass->GetName());

	// Set the new parent class directly - NO COMPILE to avoid cascade issues
	Blueprint->ParentClass = NewParentClass;

	// Mark as modified but don't refresh/compile - will be compiled on next editor load
	Blueprint->Modify();
	Blueprint->MarkPackageDirty();

	UE_LOG(LogSLFAutomation, Warning, TEXT("Successfully reparented %s to %s (compile deferred)"),
		*Blueprint->GetName(), *NewParentClass->GetName());

	return true;
}

// ============================================================================
// VARIABLE OPERATIONS
// ============================================================================

bool USLFAutomationLibrary::RemoveVariable(UObject* BlueprintAsset, const FString& VariableName)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	FName VarFName(*VariableName);

	// Check if variable exists
	int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, VarFName);
	if (VarIndex == INDEX_NONE)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Variable not found: %s"), *VariableName);
		return false;
	}

	// Remove the variable using the proper API
	FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, VarFName);

	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed variable: %s"), *VariableName);
	return true;
}

int32 USLFAutomationLibrary::RemoveVariables(UObject* BlueprintAsset, const TArray<FString>& VariableNames)
{
	int32 RemovedCount = 0;
	for (const FString& VarName : VariableNames)
	{
		if (RemoveVariable(BlueprintAsset, VarName))
		{
			RemovedCount++;
		}
	}
	return RemovedCount;
}

// ============================================================================
// FUNCTION OPERATIONS
// ============================================================================

bool USLFAutomationLibrary::RemoveFunction(UObject* BlueprintAsset, const FString& FunctionName)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	FName FuncFName(*FunctionName);

	// Find the function graph
	UEdGraph* FunctionGraph = nullptr;
	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
	{
		if (Graph && Graph->GetFName() == FuncFName)
		{
			FunctionGraph = Graph;
			break;
		}
	}

	if (!FunctionGraph)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Function not found: %s"), *FunctionName);
		return false;
	}

	// Remove the function graph using the proper API
	FBlueprintEditorUtils::RemoveGraph(Blueprint, FunctionGraph);

	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed function: %s"), *FunctionName);
	return true;
}

int32 USLFAutomationLibrary::RemoveFunctions(UObject* BlueprintAsset, const TArray<FString>& FunctionNames)
{
	int32 RemovedCount = 0;
	for (const FString& FuncName : FunctionNames)
	{
		if (RemoveFunction(BlueprintAsset, FuncName))
		{
			RemovedCount++;
		}
	}
	return RemovedCount;
}

// ============================================================================
// EVENT DISPATCHER OPERATIONS
// ============================================================================

bool USLFAutomationLibrary::RemoveEventDispatcher(UObject* BlueprintAsset, const FString& DispatcherName)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	FName DispFName(*DispatcherName);

	// Event dispatchers are stored as variables with MC_Delegate type
	// First verify it exists and is a delegate
	int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, DispFName);
	if (VarIndex == INDEX_NONE)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Event dispatcher not found: %s"), *DispatcherName);
		return false;
	}

	// Verify it's actually a delegate type
	const FBPVariableDescription& Var = Blueprint->NewVariables[VarIndex];
	if (Var.VarType.PinCategory != UEdGraphSchema_K2::PC_MCDelegate)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("%s is not an event dispatcher (type: %s)"),
			*DispatcherName, *Var.VarType.PinCategory.ToString());
		return false;
	}

	// Remove delegate signature graph if it exists
	for (int32 i = Blueprint->DelegateSignatureGraphs.Num() - 1; i >= 0; --i)
	{
		UEdGraph* Graph = Blueprint->DelegateSignatureGraphs[i];
		if (Graph && Graph->GetFName() == DispFName)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
			break;
		}
	}

	// Remove the dispatcher variable
	FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, DispFName);

	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed event dispatcher: %s"), *DispatcherName);
	return true;
}

int32 USLFAutomationLibrary::RemoveEventDispatchers(UObject* BlueprintAsset, const TArray<FString>& DispatcherNames)
{
	int32 RemovedCount = 0;
	for (const FString& DispName : DispatcherNames)
	{
		if (RemoveEventDispatcher(BlueprintAsset, DispName))
		{
			RemovedCount++;
		}
	}
	return RemovedCount;
}

// ============================================================================
// COMPONENT DELEGATE BINDING CLEARING
// ============================================================================

// Helper to clear all component delegate bindings (event connections on components)
// These are stored in the generated class and reference functions that we're clearing
static int32 ClearComponentDelegateBindings(UBlueprint* Blueprint)
{
	if (!Blueprint)
	{
		return 0;
	}

	int32 ClearedCount = 0;

	// Clear component bound event nodes from event graphs
	// These are K2Node_ComponentBoundEvent nodes that represent OnBeginOverlap, etc.
	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		if (!Graph) continue;

		TArray<UEdGraphNode*> NodesToRemove;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			// Check if this is a component bound event (OnBeginOverlap, OnEndOverlap, etc.)
			UK2Node_ComponentBoundEvent* BoundEventNode = Cast<UK2Node_ComponentBoundEvent>(Node);
			if (BoundEventNode)
			{
				NodesToRemove.Add(Node);
				ClearedCount++;
			}
		}

		for (UEdGraphNode* Node : NodesToRemove)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Removing ComponentBoundEvent node: %s"), *Node->GetNodeTitle(ENodeTitleType::ListView).ToString());
			Graph->RemoveNode(Node);
		}
	}

	// If there's a generated class, clear its dynamic binding objects
	UBlueprintGeneratedClass* GenClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
	if (GenClass)
	{
		// Clear the dynamic binding objects on the generated class
		if (GenClass->DynamicBindingObjects.Num() > 0)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Clearing %d DynamicBindingObjects from GeneratedClass"), GenClass->DynamicBindingObjects.Num());
			ClearedCount += GenClass->DynamicBindingObjects.Num();
			GenClass->DynamicBindingObjects.Empty();
		}

		// Clear Timelines from generated class
		if (GenClass->Timelines.Num() > 0)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Clearing %d Timelines from GeneratedClass"), GenClass->Timelines.Num());
			ClearedCount += GenClass->Timelines.Num();
			GenClass->Timelines.Empty();
		}
	}

	// Also clear timelines from the Blueprint itself
	if (Blueprint->Timelines.Num() > 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Clearing %d Timelines from Blueprint"), Blueprint->Timelines.Num());
		ClearedCount += Blueprint->Timelines.Num();
		Blueprint->Timelines.Empty();
	}

	return ClearedCount;
}

// ============================================================================
// GRAPH CLEARING
// ============================================================================

// Helper to clear all nodes from a graph (including collapsed graphs recursively)
static int32 ClearGraphNodes(UEdGraph* Graph, bool bKeepEntryNodes = false)
{
	if (!Graph)
	{
		return 0;
	}

	int32 RemovedCount = 0;
	TArray<UEdGraphNode*> NodesToRemove;

	for (UEdGraphNode* Node : Graph->Nodes)
	{
		if (!Node)
		{
			continue;
		}

		// Optionally keep function entry nodes
		if (bKeepEntryNodes)
		{
			UK2Node_FunctionEntry* EntryNode = Cast<UK2Node_FunctionEntry>(Node);
			if (EntryNode)
			{
				continue;
			}
		}

		// Process collapsed graphs recursively first
		UK2Node_Composite* CompositeNode = Cast<UK2Node_Composite>(Node);
		if (CompositeNode && CompositeNode->BoundGraph)
		{
			RemovedCount += ClearGraphNodes(CompositeNode->BoundGraph, false);
		}

		NodesToRemove.Add(Node);
	}

	// Remove all marked nodes
	for (UEdGraphNode* Node : NodesToRemove)
	{
		Graph->RemoveNode(Node);
		RemovedCount++;
	}

	return RemovedCount;
}

int32 USLFAutomationLibrary::ClearEventGraphs(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return 0;

	int32 TotalRemoved = 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("Clearing %d event graphs in %s"),
		Blueprint->UbergraphPages.Num(), *Blueprint->GetName());

	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		if (Graph)
		{
			int32 Removed = ClearGraphNodes(Graph, false);
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Cleared %d nodes from %s"), Removed, *Graph->GetName());
			TotalRemoved += Removed;
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d total nodes from event graphs"), TotalRemoved);
	return TotalRemoved;
}

int32 USLFAutomationLibrary::ClearFunctionGraphs(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return 0;

	int32 TotalRemoved = 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("Clearing %d function graphs in %s"),
		Blueprint->FunctionGraphs.Num(), *Blueprint->GetName());

	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
	{
		if (Graph)
		{
			// Keep function entry node, clear everything else
			int32 Removed = ClearGraphNodes(Graph, true);
			UE_LOG(LogSLFAutomation, Log, TEXT("  Cleared %d nodes from function %s"), Removed, *Graph->GetName());
			TotalRemoved += Removed;
		}
	}

	// Also clear macro graphs
	for (UEdGraph* Graph : Blueprint->MacroGraphs)
	{
		if (Graph)
		{
			int32 Removed = ClearGraphNodes(Graph, true);
			UE_LOG(LogSLFAutomation, Log, TEXT("  Cleared %d nodes from macro %s"), Removed, *Graph->GetName());
			TotalRemoved += Removed;
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d total nodes from function/macro graphs"), TotalRemoved);
	return TotalRemoved;
}

bool USLFAutomationLibrary::ClearAllBlueprintLogic(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	// Check if this is an Animation Blueprint - special handling required
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(Blueprint);
	if (AnimBlueprint)
	{
		// AnimBPs: ONLY clear EventGraph, keep variables and AnimGraph intact
		// Variables become shadows of C++ properties after reparenting
		// AnimGraph nodes continue to work because variable names match C++ property names
		UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARING EventGraph ONLY FROM AnimBlueprint %s ==="), *Blueprint->GetName());

		int32 EventNodesCleared = ClearEventGraphs(BlueprintAsset);
		UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d nodes from EventGraph"), EventNodesCleared);

		// Refresh and compile
		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

		FString AnimBPName = Blueprint->GetName(); // Cache name before compile
		UE_LOG(LogSLFAutomation, Warning, TEXT("Compiling AnimBlueprint %s..."), *AnimBPName);
		FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);

		if (IsValid(Blueprint))
		{
			if (Blueprint->Status == BS_Error)
			{
				UE_LOG(LogSLFAutomation, Error, TEXT("Compile errors in AnimBlueprint %s!"), *AnimBPName);
			}
			else
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("AnimBlueprint %s cleared successfully (EventGraph only)"), *AnimBPName);
			}
		}
		else
		{
			UE_LOG(LogSLFAutomation, Error, TEXT("AnimBlueprint %s became invalid after compile!"), *AnimBPName);
		}

		return true;
	}

	// Regular Blueprints: Clear everything
	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARING ALL LOGIC FROM %s ==="), *Blueprint->GetName());

	// 1. Clear event graphs
	int32 EventNodesCleared = ClearEventGraphs(BlueprintAsset);

	// 2. Remove function graphs
	TArray<UEdGraph*> FunctionGraphsCopy = Blueprint->FunctionGraphs;
	for (UEdGraph* Graph : FunctionGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d function graphs"), FunctionGraphsCopy.Num());

	// 3. Remove ALL macro graphs
	TArray<UEdGraph*> MacroGraphsCopy = Blueprint->MacroGraphs;
	for (UEdGraph* Graph : MacroGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d macro graphs"), MacroGraphsCopy.Num());

	// 4. Remove ALL variables
	TArray<FName> VarNames;
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		VarNames.Add(Var.VarName);
	}
	for (const FName& VarName : VarNames)
	{
		FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, VarName);
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d variables"), VarNames.Num());

	// 5. Remove delegate signature graphs
	TArray<UEdGraph*> DelegateGraphsCopy = Blueprint->DelegateSignatureGraphs;
	for (UEdGraph* Graph : DelegateGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d delegate signature graphs"), DelegateGraphsCopy.Num());

	// 6. Clear INTERFACE graphs (stored in ImplementedInterfaces)
	int32 InterfaceGraphsCleared = 0;
	for (FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Clearing interface: %s (%d graphs)"),
			Interface.Interface ? *Interface.Interface->GetName() : TEXT("null"),
			Interface.Graphs.Num());

		for (UEdGraph* Graph : Interface.Graphs)
		{
			if (Graph)
			{
				int32 Removed = ClearGraphNodes(Graph, true); // Keep entry nodes
				InterfaceGraphsCleared += Removed;
				UE_LOG(LogSLFAutomation, Warning, TEXT("  Cleared %d nodes from interface graph: %s"), Removed, *Graph->GetName());
			}
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d nodes from interface graphs"), InterfaceGraphsCleared);

	// 7. Refresh and mark modified
	UE_LOG(LogSLFAutomation, Warning, TEXT("Refreshing nodes for %s..."), *Blueprint->GetName());
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	// 8. CRITICAL: Compile to regenerate bytecode
	// Without this, the old ExecuteUbergraph bytecode would still run at runtime!
	UE_LOG(LogSLFAutomation, Warning, TEXT("Compiling %s to regenerate bytecode..."), *Blueprint->GetName());

	// Use try-catch style error handling for compile
	bool bCompileSucceeded = false;
	FString BlueprintName = Blueprint->GetName(); // Cache name before compile in case Blueprint becomes invalid

	// Use EBlueprintCompileOptions to avoid potential crashes
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);

	// Check if Blueprint is still valid after compile
	if (IsValid(Blueprint))
	{
		if (Blueprint->Status == BS_Error)
		{
			UE_LOG(LogSLFAutomation, Error, TEXT("Compile errors in %s after clearing!"), *BlueprintName);
		}
		else
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("Compilation successful for %s"), *BlueprintName);
			bCompileSucceeded = true;
		}
	}
	else
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("Blueprint %s became invalid after compile!"), *BlueprintName);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARED ALL LOGIC FROM %s ==="), *BlueprintName);
	return true;
}

// ============================================================================
// CLEAR ALL LOGIC WITHOUT COMPILE
// For batch operations where compilation order matters
// ============================================================================

bool USLFAutomationLibrary::ClearAllBlueprintLogicNoCompile(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	// Check if this is an Animation Blueprint - special handling required
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(Blueprint);
	if (AnimBlueprint)
	{
		// AnimBPs: ONLY clear EventGraph, keep variables and AnimGraph intact
		UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARING EventGraph ONLY (NO COMPILE) FROM AnimBlueprint %s ==="), *Blueprint->GetName());

		int32 EventNodesCleared = ClearEventGraphs(BlueprintAsset);
		UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d nodes from EventGraph"), EventNodesCleared);

		// Refresh but DO NOT compile
		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

		UE_LOG(LogSLFAutomation, Warning, TEXT("AnimBlueprint %s cleared (EventGraph only, NO COMPILE)"), *Blueprint->GetName());
		return true;
	}

	// Regular Blueprints: Clear everything but don't compile
	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARING ALL LOGIC (NO COMPILE) FROM %s ==="), *Blueprint->GetName());

	// 1. Clear event graphs
	int32 EventNodesCleared = ClearEventGraphs(BlueprintAsset);

	// 2. Remove function graphs
	TArray<UEdGraph*> FunctionGraphsCopy = Blueprint->FunctionGraphs;
	for (UEdGraph* Graph : FunctionGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d function graphs"), FunctionGraphsCopy.Num());

	// 3. Remove ALL macro graphs
	TArray<UEdGraph*> MacroGraphsCopy = Blueprint->MacroGraphs;
	for (UEdGraph* Graph : MacroGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d macro graphs"), MacroGraphsCopy.Num());

	// 4. Remove ALL variables
	TArray<FName> VarNames;
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		VarNames.Add(Var.VarName);
	}
	for (const FName& VarName : VarNames)
	{
		FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, VarName);
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d variables"), VarNames.Num());

	// 5. Remove delegate signature graphs
	TArray<UEdGraph*> DelegateGraphsCopy = Blueprint->DelegateSignatureGraphs;
	for (UEdGraph* Graph : DelegateGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d delegate signature graphs"), DelegateGraphsCopy.Num());

	// 6. Clear INTERFACE graphs (stored in ImplementedInterfaces)
	int32 InterfaceGraphsCleared = 0;
	for (FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Clearing interface: %s (%d graphs)"),
			Interface.Interface ? *Interface.Interface->GetName() : TEXT("null"),
			Interface.Graphs.Num());

		for (UEdGraph* Graph : Interface.Graphs)
		{
			if (Graph)
			{
				int32 Removed = ClearGraphNodes(Graph, true); // Keep entry nodes
				InterfaceGraphsCleared += Removed;
				UE_LOG(LogSLFAutomation, Warning, TEXT("  Cleared %d nodes from interface graph: %s"), Removed, *Graph->GetName());
			}
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d nodes from interface graphs"), InterfaceGraphsCleared);

	// 7. Clear component delegate bindings (OnBeginOverlap, etc. on components)
	// These reference functions we just cleared and will cause crashes if not removed
	int32 BindingsCleared = ClearComponentDelegateBindings(Blueprint);
	UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d component delegate bindings"), BindingsCleared);

	// 8. Refresh and mark modified - BUT DO NOT COMPILE
	UE_LOG(LogSLFAutomation, Warning, TEXT("Refreshing nodes for %s (NO COMPILE)..."), *Blueprint->GetName());
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	// SKIP compilation - caller will compile all Blueprints at the end

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARED ALL LOGIC (NO COMPILE) FROM %s ==="), *Blueprint->GetName());
	return true;
}

// ============================================================================
// CLEAR GRAPHS BUT KEEP VARIABLES
// For components referenced by AnimBPs - variables must remain
// ============================================================================

bool USLFAutomationLibrary::ClearGraphsKeepVariables(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARING GRAPHS (KEEPING VARIABLES) FROM %s ==="), *Blueprint->GetName());

	// 1. Clear event graphs
	int32 EventNodesCleared = ClearEventGraphs(BlueprintAsset);
	UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d nodes from event graphs"), EventNodesCleared);

	// 2. Remove function graphs
	TArray<UEdGraph*> FunctionGraphsCopy = Blueprint->FunctionGraphs;
	for (UEdGraph* Graph : FunctionGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d function graphs"), FunctionGraphsCopy.Num());

	// 3. Remove macro graphs
	TArray<UEdGraph*> MacroGraphsCopy = Blueprint->MacroGraphs;
	for (UEdGraph* Graph : MacroGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d macro graphs"), MacroGraphsCopy.Num());

	// 4. Remove delegate signature graphs
	TArray<UEdGraph*> DelegateGraphsCopy = Blueprint->DelegateSignatureGraphs;
	for (UEdGraph* Graph : DelegateGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d delegate signature graphs"), DelegateGraphsCopy.Num());

	// 5. Clear interface graphs (but don't remove interfaces themselves)
	int32 InterfaceGraphsCleared = 0;
	for (FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
	{
		for (UEdGraph* Graph : Interface.Graphs)
		{
			if (Graph)
			{
				int32 Removed = ClearGraphNodes(Graph, true);
				InterfaceGraphsCleared += Removed;
			}
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d nodes from interface graphs"), InterfaceGraphsCleared);

	// NOTE: Variables are NOT removed - they remain to satisfy external references (AnimBPs)
	UE_LOG(LogSLFAutomation, Warning, TEXT("KEEPING %d variables (for AnimBP compatibility)"), Blueprint->NewVariables.Num());

	// Refresh and compile
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	FString BlueprintName = Blueprint->GetName(); // Cache name before compile
	UE_LOG(LogSLFAutomation, Warning, TEXT("Compiling %s..."), *BlueprintName);
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);

	if (IsValid(Blueprint))
	{
		if (Blueprint->Status == BS_Error)
		{
			UE_LOG(LogSLFAutomation, Error, TEXT("Compile errors in %s!"), *BlueprintName);
		}
		else
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("Compilation successful for %s"), *BlueprintName);
		}
	}
	else
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("Blueprint %s became invalid after compile!"), *BlueprintName);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARED GRAPHS (KEPT VARIABLES) FROM %s ==="), *BlueprintName);
	return true;
}

// ============================================================================
// CLEAR GRAPHS KEEP VARIABLES - NO COMPILE
// For batch operations where compilation order matters
// ============================================================================

bool USLFAutomationLibrary::ClearGraphsKeepVariablesNoCompile(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	// Check if this is an Animation Blueprint - special handling required
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(Blueprint);
	if (AnimBlueprint)
	{
		// AnimBPs: ONLY clear EventGraph, keep variables and AnimGraph intact
		// This is critical - the AnimGraph contains the animation state machine!
		UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARING GRAPHS (KEEPING VARIABLES, NO COMPILE) FROM %s ==="), *Blueprint->GetName());

		UE_LOG(LogSLFAutomation, Warning, TEXT("Clearing %d event graphs in %s"), Blueprint->UbergraphPages.Num(), *Blueprint->GetName());
		int32 EventNodesCleared = ClearEventGraphs(BlueprintAsset);

		// NOTE: Variables are NOT removed - they remain to satisfy AnimGraph Property Access
		UE_LOG(LogSLFAutomation, Warning, TEXT("KEEPING %d variables (for AnimBP compatibility)"), Blueprint->NewVariables.Num());

		// Refresh but DO NOT compile
		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

		UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARED GRAPHS (KEPT VARIABLES, NO COMPILE) FROM %s ==="), *Blueprint->GetName());
		return true;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARING GRAPHS (KEEPING VARIABLES, NO COMPILE) FROM %s ==="), *Blueprint->GetName());

	// 1. Clear event graphs
	int32 EventNodesCleared = ClearEventGraphs(BlueprintAsset);
	UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d nodes from event graphs"), EventNodesCleared);

	// 2. Remove function graphs
	TArray<UEdGraph*> FunctionGraphsCopy = Blueprint->FunctionGraphs;
	for (UEdGraph* Graph : FunctionGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d function graphs"), FunctionGraphsCopy.Num());

	// 3. Remove macro graphs
	TArray<UEdGraph*> MacroGraphsCopy = Blueprint->MacroGraphs;
	for (UEdGraph* Graph : MacroGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d macro graphs"), MacroGraphsCopy.Num());

	// 4. Remove delegate signature graphs
	TArray<UEdGraph*> DelegateGraphsCopy = Blueprint->DelegateSignatureGraphs;
	for (UEdGraph* Graph : DelegateGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed %d delegate signature graphs"), DelegateGraphsCopy.Num());

	// 5. Clear interface graphs (but don't remove interfaces themselves)
	int32 InterfaceGraphsCleared = 0;
	for (FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
	{
		for (UEdGraph* Graph : Interface.Graphs)
		{
			if (Graph)
			{
				int32 Removed = ClearGraphNodes(Graph, true);
				InterfaceGraphsCleared += Removed;
			}
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d nodes from interface graphs"), InterfaceGraphsCleared);

	// NOTE: Variables are NOT removed - they remain to satisfy external references (AnimBPs)
	UE_LOG(LogSLFAutomation, Warning, TEXT("KEEPING %d variables (for AnimBP compatibility)"), Blueprint->NewVariables.Num());

	// Refresh BUT DO NOT COMPILE
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	// SKIP compilation - caller will compile all Blueprints at the end

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARED GRAPHS (KEPT VARIABLES, NO COMPILE) FROM %s ==="), *Blueprint->GetName());
	return true;
}

// ============================================================================
// CLEAR CONSTRUCTION SCRIPT
// Clears only the ConstructionScript so C++ OnConstruction can run
// ============================================================================

bool USLFAutomationLibrary::ClearConstructionScript(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARING CONSTRUCTION SCRIPT FROM %s ==="), *Blueprint->GetName());

	// The ConstructionScript is stored as "UserConstructionScript" in FunctionGraphs
	int32 ConstructionScriptCleared = 0;

	// Make a copy since we might be removing from the array
	TArray<UEdGraph*> FunctionGraphsCopy = Blueprint->FunctionGraphs;

	for (UEdGraph* Graph : FunctionGraphsCopy)
	{
		if (Graph)
		{
			FString GraphName = Graph->GetName();
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Found function graph: %s"), *GraphName);

			// Check for ConstructionScript or UserConstructionScript
			if (GraphName.Contains(TEXT("Construction")))
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("  >>> Found ConstructionScript: %s - clearing nodes"), *GraphName);

				// Clear all nodes except entry point
				int32 NodesRemoved = ClearGraphNodes(Graph, true);
				ConstructionScriptCleared += NodesRemoved;

				UE_LOG(LogSLFAutomation, Warning, TEXT("  >>> Cleared %d nodes from %s"), NodesRemoved, *GraphName);
			}
		}
	}

	if (ConstructionScriptCleared > 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d total nodes from ConstructionScript"), ConstructionScriptCleared);

		// Refresh and compile
		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

		FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);

		UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARED CONSTRUCTION SCRIPT FROM %s ==="), *Blueprint->GetName());
	}
	else
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("No ConstructionScript found in %s"), *Blueprint->GetName());
	}

	return true;
}

// ============================================================================
// COMPREHENSIVE MIGRATION
// ============================================================================

bool USLFAutomationLibrary::MigrateToShell(
	UObject* BlueprintAsset,
	const FString& NewParentClassPath,
	const TArray<FString>& VariablesToRemove,
	const TArray<FString>& FunctionsToRemove,
	const TArray<FString>& DispatchersToRemove)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint)
	{
		return false;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== BEGIN SHELL MIGRATION: %s ==="), *Blueprint->GetName());

	bool bSuccess = true;

	// Step 1: Clear ALL Blueprint logic FIRST (before reparenting to avoid type conflicts)
	UE_LOG(LogSLFAutomation, Warning, TEXT("Step 1: Clearing all Blueprint logic (graphs, functions, variables, dispatchers)"));

	// 1a. Clear event graphs first
	int32 EventNodesCleared = ClearEventGraphs(BlueprintAsset);
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Cleared %d nodes from event graphs"), EventNodesCleared);

	// 1b. Remove ALL function graphs (not selective - remove everything)
	TArray<UEdGraph*> FunctionGraphsCopy = Blueprint->FunctionGraphs;
	for (UEdGraph* Graph : FunctionGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Removed %d function graphs"), FunctionGraphsCopy.Num());

	// 1c. Remove ALL macro graphs
	TArray<UEdGraph*> MacroGraphsCopy = Blueprint->MacroGraphs;
	for (UEdGraph* Graph : MacroGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Removed %d macro graphs"), MacroGraphsCopy.Num());

	// 1d. Remove ALL variables (not selective)
	TArray<FName> AllVarNames;
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		AllVarNames.Add(Var.VarName);
	}
	for (const FName& VarName : AllVarNames)
	{
		FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, VarName);
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Removed %d variables"), AllVarNames.Num());

	// 1e. Remove delegate signature graphs
	TArray<UEdGraph*> DelegateGraphsCopy = Blueprint->DelegateSignatureGraphs;
	for (UEdGraph* Graph : DelegateGraphsCopy)
	{
		if (Graph)
		{
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
		}
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Removed %d delegate signature graphs"), DelegateGraphsCopy.Num());

	// Step 2: Reparent to C++ class (now safe - no conflicting nodes/variables)
	UE_LOG(LogSLFAutomation, Warning, TEXT("Step 2: Reparenting to %s"), *NewParentClassPath);
	if (!ReparentBlueprint(BlueprintAsset, NewParentClassPath))
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("Reparenting failed!"));
		bSuccess = false;
	}

	// Step 3: Save without compiling (avoid cascade compilation of dependents)
	UE_LOG(LogSLFAutomation, Warning, TEXT("Step 3: Saving (skipping compile to avoid cascade issues)"));

	// Mark as modified
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	// Save the package without compiling
	UPackage* Package = Blueprint->GetOutermost();
	if (Package)
	{
		Package->MarkPackageDirty();
		FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		bool bSaved = UPackage::SavePackage(Package, Blueprint, *PackageFileName, SaveArgs);

		if (bSaved)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Saved: %s"), *Package->GetName());
		}
		else
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Save result: %s"), *Package->GetName());
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== END SHELL MIGRATION: %s (Success: %s) ==="),
		*Blueprint->GetName(), bSuccess ? TEXT("YES") : TEXT("NO"));

	return bSuccess;
}

bool USLFAutomationLibrary::CompileAndSave(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	FString BlueprintName = Blueprint->GetName(); // Cache name before compile

	// Refresh all nodes to update references
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);

	// Mark as modified
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	// Compile with SkipGarbageCollection to prevent crashes
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);

	// Check if Blueprint is still valid after compile
	if (!IsValid(Blueprint))
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("Blueprint became invalid after compile: %s"), *BlueprintName);
		return false;
	}

	// Check for compile errors
	if (Blueprint->Status == BS_Error)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("Blueprint has compile errors: %s"), *BlueprintName);
		return false;
	}

	// Save the asset
	UPackage* Package = Blueprint->GetOutermost();
	if (Package)
	{
		Package->MarkPackageDirty();
		FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		bool bSaved = UPackage::SavePackage(Package, Blueprint, *PackageFileName, SaveArgs);

		if (bSaved)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("Saved: %s"), *Package->GetName());
		}
		else
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("Save may have failed: %s"), *Package->GetName());
		}
	}

	return true;
}

int32 USLFAutomationLibrary::RemoveImplementedInterfaces(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return 0;

	int32 RemovedCount = Blueprint->ImplementedInterfaces.Num();

	if (RemovedCount > 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Removing %d implemented interfaces from %s"), RemovedCount, *Blueprint->GetName());

		// Log which interfaces are being removed
		for (const FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
		{
			if (Interface.Interface)
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("  - %s"), *Interface.Interface->GetName());
			}
		}

		// Clear the array
		Blueprint->ImplementedInterfaces.Empty();

		// Mark as modified
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	}

	return RemovedCount;
}

// ============================================================================
// VARIABLE REFERENCE REPLACEMENT
// ============================================================================

// Forward declaration for recursive function
static int32 ProcessGraphForVariableReplace(UEdGraph* Graph, FName OldVarName, FName NewVarName, const UEdGraphSchema_K2* Schema, UClass* TargetCppClass, int32 Depth = 0);

// Helper function to process a single variable node
static bool ProcessVariableNode(UK2Node_Variable* VarNode, FName OldVarName, FName NewVarName, const UEdGraphSchema_K2* Schema, UClass* TargetCppClass, const FString& GraphName)
{
	if (!VarNode || !Schema)
	{
		return false;
	}

	// Get variable info
	FName CurrentVarName = VarNode->GetVarName();
	FName RefMemberName = VarNode->VariableReference.GetMemberName();
	bool bIsSelf = VarNode->VariableReference.IsSelfContext();

	// Check if this matches our target variable
	bool bMatches = (CurrentVarName == OldVarName) || (RefMemberName == OldVarName);

	if (!bMatches)
	{
		return false;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("  [MATCH] VarName='%s' in '%s'"),
		*CurrentVarName.ToString(), *GraphName);

	// Cache ALL pin connections before reconstruction
	struct FCachedPin
	{
		FName PinName;
		EEdGraphPinDirection Direction;
		TArray<UEdGraphPin*> LinkedTo;
	};
	TArray<FCachedPin> CachedConnections;

	for (UEdGraphPin* Pin : VarNode->Pins)
	{
		if (Pin && Pin->LinkedTo.Num() > 0)
		{
			FCachedPin Cached;
			Cached.PinName = Pin->PinName;
			Cached.Direction = Pin->Direction;
			Cached.LinkedTo = Pin->LinkedTo;
			CachedConnections.Add(Cached);
		}
	}

	// Find the C++ class that owns the new property
	UClass* NewMemberParent = TargetCppClass;
	if (NewMemberParent)
	{
		FProperty* NewProperty = NewMemberParent->FindPropertyByName(NewVarName);
		if (NewProperty)
		{
			VarNode->VariableReference.SetFromField<FProperty>(NewProperty, bIsSelf);
		}
		else
		{
			VarNode->VariableReference.SetExternalMember(NewVarName, NewMemberParent);
		}
	}
	else if (bIsSelf)
	{
		VarNode->VariableReference.SetSelfMember(NewVarName);
	}

	// Break all links BEFORE reconstruction
	VarNode->BreakAllNodeLinks();

	// Reconstruct the node
	VarNode->ReconstructNode();

	// Restore wire connections
	for (const FCachedPin& Cached : CachedConnections)
	{
		FName NewPinName = (Cached.PinName == OldVarName) ? NewVarName : Cached.PinName;

		UEdGraphPin* NewPin = nullptr;
		for (UEdGraphPin* Pin : VarNode->Pins)
		{
			if (Pin && Pin->PinName == NewPinName && Pin->Direction == Cached.Direction)
			{
				NewPin = Pin;
				break;
			}
		}

		if (!NewPin)
		{
			for (UEdGraphPin* Pin : VarNode->Pins)
			{
				if (Pin && Pin->Direction == Cached.Direction &&
					Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
				{
					NewPin = Pin;
					break;
				}
			}
		}

		if (NewPin)
		{
			for (UEdGraphPin* OtherPin : Cached.LinkedTo)
			{
				if (OtherPin && !NewPin->LinkedTo.Contains(OtherPin))
				{
					Schema->TryCreateConnection(NewPin, OtherPin);
				}
			}
		}
	}

	return true;
}

static int32 ProcessGraphForVariableReplace(UEdGraph* Graph, FName OldVarName, FName NewVarName, const UEdGraphSchema_K2* Schema, UClass* TargetCppClass, int32 Depth)
{
	if (!Graph || !Schema)
	{
		return 0;
	}

	int32 ModifiedCount = 0;
	TArray<UEdGraphNode*> NodesCopy = Graph->Nodes;

	for (UEdGraphNode* Node : NodesCopy)
	{
		if (!Node) continue;

		// Check for collapsed graphs and process recursively
		UK2Node_Composite* CompositeNode = Cast<UK2Node_Composite>(Node);
		if (CompositeNode && CompositeNode->BoundGraph)
		{
			ModifiedCount += ProcessGraphForVariableReplace(CompositeNode->BoundGraph, OldVarName, NewVarName, Schema, TargetCppClass, Depth + 1);
		}

		// Process variable nodes
		UK2Node_Variable* VarNode = Cast<UK2Node_Variable>(Node);
		if (VarNode && ProcessVariableNode(VarNode, OldVarName, NewVarName, Schema, TargetCppClass, Graph->GetName()))
		{
			ModifiedCount++;
		}
	}

	return ModifiedCount;
}

int32 USLFAutomationLibrary::ReplaceVariableReferences(UObject* BlueprintAsset, FName OldVarName, FName NewVarName)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return 0;

	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
	int32 ModifiedCount = 0;

	// Find the C++ class that owns the new property
	UClass* TargetCppClass = nullptr;
	if (Blueprint->ParentClass)
	{
		UClass* TestClass = Blueprint->ParentClass;
		while (TestClass)
		{
			if (TestClass->FindPropertyByName(NewVarName))
			{
				TargetCppClass = TestClass;
				break;
			}
			TestClass = TestClass->GetSuperClass();
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== ReplaceVariableReferences: '%s' -> '%s' in '%s' ==="),
		*OldVarName.ToString(), *NewVarName.ToString(), *Blueprint->GetName());

	// Process all graph types
	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		ModifiedCount += ProcessGraphForVariableReplace(Graph, OldVarName, NewVarName, Schema, TargetCppClass, 0);
	}
	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
	{
		ModifiedCount += ProcessGraphForVariableReplace(Graph, OldVarName, NewVarName, Schema, TargetCppClass, 0);
	}
	for (UEdGraph* Graph : Blueprint->MacroGraphs)
	{
		ModifiedCount += ProcessGraphForVariableReplace(Graph, OldVarName, NewVarName, Schema, TargetCppClass, 0);
	}

	if (ModifiedCount > 0)
	{
		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
		FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);
		if (IsValid(Blueprint))
		{
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Modified %d nodes ==="), ModifiedCount);
	return ModifiedCount;
}

// ============================================================================
// DATA ASSET OPERATIONS
// ============================================================================

bool USLFAutomationLibrary::ApplyEquipSlotsToItem(const FString& ItemAssetPath, const TArray<FString>& SlotTags)
{
	UE_LOG(LogSLFAutomation, Warning, TEXT("ApplyEquipSlotsToItem: %s with %d tags"), *ItemAssetPath, SlotTags.Num());

	// Load the asset
	UObject* Asset = LoadObject<UObject>(nullptr, *ItemAssetPath);
	if (!Asset)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to load asset: %s"), *ItemAssetPath);
		return false;
	}

	// Get the ItemInformation property
	FStructProperty* ItemInfoProp = FindFProperty<FStructProperty>(Asset->GetClass(), TEXT("ItemInformation"));
	if (!ItemInfoProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No ItemInformation property found"));
		return false;
	}

	void* ItemInfoPtr = ItemInfoProp->ContainerPtrToValuePtr<void>(Asset);
	if (!ItemInfoPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get ItemInformation pointer"));
		return false;
	}

	// Get EquipmentDetails from ItemInformation
	UScriptStruct* ItemInfoStruct = ItemInfoProp->Struct;
	FStructProperty* EquipDetailsProp = FindFProperty<FStructProperty>(ItemInfoStruct, TEXT("EquipmentDetails"));
	if (!EquipDetailsProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No EquipmentDetails property found"));
		return false;
	}

	void* EquipDetailsPtr = EquipDetailsProp->ContainerPtrToValuePtr<void>(ItemInfoPtr);
	if (!EquipDetailsPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get EquipmentDetails pointer"));
		return false;
	}

	// Get EquipSlots from EquipmentDetails
	UScriptStruct* EquipDetailsStruct = EquipDetailsProp->Struct;
	FStructProperty* EquipSlotsProp = FindFProperty<FStructProperty>(EquipDetailsStruct, TEXT("EquipSlots"));
	if (!EquipSlotsProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No EquipSlots property found"));
		return false;
	}

	void* EquipSlotsPtr = EquipSlotsProp->ContainerPtrToValuePtr<void>(EquipDetailsPtr);
	if (!EquipSlotsPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get EquipSlots pointer"));
		return false;
	}

	// Cast to FGameplayTagContainer
	FGameplayTagContainer* TagContainer = static_cast<FGameplayTagContainer*>(EquipSlotsPtr);

	// Add each tag
	int32 AddedCount = 0;
	for (const FString& TagStr : SlotTags)
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagStr), false);
		if (Tag.IsValid())
		{
			TagContainer->AddTag(Tag);
			AddedCount++;
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Added tag: %s"), *TagStr);
		}
		else
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Invalid tag (not registered): %s"), *TagStr);
		}
	}

	if (AddedCount == 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  No tags added"));
		return false;
	}

	// Mark package dirty and save
	Asset->MarkPackageDirty();

	UPackage* Package = Asset->GetOutermost();
	FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	SaveArgs.Error = GError;

	bool bSaved = UPackage::SavePackage(Package, Asset, *PackageFileName, SaveArgs);

	if (bSaved)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Saved successfully with %d tags"), AddedCount);
	}
	else
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to save"));
	}

	return bSaved;
}

bool USLFAutomationLibrary::ApplySkeletalMeshInfoToItem(const FString& ItemAssetPath, const TArray<FString>& CharacterAssetNames, const TArray<FString>& MeshPaths)
{
	UE_LOG(LogSLFAutomation, Warning, TEXT("ApplySkeletalMeshInfoToItem: %s with %d entries"), *ItemAssetPath, CharacterAssetNames.Num());

	if (CharacterAssetNames.Num() != MeshPaths.Num())
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  CharacterAssetNames and MeshPaths arrays must have same length"));
		return false;
	}

	if (CharacterAssetNames.Num() == 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  No mesh entries to apply"));
		return true;
	}

	// Load the asset
	UObject* Asset = LoadObject<UObject>(nullptr, *ItemAssetPath);
	if (!Asset)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to load asset: %s"), *ItemAssetPath);
		return false;
	}

	// Get the ItemInformation property
	FStructProperty* ItemInfoProp = FindFProperty<FStructProperty>(Asset->GetClass(), TEXT("ItemInformation"));
	if (!ItemInfoProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No ItemInformation property found"));
		return false;
	}

	void* ItemInfoPtr = ItemInfoProp->ContainerPtrToValuePtr<void>(Asset);
	if (!ItemInfoPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get ItemInformation pointer"));
		return false;
	}

	// Get EquipmentDetails from ItemInformation
	UScriptStruct* ItemInfoStruct = ItemInfoProp->Struct;
	FStructProperty* EquipDetailsProp = FindFProperty<FStructProperty>(ItemInfoStruct, TEXT("EquipmentDetails"));
	if (!EquipDetailsProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No EquipmentDetails property found"));
		return false;
	}

	void* EquipDetailsPtr = EquipDetailsProp->ContainerPtrToValuePtr<void>(ItemInfoPtr);
	if (!EquipDetailsPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get EquipmentDetails pointer"));
		return false;
	}

	// Get SkeletalMeshInfo from EquipmentDetails
	UScriptStruct* EquipDetailsStruct = EquipDetailsProp->Struct;
	FMapProperty* MeshInfoProp = FindFProperty<FMapProperty>(EquipDetailsStruct, TEXT("SkeletalMeshInfo"));
	if (!MeshInfoProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No SkeletalMeshInfo property found"));
		return false;
	}

	void* MeshInfoPtr = MeshInfoProp->ContainerPtrToValuePtr<void>(EquipDetailsPtr);
	if (!MeshInfoPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get SkeletalMeshInfo pointer"));
		return false;
	}

	// The TMap is: TMap<TSoftObjectPtr<UPrimaryDataAsset>, TSoftObjectPtr<USkeletalMesh>>
	// We need to use reflection to add entries
	FScriptMapHelper MapHelper(MeshInfoProp, MeshInfoPtr);

	int32 AddedCount = 0;
	for (int32 i = 0; i < CharacterAssetNames.Num(); i++)
	{
		const FString& CharName = CharacterAssetNames[i];
		const FString& MeshPath = MeshPaths[i];

		// Find the character data asset - try common paths
		FString CharAssetPath = FString::Printf(TEXT("/Game/SoulslikeFramework/Data/BaseCharacters/%s.%s"), *CharName, *CharName);
		UPrimaryDataAsset* CharAsset = LoadObject<UPrimaryDataAsset>(nullptr, *CharAssetPath);

		if (!CharAsset)
		{
			// Try Characters subfolder
			CharAssetPath = FString::Printf(TEXT("/Game/SoulslikeFramework/Data/Characters/%s.%s"), *CharName, *CharName);
			CharAsset = LoadObject<UPrimaryDataAsset>(nullptr, *CharAssetPath);
		}

		if (!CharAsset)
		{
			// Try directly in Data folder
			CharAssetPath = FString::Printf(TEXT("/Game/SoulslikeFramework/Data/%s.%s"), *CharName, *CharName);
			CharAsset = LoadObject<UPrimaryDataAsset>(nullptr, *CharAssetPath);
		}

		if (!CharAsset)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Could not find character asset: %s"), *CharName);
			continue;
		}

		// Load the skeletal mesh
		USkeletalMesh* SkMesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath);
		if (!SkMesh)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Could not load skeletal mesh: %s"), *MeshPath);
			continue;
		}

		// Create soft object pointers
		TSoftObjectPtr<UPrimaryDataAsset> CharSoftPtr(CharAsset);
		TSoftObjectPtr<USkeletalMesh> MeshSoftPtr(SkMesh);

		// Add to the map using the helper
		// MapHelper doesn't work well with soft object ptrs, so we'll use direct cast approach
		typedef TMap<TSoftObjectPtr<UPrimaryDataAsset>, TSoftObjectPtr<USkeletalMesh>> MeshMapType;
		MeshMapType* ActualMap = static_cast<MeshMapType*>(MeshInfoPtr);

		ActualMap->Add(CharSoftPtr, MeshSoftPtr);
		AddedCount++;

		UE_LOG(LogSLFAutomation, Warning, TEXT("  Added mesh mapping: %s -> %s"), *CharName, *SkMesh->GetName());
	}

	if (AddedCount == 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  No mesh entries added"));
		return false;
	}

	// Mark package dirty and save
	Asset->MarkPackageDirty();

	UPackage* Package = Asset->GetOutermost();
	FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	SaveArgs.Error = GError;

	bool bSaved = UPackage::SavePackage(Package, Asset, *PackageFileName, SaveArgs);

	if (bSaved)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Saved successfully with %d mesh entries"), AddedCount);
	}
	else
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to save"));
	}

	return bSaved;
}

#endif // WITH_EDITOR
