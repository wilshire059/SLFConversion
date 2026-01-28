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
#include "SLFGameTypes.h" // FSLFWeightedLoot, FSLFLootItem for DataTable migration
// LootDropManager for container loot functions
#include "Components/AC_LootDropManager.h"
#include "SLFPrimaryDataAssets.h"
#include "Components/LootDropManagerComponent.h"
#include "EditorAssetLibrary.h"
#include "FileHelpers.h" // For FEditorFileUtils
#include "EngineUtils.h" // For TActorIterator
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
// Destructible diagnostics
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionObject.h"
// Interface diagnostics
#include "Interfaces/SLFDestructibleHelperInterface.h"
// WidgetBlueprint for widget delegate binding clearing
#include "WidgetBlueprint.h"

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
// WIDGET DELEGATE BINDINGS (for WidgetBlueprint)
// ============================================================================

int32 USLFAutomationLibrary::ClearWidgetDelegateBindings(UObject* WidgetBlueprintAsset)
{
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(WidgetBlueprintAsset);
	if (!WidgetBP)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("ClearWidgetDelegateBindings: Not a WidgetBlueprint"));
		return 0;
	}

	int32 ClearedCount = 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== CLEARING WIDGET DELEGATE BINDINGS FROM %s ==="), *WidgetBP->GetName());

	// WidgetBlueprint has a Bindings array that stores widget property bindings
	// These include bindings like OnGenerateItemWidget on ComboBoxKey widgets
	if (WidgetBP->Bindings.Num() > 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Found %d widget bindings"), WidgetBP->Bindings.Num());

		// Log what we're clearing
		for (int32 i = 0; i < WidgetBP->Bindings.Num(); ++i)
		{
			const FDelegateEditorBinding& Binding = WidgetBP->Bindings[i];
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Binding %d: Object=%s, Property=%s, Function=%s"),
				i,
				*Binding.ObjectName,
				*Binding.PropertyName.ToString(),
				*Binding.FunctionName.ToString());
		}

		ClearedCount = WidgetBP->Bindings.Num();
		WidgetBP->Bindings.Empty();

		UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d widget bindings"), ClearedCount);
	}
	else
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("No widget bindings found"));
	}

	// Mark as modified
	WidgetBP->Modify();

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

	// 8. For WidgetBlueprints, clear widget delegate bindings (ComboBox OnGenerateItemWidget, etc.)
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Blueprint);
	if (WidgetBP)
	{
		int32 WidgetBindingsCleared = ClearWidgetDelegateBindings(WidgetBP);
		UE_LOG(LogSLFAutomation, Warning, TEXT("Cleared %d widget delegate bindings"), WidgetBindingsCleared);
	}

	// 9. Refresh and mark modified - BUT DO NOT COMPILE
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

bool USLFAutomationLibrary::ApplyDeathMontagesToAnimset(const FString& AnimsetPath, const TArray<FString>& DirectionNames, const TArray<FString>& MontagePaths)
{
	UE_LOG(LogSLFAutomation, Warning, TEXT("ApplyDeathMontagesToAnimset: %s with %d entries"), *AnimsetPath, DirectionNames.Num());

	if (DirectionNames.Num() != MontagePaths.Num())
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  DirectionNames and MontagePaths arrays must have same length"));
		return false;
	}

	if (DirectionNames.Num() == 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  No death montage entries to apply"));
		return true;
	}

	// Load the animset asset
	UObject* Asset = LoadObject<UObject>(nullptr, *AnimsetPath);
	if (!Asset)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to load asset: %s"), *AnimsetPath);
		return false;
	}

	// Try to cast to UPDA_CombatReactionAnimData
	UPDA_CombatReactionAnimData* AnimsetData = Cast<UPDA_CombatReactionAnimData>(Asset);
	if (!AnimsetData)
	{
		// Try Blueprint generated class
		if (UBlueprint* BP = Cast<UBlueprint>(Asset))
		{
			if (BP->GeneratedClass)
			{
				AnimsetData = Cast<UPDA_CombatReactionAnimData>(BP->GeneratedClass->GetDefaultObject());
			}
		}
	}

	if (!AnimsetData)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Asset is not UPDA_CombatReactionAnimData: %s"), *AnimsetPath);
		return false;
	}

	// Direction name to enum mapping
	TMap<FString, ESLFDirection> DirectionMap;
	DirectionMap.Add(TEXT("Idle"), ESLFDirection::Idle);
	DirectionMap.Add(TEXT("Fwd"), ESLFDirection::Fwd);
	DirectionMap.Add(TEXT("FwdLeft"), ESLFDirection::FwdLeft);
	DirectionMap.Add(TEXT("FwdRight"), ESLFDirection::FwdRight);
	DirectionMap.Add(TEXT("Left"), ESLFDirection::Left);
	DirectionMap.Add(TEXT("Right"), ESLFDirection::Right);
	DirectionMap.Add(TEXT("Bwd"), ESLFDirection::Bwd);
	DirectionMap.Add(TEXT("BwdLeft"), ESLFDirection::BwdLeft);
	DirectionMap.Add(TEXT("BwdRight"), ESLFDirection::BwdRight);

	// Clear existing Death map
	AnimsetData->Death.Empty();

	int32 AddedCount = 0;
	for (int32 i = 0; i < DirectionNames.Num(); i++)
	{
		const FString& DirName = DirectionNames[i];
		const FString& MontagePath = MontagePaths[i];

		// Get direction enum
		ESLFDirection* DirEnumPtr = DirectionMap.Find(DirName);
		if (!DirEnumPtr)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Unknown direction name: %s"), *DirName);
			continue;
		}

		// Load the montage
		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *MontagePath);
		if (!Montage)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Could not load montage: %s"), *MontagePath);
			continue;
		}

		// Create soft object pointer and add to map
		TSoftObjectPtr<UAnimMontage> MontageSoftPtr(Montage);
		AnimsetData->Death.Add(*DirEnumPtr, MontageSoftPtr);
		AddedCount++;

		UE_LOG(LogSLFAutomation, Log, TEXT("  Added death montage: %s -> %s"), *DirName, *Montage->GetName());
	}

	if (AddedCount == 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  No death montage entries added"));
		return false;
	}

	// Mark package dirty and save
	AnimsetData->MarkPackageDirty();

	UPackage* Package = AnimsetData->GetOutermost();
	FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	SaveArgs.Error = GError;

	bool bSaved = UPackage::SavePackage(Package, AnimsetData, *PackageFileName, SaveArgs);

	if (bSaved)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Saved %s with %d death montages"), *AnimsetPath, AddedCount);
	}
	else
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to save: %s"), *AnimsetPath);
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

// ============================================================================
// EXECUTION ANIMATION DATA
// ============================================================================

bool USLFAutomationLibrary::SetExecutionAnimData(const FString& ExecutionDataAssetPath, const FString& FrontAnimPath, const FString& BackAnimPath)
{
	// Load the execution data asset
	UObject* Asset = LoadObject<UObject>(nullptr, *ExecutionDataAssetPath);
	if (!Asset)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("SetExecutionAnimData: Failed to load asset: %s"), *ExecutionDataAssetPath);
		return false;
	}

	// Cast to UPDA_ExecutionAnimData
	UPDA_ExecutionAnimData* ExecData = Cast<UPDA_ExecutionAnimData>(Asset);
	if (!ExecData)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("SetExecutionAnimData: Asset is not UPDA_ExecutionAnimData: %s (class: %s)"),
			*ExecutionDataAssetPath, *Asset->GetClass()->GetName());
		return false;
	}

	bool bModified = false;

	// Set ExecuteFront animation
	if (!FrontAnimPath.IsEmpty())
	{
		ExecData->ExecuteFront.Animation = FSoftObjectPath(FrontAnimPath);
		UE_LOG(LogSLFAutomation, Warning, TEXT("SetExecutionAnimData: Set ExecuteFront.Animation = %s"), *FrontAnimPath);
		bModified = true;
	}

	// Set ExecuteBack animation
	if (!BackAnimPath.IsEmpty())
	{
		ExecData->ExecuteBack.Animation = FSoftObjectPath(BackAnimPath);
		UE_LOG(LogSLFAutomation, Warning, TEXT("SetExecutionAnimData: Set ExecuteBack.Animation = %s"), *BackAnimPath);
		bModified = true;
	}

	if (bModified)
	{
		// Mark package dirty and save
		ExecData->MarkPackageDirty();

		// Save the asset
		UPackage* Package = ExecData->GetOutermost();
		FString PackageFilename;
		if (FPackageName::TryConvertLongPackageNameToFilename(Package->GetName(), PackageFilename, FPackageName::GetAssetPackageExtension()))
		{
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Standalone;
			SaveArgs.Error = GError;
			UPackage::SavePackage(Package, ExecData, *PackageFilename, SaveArgs);
			UE_LOG(LogSLFAutomation, Warning, TEXT("SetExecutionAnimData: Saved %s"), *ExecutionDataAssetPath);
		}
	}

	return true;
}

FString USLFAutomationLibrary::GetExecutionAnimData(const FString& ExecutionDataAssetPath)
{
	FString Result;

	// Load the execution data asset
	UObject* Asset = LoadObject<UObject>(nullptr, *ExecutionDataAssetPath);
	if (!Asset)
	{
		return FString::Printf(TEXT("ERROR: Failed to load asset: %s"), *ExecutionDataAssetPath);
	}

	Result += FString::Printf(TEXT("Asset: %s\n"), *ExecutionDataAssetPath);
	Result += FString::Printf(TEXT("Class: %s\n"), *Asset->GetClass()->GetName());

	// Try to cast to UPDA_ExecutionAnimData
	UPDA_ExecutionAnimData* ExecData = Cast<UPDA_ExecutionAnimData>(Asset);
	if (ExecData)
	{
		Result += TEXT("Cast to UPDA_ExecutionAnimData: SUCCESS\n");

		// ExecuteFront
		Result += TEXT("\nExecuteFront:\n");
		Result += FString::Printf(TEXT("  Tag: %s\n"), *ExecData->ExecuteFront.Tag.ToString());
		if (ExecData->ExecuteFront.Animation.IsNull())
		{
			Result += TEXT("  Animation: NULL\n");
		}
		else
		{
			Result += FString::Printf(TEXT("  Animation: %s\n"), *ExecData->ExecuteFront.Animation.ToString());
		}

		// ExecuteBack
		Result += TEXT("\nExecuteBack:\n");
		Result += FString::Printf(TEXT("  Tag: %s\n"), *ExecData->ExecuteBack.Tag.ToString());
		if (ExecData->ExecuteBack.Animation.IsNull())
		{
			Result += TEXT("  Animation: NULL\n");
		}
		else
		{
			Result += FString::Printf(TEXT("  Animation: %s\n"), *ExecData->ExecuteBack.Animation.ToString());
		}
	}
	else
	{
		Result += TEXT("Cast to UPDA_ExecutionAnimData: FAILED\n");
		Result += TEXT("Checking if it's still a Blueprint class...\n");

		// Try to get generated class
		if (UBlueprint* BP = Cast<UBlueprint>(Asset))
		{
			Result += FString::Printf(TEXT("It's a Blueprint: %s\n"), *BP->GetName());
			if (BP->GeneratedClass)
			{
				Result += FString::Printf(TEXT("GeneratedClass: %s\n"), *BP->GeneratedClass->GetName());
				Result += FString::Printf(TEXT("ParentClass: %s\n"), *BP->ParentClass->GetName());
			}
		}
	}

	return Result;
}

bool USLFAutomationLibrary::ApplyWeaponStatData(
	const FString& ItemAssetPath,
	bool bHasStatScaling,
	const TArray<FString>& ScalingStats,
	bool bHasStatRequirement,
	const TArray<FString>& RequirementStats,
	const TArray<FString>& StatChanges)
{
	// Load the item asset
	UObject* Asset = LoadObject<UObject>(nullptr, *ItemAssetPath);
	if (!Asset)
	{
		UE_LOG(LogTemp, Error, TEXT("[ApplyWeaponStatData] Failed to load asset: %s"), *ItemAssetPath);
		return false;
	}

	// Cast to UPDA_Item
	UPDA_Item* ItemData = Cast<UPDA_Item>(Asset);
	if (!ItemData)
	{
		// Try to get generated class if it's a Blueprint
		if (UBlueprint* BP = Cast<UBlueprint>(Asset))
		{
			if (BP->GeneratedClass)
			{
				ItemData = Cast<UPDA_Item>(BP->GeneratedClass->GetDefaultObject());
			}
		}
	}

	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("[ApplyWeaponStatData] Asset is not UPDA_Item: %s"), *ItemAssetPath);
		return false;
	}

	// Get reference to the equipment info
	FSLFEquipmentInfo& EquipInfo = ItemData->ItemInformation.EquipmentDetails;

	// Set scaling info
	EquipInfo.WeaponStatInfo.bHasStatScaling = bHasStatScaling;
	EquipInfo.WeaponStatInfo.ScalingInfo.Empty();

	for (const FString& ScalingStat : ScalingStats)
	{
		// Format: "Dexterity:A" or "Strength:S"
		FString StatName, GradeStr;
		if (ScalingStat.Split(TEXT(":"), &StatName, &GradeStr))
		{
			// Build the gameplay tag for this stat
			FString TagString = FString::Printf(TEXT("SoulslikeFramework.Stat.Primary.%s"), *StatName);
			FGameplayTag StatTag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);

			if (!StatTag.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("[ApplyWeaponStatData] Invalid stat tag: %s"), *TagString);
				continue;
			}

			// Convert grade string to enum
			ESLFStatScaling Grade = ESLFStatScaling::C; // Default
			if (GradeStr == TEXT("S")) Grade = ESLFStatScaling::S;
			else if (GradeStr == TEXT("A")) Grade = ESLFStatScaling::A;
			else if (GradeStr == TEXT("B")) Grade = ESLFStatScaling::B;
			else if (GradeStr == TEXT("C")) Grade = ESLFStatScaling::C;
			else if (GradeStr == TEXT("D")) Grade = ESLFStatScaling::D;
			else if (GradeStr == TEXT("E")) Grade = ESLFStatScaling::E;

			EquipInfo.WeaponStatInfo.ScalingInfo.Add(StatTag, Grade);
			UE_LOG(LogTemp, Log, TEXT("[ApplyWeaponStatData] Added scaling: %s -> %s"), *StatName, *GradeStr);
		}
	}

	// Set requirement info
	EquipInfo.WeaponStatInfo.bHasStatRequirement = bHasStatRequirement;
	EquipInfo.WeaponStatInfo.StatRequirementInfo.Empty();

	for (const FString& ReqStat : RequirementStats)
	{
		// Format: "Dexterity:10"
		FString StatName, ValueStr;
		if (ReqStat.Split(TEXT(":"), &StatName, &ValueStr))
		{
			// Build the gameplay tag for this stat
			FString TagString = FString::Printf(TEXT("SoulslikeFramework.Stat.Primary.%s"), *StatName);
			FGameplayTag StatTag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);

			if (!StatTag.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("[ApplyWeaponStatData] Invalid stat tag: %s"), *TagString);
				continue;
			}

			int32 Value = FCString::Atoi(*ValueStr);
			EquipInfo.WeaponStatInfo.StatRequirementInfo.Add(StatTag, Value);
			UE_LOG(LogTemp, Log, TEXT("[ApplyWeaponStatData] Added requirement: %s -> %d"), *StatName, Value);
		}
	}

	// Set stat changes (attack power)
	EquipInfo.StatChanges.Empty();

	for (const FString& Change : StatChanges)
	{
		// Format: "Physical:34" or "Magic:10"
		FString DamageType, ValueStr;
		if (Change.Split(TEXT(":"), &DamageType, &ValueStr))
		{
			// Build the gameplay tag for attack power
			FString TagString = FString::Printf(TEXT("SoulslikeFramework.Stat.Secondary.AttackPower.%s"), *DamageType);
			FGameplayTag StatTag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);

			if (!StatTag.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("[ApplyWeaponStatData] Invalid attack power tag: %s"), *TagString);
				continue;
			}

			double Value = FCString::Atod(*ValueStr);

			// Create equipment stat entry
			FSLFEquipmentStat StatEntry;
			StatEntry.StatTag = StatTag;
			StatEntry.Delta = Value;

			EquipInfo.StatChanges.Add(StatTag, StatEntry);
			UE_LOG(LogTemp, Log, TEXT("[ApplyWeaponStatData] Added stat change: %s -> %.0f"), *DamageType, Value);
		}
	}

	// Mark dirty and save
	ItemData->MarkPackageDirty();

	// Save the asset
	FString PackagePath = ItemData->GetPackage()->GetPathName();
	if (UEditorAssetLibrary::SaveAsset(PackagePath, false))
	{
		UE_LOG(LogTemp, Log, TEXT("[ApplyWeaponStatData] Saved: %s"), *ItemAssetPath);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ApplyWeaponStatData] Failed to save: %s"), *ItemAssetPath);
		return false;
	}
}

bool USLFAutomationLibrary::ApplyArmorStatChanges(
	const FString& ItemAssetPath,
	const TArray<FString>& StatChanges,
	const TArray<FString>& EquipSlots)
{
	UE_LOG(LogTemp, Log, TEXT("[ApplyArmorStatChanges] Processing: %s"), *ItemAssetPath);

	// Load the item asset
	UObject* Asset = LoadObject<UObject>(nullptr, *ItemAssetPath);
	if (!Asset)
	{
		UE_LOG(LogTemp, Error, TEXT("[ApplyArmorStatChanges] Failed to load asset: %s"), *ItemAssetPath);
		return false;
	}

	// Cast to UPDA_Item
	UPDA_Item* ItemData = Cast<UPDA_Item>(Asset);
	if (!ItemData)
	{
		// Try to get generated class if it's a Blueprint
		if (UBlueprint* BP = Cast<UBlueprint>(Asset))
		{
			if (BP->GeneratedClass)
			{
				ItemData = Cast<UPDA_Item>(BP->GeneratedClass->GetDefaultObject());
			}
		}
	}

	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("[ApplyArmorStatChanges] Asset is not UPDA_Item: %s"), *ItemAssetPath);
		return false;
	}

	// Get reference to the equipment info
	FSLFEquipmentInfo& EquipInfo = ItemData->ItemInformation.EquipmentDetails;

	// Clear and set stat changes
	EquipInfo.StatChanges.Empty();

	for (const FString& Change : StatChanges)
	{
		// Format: "SoulslikeFramework.Stat.Defense.Negation.Physical:5"
		FString TagString, ValueStr;
		if (Change.Split(TEXT(":"), &TagString, &ValueStr))
		{
			FGameplayTag StatTag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);

			if (!StatTag.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("[ApplyArmorStatChanges] Invalid tag: %s"), *TagString);
				continue;
			}

			double Value = FCString::Atod(*ValueStr);

			// Create equipment stat entry
			FSLFEquipmentStat StatEntry;
			StatEntry.StatTag = StatTag;
			StatEntry.Delta = Value;

			EquipInfo.StatChanges.Add(StatTag, StatEntry);
			UE_LOG(LogTemp, Log, TEXT("[ApplyArmorStatChanges] Added stat: %s -> %.1f"), *TagString, Value);
		}
	}

	// Set equip slots
	EquipInfo.EquipSlots.Reset();
	for (const FString& SlotTag : EquipSlots)
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*SlotTag), false);
		if (Tag.IsValid())
		{
			EquipInfo.EquipSlots.AddTag(Tag);
			UE_LOG(LogTemp, Log, TEXT("[ApplyArmorStatChanges] Added equip slot: %s"), *SlotTag);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ApplyArmorStatChanges] Invalid equip slot tag: %s"), *SlotTag);
		}
	}

	// Mark dirty and save
	ItemData->MarkPackageDirty();

	// Save the asset
	FString PackagePath = ItemData->GetPackage()->GetPathName();
	if (UEditorAssetLibrary::SaveAsset(PackagePath, false))
	{
		UE_LOG(LogTemp, Log, TEXT("[ApplyArmorStatChanges] Saved: %s with %d stats, %d slots"),
			*ItemAssetPath, EquipInfo.StatChanges.Num(), EquipInfo.EquipSlots.Num());
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ApplyArmorStatChanges] Failed to save: %s"), *ItemAssetPath);
		return false;
	}
}

