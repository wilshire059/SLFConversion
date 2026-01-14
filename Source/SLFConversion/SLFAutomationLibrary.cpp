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
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
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
// AnimBP LinkedAnimLayer node support (AAA-quality fix)
#include "AnimGraphNode_LinkedAnimLayer.h"
#include "Animation/AnimNode_LinkedAnimLayer.h"
// AnimBP State Machine support for variable reference replacement
#include "AnimStateTransitionNode.h"
#include "AnimStateNodeBase.h"
#include "AnimGraphNode_TransitionResult.h"
#include "AnimGraphNode_BlendListByEnum.h"
// K2Node types for transition graph repair
#include "K2Node_CallFunction.h"
#include "K2Node_CommutativeAssociativeBinaryOperator.h"
// BehaviorTree support for AI migration
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "SLFEnums.h"

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

bool USLFAutomationLibrary::RemoveSCSComponentByClass(UObject* BlueprintAsset, const FString& ComponentClassName)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
	if (!SCS)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Blueprint %s has no SimpleConstructionScript"), *Blueprint->GetName());
		return false;
	}

	TArray<USCS_Node*> AllNodes = SCS->GetAllNodes();
	USCS_Node* NodeToRemove = nullptr;

	for (USCS_Node* Node : AllNodes)
	{
		if (Node && Node->ComponentTemplate)
		{
			FString ClassName = Node->ComponentTemplate->GetClass()->GetName();
			if (ClassName.Contains(ComponentClassName))
			{
				NodeToRemove = Node;
				UE_LOG(LogSLFAutomation, Warning, TEXT("Found SCS component to remove: %s (%s)"),
					*Node->ComponentTemplate->GetName(), *ClassName);
				break;
			}
		}
	}

	if (!NodeToRemove)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("No SCS component matching %s found"), *ComponentClassName);
		return false;
	}

	// Remove the node from SCS
	SCS->RemoveNode(NodeToRemove);

	// Mark blueprint as modified
	Blueprint->Modify();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed SCS component from %s"), *Blueprint->GetName());
	return true;
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

int32 USLFAutomationLibrary::RemoveAllVariables(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return 0;

	int32 RemovedCount = Blueprint->NewVariables.Num();

	if (RemovedCount == 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("No variables to remove from %s"), *Blueprint->GetName());
		return 0;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("Removing ALL %d variables from %s"), RemovedCount, *Blueprint->GetName());

	// Get all variable names first (to use proper removal API)
	TArray<FName> VarNamesToRemove;
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		VarNamesToRemove.Add(Var.VarName);
		UE_LOG(LogSLFAutomation, Log, TEXT("  - %s (%s)"), *Var.VarName.ToString(), *Var.VarType.PinCategory.ToString());
	}

	// Remove each variable using the proper API
	for (const FName& VarName : VarNamesToRemove)
	{
		FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, VarName);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("Removed ALL %d variables from %s"), RemovedCount, *Blueprint->GetName());
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

	// Log ALL variable get nodes to understand the naming
	FString CurrentStr = CurrentVarName.ToString();
	FString RefStr = RefMemberName.ToString();
	FString OldStr = OldVarName.ToString();

	// Debug: log first few nodes with target-like names
	if (CurrentStr.Contains(TEXT("Accelerating")) || CurrentStr.Contains(TEXT("Blocking")) ||
		CurrentStr.Contains(TEXT("Falling")) || CurrentStr.Contains(TEXT("Resting")) ||
		RefStr.Contains(TEXT("Accelerating")) || RefStr.Contains(TEXT("Blocking")) ||
		RefStr.Contains(TEXT("Falling")) || RefStr.Contains(TEXT("Resting")))
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("    [DEBUG] VarNode: GetVarName='%s', MemberName='%s', looking for OldVarName='%s'"),
			*CurrentStr, *RefStr, *OldStr);
	}

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

			// Log what we're caching
			for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Caching connection: '%s' (%s) -> '%s' on '%s'"),
					*Pin->PinName.ToString(),
					Pin->Direction == EGPD_Input ? TEXT("IN") : TEXT("OUT"),
					*LinkedPin->PinName.ToString(),
					LinkedPin->GetOwningNode() ? *LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString() : TEXT("Unknown"));
			}
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

	// Log pins after reconstruction
	UE_LOG(LogSLFAutomation, Warning, TEXT("    After reconstruction, node has %d pins:"), VarNode->Pins.Num());
	for (UEdGraphPin* Pin : VarNode->Pins)
	{
		if (Pin)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("      Pin '%s' (%s) type=%s"),
				*Pin->PinName.ToString(),
				Pin->Direction == EGPD_Input ? TEXT("IN") : TEXT("OUT"),
				*Pin->PinType.PinCategory.ToString());
		}
	}

	// Restore wire connections
	for (const FCachedPin& Cached : CachedConnections)
	{
		FName NewPinName = (Cached.PinName == OldVarName) ? NewVarName : Cached.PinName;
		UE_LOG(LogSLFAutomation, Warning, TEXT("    Looking for pin: '%s' (from cached '%s', OldVarName='%s', NewVarName='%s')"),
			*NewPinName.ToString(), *Cached.PinName.ToString(), *OldVarName.ToString(), *NewVarName.ToString());

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
					bool bConnected = Schema->TryCreateConnection(NewPin, OtherPin);
					if (bConnected)
					{
						UE_LOG(LogSLFAutomation, Warning, TEXT("    [OK] Reconnected '%s' -> '%s' (on node '%s')"),
							*NewPin->PinName.ToString(),
							*OtherPin->PinName.ToString(),
							OtherPin->GetOwningNode() ? *OtherPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString() : TEXT("Unknown"));
					}
					else
					{
						UE_LOG(LogSLFAutomation, Error, TEXT("    [FAIL] Could not reconnect '%s' (type: %s) -> '%s' (type: %s)"),
							*NewPin->PinName.ToString(),
							*NewPin->PinType.PinCategory.ToString(),
							*OtherPin->PinName.ToString(),
							*OtherPin->PinType.PinCategory.ToString());
					}
				}
			}
		}
		else
		{
			UE_LOG(LogSLFAutomation, Error, TEXT("    [FAIL] Could not find NewPin for cached '%s' (%s)"),
				*Cached.PinName.ToString(),
				Cached.Direction == EGPD_Input ? TEXT("Input") : TEXT("Output"));
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

		// Process AnimBP state transition graphs (CRITICAL for locomotion state machine)
		UAnimStateTransitionNode* TransitionNode = Cast<UAnimStateTransitionNode>(Node);
		if (TransitionNode && TransitionNode->BoundGraph)
		{
			ModifiedCount += ProcessGraphForVariableReplace(TransitionNode->BoundGraph, OldVarName, NewVarName, Schema, TargetCppClass, Depth + 1);
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

	// First try Blueprint->ParentClass (works for regular Blueprints)
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

	// For AnimBlueprints, ParentClass may be null - search through generated class's parent chain
	if (!TargetCppClass && Blueprint->GeneratedClass)
	{
		UClass* TestClass = Blueprint->GeneratedClass->GetSuperClass();
		while (TestClass)
		{
			if (TestClass->FindPropertyByName(NewVarName))
			{
				TargetCppClass = TestClass;
				UE_LOG(LogSLFAutomation, Warning, TEXT("  Found property '%s' in class '%s' (via GeneratedClass parent chain)"),
					*NewVarName.ToString(), *TestClass->GetName());
				break;
			}
			TestClass = TestClass->GetSuperClass();
		}
	}

	// Special case for AnimBlueprints: Try to load the matching C++ AnimInstance class
	// (AnimBP "ABP_SoulslikeCharacter_Additive" -> C++ class "UABP_SoulslikeCharacter_Additive")
	if (!TargetCppClass)
	{
		UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(Blueprint);
		if (AnimBP)
		{
			// Build the expected C++ class path: /Script/SLFConversion.ABP_SoulslikeCharacter_Additive
			FString BlueprintName = Blueprint->GetName();
			FString CppClassPath = FString::Printf(TEXT("/Script/SLFConversion.%s"), *BlueprintName);

			UClass* CppAnimInstanceClass = LoadObject<UClass>(nullptr, *CppClassPath);
			if (CppAnimInstanceClass && CppAnimInstanceClass->FindPropertyByName(NewVarName))
			{
				TargetCppClass = CppAnimInstanceClass;
				UE_LOG(LogSLFAutomation, Warning, TEXT("  Found property '%s' in C++ AnimInstance class '%s'"),
					*NewVarName.ToString(), *CppAnimInstanceClass->GetName());
			}
		}
	}

	if (!TargetCppClass)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  WARNING: Could not find C++ class with property '%s' - will use self-member fallback"), *NewVarName.ToString());
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== ReplaceVariableReferences: '%s' -> '%s' in '%s' ==="),
		*OldVarName.ToString(), *NewVarName.ToString(), *Blueprint->GetName());

	// Check if this is an AnimBlueprint - if so, use GetAllGraphs to include transition graphs
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(Blueprint);
	if (AnimBP)
	{
		// For AnimBlueprints, GetAllGraphs() returns ALL graphs including transition graphs inside state machines
		TArray<UEdGraph*> AllGraphs;
		AnimBP->GetAllGraphs(AllGraphs);
		UE_LOG(LogSLFAutomation, Warning, TEXT("  AnimBlueprint detected - processing %d total graphs (including transitions)"), AllGraphs.Num());

		for (UEdGraph* Graph : AllGraphs)
		{
			if (Graph)
			{
				int32 BeforeCount = ModifiedCount;
				ModifiedCount += ProcessGraphForVariableReplace(Graph, OldVarName, NewVarName, Schema, TargetCppClass, 0);
				if (ModifiedCount > BeforeCount)
				{
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Graph '%s': fixed %d references"), *Graph->GetName(), ModifiedCount - BeforeCount);
				}
			}
		}
	}
	else
	{
		// Regular Blueprint - process standard graph types
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
// VARIABLE RENAMING (For "?" suffix fix)
// ============================================================================

bool USLFAutomationLibrary::RenameVariable(UObject* BlueprintAsset, const FString& OldVarName, const FString& NewVarName)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint)
	{
		return false;
	}

	FName OldName(*OldVarName);
	FName NewName(*NewVarName);

	UE_LOG(LogSLFAutomation, Warning, TEXT("RenameVariable: '%s' -> '%s' in '%s'"),
		*OldVarName, *NewVarName, *Blueprint->GetName());

	// Check if variable exists
	bool bFound = false;
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		if (Var.VarName == OldName)
		{
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Variable '%s' not found in Blueprint"), *OldVarName);
		return false;
	}

	// Use FBlueprintEditorUtils::RenameMemberVariable which properly updates all references
	FBlueprintEditorUtils::RenameMemberVariable(Blueprint, OldName, NewName);

	// Compile to apply changes
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);

	UE_LOG(LogSLFAutomation, Warning, TEXT("  Successfully renamed '%s' to '%s'"), *OldVarName, *NewVarName);
	return true;
}