// ============================================================================
// CONTAINER LOOT EXTRACTION
// ============================================================================

FString USLFAutomationLibrary::ExtractContainerLootConfig(const FString& OutputFilePath)
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("EXTRACTING CONTAINER LOOT CONFIGURATION"));
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	// Get editor world
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[ExtractContainerLootConfig] No editor world"));
		return TEXT("{}");
	}

	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	int32 ContainerCount = 0;

	// Iterate all actors
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		FString ClassName = Actor->GetClass()->GetName();
		if (!ClassName.Contains(TEXT("Container"), ESearchCase::IgnoreCase))
		{
			continue;
		}

		FString ActorName = Actor->GetName();
		FVector Location = Actor->GetActorLocation();

		UE_LOG(LogTemp, Warning, TEXT("\nFound container: %s"), *ActorName);
		UE_LOG(LogTemp, Warning, TEXT("  Location: X=%.1f Y=%.1f Z=%.1f"), Location.X, Location.Y, Location.Z);

		TSharedPtr<FJsonObject> ContainerObj = MakeShareable(new FJsonObject);
		ContainerObj->SetStringField(TEXT("name"), ActorName);

		TSharedPtr<FJsonObject> LocObj = MakeShareable(new FJsonObject);
		LocObj->SetNumberField(TEXT("x"), Location.X);
		LocObj->SetNumberField(TEXT("y"), Location.Y);
		LocObj->SetNumberField(TEXT("z"), Location.Z);
		ContainerObj->SetObjectField(TEXT("location"), LocObj);

		// Find LootDropManager component
		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);

		for (UActorComponent* Comp : Components)
		{
			if (!Comp) continue;

			FString CompClass = Comp->GetClass()->GetName();
			if (!CompClass.Contains(TEXT("LootDropManager"), ESearchCase::IgnoreCase))
			{
				continue;
			}

			UE_LOG(LogTemp, Warning, TEXT("  LootDropManager: %s"), *Comp->GetName());

			// Try to get OverrideItem via reflection
			FProperty* OverrideItemProp = Comp->GetClass()->FindPropertyByName(TEXT("OverrideItem"));
			if (OverrideItemProp)
			{
				if (FStructProperty* StructProp = CastField<FStructProperty>(OverrideItemProp))
				{
					const void* StructData = StructProp->ContainerPtrToValuePtr<void>(Comp);

					// Find Item property in the struct
					FProperty* ItemProp = StructProp->Struct->FindPropertyByName(TEXT("Item"));
					if (ItemProp)
					{
						if (FObjectProperty* ObjProp = CastField<FObjectProperty>(ItemProp))
						{
							UObject* ItemObj = ObjProp->GetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(StructData));
							if (ItemObj)
							{
								FString ItemPath = ItemObj->GetPathName();
								ContainerObj->SetStringField(TEXT("override_item"), ItemPath);
								UE_LOG(LogTemp, Warning, TEXT("    -> Item: %s"), *ItemPath);
							}
						}
					}

					// Find ItemClass property
					FProperty* ItemClassProp = StructProp->Struct->FindPropertyByName(TEXT("ItemClass"));
					if (ItemClassProp)
					{
						if (FClassProperty* ClassProp = CastField<FClassProperty>(ItemClassProp))
						{
							UClass* ItemClass = Cast<UClass>(ClassProp->GetObjectPropertyValue(ClassProp->ContainerPtrToValuePtr<void>(StructData)));
							if (ItemClass)
							{
								FString ClassPath = ItemClass->GetPathName();
								ContainerObj->SetStringField(TEXT("override_item_class"), ClassPath);
								UE_LOG(LogTemp, Warning, TEXT("    -> ItemClass: %s"), *ClassPath);
							}
						}
					}

					// Find MinAmount/MaxAmount
					FProperty* MinProp = StructProp->Struct->FindPropertyByName(TEXT("MinAmount"));
					FProperty* MaxProp = StructProp->Struct->FindPropertyByName(TEXT("MaxAmount"));
					if (MinProp && MaxProp)
					{
						if (FIntProperty* MinInt = CastField<FIntProperty>(MinProp))
						{
							int32 MinVal = MinInt->GetPropertyValue(MinInt->ContainerPtrToValuePtr<void>(StructData));
							ContainerObj->SetNumberField(TEXT("min_amount"), MinVal);
						}
						if (FIntProperty* MaxInt = CastField<FIntProperty>(MaxProp))
						{
							int32 MaxVal = MaxInt->GetPropertyValue(MaxInt->ContainerPtrToValuePtr<void>(StructData));
							ContainerObj->SetNumberField(TEXT("max_amount"), MaxVal);
						}
					}
				}
			}

			// Try to get LootTable
			FProperty* LootTableProp = Comp->GetClass()->FindPropertyByName(TEXT("LootTable"));
			if (LootTableProp)
			{
				if (FSoftObjectProperty* SoftProp = CastField<FSoftObjectProperty>(LootTableProp))
				{
					FSoftObjectPtr SoftPtr = SoftProp->GetPropertyValue(SoftProp->ContainerPtrToValuePtr<void>(Comp));
					if (!SoftPtr.IsNull())
					{
						FString TablePath = SoftPtr.ToString();
						ContainerObj->SetStringField(TEXT("loot_table"), TablePath);
						UE_LOG(LogTemp, Warning, TEXT("    -> LootTable: %s"), *TablePath);
					}
				}
			}
		}

		// Only add if we have loot config
		if (ContainerObj->HasField(TEXT("override_item")) || ContainerObj->HasField(TEXT("loot_table")))
		{
			RootObject->SetObjectField(ActorName, ContainerObj);
			ContainerCount++;
			UE_LOG(LogTemp, Warning, TEXT("  => Saved loot config"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  => No loot configured"));
		}
	}

	// Serialize to JSON string
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

	// Save to file if path provided
	if (!OutputFilePath.IsEmpty())
	{
		if (FFileHelper::SaveStringToFile(OutputString, *OutputFilePath))
		{
			UE_LOG(LogTemp, Warning, TEXT("\nSaved %d container configs to: %s"), ContainerCount, *OutputFilePath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save to: %s"), *OutputFilePath);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	return OutputString;
}

int32 USLFAutomationLibrary::ApplyContainerLootConfig(const FString& JsonFilePath)
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("APPLYING CONTAINER LOOT CONFIGURATION"));
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	// Load JSON file
	FString JsonContent;
	if (!FFileHelper::LoadFileToString(JsonContent, *JsonFilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("[ApplyContainerLootConfig] Failed to load: %s"), *JsonFilePath);
		return 0;
	}

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);
	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[ApplyContainerLootConfig] Failed to parse JSON"));
		return 0;
	}

	// Get editor world
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[ApplyContainerLootConfig] No editor world"));
		return 0;
	}

	int32 AppliedCount = 0;

	// Build a map of container names to actors
	TMap<FString, AActor*> ContainerActors;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		FString ClassName = Actor->GetClass()->GetName();
		if (ClassName.Contains(TEXT("Container"), ESearchCase::IgnoreCase))
		{
			ContainerActors.Add(Actor->GetName(), Actor);
		}
	}

	// Apply config to each container
	for (const auto& Pair : RootObject->Values)
	{
		FString ContainerName = Pair.Key;
		TSharedPtr<FJsonObject> ConfigObj = Pair.Value->AsObject();
		if (!ConfigObj.IsValid()) continue;

		// Find matching actor (by name or by location)
		AActor* TargetActor = nullptr;

		// First try exact name match
		if (AActor** Found = ContainerActors.Find(ContainerName))
		{
			TargetActor = *Found;
		}
		else
		{
			// Try location-based match
			const TSharedPtr<FJsonObject>* LocObj = nullptr;
			if (ConfigObj->TryGetObjectField(TEXT("location"), LocObj))
			{
				FVector ConfigLoc(
					(*LocObj)->GetNumberField(TEXT("x")),
					(*LocObj)->GetNumberField(TEXT("y")),
					(*LocObj)->GetNumberField(TEXT("z"))
				);

				// Find closest container
				float ClosestDist = 100.0f; // 100 unit tolerance
				for (const auto& ActorPair : ContainerActors)
				{
					float Dist = FVector::Dist(ActorPair.Value->GetActorLocation(), ConfigLoc);
					if (Dist < ClosestDist)
					{
						ClosestDist = Dist;
						TargetActor = ActorPair.Value;
					}
				}
			}
		}

		if (!TargetActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find container: %s"), *ContainerName);
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("\nApplying config to: %s"), *TargetActor->GetName());

		// Find LootDropManager component
		TArray<UActorComponent*> Components;
		TargetActor->GetComponents(Components);

		for (UActorComponent* Comp : Components)
		{
			if (!Comp) continue;

			FString CompClass = Comp->GetClass()->GetName();
			if (!CompClass.Contains(TEXT("LootDropManager"), ESearchCase::IgnoreCase))
			{
				continue;
			}

			// Apply override item
			FString OverrideItemPath;
			if (ConfigObj->TryGetStringField(TEXT("override_item"), OverrideItemPath))
			{
				UObject* ItemObj = LoadObject<UObject>(nullptr, *OverrideItemPath);
				if (ItemObj)
				{
					FProperty* OverrideItemProp = Comp->GetClass()->FindPropertyByName(TEXT("OverrideItem"));
					if (OverrideItemProp)
					{
						if (FStructProperty* StructProp = CastField<FStructProperty>(OverrideItemProp))
						{
							void* StructData = StructProp->ContainerPtrToValuePtr<void>(Comp);

							FProperty* ItemProp = StructProp->Struct->FindPropertyByName(TEXT("Item"));
							if (ItemProp)
							{
								if (FObjectProperty* ObjProp = CastField<FObjectProperty>(ItemProp))
								{
									ObjProp->SetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(StructData), ItemObj);
									UE_LOG(LogTemp, Warning, TEXT("  Set OverrideItem.Item: %s"), *ItemObj->GetName());
								}
							}
						}
					}
				}
			}

			// Apply loot table
			FString LootTablePath;
			if (ConfigObj->TryGetStringField(TEXT("loot_table"), LootTablePath))
			{
				FProperty* LootTableProp = Comp->GetClass()->FindPropertyByName(TEXT("LootTable"));
				if (LootTableProp)
				{
					if (FSoftObjectProperty* SoftProp = CastField<FSoftObjectProperty>(LootTableProp))
					{
						FSoftObjectPath SoftPath(LootTablePath);
						FSoftObjectPtr SoftPtr(SoftPath);
						SoftProp->SetPropertyValue(SoftProp->ContainerPtrToValuePtr<void>(Comp), SoftPtr);
						UE_LOG(LogTemp, Warning, TEXT("  Set LootTable: %s"), *LootTablePath);
					}
				}
			}

			AppliedCount++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("\nApplied loot config to %d containers"), AppliedCount);
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	return AppliedCount;
}

bool USLFAutomationLibrary::SetContainerLoot(int32 ContainerIndex, const FString& ItemAssetPath, int32 MinAmount, int32 MaxAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("[SetContainerLoot] Container %d -> %s (x%d-%d)"),
		ContainerIndex, *ItemAssetPath, MinAmount, MaxAmount);

	// Get editor world
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[SetContainerLoot] No editor world - load a level first"));
		return false;
	}

	// Load the item asset
	UObject* ItemObj = LoadObject<UObject>(nullptr, *ItemAssetPath);
	if (!ItemObj)
	{
		UE_LOG(LogTemp, Error, TEXT("[SetContainerLoot] Failed to load item: %s"), *ItemAssetPath);
		return false;
	}

	// Collect all containers sorted by location
	TArray<AActor*> Containers;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		FString ClassName = Actor->GetClass()->GetName();
		if (ClassName.Contains(TEXT("Container"), ESearchCase::IgnoreCase) &&
			!ClassName.Contains(TEXT("Currency"), ESearchCase::IgnoreCase))
		{
			Containers.Add(Actor);
		}
	}

	// Sort by X location for consistent ordering
	Containers.Sort([](const AActor& A, const AActor& B) {
		return A.GetActorLocation().X < B.GetActorLocation().X;
	});

	UE_LOG(LogTemp, Warning, TEXT("[SetContainerLoot] Found %d containers"), Containers.Num());

	if (ContainerIndex < 0 || ContainerIndex >= Containers.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("[SetContainerLoot] Invalid index %d (have %d containers)"),
			ContainerIndex, Containers.Num());
		return false;
	}

	AActor* TargetContainer = Containers[ContainerIndex];
	UE_LOG(LogTemp, Warning, TEXT("[SetContainerLoot] Target: %s at (%.1f, %.1f, %.1f)"),
		*TargetContainer->GetName(),
		TargetContainer->GetActorLocation().X,
		TargetContainer->GetActorLocation().Y,
		TargetContainer->GetActorLocation().Z);

	// Find LootDropManager component
	TArray<UActorComponent*> Components;
	TargetContainer->GetComponents(Components);

	for (UActorComponent* Comp : Components)
	{
		if (!Comp) continue;

		FString CompClass = Comp->GetClass()->GetName();
		if (!CompClass.Contains(TEXT("LootDropManager"), ESearchCase::IgnoreCase))
		{
			continue;
		}

		// Try to cast to C++ LootDropManagerComponent
		if (ULootDropManagerComponent* LootMgr = Cast<ULootDropManagerComponent>(Comp))
		{
			LootMgr->OverrideItem.Item = ItemObj;
			LootMgr->OverrideItem.MinAmount = MinAmount;
			LootMgr->OverrideItem.MaxAmount = MaxAmount;

			UE_LOG(LogTemp, Warning, TEXT("[SetContainerLoot] Set OverrideItem.Item = %s"), *ItemObj->GetName());

			// Mark the actor as modified
			TargetContainer->MarkPackageDirty();
			return true;
		}

		// Fallback: use reflection for Blueprint component
		FProperty* OverrideItemProp = Comp->GetClass()->FindPropertyByName(TEXT("OverrideItem"));
		if (!OverrideItemProp)
		{
			OverrideItemProp = Comp->GetClass()->FindPropertyByName(TEXT("Override Item"));
		}

		if (OverrideItemProp)
		{
			if (FStructProperty* StructProp = CastField<FStructProperty>(OverrideItemProp))
			{
				void* StructData = StructProp->ContainerPtrToValuePtr<void>(Comp);

				// Set Item property
				FProperty* ItemProp = StructProp->Struct->FindPropertyByName(TEXT("Item"));
				if (ItemProp)
				{
					if (FObjectProperty* ObjProp = CastField<FObjectProperty>(ItemProp))
					{
						ObjProp->SetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(StructData), ItemObj);
					}
				}

				// Set MinAmount
				FProperty* MinProp = StructProp->Struct->FindPropertyByName(TEXT("MinAmount"));
				if (MinProp)
				{
					if (FIntProperty* IntProp = CastField<FIntProperty>(MinProp))
					{
						IntProp->SetPropertyValue(IntProp->ContainerPtrToValuePtr<void>(StructData), MinAmount);
					}
				}

				// Set MaxAmount
				FProperty* MaxProp = StructProp->Struct->FindPropertyByName(TEXT("MaxAmount"));
				if (MaxProp)
				{
					if (FIntProperty* IntProp = CastField<FIntProperty>(MaxProp))
					{
						IntProp->SetPropertyValue(IntProp->ContainerPtrToValuePtr<void>(StructData), MaxAmount);
					}
				}

				UE_LOG(LogTemp, Warning, TEXT("[SetContainerLoot] Set via reflection: %s"), *ItemObj->GetName());
				TargetContainer->MarkPackageDirty();
				return true;
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("[SetContainerLoot] No LootDropManager component found on %s"),
		*TargetContainer->GetName());
	return false;
}

int32 USLFAutomationLibrary::ConfigureDefaultContainerLoot()
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("CONFIGURING DEFAULT CONTAINER LOOT"));
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	// Load the demo level
	const FString LevelPath = TEXT("/Game/SoulslikeFramework/Maps/L_Demo_Showcase");

	FEditorFileUtils::LoadMap(LevelPath, false, true);
	UE_LOG(LogTemp, Warning, TEXT("Loaded level: %s"), *LevelPath);

	// Define different items for each container
	struct FContainerConfig
	{
		FString ItemPath;
		int32 MinAmount;
		int32 MaxAmount;
	};

	TArray<FContainerConfig> Configs;
	// Container 0 (first by X location): Katana
	Configs.Add({TEXT("/Game/SoulslikeFramework/Data/Items/DA_Katana"), 1, 1});
	// Container 1 (second by X location): Health Flask
	Configs.Add({TEXT("/Game/SoulslikeFramework/Data/Items/DA_HealthFlask"), 2, 3});
	// Add more configs as needed for additional containers

	int32 ConfiguredCount = 0;

	for (int32 i = 0; i < Configs.Num(); i++)
	{
		if (SetContainerLoot(i, Configs[i].ItemPath, Configs[i].MinAmount, Configs[i].MaxAmount))
		{
			ConfiguredCount++;
		}
	}

	// Save the level
	if (ConfiguredCount > 0)
	{
		UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
		if (World)
		{
			FEditorFileUtils::SaveCurrentLevel();
			UE_LOG(LogTemp, Warning, TEXT("Saved level with %d configured containers"), ConfiguredCount);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("Configured %d containers with default loot"), ConfiguredCount);
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	return ConfiguredCount;
}