int32 USLFAutomationLibrary::RenameVariables(UObject* BlueprintAsset, const TArray<FString>& OldNames, const TArray<FString>& NewNames)
{
	if (OldNames.Num() != NewNames.Num())
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("RenameVariables: OldNames and NewNames arrays must have same length"));
		return 0;
	}

	int32 SuccessCount = 0;
	for (int32 i = 0; i < OldNames.Num(); ++i)
	{
		if (RenameVariable(BlueprintAsset, OldNames[i], NewNames[i]))
		{
			++SuccessCount;
		}
	}

	return SuccessCount;
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

FString USLFAutomationLibrary::GetWeaponOverlayTag(const FString& ItemAssetPath)
{
	UE_LOG(LogSLFAutomation, Warning, TEXT("GetWeaponOverlayTag: %s"), *ItemAssetPath);

	// Load the asset
	UObject* Asset = LoadObject<UObject>(nullptr, *ItemAssetPath);
	if (!Asset)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to load asset: %s"), *ItemAssetPath);
		return TEXT("");
	}

	// Get the ItemInformation property
	FStructProperty* ItemInfoProp = FindFProperty<FStructProperty>(Asset->GetClass(), TEXT("ItemInformation"));
	if (!ItemInfoProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No ItemInformation property found"));
		return TEXT("");
	}

	void* ItemInfoPtr = ItemInfoProp->ContainerPtrToValuePtr<void>(Asset);
	if (!ItemInfoPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get ItemInformation pointer"));
		return TEXT("");
	}

	// Get EquipmentDetails from ItemInformation
	UScriptStruct* ItemInfoStruct = ItemInfoProp->Struct;
	FStructProperty* EquipDetailsProp = FindFProperty<FStructProperty>(ItemInfoStruct, TEXT("EquipmentDetails"));
	if (!EquipDetailsProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No EquipmentDetails property found"));
		return TEXT("");
	}

	void* EquipDetailsPtr = EquipDetailsProp->ContainerPtrToValuePtr<void>(ItemInfoPtr);
	if (!EquipDetailsPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get EquipmentDetails pointer"));
		return TEXT("");
	}

	// Get WeaponOverlay from EquipmentDetails (it's a FGameplayTag)
	UScriptStruct* EquipDetailsStruct = EquipDetailsProp->Struct;
	FStructProperty* OverlayProp = FindFProperty<FStructProperty>(EquipDetailsStruct, TEXT("WeaponOverlay"));
	if (!OverlayProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No WeaponOverlay property found"));
		return TEXT("");
	}

	void* OverlayPtr = OverlayProp->ContainerPtrToValuePtr<void>(EquipDetailsPtr);
	if (!OverlayPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get WeaponOverlay pointer"));
		return TEXT("");
	}

	// Cast to FGameplayTag
	FGameplayTag* Tag = static_cast<FGameplayTag*>(OverlayPtr);
	if (Tag->IsValid())
	{
		FString TagStr = Tag->ToString();
		UE_LOG(LogSLFAutomation, Warning, TEXT("  WeaponOverlay: %s"), *TagStr);
		return TagStr;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("  No WeaponOverlay tag set"));
	return TEXT("");
}

bool USLFAutomationLibrary::ApplyWeaponOverlayTag(const FString& ItemAssetPath, const FString& OverlayTagString)
{
	UE_LOG(LogSLFAutomation, Warning, TEXT("ApplyWeaponOverlayTag: %s -> %s"), *ItemAssetPath, *OverlayTagString);

	if (OverlayTagString.IsEmpty())
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Empty overlay tag string, skipping"));
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

	// Get WeaponOverlay from EquipmentDetails
	UScriptStruct* EquipDetailsStruct = EquipDetailsProp->Struct;
	FStructProperty* OverlayProp = FindFProperty<FStructProperty>(EquipDetailsStruct, TEXT("WeaponOverlay"));
	if (!OverlayProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  No WeaponOverlay property found"));
		return false;
	}

	void* OverlayPtr = OverlayProp->ContainerPtrToValuePtr<void>(EquipDetailsPtr);
	if (!OverlayPtr)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not get WeaponOverlay pointer"));
		return false;
	}

	// Request the gameplay tag
	FGameplayTag NewTag = FGameplayTag::RequestGameplayTag(FName(*OverlayTagString), false);
	if (!NewTag.IsValid())
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Invalid tag (not registered): %s"), *OverlayTagString);
		return false;
	}

	// Set the tag
	FGameplayTag* Tag = static_cast<FGameplayTag*>(OverlayPtr);
	*Tag = NewTag;

	UE_LOG(LogSLFAutomation, Warning, TEXT("  Set WeaponOverlay to: %s"), *OverlayTagString);

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
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Saved successfully"));
	}
	else
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to save"));
	}

	return bSaved;
}

// Include AnimBP Export/Diff/Fix implementations
#include "SLFAnimBPExport.inl"


// ============================================================================
// MONTAGE NOTIFY FIXING
// ============================================================================

int32 USLFAutomationLibrary::FixMontageComboNotifies(UObject* MontageAsset)
{
	UAnimMontage* Montage = Cast<UAnimMontage>(MontageAsset);
	if (!Montage)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("FixMontageComboNotifies: Invalid montage asset"));
		return 0;
	}

	int32 FixedCount = 0;
	FString MontageName = Montage->GetName();

	UE_LOG(LogSLFAutomation, Warning, TEXT("Processing montage: %s"), *MontageName);

	// Enum value to section name mapping
	static const TMap<int32, FName> EnumToSection = {
		{0, FName("Light_01")}, {1, FName("Light_02")}, {2, FName("Light_03")}, {3, FName("Light_04")},
		{4, FName("Light_05")}, {5, FName("Light_06")}, {6, FName("Light_07")}, {7, FName("Light_08")},
		{8, FName("Light_09")}, {9, FName("Light_10")}, {10, FName("Light_11")}, {11, FName("Light_12")},
		{12, FName("Light_13")}, {13, FName("Light_14")}, {14, FName("Light_15")},
		{15, FName("Heavy_01")}, {16, FName("Heavy_02")}, {17, FName("Heavy_03")}, {18, FName("Heavy_04")},
		{19, FName("Heavy_05")}, {20, FName("Heavy_06")}, {21, FName("Heavy_07")}, {22, FName("Heavy_08")},
		{23, FName("Heavy_09")}, {24, FName("Heavy_10")}, {25, FName("Heavy_11")}, {26, FName("Heavy_12")},
		{27, FName("Heavy_13")}, {28, FName("Heavy_14")}, {29, FName("Heavy_15")},
		{30, FName("Charge_01")}, {31, FName("Charge_02")}, {32, FName("Charge_03")}, {33, FName("Charge_04")},
		{34, FName("Charge_05")}, {35, FName("Charge_06")}, {36, FName("Charge_07")}, {37, FName("Charge_08")},
		{38, FName("Charge_09")}, {39, FName("Charge_10")}
	};

	// Iterate through all AnimNotifyState events
	for (FAnimNotifyEvent& NotifyEvent : Montage->Notifies)
	{
		UAnimNotifyState* NotifyState = NotifyEvent.NotifyStateClass;
		if (!NotifyState)
		{
			continue;
		}

		// Check if this is ANS_RegisterAttackSequence
		FString ClassName = NotifyState->GetClass()->GetName();
		if (!ClassName.Contains(TEXT("RegisterAttackSequence")))
		{
			continue;
		}

		UE_LOG(LogSLFAutomation, Warning, TEXT("  Found ANS_RegisterAttackSequence notify"));

		// Get the QueuedSection property
		FProperty* QueuedSectionProp = NotifyState->GetClass()->FindPropertyByName(FName("QueuedSection"));
		if (!QueuedSectionProp)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Could not find QueuedSection property"));
			continue;
		}

		// Check if it's an FName property (our new type)
		FNameProperty* NameProp = CastField<FNameProperty>(QueuedSectionProp);
		if (NameProp)
		{
			// Already FName type - read current value
			FName* CurrentValue = NameProp->ContainerPtrToValuePtr<FName>(NotifyState);
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Current QueuedSection (FName): %s"), *CurrentValue->ToString());

			// If empty or invalid, try to determine from montage section structure
			if (CurrentValue->IsNone() || CurrentValue->ToString().IsEmpty())
			{
				// Get the section this notify is in
				float NotifyTime = NotifyEvent.GetTriggerTime();
				int32 SectionIdx = Montage->GetSectionIndexFromPosition(NotifyTime);

				if (SectionIdx != INDEX_NONE)
				{
					FName CurrentSectionName = Montage->GetSectionName(SectionIdx);
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Notify is in section: %s (index %d)"), *CurrentSectionName.ToString(), SectionIdx);

					// Determine next section based on current section name
					FString SectionStr = CurrentSectionName.ToString();
					FString NextSection;

					// Parse section name to determine next (e.g., Light_01 -> Light_02)
					if (SectionStr.StartsWith(TEXT("Light_")) || SectionStr.StartsWith(TEXT("Heavy_")) || SectionStr.StartsWith(TEXT("Charge_")))
					{
						int32 UnderscoreIdx;
						if (SectionStr.FindLastChar('_', UnderscoreIdx))
						{
							FString Prefix = SectionStr.Left(UnderscoreIdx + 1);
							FString NumStr = SectionStr.Mid(UnderscoreIdx + 1);
							int32 Num = FCString::Atoi(*NumStr);
							NextSection = FString::Printf(TEXT("%s%02d"), *Prefix, Num + 1);
						}
					}

					if (!NextSection.IsEmpty())
					{
						*CurrentValue = FName(*NextSection);
						UE_LOG(LogSLFAutomation, Warning, TEXT("    Set QueuedSection to: %s"), *NextSection);
						FixedCount++;
					}
				}
			}
			else
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    QueuedSection already has valid value"));
			}
		}
		else
		{
			// Check if it's still an enum/byte property (old type)
			FByteProperty* ByteProp = CastField<FByteProperty>(QueuedSectionProp);
			FEnumProperty* EnumProp = CastField<FEnumProperty>(QueuedSectionProp);

			if (ByteProp || EnumProp)
			{
				// Read the raw byte value
				uint8* ValuePtr = QueuedSectionProp->ContainerPtrToValuePtr<uint8>(NotifyState);
				int32 EnumValue = static_cast<int32>(*ValuePtr);
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Current QueuedSection (enum): %d"), EnumValue);

				// Map to section name
				const FName* SectionName = EnumToSection.Find(EnumValue);
				if (SectionName)
				{
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Maps to section: %s"), *SectionName->ToString());
				}
				else
				{
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Invalid enum value, no mapping found"));
				}
			}
			else
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Unexpected property type for QueuedSection"));
			}
		}
	}

	if (FixedCount > 0)
	{
		Montage->MarkPackageDirty();

		UPackage* Package = Montage->GetOutermost();
		FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		SaveArgs.Error = GError;

		bool bSaved = UPackage::SavePackage(Package, Montage, *PackageFileName, SaveArgs);

		if (bSaved)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Saved %s with %d fixes"), *MontageName, FixedCount);
		}
	}

	return FixedCount;
}