FString USLFAutomationLibrary::DiagnoseContainerLoot()
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("DIAGNOSING CONTAINER LOOT CONFIGURATION"));
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	FString Result;

	// Get editor world
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		Result = TEXT("ERROR: No editor world - load a level first");
		UE_LOG(LogTemp, Error, TEXT("%s"), *Result);
		return Result;
	}

	// Collect all containers
	TArray<AActor*> Containers;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		FString ClassName = Actor->GetClass()->GetName();
		if (ClassName.Contains(TEXT("B_Container")))
		{
			Containers.Add(Actor);
		}
	}

	// Sort by X location
	Containers.Sort([](const AActor& A, const AActor& B) {
		return A.GetActorLocation().X < B.GetActorLocation().X;
	});

	Result = FString::Printf(TEXT("Found %d containers\n"), Containers.Num());
	UE_LOG(LogTemp, Warning, TEXT("Found %d containers"), Containers.Num());

	for (int32 i = 0; i < Containers.Num(); i++)
	{
		AActor* Container = Containers[i];
		FVector Loc = Container->GetActorLocation();

		FString ContainerInfo = FString::Printf(TEXT("\n[Container %d] %s at (%.1f, %.1f, %.1f)\n"),
			i, *Container->GetName(), Loc.X, Loc.Y, Loc.Z);
		Result += ContainerInfo;
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ContainerInfo);

		// Find LootDropManager component
		UAC_LootDropManager* LootMgr = Container->FindComponentByClass<UAC_LootDropManager>();
		if (LootMgr)
		{
			// Check OverrideItem
			FString OverrideInfo;
			if (LootMgr->OverrideItem.Item)
			{
				OverrideInfo = FString::Printf(TEXT("  OverrideItem: %s (x%d-%d)"),
					*LootMgr->OverrideItem.Item->GetName(),
					LootMgr->OverrideItem.MinAmount,
					LootMgr->OverrideItem.MaxAmount);
			}
			else if (LootMgr->OverrideItem.ItemClass.Get())
			{
				OverrideInfo = FString::Printf(TEXT("  OverrideItem: ItemClass=%s (x%d-%d)"),
					*LootMgr->OverrideItem.ItemClass->GetName(),
					LootMgr->OverrideItem.MinAmount,
					LootMgr->OverrideItem.MaxAmount);
			}
			else
			{
				OverrideInfo = TEXT("  OverrideItem: EMPTY (will use LootTable)");
			}
			Result += OverrideInfo + TEXT("\n");
			UE_LOG(LogTemp, Warning, TEXT("%s"), *OverrideInfo);

			// Check LootTable
			FString TableInfo;
			if (!LootMgr->LootTable.IsNull())
			{
				TableInfo = FString::Printf(TEXT("  LootTable: %s"), *LootMgr->LootTable.GetAssetName());
			}
			else
			{
				TableInfo = TEXT("  LootTable: NOT SET");
			}
			Result += TableInfo + TEXT("\n");
			UE_LOG(LogTemp, Warning, TEXT("%s"), *TableInfo);
		}
		else
		{
			// Try via reflection for Blueprint component
			UActorComponent* BPLootMgr = nullptr;
			for (UActorComponent* Comp : Container->GetComponents())
			{
				if (Comp && Comp->GetName().Contains(TEXT("LootDropManager")))
				{
					BPLootMgr = Comp;
					break;
				}
			}

			if (BPLootMgr)
			{
				Result += FString::Printf(TEXT("  LootDropManager (BP): %s\n"), *BPLootMgr->GetClass()->GetName());
				UE_LOG(LogTemp, Warning, TEXT("  LootDropManager (BP): %s"), *BPLootMgr->GetClass()->GetName());

				// Try to get properties via reflection
				UClass* CompClass = BPLootMgr->GetClass();
				for (TFieldIterator<FProperty> PropIt(CompClass); PropIt; ++PropIt)
				{
					FProperty* Prop = *PropIt;
					FString PropName = Prop->GetName();
					if (PropName.Contains(TEXT("LootTable")) || PropName.Contains(TEXT("OverrideItem")))
					{
						FString PropInfo = FString::Printf(TEXT("    Property: %s (%s)"), *PropName, *Prop->GetClass()->GetName());
						Result += PropInfo + TEXT("\n");
						UE_LOG(LogTemp, Warning, TEXT("%s"), *PropInfo);
					}
				}
			}
			else
			{
				Result += TEXT("  No LootDropManager component found\n");
				UE_LOG(LogTemp, Warning, TEXT("  No LootDropManager component found"));
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	return Result;
}

bool USLFAutomationLibrary::SetContainerLootTable(int32 ContainerIndex, const FString& LootTablePath, bool bClearOverride)
{
	UE_LOG(LogTemp, Warning, TEXT("[SetContainerLootTable] Container %d -> %s (ClearOverride: %s)"),
		ContainerIndex, *LootTablePath, bClearOverride ? TEXT("true") : TEXT("false"));

	// Get editor world
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[SetContainerLootTable] No editor world - load a level first"));
		return false;
	}

	// Collect all containers sorted by location
	TArray<AActor*> Containers;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		FString ClassName = Actor->GetClass()->GetName();
		if (ClassName.Contains(TEXT("B_Container")))
		{
			Containers.Add(Actor);
		}
	}

	// Sort by X location
	Containers.Sort([](const AActor& A, const AActor& B) {
		return A.GetActorLocation().X < B.GetActorLocation().X;
	});

	if (ContainerIndex < 0 || ContainerIndex >= Containers.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("[SetContainerLootTable] Invalid index %d (have %d containers)"),
			ContainerIndex, Containers.Num());
		return false;
	}

	AActor* TargetContainer = Containers[ContainerIndex];

	// First try C++ type
	UAC_LootDropManager* LootMgr = TargetContainer->FindComponentByClass<UAC_LootDropManager>();
	if (LootMgr)
	{
		// Clear OverrideItem if requested
		if (bClearOverride)
		{
			LootMgr->OverrideItem.Item = nullptr;
			LootMgr->OverrideItem.ItemClass = nullptr;
			LootMgr->OverrideItem.MinAmount = 1;
			LootMgr->OverrideItem.MaxAmount = 1;
			UE_LOG(LogTemp, Warning, TEXT("[SetContainerLootTable] Cleared OverrideItem"));
		}

		// Set LootTable
		LootMgr->LootTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(LootTablePath));
		UE_LOG(LogTemp, Warning, TEXT("[SetContainerLootTable] Set LootTable = %s"), *LootTablePath);

		TargetContainer->MarkPackageDirty();
		return true;
	}

	// Fallback: Find by name and use reflection for Blueprint components
	UActorComponent* BPLootMgr = nullptr;
	for (UActorComponent* Comp : TargetContainer->GetComponents())
	{
		if (Comp && Comp->GetName().Contains(TEXT("LootDropManager")))
		{
			BPLootMgr = Comp;
			break;
		}
	}

	if (BPLootMgr)
	{
		UClass* CompClass = BPLootMgr->GetClass();
		UE_LOG(LogTemp, Warning, TEXT("[SetContainerLootTable] Found BP component: %s"), *CompClass->GetName());

		bool bSuccess = false;

		// Clear OverrideItem via reflection
		if (bClearOverride)
		{
			FStructProperty* OverrideProp = CastField<FStructProperty>(CompClass->FindPropertyByName(TEXT("OverrideItem")));
			if (OverrideProp)
			{
				void* OverrideData = OverrideProp->ContainerPtrToValuePtr<void>(BPLootMgr);
				// Find Item property within struct and set to nullptr
				FObjectPropertyBase* ItemProp = CastField<FObjectPropertyBase>(OverrideProp->Struct->FindPropertyByName(TEXT("Item")));
				if (ItemProp)
				{
					ItemProp->SetObjectPropertyValue(ItemProp->ContainerPtrToValuePtr<void>(OverrideData), nullptr);
					UE_LOG(LogTemp, Warning, TEXT("[SetContainerLootTable] Cleared OverrideItem.Item via reflection"));
				}
			}
		}

		// Set LootTable via reflection
		FSoftObjectProperty* TableProp = CastField<FSoftObjectProperty>(CompClass->FindPropertyByName(TEXT("LootTable")));
		if (TableProp)
		{
			void* PropData = TableProp->ContainerPtrToValuePtr<void>(BPLootMgr);
			TSoftObjectPtr<UDataTable>* SoftPtr = reinterpret_cast<TSoftObjectPtr<UDataTable>*>(PropData);
			*SoftPtr = TSoftObjectPtr<UDataTable>(FSoftObjectPath(LootTablePath));
			UE_LOG(LogTemp, Warning, TEXT("[SetContainerLootTable] Set LootTable = %s via reflection"), *LootTablePath);
			bSuccess = true;
		}

		if (bSuccess)
		{
			TargetContainer->MarkPackageDirty();
			return true;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("[SetContainerLootTable] No LootDropManager found on %s"),
		*TargetContainer->GetName());
	return false;
}

int32 USLFAutomationLibrary::ConfigureContainersToUseLootTable()
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("CONFIGURING CONTAINERS TO USE LOOT TABLE"));
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	// Default loot table path - FSoftObjectPath format requires AssetPath.AssetName
	const FString DefaultLootTable = TEXT("/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier.DT_ExampleChestTier");

	// Get editor world
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("No editor world - load a level first"));
		return 0;
	}

	// Collect all containers
	TArray<AActor*> Containers;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		FString ClassName = Actor->GetClass()->GetName();
		if (ClassName.Contains(TEXT("B_Container")))
		{
			Containers.Add(Actor);
		}
	}

	// Sort by X location
	Containers.Sort([](const AActor& A, const AActor& B) {
		return A.GetActorLocation().X < B.GetActorLocation().X;
	});

	UE_LOG(LogTemp, Warning, TEXT("Found %d containers"), Containers.Num());

	int32 ConfiguredCount = 0;
	for (int32 i = 0; i < Containers.Num(); i++)
	{
		if (SetContainerLootTable(i, DefaultLootTable, true))
		{
			ConfiguredCount++;
		}
	}

	// Save the level
	if (ConfiguredCount > 0)
	{
		FEditorFileUtils::SaveCurrentLevel();
		UE_LOG(LogTemp, Warning, TEXT("Saved level with %d configured containers"), ConfiguredCount);
	}

	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("Configured %d containers to use LootTable: %s"), ConfiguredCount, *DefaultLootTable);
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	return ConfiguredCount;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DATATABLE MIGRATION - Blueprint struct to C++ struct
// ═══════════════════════════════════════════════════════════════════════════════

// Helper: Strip GUID suffix from Blueprint property name
// "Item_10_589DC6564563A35CDB3A35BAEAD8377D" -> "Item"
static FString StripGuidSuffix(const FString& PropertyName)
{
	// Look for pattern: Name_Number_GUID
	int32 FirstUnderscore = INDEX_NONE;
	if (PropertyName.FindChar('_', FirstUnderscore))
	{
		// Check if there's another underscore after (indicates GUID pattern)
		FString AfterFirst = PropertyName.Mid(FirstUnderscore + 1);
		int32 SecondUnderscore = INDEX_NONE;
		if (AfterFirst.FindChar('_', SecondUnderscore))
		{
			// Check if what's after the second underscore looks like a GUID (32 hex chars)
			FString PossibleGuid = AfterFirst.Mid(SecondUnderscore + 1);
			if (PossibleGuid.Len() == 32)
			{
				// This looks like a GUID-suffixed property - return base name
				return PropertyName.Left(FirstUnderscore);
			}
		}
	}
	return PropertyName;
}

FString USLFAutomationLibrary::MigrateWeightedLootDataTable(const FString& SourceTablePath)
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("MIGRATING DATATABLE: Blueprint struct -> C++ struct"));
	UE_LOG(LogTemp, Warning, TEXT("Source: %s"), *SourceTablePath);
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	// Load the source DataTable
	UDataTable* SourceTable = LoadObject<UDataTable>(nullptr, *SourceTablePath);
	if (!SourceTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load DataTable: %s"), *SourceTablePath);
		return TEXT("{\"success\": false, \"error\": \"Failed to load source DataTable\"}");
	}

	// Get the row struct (this is the Blueprint struct FWeightedLoot)
	const UScriptStruct* RowStruct = SourceTable->GetRowStruct();
	if (!RowStruct)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable has no row struct"));
		return TEXT("{\"success\": false, \"error\": \"No row struct\"}");
	}

	UE_LOG(LogTemp, Warning, TEXT("Row Struct: %s"), *RowStruct->GetPathName());

	// Get all row names
	TArray<FName> RowNames = SourceTable->GetRowNames();
	UE_LOG(LogTemp, Warning, TEXT("Row Count: %d"), RowNames.Num());

	if (RowNames.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("DataTable is empty"));
		return TEXT("{\"success\": true, \"rows\": 0, \"message\": \"Table is empty\"}");
	}

	// Extract data from Blueprint struct rows
	TArray<TPair<FName, FSLFWeightedLoot>> ExtractedRows;

	for (const FName& RowName : RowNames)
	{
		// Get raw row data
		const uint8* RowData = SourceTable->FindRowUnchecked(RowName);
		if (!RowData)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Row %s: no data"), *RowName.ToString());
			continue;
		}

		FSLFWeightedLoot ExtractedLoot;
		bool bFoundWeight = false;
		bool bFoundItem = false;

		// Iterate through properties of the Blueprint struct
		for (TFieldIterator<FProperty> PropIt(RowStruct); PropIt; ++PropIt)
		{
			FProperty* Prop = *PropIt;
			FString PropName = Prop->GetName();
			FString BaseName = StripGuidSuffix(PropName);

			UE_LOG(LogTemp, Log, TEXT("  Property: %s -> BaseName: %s"), *PropName, *BaseName);

			if (BaseName.Equals(TEXT("Weight"), ESearchCase::IgnoreCase))
			{
				// Extract Weight (double)
				if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Prop))
				{
					ExtractedLoot.Weight = DoubleProp->GetPropertyValue_InContainer(RowData);
					bFoundWeight = true;
					UE_LOG(LogTemp, Warning, TEXT("  Row %s: Weight = %.2f"), *RowName.ToString(), ExtractedLoot.Weight);
				}
				else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop))
				{
					ExtractedLoot.Weight = (double)FloatProp->GetPropertyValue_InContainer(RowData);
					bFoundWeight = true;
					UE_LOG(LogTemp, Warning, TEXT("  Row %s: Weight = %.2f (from float)"), *RowName.ToString(), ExtractedLoot.Weight);
				}
			}
			else if (BaseName.Equals(TEXT("Item"), ESearchCase::IgnoreCase))
			{
				// Extract Item (nested struct FLootItem -> FSLFLootItem)
				if (FStructProperty* StructProp = CastField<FStructProperty>(Prop))
				{
					const uint8* ItemData = Prop->ContainerPtrToValuePtr<uint8>(RowData);
					const UScriptStruct* ItemStruct = StructProp->Struct;

					// Extract nested struct properties
					for (TFieldIterator<FProperty> ItemPropIt(ItemStruct); ItemPropIt; ++ItemPropIt)
					{
						FProperty* ItemProp = *ItemPropIt;
						FString ItemPropName = ItemProp->GetName();
						FString ItemBaseName = StripGuidSuffix(ItemPropName);

						UE_LOG(LogTemp, Log, TEXT("    Item Property: %s -> BaseName: %s"), *ItemPropName, *ItemBaseName);

						if (ItemBaseName.Equals(TEXT("ItemClass"), ESearchCase::IgnoreCase))
						{
							if (FClassProperty* ClassProp = CastField<FClassProperty>(ItemProp))
							{
								UClass* ItemClass = Cast<UClass>(ClassProp->GetObjectPropertyValue_InContainer(ItemData));
								ExtractedLoot.Item.ItemClass = ItemClass;
								if (ItemClass)
								{
									UE_LOG(LogTemp, Warning, TEXT("  Row %s: ItemClass = %s"), *RowName.ToString(), *ItemClass->GetName());
								}
							}
						}
						else if (ItemBaseName.Equals(TEXT("Item"), ESearchCase::IgnoreCase))
						{
							if (FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(ItemProp))
							{
								UObject* ItemObj = ObjProp->GetObjectPropertyValue_InContainer(ItemData);
								ExtractedLoot.Item.Item = ItemObj;
								if (ItemObj)
								{
									UE_LOG(LogTemp, Warning, TEXT("  Row %s: Item = %s"), *RowName.ToString(), *ItemObj->GetName());
								}
							}
							else if (FSoftObjectProperty* SoftProp = CastField<FSoftObjectProperty>(ItemProp))
							{
								FSoftObjectPtr SoftPtr = SoftProp->GetPropertyValue_InContainer(ItemData);
								ExtractedLoot.Item.Item = SoftPtr.Get();
								if (ExtractedLoot.Item.Item)
								{
									UE_LOG(LogTemp, Warning, TEXT("  Row %s: Item = %s (from soft ptr)"), *RowName.ToString(), *ExtractedLoot.Item.Item->GetName());
								}
							}
						}
						else if (ItemBaseName.Equals(TEXT("MinAmount"), ESearchCase::IgnoreCase))
						{
							if (FIntProperty* IntProp = CastField<FIntProperty>(ItemProp))
							{
								ExtractedLoot.Item.MinAmount = IntProp->GetPropertyValue_InContainer(ItemData);
							}
						}
						else if (ItemBaseName.Equals(TEXT("MaxAmount"), ESearchCase::IgnoreCase))
						{
							if (FIntProperty* IntProp = CastField<FIntProperty>(ItemProp))
							{
								ExtractedLoot.Item.MaxAmount = IntProp->GetPropertyValue_InContainer(ItemData);
							}
						}
					}
					bFoundItem = true;
				}
			}
		}

		if (bFoundWeight || bFoundItem)
		{
			ExtractedRows.Add(TPair<FName, FSLFWeightedLoot>(RowName, ExtractedLoot));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Extracted %d rows"), ExtractedRows.Num());

	if (ExtractedRows.Num() == 0)
	{
		return TEXT("{\"success\": false, \"error\": \"Failed to extract any data from rows\"}");
	}

	// Create new DataTable with C++ struct
	// We'll modify the existing table's row struct and repopulate it

	// Get the C++ struct
	UScriptStruct* CppStruct = FSLFWeightedLoot::StaticStruct();
	if (!CppStruct)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get FSLFWeightedLoot static struct"));
		return TEXT("{\"success\": false, \"error\": \"Failed to get C++ struct\"}");
	}

	UE_LOG(LogTemp, Warning, TEXT("C++ Struct: %s"), *CppStruct->GetPathName());

	// Clear existing rows from source table
	SourceTable->EmptyTable();

	// Change the row struct to C++
	// Note: This is a private property, but we can access it via reflection for migration
	FProperty* RowStructProp = UDataTable::StaticClass()->FindPropertyByName(TEXT("RowStruct"));
	if (RowStructProp)
	{
		FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(RowStructProp);
		if (ObjProp)
		{
			ObjProp->SetObjectPropertyValue_InContainer(SourceTable, CppStruct);
			UE_LOG(LogTemp, Warning, TEXT("Set RowStruct to FSLFWeightedLoot"));
		}
	}

	// Add rows with extracted data
	for (const TPair<FName, FSLFWeightedLoot>& Row : ExtractedRows)
	{
		// AddRow requires the struct data
		SourceTable->AddRow(Row.Key, Row.Value);
		UE_LOG(LogTemp, Warning, TEXT("Added row: %s (Weight=%.2f, Item=%s)"),
			*Row.Key.ToString(),
			Row.Value.Weight,
			Row.Value.Item.Item ? *Row.Value.Item.Item->GetName() : TEXT("null"));
	}

	// Mark dirty and save
	SourceTable->MarkPackageDirty();

	// Save the asset
	FString PackagePath = SourceTable->GetOutermost()->GetPathName();
	UPackage* Package = SourceTable->GetOutermost();
	FString PackageFilename = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());

	bool bSaved = UPackage::SavePackage(Package, SourceTable, *PackageFilename, FSavePackageArgs());

	if (bSaved)
	{
		UE_LOG(LogTemp, Warning, TEXT("Saved migrated DataTable: %s"), *PackageFilename);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save DataTable"));
	}

	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("MIGRATION COMPLETE: %d rows migrated"), ExtractedRows.Num());
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	return FString::Printf(TEXT("{\"success\": %s, \"rows\": %d}"),
		bSaved ? TEXT("true") : TEXT("false"),
		ExtractedRows.Num());
}

// ============================================================================
// STAT DEFAULTS EXPORT - Extract stat CDO values via C++ property iteration
// ============================================================================

FString USLFAutomationLibrary::ExportStatDefaults(const FString& OutputFilePath)
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("EXPORTING STAT DEFAULTS TO: %s"), *OutputFilePath);
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	// Map of stat name -> asset path
	TMap<FString, FString> StatPaths;

	// Defense (Damage Negation)
	StatPaths.Add(TEXT("B_DN_Physical"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Physical"));
	StatPaths.Add(TEXT("B_DN_Fire"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Fire"));
	StatPaths.Add(TEXT("B_DN_Frost"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Frost"));
	StatPaths.Add(TEXT("B_DN_Holy"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Holy"));
	StatPaths.Add(TEXT("B_DN_Lightning"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Lightning"));
	StatPaths.Add(TEXT("B_DN_Magic"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Magic"));

	// Resistances
	StatPaths.Add(TEXT("B_Resistance_Focus"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Focus"));
	StatPaths.Add(TEXT("B_Resistance_Immunity"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Immunity"));
	StatPaths.Add(TEXT("B_Resistance_Robustness"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Robustness"));
	StatPaths.Add(TEXT("B_Resistance_Vitality"), TEXT("/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Vitality"));

	// Attack Power
	StatPaths.Add(TEXT("B_AP_Physical"), TEXT("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical"));
	StatPaths.Add(TEXT("B_AP_Fire"), TEXT("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire"));
	StatPaths.Add(TEXT("B_AP_Frost"), TEXT("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Frost"));
	StatPaths.Add(TEXT("B_AP_Holy"), TEXT("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Holy"));
	StatPaths.Add(TEXT("B_AP_Lightning"), TEXT("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Lightning"));
	StatPaths.Add(TEXT("B_AP_Magic"), TEXT("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic"));

	// Backend stats
	StatPaths.Add(TEXT("B_Poise"), TEXT("/Game/SoulslikeFramework/Data/Stats/_Backend/B_Poise"));
	StatPaths.Add(TEXT("B_Stance"), TEXT("/Game/SoulslikeFramework/Data/Stats/_Backend/B_Stance"));
	StatPaths.Add(TEXT("B_IncantationPower"), TEXT("/Game/SoulslikeFramework/Data/Stats/_Backend/B_IncantationPower"));

	// Misc
	StatPaths.Add(TEXT("B_Weight"), TEXT("/Game/SoulslikeFramework/Data/Stats/Misc/B_Weight"));

	// Primary
	StatPaths.Add(TEXT("B_Vigor"), TEXT("/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor"));
	StatPaths.Add(TEXT("B_Strength"), TEXT("/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength"));
	StatPaths.Add(TEXT("B_Dexterity"), TEXT("/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity"));
	StatPaths.Add(TEXT("B_Intelligence"), TEXT("/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence"));
	StatPaths.Add(TEXT("B_Faith"), TEXT("/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith"));
	StatPaths.Add(TEXT("B_Arcane"), TEXT("/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane"));
	StatPaths.Add(TEXT("B_Mind"), TEXT("/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind"));
	StatPaths.Add(TEXT("B_Endurance"), TEXT("/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance"));

	// Secondary
	StatPaths.Add(TEXT("B_HP"), TEXT("/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP"));
	StatPaths.Add(TEXT("B_Stamina"), TEXT("/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina"));
	StatPaths.Add(TEXT("B_FP"), TEXT("/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP"));

	// Build JSON
	TSharedRef<FJsonObject> RootJson = MakeShared<FJsonObject>();
	int32 ExportCount = 0;

	for (const TPair<FString, FString>& StatEntry : StatPaths)
	{
		const FString& StatName = StatEntry.Key;
		const FString& StatPath = StatEntry.Value;

		UE_LOG(LogTemp, Warning, TEXT("Processing: %s (%s)"), *StatName, *StatPath);

		// Load the Blueprint asset
		UObject* LoadedObj = StaticLoadObject(UBlueprint::StaticClass(), nullptr, *StatPath);
		UBlueprint* BP = Cast<UBlueprint>(LoadedObj);
		if (!BP)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Could not load Blueprint: %s"), *StatPath);
			continue;
		}

		// Get generated class
		UClass* GenClass = BP->GeneratedClass;
		if (!GenClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("  No generated class"));
			continue;
		}

		// Get CDO
		UObject* CDO = GenClass->GetDefaultObject();
		if (!CDO)
		{
			UE_LOG(LogTemp, Warning, TEXT("  No CDO"));
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("  CDO class: %s"), *CDO->GetClass()->GetName());

		// Create JSON object for this stat
		TSharedRef<FJsonObject> StatJson = MakeShared<FJsonObject>();
		StatJson->SetStringField(TEXT("path"), StatPath);

		// Find StatInfo property by iterating through class properties
		// Blueprint property names have GUID suffixes, so we search for "StatInfo" prefix
		for (TFieldIterator<FProperty> PropIt(GenClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
		{
			FProperty* Prop = *PropIt;
			FString PropName = Prop->GetName();

			if (PropName.StartsWith(TEXT("StatInfo")))
			{
				UE_LOG(LogTemp, Warning, TEXT("  Found property: %s"), *PropName);

				// This is a struct property - get its internal properties
				FStructProperty* StructProp = CastField<FStructProperty>(Prop);
				if (!StructProp)
				{
					UE_LOG(LogTemp, Warning, TEXT("    Not a struct property"));
					continue;
				}

				void* StructData = Prop->ContainerPtrToValuePtr<void>(CDO);
				UScriptStruct* StructType = StructProp->Struct;

				UE_LOG(LogTemp, Warning, TEXT("    Struct type: %s"), *StructType->GetName());

				// Iterate through struct members
				for (TFieldIterator<FProperty> StructIt(StructType); StructIt; ++StructIt)
				{
					FProperty* MemberProp = *StructIt;
					FString MemberName = MemberProp->GetName();

					// Extract CurrentValue
					if (MemberName.StartsWith(TEXT("CurrentValue")))
					{
						if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(MemberProp))
						{
							double Value = DoubleProp->GetPropertyValue(MemberProp->ContainerPtrToValuePtr<void>(StructData));
							StatJson->SetNumberField(TEXT("current_value"), Value);
							UE_LOG(LogTemp, Warning, TEXT("    CurrentValue: %.2f"), Value);
						}
					}
					// Extract MaxValue
					else if (MemberName.StartsWith(TEXT("MaxValue")))
					{
						if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(MemberProp))
						{
							double Value = DoubleProp->GetPropertyValue(MemberProp->ContainerPtrToValuePtr<void>(StructData));
							StatJson->SetNumberField(TEXT("max_value"), Value);
							UE_LOG(LogTemp, Warning, TEXT("    MaxValue: %.2f"), Value);
						}
					}
					// Extract DisplayName
					else if (MemberName.StartsWith(TEXT("DisplayName")))
					{
						if (FTextProperty* TextProp = CastField<FTextProperty>(MemberProp))
						{
							FText Value = TextProp->GetPropertyValue(MemberProp->ContainerPtrToValuePtr<void>(StructData));
							StatJson->SetStringField(TEXT("display_name"), Value.ToString());
							UE_LOG(LogTemp, Warning, TEXT("    DisplayName: %s"), *Value.ToString());
						}
					}
					// Extract Tag
					else if (MemberName.StartsWith(TEXT("Tag")))
					{
						if (FStructProperty* TagStructProp = CastField<FStructProperty>(MemberProp))
						{
							void* TagData = MemberProp->ContainerPtrToValuePtr<void>(StructData);
							FGameplayTag* Tag = static_cast<FGameplayTag*>(TagData);
							if (Tag)
							{
								StatJson->SetStringField(TEXT("tag"), Tag->ToString());
								UE_LOG(LogTemp, Warning, TEXT("    Tag: %s"), *Tag->ToString());
							}
						}
					}
					// Extract ShowMaxValue
					else if (MemberName.StartsWith(TEXT("ShowMaxValue")))
					{
						if (FBoolProperty* BoolProp = CastField<FBoolProperty>(MemberProp))
						{
							bool Value = BoolProp->GetPropertyValue(MemberProp->ContainerPtrToValuePtr<void>(StructData));
							StatJson->SetBoolField(TEXT("show_max_value"), Value);
						}
					}
					// Extract DisplayAsPercent
					else if (MemberName.StartsWith(TEXT("DisplayasPercent")) || MemberName.StartsWith(TEXT("DisplayAsPercent")))
					{
						if (FBoolProperty* BoolProp = CastField<FBoolProperty>(MemberProp))
						{
							bool Value = BoolProp->GetPropertyValue(MemberProp->ContainerPtrToValuePtr<void>(StructData));
							StatJson->SetBoolField(TEXT("display_as_percent"), Value);
						}
					}
				}

				break; // Found StatInfo, no need to continue
			}
		}

		RootJson->SetObjectField(StatName, StatJson);
		ExportCount++;
	}

	// Serialize to string
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(RootJson, Writer);

	// Write to file
	if (!OutputFilePath.IsEmpty())
	{
		if (FFileHelper::SaveStringToFile(JsonString, *OutputFilePath))
		{
			UE_LOG(LogTemp, Warning, TEXT("Saved to: %s"), *OutputFilePath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save to: %s"), *OutputFilePath);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("EXPORTED %d STAT DEFAULTS"), ExportCount);
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	return JsonString;
}

// ============================================================================
// DESTRUCTIBLE DIAGNOSTICS
// ============================================================================

FString USLFAutomationLibrary::DiagnoseDestructible(const FString& BlueprintPath)
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("DIAGNOSING DESTRUCTIBLE: %s"), *BlueprintPath);
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	FString Result;
	Result += TEXT("=== DESTRUCTIBLE DIAGNOSIS ===\n");
	Result += FString::Printf(TEXT("Blueprint: %s\n\n"), *BlueprintPath);

	// Load the Blueprint
	UObject* LoadedObj = StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath);
	UBlueprint* BP = Cast<UBlueprint>(LoadedObj);
	if (!BP)
	{
		Result += TEXT("ERROR: Could not load Blueprint\n");
		UE_LOG(LogTemp, Error, TEXT("Could not load Blueprint: %s"), *BlueprintPath);
		return Result;
	}

	// Check parent class
	UClass* ParentClass = BP->ParentClass;
	Result += FString::Printf(TEXT("Parent Class: %s\n"), ParentClass ? *ParentClass->GetPathName() : TEXT("None"));
	UE_LOG(LogTemp, Warning, TEXT("Parent Class: %s"), ParentClass ? *ParentClass->GetPathName() : TEXT("None"));

	// Get generated class
	UClass* GenClass = BP->GeneratedClass;
	if (!GenClass)
	{
		Result += TEXT("ERROR: No generated class\n");
		return Result;
	}

	// Get CDO
	UObject* CDO = GenClass->GetDefaultObject();
	if (!CDO)
	{
		Result += TEXT("ERROR: No CDO\n");
		return Result;
	}

	Result += TEXT("\n=== CDO PROPERTIES ===\n");
	UE_LOG(LogTemp, Warning, TEXT("CDO Class: %s"), *CDO->GetClass()->GetName());

	// Check for GeometryCollection property
	FProperty* GCProp = GenClass->FindPropertyByName(TEXT("GeometryCollection"));
	if (GCProp)
	{
		FObjectProperty* ObjProp = CastField<FObjectProperty>(GCProp);
		if (ObjProp)
		{
			UObject* GCValue = ObjProp->GetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(CDO));
			FString GCPath = GCValue ? GCValue->GetPathName() : TEXT("None");
			Result += FString::Printf(TEXT("GeometryCollection (CDO): %s\n"), *GCPath);
			UE_LOG(LogTemp, Warning, TEXT("GeometryCollection (CDO): %s"), *GCPath);
		}
	}
	else
	{
		Result += TEXT("GeometryCollection property NOT FOUND on CDO!\n");
		UE_LOG(LogTemp, Warning, TEXT("GeometryCollection property NOT FOUND"));
	}

	// Check for DestructionSound property
	FProperty* SoundProp = GenClass->FindPropertyByName(TEXT("DestructionSound"));
	if (SoundProp)
	{
		// Soft object pointer
		FSoftObjectProperty* SoftProp = CastField<FSoftObjectProperty>(SoundProp);
		if (SoftProp)
		{
			FSoftObjectPtr SoftPtr = SoftProp->GetPropertyValue(SoftProp->ContainerPtrToValuePtr<void>(CDO));
			Result += FString::Printf(TEXT("DestructionSound (CDO): %s\n"), *SoftPtr.ToString());
			UE_LOG(LogTemp, Warning, TEXT("DestructionSound (CDO): %s"), *SoftPtr.ToString());
		}
	}

	// Now spawn and check runtime state
	Result += TEXT("\n=== SPAWN TEST ===\n");

	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		Result += TEXT("ERROR: No editor world for spawn test\n");
		return Result;
	}

	FVector SpawnLocation(0.0f, 0.0f, 10000.0f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedActor = World->SpawnActor<AActor>(GenClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (!SpawnedActor)
	{
		Result += TEXT("ERROR: Failed to spawn actor\n");
		return Result;
	}

	Result += FString::Printf(TEXT("Spawned: %s\n"), *SpawnedActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Spawned: %s"), *SpawnedActor->GetName());

	// Check GeometryCollection property on spawned instance
	if (GCProp)
	{
		FObjectProperty* ObjProp = CastField<FObjectProperty>(GCProp);
		if (ObjProp)
		{
			UObject* GCValue = ObjProp->GetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(SpawnedActor));
			FString GCPath = GCValue ? GCValue->GetPathName() : TEXT("None");
			Result += FString::Printf(TEXT("GeometryCollection (Spawned): %s\n"), *GCPath);
			UE_LOG(LogTemp, Warning, TEXT("GeometryCollection (Spawned): %s"), *GCPath);
		}
	}

	// Find GeometryCollectionComponent
	TArray<UGeometryCollectionComponent*> GCComponents;
	SpawnedActor->GetComponents<UGeometryCollectionComponent>(GCComponents);

	Result += FString::Printf(TEXT("\nGeometryCollectionComponents: %d\n"), GCComponents.Num());
	UE_LOG(LogTemp, Warning, TEXT("GeometryCollectionComponents: %d"), GCComponents.Num());

	for (int32 i = 0; i < GCComponents.Num(); i++)
	{
		UGeometryCollectionComponent* GC = GCComponents[i];
		Result += FString::Printf(TEXT("\n--- Component %d: %s ---\n"), i, *GC->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Component %d: %s"), i, *GC->GetName());

		// Check RestCollection
		const UGeometryCollection* RestColl = GC->GetRestCollection();
		FString RestPath = RestColl ? RestColl->GetPathName() : TEXT("None!");
		Result += FString::Printf(TEXT("RestCollection: %s\n"), *RestPath);
		UE_LOG(LogTemp, Warning, TEXT("  RestCollection: %s"), *RestPath);

		// Physics state
		bool bIsActive = GC->IsActive();
		bool bSimPhysics = GC->IsSimulatingPhysics();
		bool bIsVisible = GC->IsVisible();

		Result += FString::Printf(TEXT("IsActive: %s\n"), bIsActive ? TEXT("Yes") : TEXT("No"));
		Result += FString::Printf(TEXT("IsSimulatingPhysics: %s\n"), bSimPhysics ? TEXT("Yes") : TEXT("No"));
		Result += FString::Printf(TEXT("IsVisible: %s\n"), bIsVisible ? TEXT("Yes") : TEXT("No"));

		UE_LOG(LogTemp, Warning, TEXT("  IsActive: %s, SimPhysics: %s, Visible: %s"),
			bIsActive ? TEXT("Yes") : TEXT("No"),
			bSimPhysics ? TEXT("Yes") : TEXT("No"),
			bIsVisible ? TEXT("Yes") : TEXT("No"));

		// Collision settings
		ECollisionEnabled::Type CollisionEnabled = GC->GetCollisionEnabled();
		Result += FString::Printf(TEXT("CollisionEnabled: %d\n"), (int32)CollisionEnabled);

		// Check BodyInstance
		const FBodyInstance* BodyInst = GC->GetBodyInstance();
		if (BodyInst)
		{
			Result += FString::Printf(TEXT("BodyInstance.bSimulatePhysics: %s\n"),
				BodyInst->bSimulatePhysics ? TEXT("true") : TEXT("false"));
			Result += FString::Printf(TEXT("BodyInstance.MassInKg: %.2f\n"), BodyInst->GetMassOverride());

			UE_LOG(LogTemp, Warning, TEXT("  BodyInstance: SimPhysics=%s"),
				BodyInst->bSimulatePhysics ? TEXT("true") : TEXT("false"));
		}

		// Chaos-specific settings - access directly via property
		bool bNotifyBreaks = GC->bNotifyBreaks;
		bool bNotifyCollisions = GC->bNotifyCollisions;
		Result += FString::Printf(TEXT("NotifyBreaks: %s\n"), bNotifyBreaks ? TEXT("Yes") : TEXT("No"));
		Result += FString::Printf(TEXT("NotifyCollisions: %s\n"), bNotifyCollisions ? TEXT("Yes") : TEXT("No"));

		UE_LOG(LogTemp, Warning, TEXT("  NotifyBreaks: %s, NotifyCollisions: %s"),
			bNotifyBreaks ? TEXT("Yes") : TEXT("No"),
			bNotifyCollisions ? TEXT("Yes") : TEXT("No"));
	}

	// Cleanup
	SpawnedActor->Destroy();

	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("DIAGNOSIS COMPLETE"));
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	return Result;
}

FString USLFAutomationLibrary::CompareDestructibleSettings(const FString& BlueprintPath)
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("COMPARING DESTRUCTIBLE SETTINGS: %s"), *BlueprintPath);
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	FString Result;
	Result += TEXT("=== DESTRUCTIBLE COMPARISON ===\n\n");

	// Expected working values (from bp_only analysis)
	struct FExpectedSettings
	{
		bool bSimulatePhysics = true;
		bool bEnableGravity = true;
		bool bNotifyBreaks = true;
		bool bNotifyCollisions = false;
		ECollisionEnabled::Type CollisionEnabled = ECollisionEnabled::QueryAndPhysics;
		float MassInKg = 100.0f;
	};
	FExpectedSettings Expected;

	// Load and spawn
	UObject* LoadedObj = StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath);
	UBlueprint* BP = Cast<UBlueprint>(LoadedObj);
	if (!BP || !BP->GeneratedClass)
	{
		Result += TEXT("ERROR: Could not load Blueprint\n");
		return Result;
	}

	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		Result += TEXT("ERROR: No editor world\n");
		return Result;
	}

	FVector SpawnLocation(0.0f, 0.0f, 10000.0f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedActor = World->SpawnActor<AActor>(BP->GeneratedClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (!SpawnedActor)
	{
		Result += TEXT("ERROR: Failed to spawn\n");
		return Result;
	}

	// Find GC component
	TArray<UGeometryCollectionComponent*> GCComponents;
	SpawnedActor->GetComponents<UGeometryCollectionComponent>(GCComponents);

	if (GCComponents.Num() == 0)
	{
		Result += TEXT("ERROR: No GeometryCollectionComponent found!\n");
		SpawnedActor->Destroy();
		return Result;
	}

	UGeometryCollectionComponent* GC = GCComponents[0];

	// Check RestCollection FIRST - this is critical
	const UGeometryCollection* RestColl = GC->GetRestCollection();
	if (!RestColl)
	{
		Result += TEXT("*** CRITICAL: RestCollection is NULL! ***\n");
		Result += TEXT("This means OnConstruction isn't setting the GeometryCollection.\n");
		Result += TEXT("Check: 1) GeometryCollection property is set on Blueprint CDO\n");
		Result += TEXT("       2) OnConstruction calls SetRestCollection(GC, true)\n\n");
		UE_LOG(LogTemp, Error, TEXT("CRITICAL: RestCollection is NULL!"));
	}
	else
	{
		Result += FString::Printf(TEXT("RestCollection: %s (OK)\n\n"), *RestColl->GetPathName());
	}

	// Compare physics settings
	Result += TEXT("Setting                  | Actual    | Expected  | Status\n");
	Result += TEXT("-------------------------|-----------|-----------|-------\n");

	// SimulatePhysics
	const FBodyInstance* BodyInst = GC->GetBodyInstance();
	bool bActualSimPhysics = BodyInst ? BodyInst->bSimulatePhysics : false;
	Result += FString::Printf(TEXT("SimulatePhysics          | %-9s | %-9s | %s\n"),
		bActualSimPhysics ? TEXT("true") : TEXT("false"),
		Expected.bSimulatePhysics ? TEXT("true") : TEXT("false"),
		bActualSimPhysics == Expected.bSimulatePhysics ? TEXT("OK") : TEXT("MISMATCH"));

	// Collision
	ECollisionEnabled::Type ActualCollision = GC->GetCollisionEnabled();
	Result += FString::Printf(TEXT("CollisionEnabled         | %-9d | %-9d | %s\n"),
		(int32)ActualCollision,
		(int32)Expected.CollisionEnabled,
		ActualCollision == Expected.CollisionEnabled ? TEXT("OK") : TEXT("MISMATCH"));

	// NotifyBreaks
	bool bActualNotifyBreaks = GC->bNotifyBreaks;
	Result += FString::Printf(TEXT("NotifyBreaks             | %-9s | %-9s | %s\n"),
		bActualNotifyBreaks ? TEXT("true") : TEXT("false"),
		Expected.bNotifyBreaks ? TEXT("true") : TEXT("false"),
		bActualNotifyBreaks == Expected.bNotifyBreaks ? TEXT("OK") : TEXT("MISMATCH"));

	// NotifyCollisions
	bool bActualNotifyCollisions = GC->bNotifyCollisions;
	Result += FString::Printf(TEXT("NotifyCollisions         | %-9s | %-9s | %s\n"),
		bActualNotifyCollisions ? TEXT("true") : TEXT("false"),
		Expected.bNotifyCollisions ? TEXT("true") : TEXT("false"),
		bActualNotifyCollisions == Expected.bNotifyCollisions ? TEXT("OK") : TEXT("MISMATCH"));

	// IsActive
	bool bIsActive = GC->IsActive();
	Result += FString::Printf(TEXT("IsActive                 | %-9s | %-9s | %s\n"),
		bIsActive ? TEXT("true") : TEXT("false"),
		TEXT("true"),
		bIsActive ? TEXT("OK") : TEXT("MISMATCH"));

	// IsSimulatingPhysics (runtime)
	bool bSimPhysics = GC->IsSimulatingPhysics();
	Result += FString::Printf(TEXT("IsSimulatingPhysics      | %-9s | %-9s | %s\n"),
		bSimPhysics ? TEXT("true") : TEXT("false"),
		TEXT("true"),
		bSimPhysics ? TEXT("OK") : TEXT("MISMATCH"));

	SpawnedActor->Destroy();

	UE_LOG(LogTemp, Warning, TEXT("\n%s"), *Result);
	return Result;
}

FString USLFAutomationLibrary::FixDestructibleGeometryCollection(const FString& BlueprintPath, const FString& GeometryCollectionPath)
{
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	UE_LOG(LogTemp, Warning, TEXT("FIXING DESTRUCTIBLE: %s"), *BlueprintPath);
	UE_LOG(LogTemp, Warning, TEXT("GC Asset: %s"), *GeometryCollectionPath);
	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));

	FString Result;

	// Load the GeometryCollection asset
	UGeometryCollection* GCAsset = LoadObject<UGeometryCollection>(nullptr, *GeometryCollectionPath);
	if (!GCAsset)
	{
		Result = FString::Printf(TEXT("ERROR: Could not load GeometryCollection: %s"), *GeometryCollectionPath);
		UE_LOG(LogTemp, Error, TEXT("%s"), *Result);
		return Result;
	}

	UE_LOG(LogTemp, Warning, TEXT("Loaded GC Asset: %s"), *GCAsset->GetPathName());

	// Load the Blueprint
	UObject* LoadedObj = StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath);
	UBlueprint* BP = Cast<UBlueprint>(LoadedObj);
	if (!BP)
	{
		Result = FString::Printf(TEXT("ERROR: Could not load Blueprint: %s"), *BlueprintPath);
		UE_LOG(LogTemp, Error, TEXT("%s"), *Result);
		return Result;
	}

	UClass* GenClass = BP->GeneratedClass;
	if (!GenClass)
	{
		Result = TEXT("ERROR: No generated class");
		return Result;
	}

	// Get CDO
	UObject* CDO = GenClass->GetDefaultObject();
	if (!CDO)
	{
		Result = TEXT("ERROR: No CDO");
		return Result;
	}

	// Find and set GeometryCollection property
	FProperty* GCProp = GenClass->FindPropertyByName(TEXT("GeometryCollection"));
	if (!GCProp)
	{
		Result = TEXT("ERROR: GeometryCollection property not found on CDO");
		UE_LOG(LogTemp, Error, TEXT("%s"), *Result);
		return Result;
	}

	FObjectProperty* ObjProp = CastField<FObjectProperty>(GCProp);
	if (!ObjProp)
	{
		Result = TEXT("ERROR: GeometryCollection is not an object property");
		return Result;
	}

	// Check current value
	UObject* CurrentValue = ObjProp->GetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(CDO));
	UE_LOG(LogTemp, Warning, TEXT("Current GeometryCollection: %s"), CurrentValue ? *CurrentValue->GetPathName() : TEXT("None"));

	// Set the new value
	ObjProp->SetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(CDO), GCAsset);

	// Verify
	UObject* NewValue = ObjProp->GetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(CDO));
	UE_LOG(LogTemp, Warning, TEXT("Set GeometryCollection to: %s"), NewValue ? *NewValue->GetPathName() : TEXT("None"));

	// Mark dirty
	CDO->MarkPackageDirty();
	BP->MarkPackageDirty();

	// Compile the Blueprint
	FKismetEditorUtilities::CompileBlueprint(BP);
	UE_LOG(LogTemp, Warning, TEXT("Compiled Blueprint"));

	// Save the Blueprint
	FString PackagePath = BP->GetOutermost()->GetPathName();
	UPackage* Package = BP->GetOutermost();
	FString PackageFilename = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());

	bool bSaved = UPackage::SavePackage(Package, BP, *PackageFilename, FSavePackageArgs());

	if (bSaved)
	{
		Result = FString::Printf(TEXT("SUCCESS: Set GeometryCollection to %s and saved"), *GCAsset->GetName());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
	}
	else
	{
		Result = TEXT("WARNING: Set property but failed to save package");
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
	}

	// Spawn test to verify
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (World)
	{
		FVector SpawnLocation(0.0f, 0.0f, 10000.0f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* TestActor = World->SpawnActor<AActor>(GenClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if (TestActor)
		{
			TArray<UGeometryCollectionComponent*> GCComponents;
			TestActor->GetComponents<UGeometryCollectionComponent>(GCComponents);

			if (GCComponents.Num() > 0)
			{
				const UGeometryCollection* RestColl = GCComponents[0]->GetRestCollection();
				Result += FString::Printf(TEXT("\nVerification - RestCollection: %s"),
					RestColl ? *RestColl->GetPathName() : TEXT("STILL NULL - OnConstruction may not be running!"));
				UE_LOG(LogTemp, Warning, TEXT("Verification - RestCollection: %s"),
					RestColl ? *RestColl->GetPathName() : TEXT("STILL NULL"));
			}

			TestActor->Destroy();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("======================================================================"));
	return Result;
}

// ============================================================================
// COMPREHENSIVE VALIDATION FUNCTIONS
// ============================================================================

FString USLFAutomationLibrary::ValidateBlueprintMigration(
	const FString& BlueprintPath,
	const FString& ExpectedParentClassPath,
	bool bExpectClearedEventGraphs,
	bool bExpectClearedFunctions)
{
	TSharedRef<FJsonObject> ResultJson = MakeShared<FJsonObject>();
	ResultJson->SetStringField(TEXT("BlueprintPath"), BlueprintPath);
	ResultJson->SetBoolField(TEXT("Success"), false);

	UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath));
	if (!BP)
	{
		ResultJson->SetStringField(TEXT("Error"), TEXT("Failed to load Blueprint"));
		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(ResultJson, Writer);
		return OutputString;
	}

	// Check parent class
	FString ActualParent = BP->ParentClass ? BP->ParentClass->GetPathName() : TEXT("None");
	bool bParentMatches = ActualParent.Contains(ExpectedParentClassPath) || ActualParent == ExpectedParentClassPath;
	ResultJson->SetStringField(TEXT("ExpectedParent"), ExpectedParentClassPath);
	ResultJson->SetStringField(TEXT("ActualParent"), ActualParent);
	ResultJson->SetBoolField(TEXT("ParentCorrect"), bParentMatches);

	// Check event graphs
	int32 EventGraphNodeCount = 0;
	for (UEdGraph* Graph : BP->UbergraphPages)
	{
		if (Graph)
		{
			// Count non-entry nodes
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node && !Node->IsA<UK2Node_Event>())
				{
					EventGraphNodeCount++;
				}
			}
		}
	}
	bool bEventGraphsCleared = (EventGraphNodeCount == 0);
	ResultJson->SetNumberField(TEXT("EventGraphNodeCount"), EventGraphNodeCount);
	ResultJson->SetBoolField(TEXT("EventGraphsCleared"), bEventGraphsCleared);
	ResultJson->SetBoolField(TEXT("EventGraphsClearedAsExpected"), bExpectClearedEventGraphs ? bEventGraphsCleared : true);

	// Check function graphs
	int32 TotalFunctionNodes = 0;
	TArray<TSharedPtr<FJsonValue>> FunctionArray;
	for (UEdGraph* Graph : BP->FunctionGraphs)
	{
		if (Graph)
		{
			TSharedRef<FJsonObject> FuncObj = MakeShared<FJsonObject>();
			FuncObj->SetStringField(TEXT("Name"), Graph->GetName());
			int32 NodeCount = 0;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node && !Node->IsA<UK2Node_FunctionEntry>())
				{
					NodeCount++;
				}
			}
			FuncObj->SetNumberField(TEXT("NodeCount"), NodeCount);
			FunctionArray.Add(MakeShared<FJsonValueObject>(FuncObj));
			TotalFunctionNodes += NodeCount;
		}
	}
	ResultJson->SetArrayField(TEXT("Functions"), FunctionArray);
	bool bFunctionsCleared = (TotalFunctionNodes == 0);
	ResultJson->SetBoolField(TEXT("FunctionsCleared"), bFunctionsCleared);
	ResultJson->SetBoolField(TEXT("FunctionsClearedAsExpected"), bExpectClearedFunctions ? bFunctionsCleared : true);

	// Check variables
	TArray<TSharedPtr<FJsonValue>> VarArray;
	for (const FBPVariableDescription& Var : BP->NewVariables)
	{
		TSharedRef<FJsonObject> VarObj = MakeShared<FJsonObject>();
		VarObj->SetStringField(TEXT("Name"), Var.VarName.ToString());
		VarObj->SetStringField(TEXT("Type"), Var.VarType.PinCategory.ToString());
		VarArray.Add(MakeShared<FJsonValueObject>(VarObj));
	}
	ResultJson->SetArrayField(TEXT("Variables"), VarArray);
	ResultJson->SetNumberField(TEXT("VariableCount"), BP->NewVariables.Num());

	// Check SCS components
	TArray<TSharedPtr<FJsonValue>> SCSArray;
	if (USimpleConstructionScript* SCS = BP->SimpleConstructionScript)
	{
		for (USCS_Node* Node : SCS->GetAllNodes())
		{
			if (Node && Node->ComponentTemplate)
			{
				TSharedRef<FJsonObject> CompObj = MakeShared<FJsonObject>();
				CompObj->SetStringField(TEXT("Name"), Node->GetVariableName().ToString());
				CompObj->SetStringField(TEXT("Class"), Node->ComponentTemplate->GetClass()->GetName());
				SCSArray.Add(MakeShared<FJsonValueObject>(CompObj));
			}
		}
	}
	ResultJson->SetArrayField(TEXT("SCSComponents"), SCSArray);

	// Overall success
	bool bSuccess = bParentMatches &&
		(bExpectClearedEventGraphs ? bEventGraphsCleared : true) &&
		(bExpectClearedFunctions ? bFunctionsCleared : true);
	ResultJson->SetBoolField(TEXT("Success"), bSuccess);

	// Serialize to string
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(ResultJson, Writer);

	UE_LOG(LogTemp, Warning, TEXT("ValidateBlueprintMigration: %s - Success=%s, Parent=%s"),
		*BlueprintPath, bSuccess ? TEXT("YES") : TEXT("NO"), *ActualParent);

	return OutputString;
}

FString USLFAutomationLibrary::ExportAnimNotifyStateDetails(const FString& BlueprintPath)
{
	TSharedRef<FJsonObject> ResultJson = MakeShared<FJsonObject>();
	ResultJson->SetStringField(TEXT("BlueprintPath"), BlueprintPath);

	UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath));
	if (!BP)
	{
		ResultJson->SetStringField(TEXT("Error"), TEXT("Failed to load Blueprint"));
		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(ResultJson, Writer);
		return OutputString;
	}

	ResultJson->SetStringField(TEXT("BlueprintName"), BP->GetName());

	// Parent class
	FString ParentPath = BP->ParentClass ? BP->ParentClass->GetPathName() : TEXT("None");
	ResultJson->SetStringField(TEXT("ParentClass"), ParentPath);

	// Check if parent is C++ native
	bool bParentIsNative = BP->ParentClass && !BP->ParentClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint);
	ResultJson->SetBoolField(TEXT("ParentIsNative"), bParentIsNative);

	// Event graphs with details
	TArray<TSharedPtr<FJsonValue>> EventGraphArray;
	for (UEdGraph* Graph : BP->UbergraphPages)
	{
		if (Graph)
		{
			TSharedRef<FJsonObject> GraphObj = MakeShared<FJsonObject>();
			GraphObj->SetStringField(TEXT("Name"), Graph->GetName());

			int32 TotalNodes = Graph->Nodes.Num();
			int32 EventNodes = 0;
			int32 LogicNodes = 0;
			TArray<FString> NodeTypes;

			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node)
				{
					NodeTypes.AddUnique(Node->GetClass()->GetName());
					if (Node->IsA<UK2Node_Event>() || Node->IsA<UK2Node_CustomEvent>())
					{
						EventNodes++;
					}
					else
					{
						LogicNodes++;
					}
				}
			}

			GraphObj->SetNumberField(TEXT("TotalNodes"), TotalNodes);
			GraphObj->SetNumberField(TEXT("EventNodes"), EventNodes);
			GraphObj->SetNumberField(TEXT("LogicNodes"), LogicNodes);
			GraphObj->SetBoolField(TEXT("HasLogic"), LogicNodes > 0);

			TArray<TSharedPtr<FJsonValue>> TypesArray;
			for (const FString& Type : NodeTypes)
			{
				TypesArray.Add(MakeShared<FJsonValueString>(Type));
			}
			GraphObj->SetArrayField(TEXT("NodeTypes"), TypesArray);

			EventGraphArray.Add(MakeShared<FJsonValueObject>(GraphObj));
		}
	}
	ResultJson->SetArrayField(TEXT("EventGraphs"), EventGraphArray);

	// Check for NotifyBegin/NotifyEnd/NotifyTick implementations
	bool bHasNotifyBeginImpl = false;
	bool bHasNotifyEndImpl = false;
	bool bHasNotifyTickImpl = false;

	for (UEdGraph* Graph : BP->FunctionGraphs)
	{
		if (Graph)
		{
			FString GraphName = Graph->GetName();
			int32 LogicNodeCount = 0;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node && !Node->IsA<UK2Node_FunctionEntry>())
				{
					LogicNodeCount++;
				}
			}

			if (GraphName.Contains(TEXT("NotifyBegin")) || GraphName.Contains(TEXT("Received_NotifyBegin")))
			{
				bHasNotifyBeginImpl = LogicNodeCount > 0;
				ResultJson->SetNumberField(TEXT("NotifyBeginNodeCount"), LogicNodeCount);
			}
			else if (GraphName.Contains(TEXT("NotifyEnd")) || GraphName.Contains(TEXT("Received_NotifyEnd")))
			{
				bHasNotifyEndImpl = LogicNodeCount > 0;
				ResultJson->SetNumberField(TEXT("NotifyEndNodeCount"), LogicNodeCount);
			}
			else if (GraphName.Contains(TEXT("NotifyTick")) || GraphName.Contains(TEXT("Received_NotifyTick")))
			{
				bHasNotifyTickImpl = LogicNodeCount > 0;
				ResultJson->SetNumberField(TEXT("NotifyTickNodeCount"), LogicNodeCount);
			}
		}
	}

	ResultJson->SetBoolField(TEXT("HasNotifyBeginLogic"), bHasNotifyBeginImpl);
	ResultJson->SetBoolField(TEXT("HasNotifyEndLogic"), bHasNotifyEndImpl);
	ResultJson->SetBoolField(TEXT("HasNotifyTickLogic"), bHasNotifyTickImpl);

	// Serialize
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(ResultJson, Writer);

	return OutputString;
}