int32 USLFAutomationLibrary::FixAllMontageComboNotifies()
{
	int32 TotalFixed = 0;

	// Get all AnimMontage assets
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassPaths.Add(UAnimMontage::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add(FName("/Game/SoulslikeFramework"));

	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssets(Filter, AssetDataList);

	UE_LOG(LogSLFAutomation, Warning, TEXT("Found %d AnimMontage assets"), AssetDataList.Num());

	for (const FAssetData& AssetData : AssetDataList)
	{
		FString AssetPath = AssetData.PackageName.ToString();

		// Only process combat montages
		if (!AssetPath.Contains(TEXT("Combat")) && !AssetPath.Contains(TEXT("Light")) && !AssetPath.Contains(TEXT("Heavy")))
		{
			continue;
		}

		UAnimMontage* Montage = Cast<UAnimMontage>(AssetData.GetAsset());
		if (Montage)
		{
			TotalFixed += FixMontageComboNotifies(Montage);
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("Total notifies fixed: %d"), TotalFixed);
	return TotalFixed;
}

// ============================================================================
// FIX BT STATE DECORATOR INTVALUE
// ============================================================================

FString USLFAutomationLibrary::FixBTStateDecoratorToValue(const FString& BehaviorTreePath, int32 ExpectedStateValue)
{
	FString Result;

	UBehaviorTree* BT = LoadObject<UBehaviorTree>(nullptr, *BehaviorTreePath);
	if (!BT)
	{
		return FString::Printf(TEXT("ERROR: Could not load BehaviorTree: %s"), *BehaviorTreePath);
	}

	Result += FString::Printf(TEXT("BehaviorTree: %s\n"), *BT->GetName());
	Result += FString::Printf(TEXT("Expected State value: %d\n\n"), ExpectedStateValue);

	bool bMadeChanges = false;

	// Process root decorators
	for (int32 i = 0; i < BT->RootDecorators.Num(); ++i)
	{
		UBTDecorator_Blackboard* BBDec = Cast<UBTDecorator_Blackboard>(BT->RootDecorators[i]);
		if (!BBDec) continue;

		FName KeyName = BBDec->GetSelectedBlackboardKey();
		if (KeyName != FName("State")) continue;

		Result += FString::Printf(TEXT("[%d] %s - Key: %s\n"), i, *BBDec->GetNodeName(), *KeyName.ToString());

		// Use reflection to access IntValue (protected member)
		if (FIntProperty* IntValueProp = CastField<FIntProperty>(BBDec->GetClass()->FindPropertyByName(TEXT("IntValue"))))
		{
			int32 CurrentValue = IntValueProp->GetPropertyValue_InContainer(BBDec);
			Result += FString::Printf(TEXT("    Current IntValue: %d\n"), CurrentValue);

			if (CurrentValue != ExpectedStateValue)
			{
				IntValueProp->SetPropertyValue_InContainer(BBDec, ExpectedStateValue);
				Result += FString::Printf(TEXT("    FIXED: Set IntValue from %d to %d\n"), CurrentValue, ExpectedStateValue);
				bMadeChanges = true;
			}
			else
			{
				Result += TEXT("    OK: IntValue already correct\n");
			}
		}
		Result += TEXT("\n");
	}

	// Save if changes were made
	if (bMadeChanges)
	{
		BT->MarkPackageDirty();
		FString PackageName = BT->GetOutermost()->GetName();
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
		bool bSaved = UPackage::SavePackage(BT->GetOutermost(), BT, *FilePath, SaveArgs);

		if (bSaved)
		{
			Result += TEXT("Saved BehaviorTree with fixes.\n");
		}
		else
		{
			Result += TEXT("Failed to save BehaviorTree!\n");
		}
	}
	else
	{
		Result += TEXT("No changes needed.\n");
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("%s"), *Result);
	return Result;
}

// Helper to recursively process BT nodes and extract decorator info
static void ProcessBTNodeDecorators(UBTCompositeNode* Node, FString& Result, int32 Depth, int32& TotalDecorators, int32& StateDecorators, int32& TotalTasks)
{
	if (!Node) return;

	FString Indent = FString::ChrN(Depth * 2, TEXT(' '));
	Result += FString::Printf(TEXT("%sNode: %s (%s)\n"), *Indent, *Node->GetNodeName(), *Node->GetClass()->GetName());

	// Process children and their decorators
	int32 NumChildren = Node->GetChildrenNum();
	Result += FString::Printf(TEXT("%s  NumChildren: %d\n"), *Indent, NumChildren);

	for (int32 i = 0; i < NumChildren; ++i)
	{
		// Get child info from Children array
		const FBTCompositeChild& Child = Node->Children[i];

		// Get child name and type
		FString ChildName = TEXT("Unknown");
		FString ChildType = TEXT("?");
		if (Child.ChildTask)
		{
			ChildName = Child.ChildTask->GetNodeName();
			ChildType = Child.ChildTask->GetClass()->GetName();
			TotalTasks++;
		}
		else if (Child.ChildComposite)
		{
			ChildName = Child.ChildComposite->GetNodeName();
			ChildType = Child.ChildComposite->GetClass()->GetName();
		}

		Result += FString::Printf(TEXT("%s  [Branch %d] Child: %s (%s), Decorators: %d\n"),
			*Indent, i, *ChildName, *ChildType, Child.Decorators.Num());

		// Process child's decorators
		for (int32 j = 0; j < Child.Decorators.Num(); ++j)
		{
			UBTDecorator* Decorator = Child.Decorators[j];
			if (!Decorator) continue;

			TotalDecorators++;

			UBTDecorator_Blackboard* BBDec = Cast<UBTDecorator_Blackboard>(Decorator);
			if (BBDec)
			{
				FName KeyName = BBDec->GetSelectedBlackboardKey();
				Result += FString::Printf(TEXT("%s      Decorator[%d]: %s - Key: %s\n"),
					*Indent, j, *Decorator->GetNodeName(), *KeyName.ToString());

				// Get IntValue via reflection
				if (FIntProperty* IntValueProp = CastField<FIntProperty>(BBDec->GetClass()->FindPropertyByName(TEXT("IntValue"))))
				{
					int32 IntValue = IntValueProp->GetPropertyValue_InContainer(BBDec);
					Result += FString::Printf(TEXT("%s        IntValue: %d\n"), *Indent, IntValue);

					if (KeyName == FName("State"))
					{
						StateDecorators++;
						Result += FString::Printf(TEXT("%s        *** STATE KEY DECORATOR: IntValue=%d ***\n"), *Indent, IntValue);
					}
				}
			}
			else
			{
				Result += FString::Printf(TEXT("%s      Decorator[%d]: %s (%s)\n"),
					*Indent, j, *Decorator->GetNodeName(), *Decorator->GetClass()->GetName());
			}
		}

		// Recurse into child composites
		if (Child.ChildComposite)
		{
			ProcessBTNodeDecorators(Child.ChildComposite, Result, Depth + 1, TotalDecorators, StateDecorators, TotalTasks);
		}
	}
}

FString USLFAutomationLibrary::ExportBTAllDecoratorIntValues(const FString& BehaviorTreePath)
{
	FString Result;

	UBehaviorTree* BT = LoadObject<UBehaviorTree>(nullptr, *BehaviorTreePath);
	if (!BT)
	{
		return FString::Printf(TEXT("ERROR: Could not load BehaviorTree: %s"), *BehaviorTreePath);
	}

	Result += FString::Printf(TEXT("=== BehaviorTree: %s ===\n\n"), *BT->GetName());

	int32 TotalDecorators = 0;
	int32 StateDecorators = 0;

	// Process root decorators
	Result += TEXT("--- Root Decorators ---\n");
	for (int32 i = 0; i < BT->RootDecorators.Num(); ++i)
	{
		UBTDecorator* Decorator = BT->RootDecorators[i];
		if (!Decorator) continue;

		TotalDecorators++;

		UBTDecorator_Blackboard* BBDec = Cast<UBTDecorator_Blackboard>(Decorator);
		if (BBDec)
		{
			FName KeyName = BBDec->GetSelectedBlackboardKey();
			Result += FString::Printf(TEXT("[Root][%d] %s - Key: %s\n"), i, *Decorator->GetNodeName(), *KeyName.ToString());

			if (FIntProperty* IntValueProp = CastField<FIntProperty>(BBDec->GetClass()->FindPropertyByName(TEXT("IntValue"))))
			{
				int32 IntValue = IntValueProp->GetPropertyValue_InContainer(BBDec);
				Result += FString::Printf(TEXT("    IntValue: %d\n"), IntValue);

				if (KeyName == FName("State"))
				{
					StateDecorators++;
					Result += FString::Printf(TEXT("    ^^^ STATE KEY DECORATOR - IntValue=%d ^^^\n"), IntValue);
				}
			}
		}
		else
		{
			Result += FString::Printf(TEXT("[Root][%d] %s (non-blackboard)\n"), i, *Decorator->GetNodeName());
		}
	}

	// Process node tree
	Result += TEXT("\n--- Node Tree ---\n");
	int32 TotalTasks = 0;
	if (BT->RootNode)
	{
		ProcessBTNodeDecorators(BT->RootNode, Result, 0, TotalDecorators, StateDecorators, TotalTasks);
	}
	else
	{
		Result += TEXT("No RootNode!\n");
	}

	Result += FString::Printf(TEXT("\n=== Summary ===\n"));
	Result += FString::Printf(TEXT("Total Decorators: %d\n"), TotalDecorators);
	Result += FString::Printf(TEXT("State Key Decorators: %d\n"), StateDecorators);
	Result += FString::Printf(TEXT("Total Tasks: %d\n"), TotalTasks);

	// Expected values for reference
	Result += TEXT("\n=== Expected State Values (ESLFAIStates) ===\n");
	Result += TEXT("  0 = Idle\n");
	Result += TEXT("  1 = RandomRoam\n");
	Result += TEXT("  2 = Patrolling\n");
	Result += TEXT("  3 = Investigating\n");
	Result += TEXT("  4 = Combat\n");
	Result += TEXT("  5 = PoiseBroken\n");
	Result += TEXT("  6 = Uninterruptable\n");
	Result += TEXT("  7 = OutOfBounds\n");

	UE_LOG(LogSLFAutomation, Warning, TEXT("%s"), *Result);
	return Result;
}



FString USLFAutomationLibrary::MigrateBlackboardEnumToCpp(const FString& BlackboardPath, const FString& KeyName)
{
	FString Result;

	// Load the blackboard asset
	UBlackboardData* BB = LoadObject<UBlackboardData>(nullptr, *BlackboardPath);
	if (!BB)
	{
		return FString::Printf(TEXT("ERROR: Could not load BlackboardData: %s"), *BlackboardPath);
	}

	Result += FString::Printf(TEXT("BlackboardData: %s\n"), *BB->GetName());
	Result += FString::Printf(TEXT("Looking for key: %s\n\n"), *KeyName);

	// Find the C++ enum
	UEnum* CppEnum = StaticEnum<ESLFAIStates>();
	if (!CppEnum)
	{
		return TEXT("ERROR: Could not find C++ enum ESLFAIStates");
	}
	Result += FString::Printf(TEXT("Found C++ enum: %s\n"), *CppEnum->GetPathName());

	// Find the specified key
	bool bFound = false;
	bool bMadeChanges = false;

	for (FBlackboardEntry& Key : BB->Keys)
	{
		if (Key.EntryName.ToString() != KeyName)
		{
			continue;
		}

		bFound = true;
		Result += FString::Printf(TEXT("Found key: %s\n"), *Key.EntryName.ToString());

		UBlackboardKeyType_Enum* EnumKeyType = Cast<UBlackboardKeyType_Enum>(Key.KeyType);
		if (!EnumKeyType)
		{
			Result += TEXT("    ERROR: Key is not Enum type\n");
			continue;
		}

		Result += FString::Printf(TEXT("    KeyType: %s\n"), *EnumKeyType->GetClass()->GetName());

		// Get current enum type using reflection (EnumType is protected)
		if (FObjectProperty* EnumTypeProp = CastField<FObjectProperty>(EnumKeyType->GetClass()->FindPropertyByName(TEXT("EnumType"))))
		{
			UEnum* CurrentEnum = Cast<UEnum>(EnumTypeProp->GetObjectPropertyValue_InContainer(EnumKeyType));
			if (CurrentEnum)
			{
				Result += FString::Printf(TEXT("    Current EnumType: %s\n"), *CurrentEnum->GetPathName());
			}
			else
			{
				Result += TEXT("    Current EnumType: None\n");
			}

			// Set the new C++ enum type
			EnumTypeProp->SetObjectPropertyValue_InContainer(EnumKeyType, CppEnum);
			Result += FString::Printf(TEXT("    Set EnumType to: %s\n"), *CppEnum->GetPathName());
			bMadeChanges = true;

			// Verify the change
			UEnum* NewEnum = Cast<UEnum>(EnumTypeProp->GetObjectPropertyValue_InContainer(EnumKeyType));
			if (NewEnum)
			{
				Result += FString::Printf(TEXT("    Verified new EnumType: %s\n"), *NewEnum->GetPathName());
			}
		}
		else
		{
			Result += TEXT("    ERROR: Could not find EnumType property via reflection\n");
		}

		// Also try setting EnumName property (for validation/display purposes)
		if (FStrProperty* EnumNameProp = CastField<FStrProperty>(EnumKeyType->GetClass()->FindPropertyByName(TEXT("EnumName"))))
		{
			const FString& CurrentName = EnumNameProp->GetPropertyValue_InContainer(EnumKeyType);
			Result += FString::Printf(TEXT("    Current EnumName: %s\n"), *CurrentName);

			FString NewName = TEXT("ESLFAIStates");
			EnumNameProp->SetPropertyValue_InContainer(EnumKeyType, NewName);
			Result += FString::Printf(TEXT("    Set EnumName to: %s\n"), *NewName);
		}

		// Set bIsEnumNameValid to true
		if (FBoolProperty* ValidProp = CastField<FBoolProperty>(EnumKeyType->GetClass()->FindPropertyByName(TEXT("bIsEnumNameValid"))))
		{
			ValidProp->SetPropertyValue_InContainer(EnumKeyType, true);
			Result += TEXT("    Set bIsEnumNameValid to true\n");
		}

		break;
	}

	if (!bFound)
	{
		return FString::Printf(TEXT("ERROR: Key '%s' not found in blackboard"), *KeyName);
	}

	// Save if changes were made
	if (bMadeChanges)
	{
		BB->MarkPackageDirty();
		FString PackageName = BB->GetOutermost()->GetName();
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
		bool bSaved = UPackage::SavePackage(BB->GetOutermost(), BB, *FilePath, SaveArgs);

		if (bSaved)
		{
			Result += TEXT("\nSaved BlackboardData with enum migration.\n");
		}
		else
		{
			Result += TEXT("\nFailed to save BlackboardData!\n");
		}
	}
	else
	{
		Result += TEXT("\nNo changes needed.\n");
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("%s"), *Result);
	return Result;
}


FString USLFAutomationLibrary::ExportBlueprintEnumValues(const FString& EnumPath)
{
	FString Result;
	Result += FString::Printf(TEXT("=== EXPORT BLUEPRINT ENUM: %s ===\n"), *EnumPath);

	// Load the enum asset
	UEnum* EnumAsset = LoadObject<UEnum>(nullptr, *EnumPath);
	if (!EnumAsset)
	{
		// Try with /Script/ prefix for C++ enums
		EnumAsset = LoadObject<UEnum>(nullptr, *(FString("/Script/SLFConversion.") + EnumPath));
	}

	if (!EnumAsset)
	{
		Result += TEXT("ERROR: Could not load enum asset\n");
		UE_LOG(LogSLFAutomation, Error, TEXT("%s"), *Result);
		return Result;
	}

	Result += FString::Printf(TEXT("Loaded: %s\n"), *EnumAsset->GetPathName());
	Result += FString::Printf(TEXT("Class: %s\n"), *EnumAsset->GetClass()->GetName());

	// Get enum value count
	int32 NumEnums = EnumAsset->NumEnums();
	Result += FString::Printf(TEXT("Value Count: %d (includes MAX)\n\n"), NumEnums);

	Result += TEXT("| Index | Value | InternalName | DisplayName |\n");
	Result += TEXT("|-------|-------|--------------|-------------|\n");

	// Iterate through all values (except MAX which is usually last)
	for (int32 i = 0; i < NumEnums; ++i)
	{
		int64 Value = EnumAsset->GetValueByIndex(i);
		FName InternalName = EnumAsset->GetNameByIndex(i);
		FText DisplayName = EnumAsset->GetDisplayNameTextByIndex(i);

		// Skip MAX entry
		if (InternalName.ToString().Contains(TEXT("MAX")))
		{
			continue;
		}

		Result += FString::Printf(TEXT("| %d | %lld | %s | %s |\n"),
			i,
			Value,
			*InternalName.ToString(),
			*DisplayName.ToString());
	}

	// Also show what our C++ enum looks like
	Result += TEXT("\n=== C++ ESLFAIStates Comparison ===\n");
	UEnum* CppEnum = StaticEnum<ESLFAIStates>();
	if (CppEnum)
	{
		Result += TEXT("| Index | Value | Name |\n");
		Result += TEXT("|-------|-------|------|\n");
		for (int32 i = 0; i < CppEnum->NumEnums(); ++i)
		{
			int64 Value = CppEnum->GetValueByIndex(i);
			FName Name = CppEnum->GetNameByIndex(i);
			if (Name.ToString().Contains(TEXT("MAX")))
			{
				continue;
			}
			Result += FString::Printf(TEXT("| %d | %lld | %s |\n"), i, Value, *Name.ToString());
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("%s"), *Result);
	return Result;
}

FString USLFAutomationLibrary::DiagnoseBlackboardStateKey(const FString& BlackboardPath)
{
	FString Result;
	Result += FString::Printf(TEXT("=== DIAGNOSE BB STATE KEY: %s ===\n"), *BlackboardPath);

	// Load blackboard
	UBlackboardData* BB = LoadObject<UBlackboardData>(nullptr, *BlackboardPath);
	if (!BB)
	{
		Result += TEXT("ERROR: Could not load BlackboardData\n");
		return Result;
	}

	Result += FString::Printf(TEXT("Loaded: %s\n"), *BB->GetPathName());

	// Find State key
	const TArray<FBlackboardEntry>& Keys = BB->Keys;
	Result += FString::Printf(TEXT("Total Keys: %d\n\n"), Keys.Num());

	for (const FBlackboardEntry& Key : Keys)
	{
		FString KeyName = Key.EntryName.ToString();
		FString KeyType = Key.KeyType ? Key.KeyType->GetClass()->GetName() : TEXT("NULL");

		Result += FString::Printf(TEXT("Key: %s (Type: %s)\n"), *KeyName, *KeyType);

		// If it's the State key, get more details
		if (KeyName == TEXT("State"))
		{
			Result += TEXT("  >>> THIS IS THE STATE KEY <<<\n");

			if (UBlackboardKeyType_Enum* EnumKeyType = Cast<UBlackboardKeyType_Enum>(Key.KeyType))
			{
				Result += TEXT("  KeyType is Enum\n");

				// Get the enum type via reflection
				if (FObjectProperty* EnumTypeProp = CastField<FObjectProperty>(EnumKeyType->GetClass()->FindPropertyByName(TEXT("EnumType"))))
				{
					UEnum* CurrentEnum = Cast<UEnum>(EnumTypeProp->GetObjectPropertyValue_InContainer(EnumKeyType));
					if (CurrentEnum)
					{
						Result += FString::Printf(TEXT("  EnumType: %s\n"), *CurrentEnum->GetPathName());

						// Show all enum values
						Result += TEXT("  Enum Values:\n");
						for (int32 i = 0; i < CurrentEnum->NumEnums(); ++i)
						{
							int64 Value = CurrentEnum->GetValueByIndex(i);
							FName Name = CurrentEnum->GetNameByIndex(i);
							FText DisplayName = CurrentEnum->GetDisplayNameTextByIndex(i);
							if (!Name.ToString().Contains(TEXT("MAX")))
							{
								Result += FString::Printf(TEXT("    [%d] Value=%lld Name=%s Display=%s\n"),
									i, Value, *Name.ToString(), *DisplayName.ToString());
							}
						}
					}
					else
					{
						Result += TEXT("  EnumType: NULL\n");
					}
				}
			}
			else if (UBlackboardKeyType_Int* IntKeyType = Cast<UBlackboardKeyType_Int>(Key.KeyType))
			{
				Result += TEXT("  KeyType is Int\n");
			}
			else
			{
				Result += FString::Printf(TEXT("  KeyType is %s (not Enum or Int)\n"), *KeyType);
			}
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("%s"), *Result);
	return Result;
}

int32 USLFAutomationLibrary::RepairTransitionWiring(UObject* AnimBlueprintAsset)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("RepairTransitionWiring: Not an AnimBlueprint"));
		return 0;
	}

	int32 RepairedCount = 0;
	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();

	// Get all graphs in the AnimBP
	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);

	UE_LOG(LogSLFAutomation, Warning, TEXT("RepairTransitionWiring: Processing %d graphs"), AllGraphs.Num());

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph)
		{
			continue;
		}

		FString GraphName = Graph->GetName();

		// Only process transition graphs
		if (!GraphName.Contains(TEXT("Transition")) && !GraphName.Contains(TEXT("AnimationTransitionGraph")))
		{
			continue;
		}

		// Find all nodes in this graph
		TArray<UK2Node_VariableGet*> DisconnectedVariableGets;
		UAnimGraphNode_TransitionResult* TransitionResultNode = nullptr;
		UK2Node_CallFunction* NotBooleanNode = nullptr;
		UK2Node_CommutativeAssociativeBinaryOperator* AndBooleanNode = nullptr;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node)
			{
				continue;
			}

			// Check for TransitionResult
			if (UAnimGraphNode_TransitionResult* ResultNode = Cast<UAnimGraphNode_TransitionResult>(Node))
			{
				TransitionResultNode = ResultNode;
			}

			// Check for VariableGet nodes
			if (UK2Node_VariableGet* VarGet = Cast<UK2Node_VariableGet>(Node))
			{
				// Check if its output is disconnected
				bool bHasConnection = false;
				for (UEdGraphPin* Pin : VarGet->Pins)
				{
					if (Pin && Pin->Direction == EGPD_Output && Pin->LinkedTo.Num() > 0)
					{
						bHasConnection = true;
						break;
					}
				}

				if (!bHasConnection)
				{
					DisconnectedVariableGets.Add(VarGet);
				}
			}

			// Check for NOT Boolean
			if (UK2Node_CallFunction* CallFunc = Cast<UK2Node_CallFunction>(Node))
			{
				FString FuncName = CallFunc->GetFunctionName().ToString();
				if (FuncName.Contains(TEXT("Not")))
				{
					NotBooleanNode = CallFunc;
				}
			}

			// Check for AND Boolean (commutative operator)
			if (UK2Node_CommutativeAssociativeBinaryOperator* BinaryOp = Cast<UK2Node_CommutativeAssociativeBinaryOperator>(Node))
			{
				AndBooleanNode = BinaryOp;
			}
		}

		// Now try to wire disconnected VariableGets
		for (UK2Node_VariableGet* VarGet : DisconnectedVariableGets)
		{
			FName VarName = VarGet->GetVarName();
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Graph '%s': Found disconnected VariableGet '%s'"), *GraphName, *VarName.ToString());

			// Get the output pin
			UEdGraphPin* OutputPin = nullptr;
			for (UEdGraphPin* Pin : VarGet->Pins)
			{
				if (Pin && Pin->Direction == EGPD_Output && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
				{
					OutputPin = Pin;
					break;
				}
			}

			if (!OutputPin)
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    No boolean output pin found"));
				continue;
			}

			bool bWired = false;

			// Strategy 1: If there's a NOT node with disconnected A input, wire to it
			if (NotBooleanNode && !bWired)
			{
				for (UEdGraphPin* Pin : NotBooleanNode->Pins)
				{
					if (Pin && Pin->PinName == TEXT("A") && Pin->Direction == EGPD_Input && Pin->LinkedTo.Num() == 0)
					{
						if (Schema->TryCreateConnection(OutputPin, Pin))
						{
							UE_LOG(LogSLFAutomation, Warning, TEXT("    Wired '%s' to NOT Boolean.A"), *VarName.ToString());
							bWired = true;
							RepairedCount++;
						}
						break;
					}
				}
			}

			// Strategy 2: If there's an AND node with disconnected B input, wire to it
			if (AndBooleanNode && !bWired)
			{
				for (UEdGraphPin* Pin : AndBooleanNode->Pins)
				{
					if (Pin && (Pin->PinName == TEXT("B") || Pin->PinName == TEXT("A")) && Pin->Direction == EGPD_Input && Pin->LinkedTo.Num() == 0)
					{
						if (Schema->TryCreateConnection(OutputPin, Pin))
						{
							UE_LOG(LogSLFAutomation, Warning, TEXT("    Wired '%s' to AND Boolean.%s"), *VarName.ToString(), *Pin->PinName.ToString());
							bWired = true;
							RepairedCount++;
						}
						break;
					}
				}
			}

			// Strategy 3: Wire directly to TransitionResult.bCanEnterTransition if it's disconnected
			if (TransitionResultNode && !bWired)
			{
				for (UEdGraphPin* Pin : TransitionResultNode->Pins)
				{
					if (Pin && Pin->PinName == TEXT("bCanEnterTransition") && Pin->Direction == EGPD_Input && Pin->LinkedTo.Num() == 0)
					{
						if (Schema->TryCreateConnection(OutputPin, Pin))
						{
							UE_LOG(LogSLFAutomation, Warning, TEXT("    Wired '%s' directly to TransitionResult.bCanEnterTransition"), *VarName.ToString());
							bWired = true;
							RepairedCount++;
						}
						break;
					}
				}
			}

			if (!bWired)
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Could not find appropriate target to wire '%s'"), *VarName.ToString());
			}
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("RepairTransitionWiring: Repaired %d connections"), RepairedCount);
	return RepairedCount;
}


int32 USLFAutomationLibrary::FixBrokenVariableGetNodes(UObject* AnimBlueprintAsset)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("FixBrokenVariableGetNodes: Not an AnimBlueprint"));
		return 0;
	}

	int32 FixedCount = 0;

	// Map of variable names to their expected C++ property
	TMap<FName, FProperty*> ValidProperties;
	ValidProperties.Add(FName("bIsAccelerating"), FindFProperty<FProperty>(AnimBP->GeneratedClass, FName("bIsAccelerating")));
	ValidProperties.Add(FName("bIsBlocking"), FindFProperty<FProperty>(AnimBP->GeneratedClass, FName("bIsBlocking")));
	ValidProperties.Add(FName("bIsFalling"), FindFProperty<FProperty>(AnimBP->GeneratedClass, FName("bIsFalling")));
	ValidProperties.Add(FName("IsResting"), FindFProperty<FProperty>(AnimBP->GeneratedClass, FName("IsResting")));
	ValidProperties.Add(FName("IsCrouched"), FindFProperty<FProperty>(AnimBP->GeneratedClass, FName("IsCrouched")));

	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);

	UE_LOG(LogSLFAutomation, Warning, TEXT("FixBrokenVariableGetNodes: Processing %d graphs"), AllGraphs.Num());

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		FString GraphName = Graph->GetName();
		if (!GraphName.Contains(TEXT("Transition"))) continue;

		TArray<UK2Node_VariableGet*> NodesToFix;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			UK2Node_VariableGet* VarGet = Cast<UK2Node_VariableGet>(Node);
			if (!VarGet) continue;
			FName VarName = VarGet->GetVarName();
			FProperty** FoundProp = ValidProperties.Find(VarName);
			if (!FoundProp || !*FoundProp) continue;
			FProperty* ResolvedProp = VarGet->GetPropertyForVariable();
			if (ResolvedProp && ResolvedProp == *FoundProp) continue;
			NodesToFix.Add(VarGet);
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Broken: %s.%s"), *GraphName, *VarName.ToString());
		}

		for (UK2Node_VariableGet* OldNode : NodesToFix)
		{
			FName VarName = OldNode->GetVarName();
			FProperty* TargetProp = *ValidProperties.Find(VarName);
			TArray<UEdGraphPin*> OutputConnections;
			for (UEdGraphPin* Pin : OldNode->Pins)
			{
				if (Pin && Pin->Direction == EGPD_Output)
				{
					OutputConnections = Pin->LinkedTo;
					break;
				}
			}

			UK2Node_VariableGet* NewNode = NewObject<UK2Node_VariableGet>(Graph);
			NewNode->VariableReference.SetSelfMember(VarName);
			NewNode->SetFromProperty(TargetProp, false, AnimBP->GeneratedClass);
			NewNode->NodePosX = OldNode->NodePosX;
			NewNode->NodePosY = OldNode->NodePosY;
			NewNode->CreateNewGuid();
			Graph->AddNode(NewNode, false, false);
			NewNode->AllocateDefaultPins();

			for (UEdGraphPin* Pin : NewNode->Pins)
			{
				if (Pin && Pin->Direction == EGPD_Output)
				{
					for (UEdGraphPin* Conn : OutputConnections)
						if (Conn) Pin->MakeLinkTo(Conn);
					break;
				}
			}

			Graph->RemoveNode(OldNode);
			FixedCount++;
		}
	}

	if (FixedCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
		FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("FixBrokenVariableGetNodes: Fixed %d nodes"), FixedCount);
	return FixedCount;
}