FString USLFAutomationLibrary::ValidateChaosDestructionMigration()
{
	UE_LOG(LogTemp, Warning, TEXT("=== ValidateChaosDestructionMigration ==="));

	TSharedRef<FJsonObject> ResultJson = MakeShared<FJsonObject>();
	ResultJson->SetBoolField(TEXT("OverallSuccess"), true);

	// ---- ANS_ToggleChaosField ----
	{
		TSharedRef<FJsonObject> ANSJson = MakeShared<FJsonObject>();

		FString BPPath = TEXT("/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_ToggleChaosField");
		FString ExpectedParent = TEXT("/Script/SLFConversion.ANS_ToggleChaosField");

		UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BPPath));
		if (BP)
		{
			FString ActualParent = BP->ParentClass ? BP->ParentClass->GetPathName() : TEXT("None");
			bool bParentCorrect = ActualParent.Contains(TEXT("ANS_ToggleChaosField")) &&
				ActualParent.Contains(TEXT("/Script/SLFConversion"));

			ANSJson->SetStringField(TEXT("ActualParent"), ActualParent);
			ANSJson->SetBoolField(TEXT("ParentCorrect"), bParentCorrect);

			// Check event graph
			int32 LogicNodes = 0;
			for (UEdGraph* Graph : BP->UbergraphPages)
			{
				if (Graph)
				{
					for (UEdGraphNode* Node : Graph->Nodes)
					{
						if (Node && !Node->IsA<UK2Node_Event>())
						{
							LogicNodes++;
						}
					}
				}
			}
			ANSJson->SetNumberField(TEXT("EventGraphLogicNodes"), LogicNodes);
			ANSJson->SetBoolField(TEXT("EventGraphCleared"), LogicNodes == 0);

			// Check function graphs (should have NotifyBegin/NotifyEnd with NO logic after reparent)
			int32 NotifyBeginNodes = 0;
			int32 NotifyEndNodes = 0;
			for (UEdGraph* Graph : BP->FunctionGraphs)
			{
				if (Graph)
				{
					FString Name = Graph->GetName();
					int32 NodeCount = 0;
					for (UEdGraphNode* Node : Graph->Nodes)
					{
						if (Node && !Node->IsA<UK2Node_FunctionEntry>())
						{
							NodeCount++;
						}
					}
					if (Name.Contains(TEXT("NotifyBegin")))
					{
						NotifyBeginNodes = NodeCount;
					}
					else if (Name.Contains(TEXT("NotifyEnd")))
					{
						NotifyEndNodes = NodeCount;
					}
				}
			}
			ANSJson->SetNumberField(TEXT("NotifyBeginNodes"), NotifyBeginNodes);
			ANSJson->SetNumberField(TEXT("NotifyEndNodes"), NotifyEndNodes);

			bool bANSSuccess = bParentCorrect && LogicNodes == 0;
			ANSJson->SetBoolField(TEXT("Success"), bANSSuccess);
			if (!bANSSuccess) ResultJson->SetBoolField(TEXT("OverallSuccess"), false);

			UE_LOG(LogTemp, Warning, TEXT("ANS_ToggleChaosField: Parent=%s, EventGraphNodes=%d, Success=%s"),
				*ActualParent, LogicNodes, bANSSuccess ? TEXT("YES") : TEXT("NO"));
		}
		else
		{
			ANSJson->SetStringField(TEXT("Error"), TEXT("Failed to load"));
			ANSJson->SetBoolField(TEXT("Success"), false);
			ResultJson->SetBoolField(TEXT("OverallSuccess"), false);
			UE_LOG(LogTemp, Error, TEXT("ANS_ToggleChaosField: FAILED TO LOAD"));
		}

		ResultJson->SetObjectField(TEXT("ANS_ToggleChaosField"), ANSJson);
	}

	// ---- B_Chaos_ForceField ----
	{
		TSharedRef<FJsonObject> CFJson = MakeShared<FJsonObject>();

		FString BPPath = TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField");
		FString ExpectedParent = TEXT("/Script/SLFConversion.B_Chaos_ForceField");

		UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BPPath));
		if (BP)
		{
			FString ActualParent = BP->ParentClass ? BP->ParentClass->GetPathName() : TEXT("None");
			bool bParentCorrect = ActualParent.Contains(TEXT("B_Chaos_ForceField")) &&
				ActualParent.Contains(TEXT("/Script/SLFConversion"));

			CFJson->SetStringField(TEXT("ActualParent"), ActualParent);
			CFJson->SetBoolField(TEXT("ParentCorrect"), bParentCorrect);

			// Event graph
			int32 LogicNodes = 0;
			for (UEdGraph* Graph : BP->UbergraphPages)
			{
				if (Graph)
				{
					for (UEdGraphNode* Node : Graph->Nodes)
					{
						if (Node && !Node->IsA<UK2Node_Event>())
						{
							LogicNodes++;
						}
					}
				}
			}
			CFJson->SetNumberField(TEXT("EventGraphLogicNodes"), LogicNodes);
			CFJson->SetBoolField(TEXT("EventGraphCleared"), LogicNodes == 0);

			bool bCFSuccess = bParentCorrect && LogicNodes == 0;
			CFJson->SetBoolField(TEXT("Success"), bCFSuccess);
			if (!bCFSuccess) ResultJson->SetBoolField(TEXT("OverallSuccess"), false);

			UE_LOG(LogTemp, Warning, TEXT("B_Chaos_ForceField: Parent=%s, EventGraphNodes=%d, Success=%s"),
				*ActualParent, LogicNodes, bCFSuccess ? TEXT("YES") : TEXT("NO"));
		}
		else
		{
			CFJson->SetStringField(TEXT("Error"), TEXT("Failed to load"));
			CFJson->SetBoolField(TEXT("Success"), false);
			ResultJson->SetBoolField(TEXT("OverallSuccess"), false);
			UE_LOG(LogTemp, Error, TEXT("B_Chaos_ForceField: FAILED TO LOAD"));
		}

		ResultJson->SetObjectField(TEXT("B_Chaos_ForceField"), CFJson);
	}

	// ---- B_Destructible ----
	{
		TSharedRef<FJsonObject> BDJson = MakeShared<FJsonObject>();

		FString BPPath = TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible");
		FString ExpectedParent = TEXT("/Script/SLFConversion.B_Destructible");

		UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BPPath));
		if (BP)
		{
			FString ActualParent = BP->ParentClass ? BP->ParentClass->GetPathName() : TEXT("None");
			bool bParentCorrect = ActualParent.Contains(TEXT("B_Destructible")) &&
				ActualParent.Contains(TEXT("/Script/SLFConversion"));

			BDJson->SetStringField(TEXT("ActualParent"), ActualParent);
			BDJson->SetBoolField(TEXT("ParentCorrect"), bParentCorrect);

			// Event graph
			int32 LogicNodes = 0;
			for (UEdGraph* Graph : BP->UbergraphPages)
			{
				if (Graph)
				{
					for (UEdGraphNode* Node : Graph->Nodes)
					{
						if (Node && !Node->IsA<UK2Node_Event>())
						{
							LogicNodes++;
						}
					}
				}
			}
			BDJson->SetNumberField(TEXT("EventGraphLogicNodes"), LogicNodes);
			BDJson->SetBoolField(TEXT("EventGraphCleared"), LogicNodes == 0);

			bool bBDSuccess = bParentCorrect && LogicNodes == 0;
			BDJson->SetBoolField(TEXT("Success"), bBDSuccess);
			if (!bBDSuccess) ResultJson->SetBoolField(TEXT("OverallSuccess"), false);

			UE_LOG(LogTemp, Warning, TEXT("B_Destructible: Parent=%s, EventGraphNodes=%d, Success=%s"),
				*ActualParent, LogicNodes, bBDSuccess ? TEXT("YES") : TEXT("NO"));
		}
		else
		{
			BDJson->SetStringField(TEXT("Error"), TEXT("Failed to load"));
			BDJson->SetBoolField(TEXT("Success"), false);
			ResultJson->SetBoolField(TEXT("OverallSuccess"), false);
			UE_LOG(LogTemp, Error, TEXT("B_Destructible: FAILED TO LOAD"));
		}

		ResultJson->SetObjectField(TEXT("B_Destructible"), BDJson);
	}

	// ---- Check C++ classes exist ----
	{
		TSharedRef<FJsonObject> CppJson = MakeShared<FJsonObject>();

		UClass* ANSClass = FindObject<UClass>(nullptr, TEXT("/Script/SLFConversion.ANS_ToggleChaosField"));
		UClass* CFClass = FindObject<UClass>(nullptr, TEXT("/Script/SLFConversion.B_Chaos_ForceField"));
		UClass* BDClass = FindObject<UClass>(nullptr, TEXT("/Script/SLFConversion.B_Destructible"));

		CppJson->SetBoolField(TEXT("ANS_ToggleChaosField_Exists"), ANSClass != nullptr);
		CppJson->SetBoolField(TEXT("B_Chaos_ForceField_Exists"), CFClass != nullptr);
		CppJson->SetBoolField(TEXT("B_Destructible_Exists"), BDClass != nullptr);

		UE_LOG(LogTemp, Warning, TEXT("C++ Classes: ANS=%s, CF=%s, BD=%s"),
			ANSClass ? TEXT("EXISTS") : TEXT("MISSING"),
			CFClass ? TEXT("EXISTS") : TEXT("MISSING"),
			BDClass ? TEXT("EXISTS") : TEXT("MISSING"));

		if (!ANSClass || !CFClass || !BDClass)
		{
			ResultJson->SetBoolField(TEXT("OverallSuccess"), false);
		}

		ResultJson->SetObjectField(TEXT("CppClasses"), CppJson);
	}

	// Serialize
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(ResultJson, Writer);

	UE_LOG(LogTemp, Warning, TEXT("=== Validation Complete: %s ==="),
		ResultJson->GetBoolField(TEXT("OverallSuccess")) ? TEXT("ALL PASSED") : TEXT("ISSUES FOUND"));

	return OutputString;
}