FString USLFAutomationLibrary::ExportAnimGraphComplete(UObject* AnimBlueprintAsset, const FString& OutputFilePath)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		return TEXT("ERROR: Not an AnimBlueprint");
	}

	FString Result;
	Result += TEXT("================================================================================\n");
	Result += FString::Printf(TEXT("COMPLETE ANIMGRAPH EXPORT: %s\n"), *AnimBP->GetName());
	Result += TEXT("================================================================================\n\n");

	// Get all graphs
	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);

	Result += FString::Printf(TEXT("Total Graphs: %d\n\n"), AllGraphs.Num());

	int32 GraphIndex = 0;
	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		FString GraphName = Graph->GetName();
		FString GraphClass = Graph->GetClass()->GetName();

		Result += TEXT("--------------------------------------------------------------------------------\n");
		Result += FString::Printf(TEXT("GRAPH[%d]: %s\n"), GraphIndex, *GraphName);
		Result += FString::Printf(TEXT("  Class: %s\n"), *GraphClass);
		Result += FString::Printf(TEXT("  NodeCount: %d\n"), Graph->Nodes.Num());
		Result += TEXT("--------------------------------------------------------------------------------\n\n");

		int32 NodeIndex = 0;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			FString NodeClass = Node->GetClass()->GetName();
			FString NodeName = Node->GetName();
			FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();

			Result += FString::Printf(TEXT("  NODE[%d]: %s\n"), NodeIndex, *NodeName);
			Result += FString::Printf(TEXT("    Class: %s\n"), *NodeClass);
			Result += FString::Printf(TEXT("    Title: %s\n"), *NodeTitle);
			Result += FString::Printf(TEXT("    Position: X=%d, Y=%d\n"), Node->NodePosX, Node->NodePosY);
			Result += FString::Printf(TEXT("    GUID: %s\n"), *Node->NodeGuid.ToString());
			Result += FString::Printf(TEXT("    PinCount: %d\n"), Node->Pins.Num());

			// VariableGet nodes
			if (UK2Node_VariableGet* VarGet = Cast<UK2Node_VariableGet>(Node))
			{
				FName VarName = VarGet->GetVarName();
				Result += FString::Printf(TEXT("    [VariableGet] VarName: %s\n"), *VarName.ToString());
				FProperty* ResolvedProp = VarGet->VariableReference.ResolveMember<FProperty>(AnimBP->GeneratedClass);
				if (ResolvedProp)
				{
					Result += FString::Printf(TEXT("    [VariableGet] ResolvedProperty: %s (Owner: %s)\n"),
						*ResolvedProp->GetName(), *ResolvedProp->GetOwnerClass()->GetName());
				}
				else
				{
					Result += TEXT("    [VariableGet] ResolvedProperty: NULL (BROKEN!)\n");
				}
			}

			// Transition nodes
			if (UAnimStateTransitionNode* TransNode = Cast<UAnimStateTransitionNode>(Node))
			{
				Result += FString::Printf(TEXT("    [Transition] ErrorMsg: %s\n"),
					TransNode->ErrorMsg.IsEmpty() ? TEXT("(none)") : *TransNode->ErrorMsg);
				if (TransNode->BoundGraph)
				{
					Result += FString::Printf(TEXT("    [Transition] BoundGraph: %s\n"), *TransNode->BoundGraph->GetName());
				}
			}

			// CallFunction nodes
			if (UK2Node_CallFunction* CallFunc = Cast<UK2Node_CallFunction>(Node))
			{
				FName FuncName = CallFunc->GetFunctionName();
				Result += FString::Printf(TEXT("    [CallFunction] FunctionName: %s\n"), *FuncName.ToString());
			}

			// Export ALL pins
			Result += TEXT("    PINS:\n");
			int32 PinIndex = 0;
			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin) continue;

				FString PinName = Pin->PinName.ToString();
				FString PinCategory = Pin->PinType.PinCategory.ToString();
				FString Direction = (Pin->Direction == EGPD_Input) ? TEXT("INPUT") : TEXT("OUTPUT");

				Result += FString::Printf(TEXT("      PIN[%d]: %s (%s)\n"), PinIndex, *PinName, *Direction);
				Result += FString::Printf(TEXT("        Category: %s\n"), *PinCategory);

				if (Pin->PinType.PinSubCategoryObject.IsValid())
				{
					Result += FString::Printf(TEXT("        SubCategoryObject: %s\n"), *Pin->PinType.PinSubCategoryObject->GetName());
				}

				if (!Pin->DefaultValue.IsEmpty())
				{
					Result += FString::Printf(TEXT("        DefaultValue: %s\n"), *Pin->DefaultValue);
				}

				Result += FString::Printf(TEXT("        LinkedTo (%d):\n"), Pin->LinkedTo.Num());
				for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
				{
					if (LinkedPin && LinkedPin->GetOwningNode())
					{
						Result += FString::Printf(TEXT("          -> %s.%s\n"),
							*LinkedPin->GetOwningNode()->GetName(), *LinkedPin->PinName.ToString());
					}
					else
					{
						Result += TEXT("          -> (INVALID)\n");
					}
				}

				if (Pin->bOrphanedPin) Result += TEXT("        *** ORPHANED ***\n");
				if (Pin->bNotConnectable) Result += TEXT("        *** NOT CONNECTABLE ***\n");

				PinIndex++;
			}
			Result += TEXT("\n");
			NodeIndex++;
		}
		GraphIndex++;
	}

	// Blueprint variables
	Result += TEXT("\n================================================================================\n");
	Result += TEXT("BLUEPRINT VARIABLES\n");
	Result += TEXT("================================================================================\n");
	for (const FBPVariableDescription& Var : AnimBP->NewVariables)
	{
		Result += FString::Printf(TEXT("  %s : %s (GUID: %s)\n"),
			*Var.VarName.ToString(), *Var.VarType.PinCategory.ToString(), *Var.VarGuid.ToString());
	}

	// Parent class properties
	Result += TEXT("\n================================================================================\n");
	Result += TEXT("PARENT CLASS PROPERTIES\n");
	Result += TEXT("================================================================================\n");
	if (AnimBP->GeneratedClass)
	{
		UClass* ParentClass = AnimBP->GeneratedClass->GetSuperClass();
		if (ParentClass)
		{
			Result += FString::Printf(TEXT("ParentClass: %s\n"), *ParentClass->GetName());
			for (TFieldIterator<FProperty> PropIt(ParentClass); PropIt; ++PropIt)
			{
				FProperty* Prop = *PropIt;
				if (Prop->GetOwnerClass() == ParentClass)
				{
					Result += FString::Printf(TEXT("  %s : %s\n"), *Prop->GetName(), *Prop->GetClass()->GetName());
				}
			}
		}
	}

	if (!OutputFilePath.IsEmpty())
	{
		FFileHelper::SaveStringToFile(Result, *OutputFilePath);
		UE_LOG(LogSLFAutomation, Warning, TEXT("ExportAnimGraphComplete: Saved to %s (%d chars)"), *OutputFilePath, Result.Len());
	}

	return Result;
}

#endif // WITH_EDITOR