FString USLFAutomationLibrary::ForceReparentAndValidate(const FString& BlueprintPath, const FString& NewParentClassPath)
{
	TSharedRef<FJsonObject> ResultJson = MakeShared<FJsonObject>();
	ResultJson->SetStringField(TEXT("BlueprintPath"), BlueprintPath);
	ResultJson->SetStringField(TEXT("NewParentClassPath"), NewParentClassPath);

	UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath));
	if (!BP)
	{
		ResultJson->SetStringField(TEXT("Error"), TEXT("Failed to load Blueprint"));
		ResultJson->SetBoolField(TEXT("Success"), false);
		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(ResultJson, Writer);
		return OutputString;
	}

	// Store before state
	FString BeforeParent = BP->ParentClass ? BP->ParentClass->GetPathName() : TEXT("None");
	ResultJson->SetStringField(TEXT("BeforeParent"), BeforeParent);

	// Find new parent class
	UClass* NewParentClass = FindObject<UClass>(nullptr, *NewParentClassPath);
	if (!NewParentClass)
	{
		// Try loading it
		NewParentClass = LoadClass<UObject>(nullptr, *NewParentClassPath);
	}

	if (!NewParentClass)
	{
		ResultJson->SetStringField(TEXT("Error"), FString::Printf(TEXT("Failed to find new parent class: %s"), *NewParentClassPath));
		ResultJson->SetBoolField(TEXT("Success"), false);
		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(ResultJson, Writer);
		return OutputString;
	}

	// Reparent using FBlueprintEditorUtils
	BP->ParentClass = NewParentClass;
	FBlueprintEditorUtils::RefreshAllNodes(BP);

	// Clear event graphs
	int32 ClearedNodes = 0;
	for (UEdGraph* Graph : BP->UbergraphPages)
	{
		if (Graph)
		{
			TArray<UEdGraphNode*> NodesToRemove;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node && !Node->IsA<UK2Node_Event>())
				{
					NodesToRemove.Add(Node);
				}
			}
			for (UEdGraphNode* Node : NodesToRemove)
			{
				Graph->RemoveNode(Node);
				ClearedNodes++;
			}
		}
	}
	ResultJson->SetNumberField(TEXT("ClearedEventGraphNodes"), ClearedNodes);

	// Compile
	FKismetEditorUtilities::CompileBlueprint(BP);

	// Save
	FString PackagePath = BP->GetOutermost()->GetPathName();
	UPackage* Package = BP->GetOutermost();
	FString PackageFilename = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
	bool bSaved = UPackage::SavePackage(Package, BP, *PackageFilename, FSavePackageArgs());
	ResultJson->SetBoolField(TEXT("Saved"), bSaved);

	// Validate after state
	FString AfterParent = BP->ParentClass ? BP->ParentClass->GetPathName() : TEXT("None");
	ResultJson->SetStringField(TEXT("AfterParent"), AfterParent);

	bool bSuccess = AfterParent.Contains(NewParentClassPath) || AfterParent == NewParentClassPath;
	ResultJson->SetBoolField(TEXT("Success"), bSuccess);

	UE_LOG(LogTemp, Warning, TEXT("ForceReparentAndValidate: %s -> %s (Success=%s)"),
		*BeforeParent, *AfterParent, bSuccess ? TEXT("YES") : TEXT("NO"));

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(ResultJson, Writer);

	return OutputString;
}

FString USLFAutomationLibrary::ClearAllEventGraphNodes(const FString& BlueprintPath, bool bIncludeEventNodes)
{
	TSharedRef<FJsonObject> ResultJson = MakeShared<FJsonObject>();
	ResultJson->SetStringField(TEXT("BlueprintPath"), BlueprintPath);
	ResultJson->SetBoolField(TEXT("IncludeEventNodes"), bIncludeEventNodes);

	UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath));
	if (!BP)
	{
		ResultJson->SetStringField(TEXT("Error"), TEXT("Failed to load Blueprint"));
		ResultJson->SetBoolField(TEXT("Success"), false);
		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(ResultJson, Writer);
		return OutputString;
	}

	int32 EventNodesCleared = 0;
	int32 OtherNodesCleared = 0;
	TArray<FString> ClearedEventNames;

	// Clear all nodes from event graphs
	for (UEdGraph* Graph : BP->UbergraphPages)
	{
		if (Graph)
		{
			TArray<UEdGraphNode*> NodesToRemove;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (!Node) continue;

				// Check if this is an Event node
				bool bIsEventNode = Node->IsA<UK2Node_Event>() || Node->IsA<UK2Node_CustomEvent>();

				if (bIsEventNode)
				{
					if (bIncludeEventNodes)
					{
						// Get event name for logging
						if (UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node))
						{
							FString EventName = EventNode->GetFunctionName().ToString();
							ClearedEventNames.Add(EventName);
							UE_LOG(LogTemp, Warning, TEXT("[ClearAllEventGraphNodes] Removing Event node: %s"), *EventName);
						}
						NodesToRemove.Add(Node);
						EventNodesCleared++;
					}
				}
				else
				{
					// Not an event node, always remove
					NodesToRemove.Add(Node);
					OtherNodesCleared++;
				}
			}

			for (UEdGraphNode* Node : NodesToRemove)
			{
				Graph->RemoveNode(Node);
			}
		}
	}

	ResultJson->SetNumberField(TEXT("EventNodesCleared"), EventNodesCleared);
	ResultJson->SetNumberField(TEXT("OtherNodesCleared"), OtherNodesCleared);

	// Convert ClearedEventNames to JSON array
	TArray<TSharedPtr<FJsonValue>> EventNamesArray;
	for (const FString& Name : ClearedEventNames)
	{
		EventNamesArray.Add(MakeShared<FJsonValueString>(Name));
	}
	ResultJson->SetArrayField(TEXT("ClearedEventNames"), EventNamesArray);

	// Compile and save
	FKismetEditorUtilities::CompileBlueprint(BP);

	FString PackagePath = BP->GetOutermost()->GetPathName();
	UPackage* Package = BP->GetOutermost();
	FString PackageFilename = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
	bool bSaved = UPackage::SavePackage(Package, BP, *PackageFilename, FSavePackageArgs());
	ResultJson->SetBoolField(TEXT("Saved"), bSaved);

	bool bSuccess = (EventNodesCleared + OtherNodesCleared) > 0 || bSaved;
	ResultJson->SetBoolField(TEXT("Success"), bSuccess);

	UE_LOG(LogTemp, Warning, TEXT("[ClearAllEventGraphNodes] %s - EventNodes=%d, OtherNodes=%d, Saved=%s"),
		*BlueprintPath, EventNodesCleared, OtherNodesCleared, bSaved ? TEXT("YES") : TEXT("NO"));

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(ResultJson, Writer);

	return OutputString;
}

bool USLFAutomationLibrary::HasEventGraphLogic(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return false;

	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		if (Graph)
		{
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node && !Node->IsA<UK2Node_Event>() && !Node->IsA<UK2Node_CustomEvent>())
				{
					return true;
				}
			}
		}
	}
	return false;
}

int32 USLFAutomationLibrary::GetFunctionGraphNodeCount(UObject* BlueprintAsset, const FString& FunctionName)
{
	UBlueprint* Blueprint = GetBlueprintFromAsset(BlueprintAsset);
	if (!Blueprint) return -1;

	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
	{
		if (Graph && Graph->GetName().Contains(FunctionName))
		{
			int32 Count = 0;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node && !Node->IsA<UK2Node_FunctionEntry>())
				{
					Count++;
				}
			}
			return Count;
		}
	}
	return -1;
}

FString USLFAutomationLibrary::CompareToBpOnly(const FString& BlueprintPath, const FString& BpOnlyExportJsonPath)
{
	// Load bp_only JSON
	FString BpOnlyJson;
	if (!FFileHelper::LoadFileToString(BpOnlyJson, *BpOnlyExportJsonPath))
	{
		return FString::Printf(TEXT("Error: Failed to load bp_only export: %s"), *BpOnlyExportJsonPath);
	}

	TSharedPtr<FJsonObject> BpOnlyObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BpOnlyJson);
	if (!FJsonSerializer::Deserialize(Reader, BpOnlyObj) || !BpOnlyObj.IsValid())
	{
		return TEXT("Error: Failed to parse bp_only JSON");
	}

	// Load current Blueprint
	UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath));
	if (!BP)
	{
		return FString::Printf(TEXT("Error: Failed to load Blueprint: %s"), *BlueprintPath);
	}

	TSharedRef<FJsonObject> CompareJson = MakeShared<FJsonObject>();
	CompareJson->SetStringField(TEXT("BlueprintPath"), BlueprintPath);
	CompareJson->SetStringField(TEXT("BpOnlyExportPath"), BpOnlyExportJsonPath);

	// Compare variables
	TArray<FString> CurrentVars;
	for (const FBPVariableDescription& Var : BP->NewVariables)
	{
		CurrentVars.Add(Var.VarName.ToString());
	}
	CompareJson->SetNumberField(TEXT("CurrentVariableCount"), CurrentVars.Num());

	// Compare functions
	TArray<FString> CurrentFuncs;
	for (UEdGraph* Graph : BP->FunctionGraphs)
	{
		if (Graph)
		{
			CurrentFuncs.Add(Graph->GetName());
		}
	}
	CompareJson->SetNumberField(TEXT("CurrentFunctionCount"), CurrentFuncs.Num());

	// Event graph status
	int32 EventGraphNodes = 0;
	for (UEdGraph* Graph : BP->UbergraphPages)
	{
		if (Graph)
		{
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node && !Node->IsA<UK2Node_Event>())
				{
					EventGraphNodes++;
				}
			}
		}
	}
	CompareJson->SetNumberField(TEXT("CurrentEventGraphLogicNodes"), EventGraphNodes);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(CompareJson, Writer);

	return OutputString;
}

FString USLFAutomationLibrary::DiagnoseInterfaceImplementation(const FString& BlueprintPath)
{
	UE_LOG(LogTemp, Warning, TEXT("=== DiagnoseInterfaceImplementation: %s ==="), *BlueprintPath);

	TSharedRef<FJsonObject> ResultJson = MakeShared<FJsonObject>();
	ResultJson->SetStringField(TEXT("BlueprintPath"), BlueprintPath);

	// Load the Blueprint
	UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath));
	if (!BP)
	{
		ResultJson->SetStringField(TEXT("Error"), TEXT("Failed to load Blueprint"));
		UE_LOG(LogTemp, Error, TEXT("Failed to load Blueprint: %s"), *BlueprintPath);

		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(ResultJson, Writer);
		return OutputString;
	}

	// Get the generated class
	UBlueprintGeneratedClass* GenClass = Cast<UBlueprintGeneratedClass>(BP->GeneratedClass);
	if (!GenClass)
	{
		ResultJson->SetStringField(TEXT("Error"), TEXT("No GeneratedClass"));
		UE_LOG(LogTemp, Error, TEXT("No GeneratedClass for: %s"), *BlueprintPath);

		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(ResultJson, Writer);
		return OutputString;
	}

	ResultJson->SetStringField(TEXT("GeneratedClass"), GenClass->GetPathName());
	UE_LOG(LogTemp, Warning, TEXT("GeneratedClass: %s"), *GenClass->GetPathName());

	// Get parent class
	UClass* ParentClass = BP->ParentClass;
	if (ParentClass)
	{
		ResultJson->SetStringField(TEXT("ParentClass"), ParentClass->GetPathName());
		UE_LOG(LogTemp, Warning, TEXT("ParentClass: %s"), *ParentClass->GetPathName());

		// Check if parent is native C++ or Blueprint
		bool bParentIsNative = !ParentClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint);
		ResultJson->SetBoolField(TEXT("ParentIsNativeC++"), bParentIsNative);
		UE_LOG(LogTemp, Warning, TEXT("ParentIsNativeC++: %s"), bParentIsNative ? TEXT("YES") : TEXT("NO"));
	}

	// Check interfaces implemented by the Blueprint itself
	TArray<TSharedPtr<FJsonValue>> BlueprintInterfaceArray;
	for (const FBPInterfaceDescription& InterfaceDesc : BP->ImplementedInterfaces)
	{
		if (InterfaceDesc.Interface)
		{
			TSharedRef<FJsonObject> InterfaceObj = MakeShared<FJsonObject>();
			InterfaceObj->SetStringField(TEXT("Interface"), InterfaceDesc.Interface->GetPathName());
			InterfaceObj->SetNumberField(TEXT("GraphCount"), InterfaceDesc.Graphs.Num());

			UE_LOG(LogTemp, Warning, TEXT("Blueprint Implements Interface: %s (Graphs: %d)"),
				*InterfaceDesc.Interface->GetPathName(), InterfaceDesc.Graphs.Num());

			BlueprintInterfaceArray.Add(MakeShared<FJsonValueObject>(InterfaceObj));
		}
	}
	ResultJson->SetArrayField(TEXT("BlueprintImplementedInterfaces"), BlueprintInterfaceArray);

	// Check interfaces on the generated class hierarchy (includes C++ parent interfaces)
	TArray<TSharedPtr<FJsonValue>> ClassInterfaceArray;
	for (const FImplementedInterface& ImplementedInterface : GenClass->Interfaces)
	{
		if (ImplementedInterface.Class)
		{
			TSharedRef<FJsonObject> InterfaceObj = MakeShared<FJsonObject>();
			InterfaceObj->SetStringField(TEXT("Interface"), ImplementedInterface.Class->GetPathName());

			bool bIsNativeInterface = !ImplementedInterface.Class->HasAnyClassFlags(CLASS_CompiledFromBlueprint);
			InterfaceObj->SetBoolField(TEXT("IsNativeC++"), bIsNativeInterface);
			InterfaceObj->SetBoolField(TEXT("bImplementedByK2"), ImplementedInterface.bImplementedByK2);

			UE_LOG(LogTemp, Warning, TEXT("Class Hierarchy Implements: %s (Native: %s, ImplementedByK2: %s)"),
				*ImplementedInterface.Class->GetPathName(),
				bIsNativeInterface ? TEXT("YES") : TEXT("NO"),
				ImplementedInterface.bImplementedByK2 ? TEXT("YES") : TEXT("NO"));

			ClassInterfaceArray.Add(MakeShared<FJsonValueObject>(InterfaceObj));
		}
	}
	ResultJson->SetArrayField(TEXT("ClassHierarchyInterfaces"), ClassInterfaceArray);

	// Specifically check for ISLFDestructibleHelperInterface
	bool bImplementsDestructibleHelper = GenClass->ImplementsInterface(USLFDestructibleHelperInterface::StaticClass());
	ResultJson->SetBoolField(TEXT("ImplementsISLFDestructibleHelperInterface"), bImplementsDestructibleHelper);
	UE_LOG(LogTemp, Warning, TEXT("ImplementsISLFDestructibleHelperInterface: %s"),
		bImplementsDestructibleHelper ? TEXT("YES") : TEXT("NO"));

	// Check if the C++ parent class itself implements the interface
	if (ParentClass)
	{
		bool bParentImplementsInterface = ParentClass->ImplementsInterface(USLFDestructibleHelperInterface::StaticClass());
		ResultJson->SetBoolField(TEXT("ParentImplementsISLFDestructibleHelperInterface"), bParentImplementsInterface);
		UE_LOG(LogTemp, Warning, TEXT("ParentImplementsISLFDestructibleHelperInterface: %s"),
			bParentImplementsInterface ? TEXT("YES") : TEXT("NO"));
	}

	// Check if Blueprint has its OWN interface implementation graphs for EnableChaosDestroy/DisableChaosDestroy
	// These would override the C++ _Implementation
	bool bHasEnableChaosDestroyGraph = false;
	bool bHasDisableChaosDestroyGraph = false;

	for (const FBPInterfaceDescription& InterfaceDesc : BP->ImplementedInterfaces)
	{
		for (UEdGraph* Graph : InterfaceDesc.Graphs)
		{
			if (Graph)
			{
				FString GraphName = Graph->GetName();
				UE_LOG(LogTemp, Warning, TEXT("Interface Function Graph: %s (Nodes: %d)"),
					*GraphName, Graph->Nodes.Num());

				if (GraphName.Contains(TEXT("EnableChaosDestroy")))
				{
					bHasEnableChaosDestroyGraph = true;
					int32 LogicNodes = 0;
					for (UEdGraphNode* Node : Graph->Nodes)
					{
						if (Node && !Node->IsA<UK2Node_FunctionEntry>())
						{
							LogicNodes++;
						}
					}
					UE_LOG(LogTemp, Warning, TEXT("  -> EnableChaosDestroy Graph has %d logic nodes"), LogicNodes);
				}
				if (GraphName.Contains(TEXT("DisableChaosDestroy")))
				{
					bHasDisableChaosDestroyGraph = true;
					int32 LogicNodes = 0;
					for (UEdGraphNode* Node : Graph->Nodes)
					{
						if (Node && !Node->IsA<UK2Node_FunctionEntry>())
						{
							LogicNodes++;
						}
					}
					UE_LOG(LogTemp, Warning, TEXT("  -> DisableChaosDestroy Graph has %d logic nodes"), LogicNodes);
				}
			}
		}
	}

	ResultJson->SetBoolField(TEXT("HasEnableChaosDestroyGraph"), bHasEnableChaosDestroyGraph);
	ResultJson->SetBoolField(TEXT("HasDisableChaosDestroyGraph"), bHasDisableChaosDestroyGraph);

	// Check FunctionGraphs for interface functions as well
	for (UEdGraph* Graph : BP->FunctionGraphs)
	{
		if (Graph)
		{
			FString GraphName = Graph->GetName();
			if (GraphName.Contains(TEXT("EnableChaosDestroy")) || GraphName.Contains(TEXT("DisableChaosDestroy")))
			{
				UE_LOG(LogTemp, Warning, TEXT("FunctionGraph Found: %s (Nodes: %d)"),
					*GraphName, Graph->Nodes.Num());
			}
		}
	}

	// Test: Try to call the interface function on a spawned actor
	UE_LOG(LogTemp, Warning, TEXT("--- Testing Interface Call ---"));

	// Try spawning an actor and testing the interface
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (World && GenClass)
	{
		// Spawn the actor
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* TestActor = World->SpawnActor<AActor>(GenClass, FVector(0, 0, -10000), FRotator::ZeroRotator, SpawnParams);

		if (TestActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawned test actor: %s"), *TestActor->GetName());

			// Check if actor implements the interface
			bool bActorImplements = TestActor->GetClass()->ImplementsInterface(USLFDestructibleHelperInterface::StaticClass());
			ResultJson->SetBoolField(TEXT("SpawnedActorImplementsInterface"), bActorImplements);
			UE_LOG(LogTemp, Warning, TEXT("SpawnedActorImplementsInterface: %s"), bActorImplements ? TEXT("YES") : TEXT("NO"));

			if (bActorImplements)
			{
				// Try calling the interface function via Execute_
				UE_LOG(LogTemp, Warning, TEXT("Calling ISLFDestructibleHelperInterface::Execute_EnableChaosDestroy..."));
				ISLFDestructibleHelperInterface::Execute_EnableChaosDestroy(TestActor);
				UE_LOG(LogTemp, Warning, TEXT("Execute_ call completed"));

				// Also try direct cast to see if _Implementation can be called directly
				ISLFDestructibleHelperInterface* DirectInterface = Cast<ISLFDestructibleHelperInterface>(TestActor);
				if (DirectInterface)
				{
					UE_LOG(LogTemp, Warning, TEXT("Direct cast to ISLFDestructibleHelperInterface succeeded"));
					UE_LOG(LogTemp, Warning, TEXT("Calling _Implementation directly..."));
					DirectInterface->EnableChaosDestroy_Implementation();
					UE_LOG(LogTemp, Warning, TEXT("Direct _Implementation call completed"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Direct cast to ISLFDestructibleHelperInterface FAILED"));
				}
			}

			// Destroy test actor
			TestActor->Destroy();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn test actor"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No World available for spawn test"));
	}

	UE_LOG(LogTemp, Warning, TEXT("=== END DiagnoseInterfaceImplementation ==="));

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(ResultJson, Writer);
	return OutputString;
}

// ============================================================================
// STATUS EFFECT RANK INFO RESTORATION
// ============================================================================

FString USLFAutomationLibrary::ApplyStatusEffectRankInfo(const FString& StatusEffectAssetPath)
{
	UE_LOG(LogSLFAutomation, Log, TEXT("=== ApplyStatusEffectRankInfo: %s ==="), *StatusEffectAssetPath);

	// Load the status effect data asset
	UPDA_StatusEffect* StatusEffect = Cast<UPDA_StatusEffect>(
		StaticLoadObject(UPDA_StatusEffect::StaticClass(), nullptr, *StatusEffectAssetPath));

	if (!StatusEffect)
	{
		FString Error = FString::Printf(TEXT("ERROR: Failed to load status effect: %s"), *StatusEffectAssetPath);
		UE_LOG(LogSLFAutomation, Error, TEXT("%s"), *Error);
		return Error;
	}

	// Get the asset name to determine which damage data to apply
	FString AssetName = StatusEffect->GetName();
	UE_LOG(LogSLFAutomation, Log, TEXT("Asset Name: %s, Tag: %s"), *AssetName, *StatusEffect->Tag.ToString());

	// Get current RankInfo
	TMap<int32, FSLFStatusEffectRankInfo>& RankInfo = StatusEffect->RankInfo;
	UE_LOG(LogSLFAutomation, Log, TEXT("Current RankInfo has %d entries"), RankInfo.Num());

	int32 RanksUpdated = 0;

	// Load VFX assets (soft ptr - doesn't require loading the actual asset)
	TSoftObjectPtr<UNiagaraSystem> NS_Poison(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_Poison.NS_Poison")));
	TSoftObjectPtr<UNiagaraSystem> NS_PoisonLoop(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_PoisonLoop.NS_PoisonLoop")));
	TSoftObjectPtr<UNiagaraSystem> NS_BleedExplode(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_BleedExplode.NS_BleedExplode")));
	TSoftObjectPtr<UNiagaraSystem> NS_Burn(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_Burn.NS_Burn")));
	TSoftObjectPtr<UNiagaraSystem> NS_BurnLoop(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_BurnLoop.NS_BurnLoop")));
	TSoftObjectPtr<UNiagaraSystem> NS_FrostBite(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_FrostBite.NS_FrostBite")));
	TSoftObjectPtr<UNiagaraSystem> NS_Corruption(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_Corruption.NS_Corruption")));
	TSoftObjectPtr<UNiagaraSystem> NS_Madness(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_Madness.NS_Madness")));
	TSoftObjectPtr<UNiagaraSystem> NS_Souls(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_Souls.NS_Souls")));

	// Apply damage data based on status effect type
	if (AssetName.Contains(TEXT("Poison")))
	{
		// Poison: Rank 1 = tick damage, Rank 2 = one-shot, Rank 3 = both
		// Rank 1: FStatusEffectTick - Duration=5s, Interval=1s, HP -5 to -10 per tick
		{
			FSLFStatusEffectRankInfo& Rank1 = RankInfo.FindOrAdd(1);

			// Set VFX
			Rank1.TriggerVFX.VFXSystem = NS_Poison;
			Rank1.LoopVFX.VFXSystem = NS_PoisonLoop;
			Rank1.LoopVFX.AttachSocket = FName("spine_04");

			FStatusEffectTick TickData;
			TickData.Duration = 5.0;
			TickData.Interval = 1.0;

			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -5.0;
			HPChange.MaxAmount = -10.0;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			HPChange.bTryActivateRegen = false;
			TickData.TickingStatAdjustment.Add(HPChange);

			Rank1.RelevantData.InitializeAs<FStatusEffectTick>(TickData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 1: FStatusEffectTick + VFX (Duration=5, Interval=1, HP -5 to -10)"));
		}

		// Rank 2: FStatusEffectStatChanges - HP -50 to -100 one-shot
		{
			FSLFStatusEffectRankInfo& Rank2 = RankInfo.FindOrAdd(2);

			// Set VFX
			Rank2.TriggerVFX.VFXSystem = NS_Poison;
			Rank2.LoopVFX.VFXSystem = NS_PoisonLoop;
			Rank2.LoopVFX.AttachSocket = FName("spine_04");

			FStatusEffectStatChanges OneShotData;

			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -50.0;
			HPChange.MaxAmount = -100.0;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			HPChange.bTryActivateRegen = false;
			OneShotData.StatChanges.Add(HPChange);

			Rank2.RelevantData.InitializeAs<FStatusEffectStatChanges>(OneShotData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 2: FStatusEffectStatChanges + VFX (HP -50 to -100)"));
		}

		// Rank 3: FStatusEffectOneShotAndTick - Duration=10s, Interval=1s, Instant HP -100 to -200, Tick HP -10 to -20
		{
			FSLFStatusEffectRankInfo& Rank3 = RankInfo.FindOrAdd(3);

			// Set VFX
			Rank3.TriggerVFX.VFXSystem = NS_Poison;
			Rank3.LoopVFX.VFXSystem = NS_PoisonLoop;
			Rank3.LoopVFX.AttachSocket = FName("spine_02");

			FStatusEffectOneShotAndTick CombinedData;
			CombinedData.Duration = 10.0;
			CombinedData.Interval = 1.0;

			FStatChange InstantHP;
			InstantHP.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			InstantHP.MinAmount = -20.0;  // Reduced from test values
			InstantHP.MaxAmount = -40.0;  // Reduced from test values
			InstantHP.ValueType = ESLFValueType::CurrentValue;
			InstantHP.bTryActivateRegen = false;
			CombinedData.InstantStatAdjustment.Add(InstantHP);

			FStatChange TickHP;
			TickHP.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			TickHP.MinAmount = -5.0;  // Reduced from test values
			TickHP.MaxAmount = -10.0;  // Reduced from test values
			TickHP.ValueType = ESLFValueType::CurrentValue;
			TickHP.bTryActivateRegen = false;
			CombinedData.TickingStatAdjustment.Add(TickHP);

			Rank3.RelevantData.InitializeAs<FStatusEffectOneShotAndTick>(CombinedData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 3: FStatusEffectOneShotAndTick + VFX (Instant HP -20 to -40, Tick HP -5 to -10)"));
		}
	}
	else if (AssetName.Contains(TEXT("Bleed")))
	{
		// Bleed: One-shot HP damage, increasing with rank
		// Rank 1: HP -50 to -75
		{
			FSLFStatusEffectRankInfo& Rank1 = RankInfo.FindOrAdd(1);

			// Set VFX - Bleed uses BleedExplode as trigger VFX
			Rank1.TriggerVFX.VFXSystem = NS_BleedExplode;
			Rank1.TriggerVFX.AttachSocket = FName("spine_04");

			FStatusEffectStatChanges OneShotData;
			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -50.0;
			HPChange.MaxAmount = -75.0;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			OneShotData.StatChanges.Add(HPChange);

			Rank1.RelevantData.InitializeAs<FStatusEffectStatChanges>(OneShotData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 1: HP -50 to -75 + VFX"));
		}

		// Rank 2: HP -100 to -150
		{
			FSLFStatusEffectRankInfo& Rank2 = RankInfo.FindOrAdd(2);

			// Set VFX
			Rank2.TriggerVFX.VFXSystem = NS_BleedExplode;
			Rank2.TriggerVFX.AttachSocket = FName("spine_04");

			FStatusEffectStatChanges OneShotData;
			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -100.0;
			HPChange.MaxAmount = -150.0;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			OneShotData.StatChanges.Add(HPChange);

			Rank2.RelevantData.InitializeAs<FStatusEffectStatChanges>(OneShotData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 2: HP -100 to -150 + VFX"));
		}

		// Rank 3: Instant + tick
		{
			FSLFStatusEffectRankInfo& Rank3 = RankInfo.FindOrAdd(3);

			// Set VFX
			Rank3.TriggerVFX.VFXSystem = NS_BleedExplode;
			Rank3.TriggerVFX.AttachSocket = FName("spine_02");

			FStatusEffectOneShotAndTick CombinedData;
			CombinedData.Duration = 8.0;
			CombinedData.Interval = 1.0;

			FStatChange InstantHP;
			InstantHP.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			InstantHP.MinAmount = -150.0;
			InstantHP.MaxAmount = -250.0;
			InstantHP.ValueType = ESLFValueType::CurrentValue;
			CombinedData.InstantStatAdjustment.Add(InstantHP);

			FStatChange TickHP;
			TickHP.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			TickHP.MinAmount = -15.0;
			TickHP.MaxAmount = -25.0;
			TickHP.ValueType = ESLFValueType::CurrentValue;
			CombinedData.TickingStatAdjustment.Add(TickHP);

			Rank3.RelevantData.InitializeAs<FStatusEffectOneShotAndTick>(CombinedData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 3: Instant HP -150 to -250, Tick HP -15 to -25 + VFX"));
		}
	}
	else if (AssetName.Contains(TEXT("Burn")))
	{
		// Burn: Fast tick damage
		// Rank 1: Duration=4s, Interval=0.5s, HP -8 to -12 per tick
		{
			FSLFStatusEffectRankInfo& Rank1 = RankInfo.FindOrAdd(1);

			// Set VFX
			Rank1.TriggerVFX.VFXSystem = NS_Burn;
			Rank1.LoopVFX.VFXSystem = NS_BurnLoop;
			Rank1.LoopVFX.AttachSocket = FName("spine_04");

			FStatusEffectTick TickData;
			TickData.Duration = 4.0;
			TickData.Interval = 0.5;

			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -8.0;
			HPChange.MaxAmount = -12.0;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			TickData.TickingStatAdjustment.Add(HPChange);

			Rank1.RelevantData.InitializeAs<FStatusEffectTick>(TickData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 1: Duration=4, Interval=0.5, HP -8 to -12 + VFX"));
		}

		// Rank 2: Duration=6s, Interval=0.5s, HP -12 to -18 per tick
		{
			FSLFStatusEffectRankInfo& Rank2 = RankInfo.FindOrAdd(2);

			// Set VFX
			Rank2.TriggerVFX.VFXSystem = NS_Burn;
			Rank2.LoopVFX.VFXSystem = NS_BurnLoop;
			Rank2.LoopVFX.AttachSocket = FName("spine_04");

			FStatusEffectTick TickData;
			TickData.Duration = 6.0;
			TickData.Interval = 0.5;

			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -12.0;
			HPChange.MaxAmount = -18.0;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			TickData.TickingStatAdjustment.Add(HPChange);

			Rank2.RelevantData.InitializeAs<FStatusEffectTick>(TickData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 2: Duration=6, Interval=0.5, HP -12 to -18 + VFX"));
		}

		// Rank 3: Duration=8s, Interval=0.5s, Instant HP -50 to -100, Tick HP -15 to -25
		{
			FSLFStatusEffectRankInfo& Rank3 = RankInfo.FindOrAdd(3);

			// Set VFX
			Rank3.TriggerVFX.VFXSystem = NS_Burn;
			Rank3.LoopVFX.VFXSystem = NS_BurnLoop;
			Rank3.LoopVFX.AttachSocket = FName("spine_02");

			FStatusEffectOneShotAndTick CombinedData;
			CombinedData.Duration = 8.0;
			CombinedData.Interval = 0.5;

			FStatChange InstantHP;
			InstantHP.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			InstantHP.MinAmount = -50.0;
			InstantHP.MaxAmount = -100.0;
			InstantHP.ValueType = ESLFValueType::CurrentValue;
			CombinedData.InstantStatAdjustment.Add(InstantHP);

			FStatChange TickHP;
			TickHP.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			TickHP.MinAmount = -15.0;
			TickHP.MaxAmount = -25.0;
			TickHP.ValueType = ESLFValueType::CurrentValue;
			CombinedData.TickingStatAdjustment.Add(TickHP);

			Rank3.RelevantData.InitializeAs<FStatusEffectOneShotAndTick>(CombinedData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 3: Instant HP -50 to -100, Tick HP -15 to -25 + VFX"));
		}
	}
	else if (AssetName.Contains(TEXT("Frostbite")))
	{
		// Frostbite: HP + Stamina damage
		// Rank 1: HP -30 to -50, Stamina -20 to -30
		{
			FSLFStatusEffectRankInfo& Rank1 = RankInfo.FindOrAdd(1);

			// Set VFX
			Rank1.TriggerVFX.VFXSystem = NS_FrostBite;
			Rank1.TriggerVFX.AttachSocket = FName("spine_04");

			FStatusEffectStatChanges OneShotData;

			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -30.0;
			HPChange.MaxAmount = -50.0;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			OneShotData.StatChanges.Add(HPChange);

			FStatChange StaminaChange;
			StaminaChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
			StaminaChange.MinAmount = -20.0;
			StaminaChange.MaxAmount = -30.0;
			StaminaChange.ValueType = ESLFValueType::CurrentValue;
			OneShotData.StatChanges.Add(StaminaChange);

			Rank1.RelevantData.InitializeAs<FStatusEffectStatChanges>(OneShotData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 1: HP -30 to -50, Stamina -20 to -30 + VFX"));
		}

		// Rank 2: HP -60 to -100, Stamina -40 to -60
		{
			FSLFStatusEffectRankInfo& Rank2 = RankInfo.FindOrAdd(2);

			// Set VFX
			Rank2.TriggerVFX.VFXSystem = NS_FrostBite;
			Rank2.TriggerVFX.AttachSocket = FName("spine_04");

			FStatusEffectStatChanges OneShotData;

			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -60.0;
			HPChange.MaxAmount = -100.0;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			OneShotData.StatChanges.Add(HPChange);

			FStatChange StaminaChange;
			StaminaChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
			StaminaChange.MinAmount = -40.0;
			StaminaChange.MaxAmount = -60.0;
			StaminaChange.ValueType = ESLFValueType::CurrentValue;
			OneShotData.StatChanges.Add(StaminaChange);

			Rank2.RelevantData.InitializeAs<FStatusEffectStatChanges>(OneShotData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 2: HP -60 to -100, Stamina -40 to -60 + VFX"));
		}

		// Rank 3: Instant HP + tick Stamina drain
		{
			FSLFStatusEffectRankInfo& Rank3 = RankInfo.FindOrAdd(3);

			// Set VFX
			Rank3.TriggerVFX.VFXSystem = NS_FrostBite;
			Rank3.TriggerVFX.AttachSocket = FName("spine_02");

			FStatusEffectOneShotAndTick CombinedData;
			CombinedData.Duration = 6.0;
			CombinedData.Interval = 1.0;

			FStatChange InstantHP;
			InstantHP.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			InstantHP.MinAmount = -100.0;
			InstantHP.MaxAmount = -150.0;
			InstantHP.ValueType = ESLFValueType::CurrentValue;
			CombinedData.InstantStatAdjustment.Add(InstantHP);

			FStatChange TickStamina;
			TickStamina.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
			TickStamina.MinAmount = -10.0;
			TickStamina.MaxAmount = -15.0;
			TickStamina.ValueType = ESLFValueType::CurrentValue;
			CombinedData.TickingStatAdjustment.Add(TickStamina);

			Rank3.RelevantData.InitializeAs<FStatusEffectOneShotAndTick>(CombinedData);
			RanksUpdated++;
			UE_LOG(LogSLFAutomation, Log, TEXT("  Rank 3: Instant HP -100 to -150, Tick Stamina -10 to -15 + VFX"));
		}
	}
	else if (AssetName.Contains(TEXT("Corruption")))
	{
		// Corruption: Use NS_Corruption VFX
		for (int32 Rank = 1; Rank <= 3; Rank++)
		{
			FSLFStatusEffectRankInfo& RankEntry = RankInfo.FindOrAdd(Rank);
			RankEntry.TriggerVFX.VFXSystem = NS_Corruption;
			RankEntry.TriggerVFX.AttachSocket = FName("spine_04");

			FStatusEffectStatChanges OneShotData;
			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -20.0 * Rank;
			HPChange.MaxAmount = -40.0 * Rank;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			OneShotData.StatChanges.Add(HPChange);

			RankEntry.RelevantData.InitializeAs<FStatusEffectStatChanges>(OneShotData);
			RanksUpdated++;
		}
		UE_LOG(LogSLFAutomation, Log, TEXT("  Corruption: 3 ranks with scaling HP damage + VFX"));
	}
	else if (AssetName.Contains(TEXT("Madness")))
	{
		// Madness: Use NS_Madness VFX
		for (int32 Rank = 1; Rank <= 3; Rank++)
		{
			FSLFStatusEffectRankInfo& RankEntry = RankInfo.FindOrAdd(Rank);
			RankEntry.TriggerVFX.VFXSystem = NS_Madness;
			RankEntry.TriggerVFX.AttachSocket = FName("spine_04");

			FStatusEffectStatChanges OneShotData;
			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -25.0 * Rank;
			HPChange.MaxAmount = -50.0 * Rank;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			OneShotData.StatChanges.Add(HPChange);

			RankEntry.RelevantData.InitializeAs<FStatusEffectStatChanges>(OneShotData);
			RanksUpdated++;
		}
		UE_LOG(LogSLFAutomation, Log, TEXT("  Madness: 3 ranks with scaling HP damage + VFX"));
	}
	else if (AssetName.Contains(TEXT("Plague")))
	{
		// Plague: Use NS_Souls VFX
		for (int32 Rank = 1; Rank <= 3; Rank++)
		{
			FSLFStatusEffectRankInfo& RankEntry = RankInfo.FindOrAdd(Rank);
			RankEntry.TriggerVFX.VFXSystem = NS_Souls;
			RankEntry.TriggerVFX.AttachSocket = FName("spine_04");

			FStatusEffectStatChanges OneShotData;
			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -30.0 * Rank;
			HPChange.MaxAmount = -60.0 * Rank;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			OneShotData.StatChanges.Add(HPChange);

			RankEntry.RelevantData.InitializeAs<FStatusEffectStatChanges>(OneShotData);
			RanksUpdated++;
		}
		UE_LOG(LogSLFAutomation, Log, TEXT("  Plague: 3 ranks with scaling HP damage + VFX"));
	}
	else
	{
		// Generic default for any other status effect
		UE_LOG(LogSLFAutomation, Warning, TEXT("Unknown status effect type: %s - applying generic damage data"), *AssetName);

		// Rank 1: Basic HP tick damage
		{
			FSLFStatusEffectRankInfo& Rank1 = RankInfo.FindOrAdd(1);

			FStatusEffectTick TickData;
			TickData.Duration = 5.0;
			TickData.Interval = 1.0;

			FStatChange HPChange;
			HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			HPChange.MinAmount = -5.0;
			HPChange.MaxAmount = -10.0;
			HPChange.ValueType = ESLFValueType::CurrentValue;
			TickData.TickingStatAdjustment.Add(HPChange);

			Rank1.RelevantData.InitializeAs<FStatusEffectTick>(TickData);
			RanksUpdated++;
		}
	}

	// Mark package dirty and save
	StatusEffect->MarkPackageDirty();

	UPackage* Package = StatusEffect->GetOutermost();
	if (Package)
	{
		FString PackageFilename;
		if (FPackageName::TryConvertLongPackageNameToFilename(Package->GetName(), PackageFilename, FPackageName::GetAssetPackageExtension()))
		{
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
			SaveArgs.SaveFlags = SAVE_NoError;

			bool bSaved = UPackage::SavePackage(Package, StatusEffect, *PackageFilename, SaveArgs);
			if (bSaved)
			{
				UE_LOG(LogSLFAutomation, Log, TEXT("Saved: %s"), *StatusEffectAssetPath);
			}
			else
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("Failed to save: %s"), *StatusEffectAssetPath);
			}
		}
	}

	FString Result = FString::Printf(TEXT("SUCCESS: Updated %d ranks for %s"), RanksUpdated, *AssetName);
	UE_LOG(LogSLFAutomation, Log, TEXT("%s"), *Result);
	return Result;
}

FString USLFAutomationLibrary::ApplyAllStatusEffectRankInfo()
{
	UE_LOG(LogSLFAutomation, Log, TEXT("=== ApplyAllStatusEffectRankInfo ==="));

	TArray<FString> StatusEffects = {
		TEXT("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison"),
		TEXT("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed"),
		TEXT("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn"),
		TEXT("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Frostbite"),
		TEXT("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Corruption"),
		TEXT("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Madness"),
		TEXT("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Plague")
	};

	int32 Success = 0;
	int32 Failed = 0;

	for (const FString& Path : StatusEffects)
	{
		FString Result = ApplyStatusEffectRankInfo(Path);
		if (Result.StartsWith(TEXT("SUCCESS")))
		{
			Success++;
		}
		else
		{
			Failed++;
		}
	}

	FString Result = FString::Printf(TEXT("Applied status effect RankInfo: %d succeeded, %d failed"), Success, Failed);
	UE_LOG(LogSLFAutomation, Log, TEXT("%s"), *Result);
	return Result;
}

// ============================================================================
// FLASK DATA RESTORATION
// ============================================================================

FString USLFAutomationLibrary::ApplyFlaskData()
{
	UE_LOG(LogSLFAutomation, Log, TEXT("=== ApplyFlaskData ==="));

	const FString ActionAssetPath = TEXT("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_DrinkFlask_HP");

	// Load the action data asset
	UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(
		StaticLoadObject(UPDA_ActionBase::StaticClass(), nullptr, *ActionAssetPath));

	if (!ActionData)
	{
		FString Error = FString::Printf(TEXT("ERROR: Failed to load action data asset: %s"), *ActionAssetPath);
		UE_LOG(LogSLFAutomation, Error, TEXT("%s"), *Error);
		return Error;
	}

	UE_LOG(LogSLFAutomation, Log, TEXT("Loaded action data: %s"), *ActionData->GetName());

	// Check current state of RelevantData
	const FSLFFlaskData* CurrentFlaskData = ActionData->RelevantData.GetPtr<FSLFFlaskData>();
	if (CurrentFlaskData && CurrentFlaskData->StatChangesPercent.Num() > 0)
	{
		UE_LOG(LogSLFAutomation, Log, TEXT("RelevantData already has valid FSLFFlaskData with %d stat changes"),
			CurrentFlaskData->StatChangesPercent.Num());
		return FString::Printf(TEXT("OK: FlaskData already valid with %d stat changes"), CurrentFlaskData->StatChangesPercent.Num());
	}

	// Create FSLFFlaskData struct
	FSLFFlaskData FlaskData;

	// Create stat change for HP heal at 40%
	FStatChangePercent HPChange;
	HPChange.StatTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
	HPChange.ValueType = ESLFValueType::CurrentValue;
	HPChange.PercentChange = 40.0;
	HPChange.bTryActivateRegen = false;
	FlaskData.StatChangesPercent.Add(HPChange);

	// Set drinking montage
	FlaskData.DrinkingMontage = TSoftObjectPtr<UAnimMontage>(
		FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Demo/_Animations/Interaction/Flask/AM_SLF_DrinkFlask.AM_SLF_DrinkFlask")));

	// Set VFX
	FlaskData.VFX = TSoftObjectPtr<UNiagaraSystem>(
		FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_Flask_HP.NS_Flask_HP")));

	UE_LOG(LogSLFAutomation, Log, TEXT("Created FSLFFlaskData:"));
	UE_LOG(LogSLFAutomation, Log, TEXT("  StatChangesPercent: %d entries"), FlaskData.StatChangesPercent.Num());
	UE_LOG(LogSLFAutomation, Log, TEXT("  StatTag: %s, PercentChange: %.1f"),
		*HPChange.StatTag.ToString(), HPChange.PercentChange);
	UE_LOG(LogSLFAutomation, Log, TEXT("  DrinkingMontage: %s"),
		*FlaskData.DrinkingMontage.GetAssetName());
	UE_LOG(LogSLFAutomation, Log, TEXT("  VFX: %s"),
		*FlaskData.VFX.GetAssetName());

	// Set the InstancedStruct with FSLFFlaskData
	ActionData->RelevantData.InitializeAs<FSLFFlaskData>(FlaskData);

	// Verify the data was set
	const FSLFFlaskData* VerifyData = ActionData->RelevantData.GetPtr<FSLFFlaskData>();
	if (!VerifyData || VerifyData->StatChangesPercent.Num() == 0)
	{
		FString Error = TEXT("ERROR: Failed to set FSLFFlaskData on RelevantData");
		UE_LOG(LogSLFAutomation, Error, TEXT("%s"), *Error);
		return Error;
	}

	// Mark the package dirty and save
	ActionData->MarkPackageDirty();
	UPackage* Package = ActionData->GetOutermost();
	if (Package)
	{
		FString PackageFilename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError | SAVE_KeepDirty;
		bool bSaved = UPackage::SavePackage(Package, ActionData, *PackageFilename, SaveArgs);

		if (bSaved)
		{
			UE_LOG(LogSLFAutomation, Log, TEXT("Saved package: %s"), *PackageFilename);
		}
		else
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("Failed to save package: %s"), *PackageFilename);
		}
	}

	FString Result = FString::Printf(TEXT("SUCCESS: Applied FSLFFlaskData with %d stat changes, Montage: %s"),
		FlaskData.StatChangesPercent.Num(),
		*FlaskData.DrinkingMontage.GetAssetName());
	UE_LOG(LogSLFAutomation, Log, TEXT("%s"), *Result);
	return Result;
}

#endif // WITH_EDITOR
