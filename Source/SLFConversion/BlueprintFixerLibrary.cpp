// BlueprintFixerLibrary.cpp

#include "BlueprintFixerLibrary.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_Event.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_CustomEvent.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/UObjectGlobals.h"
#include "Misc/PackageName.h"

// Log category for Blueprint Fixer
DEFINE_LOG_CATEGORY_STATIC(LogBlueprintFixer, Log, All);

int32 UBlueprintFixerLibrary::RefreshFunctionCallsForClass(UBlueprint* Blueprint, FString OldClassName, FString NewClassName)
{
	if (!Blueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("BlueprintFixer: Blueprint is null"));
		return 0;
	}

	int32 FixedCount = 0;

	// Get all graphs in the Blueprint
	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	UE_LOG(LogTemp, Log, TEXT("BlueprintFixer: Processing %s with %d graphs"), *Blueprint->GetName(), AllGraphs.Num());

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			// Check if this is a function call node
			UK2Node_CallFunction* CallNode = Cast<UK2Node_CallFunction>(Node);
			if (!CallNode) continue;

			// Get the function reference
			FMemberReference& FuncRef = CallNode->FunctionReference;
			UClass* MemberParentClass = FuncRef.GetMemberParentClass();

			if (!MemberParentClass) continue;

			FString ClassName = MemberParentClass->GetName();

			// Check if this node calls a function on the old class
			if (ClassName.Contains(OldClassName))
			{
				FName FunctionName = FuncRef.GetMemberName();
				UE_LOG(LogTemp, Log, TEXT("BlueprintFixer: Found call to %s::%s"), *ClassName, *FunctionName.ToString());

				// Try to find the function in the new parent class
				UClass* NewClass = FindFirstObject<UClass>(*NewClassName, EFindFirstObjectOptions::None);
				if (!NewClass)
				{
					// Try with U prefix
					NewClass = FindFirstObject<UClass>(*(TEXT("U") + NewClassName), EFindFirstObjectOptions::None);
				}

				if (NewClass)
				{
					UFunction* NewFunction = NewClass->FindFunctionByName(FunctionName);
					if (NewFunction)
					{
						// Update the function reference to point to the new class
						FuncRef.SetFromField<UFunction>(NewFunction, false);

						// Reconstruct the node to refresh pins
						CallNode->ReconstructNode();

						UE_LOG(LogTemp, Log, TEXT("BlueprintFixer: Fixed %s to use %s::%s"),
							*CallNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString(),
							*NewClassName, *FunctionName.ToString());

						FixedCount++;
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("BlueprintFixer: Function %s not found in %s"),
							*FunctionName.ToString(), *NewClassName);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("BlueprintFixer: Class %s not found"), *NewClassName);
				}
			}
		}
	}

	if (FixedCount > 0)
	{
		// Mark Blueprint as modified
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		UE_LOG(LogTemp, Log, TEXT("BlueprintFixer: Fixed %d nodes in %s"), FixedCount, *Blueprint->GetName());
	}

	return FixedCount;
}

bool UBlueprintFixerLibrary::ReconstructAllNodes(UBlueprint* Blueprint)
{
	if (!Blueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("BlueprintFixer: Blueprint is null"));
		return false;
	}

	// Use the built-in utility to reconstruct all nodes
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

	UE_LOG(LogTemp, Log, TEXT("BlueprintFixer: Reconstructed all nodes in %s"), *Blueprint->GetName());

	return true;
}

TArray<FString> UBlueprintFixerLibrary::GetFunctionCallInfo(UBlueprint* Blueprint)
{
	TArray<FString> Results;

	if (!Blueprint)
	{
		Results.Add(TEXT("ERROR: Blueprint is null"));
		return Results;
	}

	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			UK2Node_CallFunction* CallNode = Cast<UK2Node_CallFunction>(Node);
			if (!CallNode) continue;

			FMemberReference& FuncRef = CallNode->FunctionReference;
			UClass* MemberParentClass = FuncRef.GetMemberParentClass();
			FName FunctionName = FuncRef.GetMemberName();

			FString ClassName = MemberParentClass ? MemberParentClass->GetName() : TEXT("Unknown");
			FString Info = FString::Printf(TEXT("Graph=%s, Node=%s, Class=%s, Function=%s"),
				*Graph->GetName(),
				*CallNode->GetName(),
				*ClassName,
				*FunctionName.ToString());

			Results.Add(Info);
		}
	}

	return Results;
}

int32 UBlueprintFixerLibrary::FixEventDispatcherBindings(UBlueprint* Blueprint, FString ComponentName)
{
	if (!Blueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("BlueprintFixer: Blueprint is null"));
		return 0;
	}

	int32 FixedCount = 0;

	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			// Check for component bound events
			UK2Node_ComponentBoundEvent* EventNode = Cast<UK2Node_ComponentBoundEvent>(Node);
			if (!EventNode) continue;

			// Check if this event is bound to our component
			FName BoundComponentName = EventNode->ComponentPropertyName;
			if (BoundComponentName.ToString().Contains(ComponentName))
			{
				UE_LOG(LogTemp, Log, TEXT("BlueprintFixer: Found event bound to %s: %s"),
					*ComponentName,
					*EventNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString());

				// Reconstruct the node to refresh the binding
				EventNode->ReconstructNode();
				FixedCount++;
			}
		}
	}

	if (FixedCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	}

	return FixedCount;
}

//////////////////////////////////////////////////////////////////////////
// Blueprint Cleanup Functions
//////////////////////////////////////////////////////////////////////////

bool UBlueprintFixerLibrary::DeleteBlueprintFunction(UBlueprint* Blueprint, FString FunctionName)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("DeleteBlueprintFunction: Blueprint is null"));
		return false;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("DeleteBlueprintFunction: Attempting to delete '%s' from '%s'"),
		*FunctionName, *Blueprint->GetName());

	// Find the function graph
	FName FuncFName(*FunctionName);
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
		UE_LOG(LogBlueprintFixer, Warning, TEXT("DeleteBlueprintFunction: Function '%s' not found in '%s'"),
			*FunctionName, *Blueprint->GetName());
		return false;
	}

	// Remove the function graph
	FBlueprintEditorUtils::RemoveGraph(Blueprint, FunctionGraph);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	UE_LOG(LogBlueprintFixer, Log, TEXT("DeleteBlueprintFunction: Successfully deleted '%s'"), *FunctionName);
	return true;
}

bool UBlueprintFixerLibrary::DeleteBlueprintVariable(UBlueprint* Blueprint, FString VariableName)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("DeleteBlueprintVariable: Blueprint is null"));
		return false;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("DeleteBlueprintVariable: Attempting to delete '%s' from '%s'"),
		*VariableName, *Blueprint->GetName());

	FName VarFName(*VariableName);

	// Find the variable
	int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, VarFName);
	if (VarIndex == INDEX_NONE)
	{
		UE_LOG(LogBlueprintFixer, Warning, TEXT("DeleteBlueprintVariable: Variable '%s' not found in '%s'"),
			*VariableName, *Blueprint->GetName());
		return false;
	}

	// Remove the variable
	FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, VarFName);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	UE_LOG(LogBlueprintFixer, Log, TEXT("DeleteBlueprintVariable: Successfully deleted '%s'"), *VariableName);
	return true;
}

int32 UBlueprintFixerLibrary::ClearEventGraph(UBlueprint* Blueprint)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("ClearEventGraph: Blueprint is null"));
		return 0;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("ClearEventGraph: Clearing event graph in '%s'"), *Blueprint->GetName());

	// Find the UbergraphPages (event graphs)
	int32 NodesRemoved = 0;

	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		if (!Graph) continue;

		UE_LOG(LogBlueprintFixer, Log, TEXT("ClearEventGraph: Processing graph '%s' with %d nodes"),
			*Graph->GetName(), Graph->Nodes.Num());

		// Collect nodes to remove (don't modify while iterating)
		TArray<UEdGraphNode*> NodesToRemove;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (Node)
			{
				UE_LOG(LogBlueprintFixer, Log, TEXT("  - Marking for removal: %s [%s]"),
					*Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString(),
					*Node->GetClass()->GetName());
				NodesToRemove.Add(Node);
			}
		}

		// Remove nodes
		for (UEdGraphNode* Node : NodesToRemove)
		{
			FBlueprintEditorUtils::RemoveNode(Blueprint, Node, true);
			NodesRemoved++;
		}
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	UE_LOG(LogBlueprintFixer, Log, TEXT("ClearEventGraph: Removed %d nodes"), NodesRemoved);
	return NodesRemoved;
}

bool UBlueprintFixerLibrary::DeleteEventDispatcher(UBlueprint* Blueprint, FString DispatcherName)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("DeleteEventDispatcher: Blueprint is null"));
		return false;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("DeleteEventDispatcher: Attempting to delete '%s' from '%s'"),
		*DispatcherName, *Blueprint->GetName());

	FName DispatcherFName(*DispatcherName);

	// Event dispatchers are stored as delegate properties
	// They appear in the DelegateSignatureGraphs array
	UEdGraph* DelegateGraph = nullptr;
	for (UEdGraph* Graph : Blueprint->DelegateSignatureGraphs)
	{
		if (Graph && Graph->GetFName() == DispatcherFName)
		{
			DelegateGraph = Graph;
			break;
		}
	}

	if (DelegateGraph)
	{
		FBlueprintEditorUtils::RemoveGraph(Blueprint, DelegateGraph);
	}

	// Also try removing as a member variable (dispatchers can be stored both ways)
	FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, DispatcherFName);

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	UE_LOG(LogBlueprintFixer, Log, TEXT("DeleteEventDispatcher: Processed '%s'"), *DispatcherName);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Diagnostic/Logging Functions
//////////////////////////////////////////////////////////////////////////

FString UBlueprintFixerLibrary::GetBlueprintDiagnostics(UBlueprint* Blueprint)
{
	if (!Blueprint)
	{
		return TEXT("ERROR: Blueprint is null");
	}

	FString Result;
	Result += FString::Printf(TEXT("=== Blueprint Diagnostics: %s ===\n"), *Blueprint->GetName());
	Result += FString::Printf(TEXT("Parent Class: %s\n"),
		Blueprint->ParentClass ? *Blueprint->ParentClass->GetName() : TEXT("None"));
	Result += FString::Printf(TEXT("Generated Class: %s\n"),
		Blueprint->GeneratedClass ? *Blueprint->GeneratedClass->GetName() : TEXT("None"));

	// Functions
	Result += FString::Printf(TEXT("\nFunctions (%d):\n"), Blueprint->FunctionGraphs.Num());
	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
	{
		if (Graph)
		{
			Result += FString::Printf(TEXT("  - %s\n"), *Graph->GetName());
		}
	}

	// Variables
	Result += FString::Printf(TEXT("\nVariables (%d):\n"), Blueprint->NewVariables.Num());
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		Result += FString::Printf(TEXT("  - %s : %s\n"),
			*Var.VarName.ToString(),
			*Var.VarType.PinCategory.ToString());
	}

	// Event Dispatchers
	Result += FString::Printf(TEXT("\nEvent Dispatchers (%d):\n"), Blueprint->DelegateSignatureGraphs.Num());
	for (UEdGraph* Graph : Blueprint->DelegateSignatureGraphs)
	{
		if (Graph)
		{
			Result += FString::Printf(TEXT("  - %s\n"), *Graph->GetName());
		}
	}

	// Graphs
	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);
	Result += FString::Printf(TEXT("\nAll Graphs (%d):\n"), AllGraphs.Num());
	for (UEdGraph* Graph : AllGraphs)
	{
		if (Graph)
		{
			Result += FString::Printf(TEXT("  - %s (%d nodes)\n"), *Graph->GetName(), Graph->Nodes.Num());
		}
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("%s"), *Result);
	return Result;
}

TArray<FString> UBlueprintFixerLibrary::ListBlueprintFunctions(UBlueprint* Blueprint)
{
	TArray<FString> Results;

	if (!Blueprint)
	{
		Results.Add(TEXT("ERROR: Blueprint is null"));
		return Results;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("ListBlueprintFunctions: %s"), *Blueprint->GetName());

	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
	{
		if (Graph)
		{
			FString Info = FString::Printf(TEXT("%s (Nodes: %d)"), *Graph->GetName(), Graph->Nodes.Num());
			Results.Add(Info);
			UE_LOG(LogBlueprintFixer, Log, TEXT("  Function: %s"), *Info);
		}
	}

	return Results;
}

TArray<FString> UBlueprintFixerLibrary::ListBlueprintVariables(UBlueprint* Blueprint)
{
	TArray<FString> Results;

	if (!Blueprint)
	{
		Results.Add(TEXT("ERROR: Blueprint is null"));
		return Results;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("ListBlueprintVariables: %s"), *Blueprint->GetName());

	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		FString Info = FString::Printf(TEXT("%s : %s (Category: %s)"),
			*Var.VarName.ToString(),
			*Var.VarType.PinCategory.ToString(),
			*Var.Category.ToString());
		Results.Add(Info);
		UE_LOG(LogBlueprintFixer, Log, TEXT("  Variable: %s"), *Info);
	}

	return Results;
}

TArray<FString> UBlueprintFixerLibrary::GetDetailedNodeInfo(UBlueprint* Blueprint, FString GraphName)
{
	TArray<FString> Results;

	if (!Blueprint)
	{
		Results.Add(TEXT("ERROR: Blueprint is null"));
		return Results;
	}

	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	UE_LOG(LogBlueprintFixer, Log, TEXT("GetDetailedNodeInfo: %s (filter: %s)"),
		*Blueprint->GetName(), GraphName.IsEmpty() ? TEXT("all") : *GraphName);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		if (!GraphName.IsEmpty() && Graph->GetName() != GraphName) continue;

		UE_LOG(LogBlueprintFixer, Log, TEXT("  Graph: %s"), *Graph->GetName());

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			FString NodeGuid = Node->NodeGuid.ToString();
			FString NodeClass = Node->GetClass()->GetName();
			FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();

			// For function calls, get additional info
			FString ExtraInfo;
			if (UK2Node_CallFunction* CallNode = Cast<UK2Node_CallFunction>(Node))
			{
				FMemberReference& FuncRef = CallNode->FunctionReference;
				UClass* MemberParentClass = FuncRef.GetMemberParentClass();
				FName FunctionName = FuncRef.GetMemberName();

				ExtraInfo = FString::Printf(TEXT(", FunctionClass=%s, FunctionName=%s"),
					MemberParentClass ? *MemberParentClass->GetName() : TEXT("None"),
					*FunctionName.ToString());
			}

			// Get pin connections
			FString Connections;
			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (Pin && Pin->LinkedTo.Num() > 0)
				{
					for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
					{
						if (LinkedPin && LinkedPin->GetOwningNode())
						{
							Connections += FString::Printf(TEXT(" [%s->%s:%s]"),
								*Pin->PinName.ToString(),
								*LinkedPin->GetOwningNode()->NodeGuid.ToString(),
								*LinkedPin->PinName.ToString());
						}
					}
				}
			}

			FString Info = FString::Printf(TEXT("NodeGuid=%s, Class=%s, Title=\"%s\"%s%s"),
				*NodeGuid, *NodeClass, *NodeTitle, *ExtraInfo,
				Connections.IsEmpty() ? TEXT("") : *FString::Printf(TEXT(", Links:%s"), *Connections));

			Results.Add(Info);
			UE_LOG(LogBlueprintFixer, Log, TEXT("    %s"), *Info);
		}
	}

	return Results;
}

//////////////////////////////////////////////////////////////////////////
// Enum Migration Functions
//////////////////////////////////////////////////////////////////////////

int32 UBlueprintFixerLibrary::FixEnumPinValues(UBlueprint* Blueprint, FString EnumPathSubstring, FString OldValue, FString NewValue)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("FixEnumPinValues: Blueprint is null"));
		return 0;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("FixEnumPinValues: Processing %s, looking for enum containing '%s', replacing '%s' with '%s'"),
		*Blueprint->GetName(), *EnumPathSubstring, *OldValue, *NewValue);

	int32 FixedCount = 0;

	// Get all graphs in the Blueprint
	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	UE_LOG(LogBlueprintFixer, Log, TEXT("  Found %d graphs"), AllGraphs.Num());

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin) continue;

				// Check if this pin's type references the enum we're looking for
				FEdGraphPinType& PinType = Pin->PinType;

				// Get the sub-category object (which is the enum for enum pins)
				UObject* SubCatObj = PinType.PinSubCategoryObject.Get();
				if (!SubCatObj) continue;

				// Check if the path contains our enum name
				FString SubCatPath = SubCatObj->GetPathName();
				if (!SubCatPath.Contains(EnumPathSubstring)) continue;

				// This is an enum pin we care about - check its default value
				FString DefaultValue = Pin->DefaultValue;

				if (DefaultValue == OldValue)
				{
					// Fix it!
					Pin->DefaultValue = NewValue;
					FixedCount++;

					UE_LOG(LogBlueprintFixer, Log, TEXT("  Fixed pin '%s' in node '%s' (graph: %s): '%s' -> '%s'"),
						*Pin->PinName.ToString(),
						*Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString(),
						*Graph->GetName(),
						*OldValue,
						*NewValue);
				}
			}
		}
	}

	if (FixedCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		UE_LOG(LogBlueprintFixer, Log, TEXT("FixEnumPinValues: Fixed %d pins in %s"), FixedCount, *Blueprint->GetName());
	}
	else
	{
		UE_LOG(LogBlueprintFixer, Log, TEXT("FixEnumPinValues: No pins needed fixing in %s"), *Blueprint->GetName());
	}

	return FixedCount;
}

TArray<FString> UBlueprintFixerLibrary::GetEnumPinInfo(UBlueprint* Blueprint, FString EnumPathSubstring)
{
	TArray<FString> Results;

	if (!Blueprint)
	{
		Results.Add(TEXT("ERROR: Blueprint is null"));
		return Results;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("GetEnumPinInfo: Analyzing %s for enum '%s'"),
		*Blueprint->GetName(), *EnumPathSubstring);

	// Get all graphs in the Blueprint
	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin) continue;

				FEdGraphPinType& PinType = Pin->PinType;
				UObject* SubCatObj = PinType.PinSubCategoryObject.Get();
				if (!SubCatObj) continue;

				FString SubCatPath = SubCatObj->GetPathName();
				if (!SubCatPath.Contains(EnumPathSubstring)) continue;

				FString Info = FString::Printf(TEXT("Graph=%s, Node=%s, Pin=%s, DefaultValue=%s, EnumPath=%s"),
					*Graph->GetName(),
					*Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString(),
					*Pin->PinName.ToString(),
					*Pin->DefaultValue,
					*SubCatPath);

				Results.Add(Info);
				UE_LOG(LogBlueprintFixer, Log, TEXT("  %s"), *Info);
			}
		}
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("GetEnumPinInfo: Found %d enum pins"), Results.Num());

	return Results;
}

//////////////////////////////////////////////////////////////////////////
// FixPinsByValue - Search by VALUE content, not type
//////////////////////////////////////////////////////////////////////////

int32 UBlueprintFixerLibrary::FixPinsByValue(UBlueprint* Blueprint, FString OldValue, FString NewValue)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("FixPinsByValue: Blueprint is null"));
		return 0;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("FixPinsByValue: Processing %s, replacing '%s' with '%s'"),
		*Blueprint->GetName(), *OldValue, *NewValue);

	int32 FixedCount = 0;

	// Get all graphs in the Blueprint
	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	UE_LOG(LogBlueprintFixer, Log, TEXT("  Found %d graphs"), AllGraphs.Num());

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin) continue;

				// Check if DefaultValue matches exactly
				if (Pin->DefaultValue == OldValue)
				{
					FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();

					UE_LOG(LogBlueprintFixer, Log, TEXT("  Fixing pin '%s' in node '%s' (graph: %s): '%s' -> '%s'"),
						*Pin->PinName.ToString(),
						*NodeTitle,
						*Graph->GetName(),
						*OldValue,
						*NewValue);

					Pin->DefaultValue = NewValue;
					FixedCount++;
				}
			}
		}
	}

	if (FixedCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		UE_LOG(LogBlueprintFixer, Log, TEXT("FixPinsByValue: Fixed %d pins in %s"), FixedCount, *Blueprint->GetName());
	}
	else
	{
		UE_LOG(LogBlueprintFixer, Log, TEXT("FixPinsByValue: No pins with value '%s' found in %s"), *OldValue, *Blueprint->GetName());
	}

	return FixedCount;
}

TArray<FString> UBlueprintFixerLibrary::GetAllPinValues(UBlueprint* Blueprint)
{
	TArray<FString> Results;

	if (!Blueprint)
	{
		Results.Add(TEXT("ERROR: Blueprint is null"));
		return Results;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("GetAllPinValues: Analyzing %s"), *Blueprint->GetName());

	// Get all graphs in the Blueprint
	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin) continue;

				// Only report pins with non-empty DefaultValue
				if (!Pin->DefaultValue.IsEmpty())
				{
					FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
					FString Info = FString::Printf(TEXT("Graph=%s, Node=%s, Pin=%s, Value=%s"),
						*Graph->GetName(),
						*NodeTitle,
						*Pin->PinName.ToString(),
						*Pin->DefaultValue);
					Results.Add(Info);
				}
			}
		}
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("GetAllPinValues: Found %d pins with values"), Results.Num());

	return Results;
}

//////////////////////////////////////////////////////////////////////////
// FixPinsByNameAndValue - Targeted fix by pin NAME and VALUE
//////////////////////////////////////////////////////////////////////////

int32 UBlueprintFixerLibrary::FixPinsByNameAndValue(UBlueprint* Blueprint, FString PinName, FString OldValue, FString NewValue)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("FixPinsByNameAndValue: Blueprint is null"));
		return 0;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("FixPinsByNameAndValue: Processing %s, pin='%s', replacing '%s' with '%s'"),
		*Blueprint->GetName(), *PinName, *OldValue, *NewValue);

	int32 FixedCount = 0;

	// Get all graphs in the Blueprint
	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin) continue;

				// Check if pin name matches AND value matches
				FString CurrentPinName = Pin->PinName.ToString();
				if (CurrentPinName == PinName && Pin->DefaultValue == OldValue)
				{
					FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();

					UE_LOG(LogBlueprintFixer, Log, TEXT("  Fixing pin '%s' in node '%s' (graph: %s): '%s' -> '%s'"),
						*CurrentPinName,
						*NodeTitle,
						*Graph->GetName(),
						*OldValue,
						*NewValue);

					Pin->DefaultValue = NewValue;
					FixedCount++;
				}
			}
		}
	}

	if (FixedCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		UE_LOG(LogBlueprintFixer, Log, TEXT("FixPinsByNameAndValue: Fixed %d pins in %s"), FixedCount, *Blueprint->GetName());
	}
	else
	{
		UE_LOG(LogBlueprintFixer, Log, TEXT("FixPinsByNameAndValue: No pins named '%s' with value '%s' found in %s"),
			*PinName, *OldValue, *Blueprint->GetName());
	}

	return FixedCount;
}

//////////////////////////////////////////////////////////////////////////
// FixVariableStructType - Fix broken struct variable types
//////////////////////////////////////////////////////////////////////////

bool UBlueprintFixerLibrary::FixVariableStructType(UBlueprint* Blueprint, FString VariableName, FString NewStructPath)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("FixVariableStructType: Blueprint is null"));
		return false;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("FixVariableStructType: Looking for variable '%s' in %s, new type: %s"),
		*VariableName, *Blueprint->GetName(), *NewStructPath);

	// Find the struct - for C++ structs we need FindObject, not LoadObject
	UScriptStruct* NewStruct = nullptr;

	// Extract struct name from path like "/Script/SLFConversion.FStatusEffectTick"
	FString StructName;
	int32 DotIndex;
	if (NewStructPath.FindLastChar('.', DotIndex))
	{
		StructName = NewStructPath.RightChop(DotIndex + 1);
	}
	else
	{
		StructName = NewStructPath;
	}

	// First try to find in the SLFConversion module package
	UPackage* Package = FindPackage(nullptr, TEXT("/Script/SLFConversion"));
	if (Package)
	{
		NewStruct = FindObject<UScriptStruct>(Package, *StructName);
		UE_LOG(LogBlueprintFixer, Log, TEXT("  Searched in /Script/SLFConversion package: %s"),
			NewStruct ? TEXT("FOUND") : TEXT("not found"));
	}

	// Try the full path
	if (!NewStruct)
	{
		NewStruct = FindObject<UScriptStruct>(nullptr, *NewStructPath);
	}

	// Try iterating through all packages to find the struct
	if (!NewStruct)
	{
		for (TObjectIterator<UScriptStruct> It; It; ++It)
		{
			if (It->GetName() == StructName)
			{
				NewStruct = *It;
				UE_LOG(LogBlueprintFixer, Log, TEXT("  Found struct via iteration: %s"), *NewStruct->GetPathName());
				break;
			}
		}
	}

	if (!NewStruct)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("FixVariableStructType: Could not find struct at path '%s'"), *NewStructPath);
		return false;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("  Found struct: %s"), *NewStruct->GetName());

	// Find the variable in the Blueprint
	FName VarName(*VariableName);
	int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, VarName);

	if (VarIndex == INDEX_NONE)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("FixVariableStructType: Variable '%s' not found in Blueprint"), *VariableName);
		return false;
	}

	// Get the variable description
	FBPVariableDescription& VarDesc = Blueprint->NewVariables[VarIndex];

	// Check current type
	FString CurrentTypePath = VarDesc.VarType.PinSubCategoryObject.IsValid() ?
		VarDesc.VarType.PinSubCategoryObject->GetPathName() : TEXT("None");

	UE_LOG(LogBlueprintFixer, Log, TEXT("  Current type path: %s"), *CurrentTypePath);

	// Update the type to the new struct
	VarDesc.VarType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	VarDesc.VarType.PinSubCategoryObject = NewStruct;

	UE_LOG(LogBlueprintFixer, Log, TEXT("  Updated variable '%s' to use struct '%s'"),
		*VariableName, *NewStruct->GetName());

	// Mark as modified and refresh
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	return true;
}

TArray<FString> UBlueprintFixerLibrary::GetVariableTypeInfo(UBlueprint* Blueprint)
{
	TArray<FString> Results;

	if (!Blueprint)
	{
		Results.Add(TEXT("ERROR: Blueprint is null"));
		return Results;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("GetVariableTypeInfo: Analyzing %s"), *Blueprint->GetName());

	for (const FBPVariableDescription& VarDesc : Blueprint->NewVariables)
	{
		FString VarName = VarDesc.VarName.ToString();
		FString Category = VarDesc.VarType.PinCategory.ToString();
		FString SubCategory = VarDesc.VarType.PinSubCategory.ToString();

		FString TypePath = TEXT("None");
		if (VarDesc.VarType.PinSubCategoryObject.IsValid())
		{
			TypePath = VarDesc.VarType.PinSubCategoryObject->GetPathName();
		}

		bool bIsArray = VarDesc.VarType.IsArray();
		bool bIsMap = VarDesc.VarType.IsMap();

		FString ContainerType = bIsArray ? TEXT("Array") : (bIsMap ? TEXT("Map") : TEXT("Single"));

		// Check if this is a broken reference
		bool bIsBroken = Category == TEXT("struct") && TypePath.Contains(TEXT("/Game/SoulslikeFramework/Structures/"));

		FString Info = FString::Printf(TEXT("Name=%s, Category=%s, TypePath=%s, Container=%s, Broken=%s"),
			*VarName, *Category, *TypePath, *ContainerType, bIsBroken ? TEXT("YES") : TEXT("no"));

		Results.Add(Info);
		UE_LOG(LogBlueprintFixer, Log, TEXT("  %s"), *Info);
	}

	return Results;
}

//////////////////////////////////////////////////////////////////////////
// Node Management Functions
//////////////////////////////////////////////////////////////////////////

bool UBlueprintFixerLibrary::DeleteNodeByGuid(UBlueprint* Blueprint, FString NodeGuidString)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("DeleteNodeByGuid: Blueprint is null"));
		return false;
	}

	// Parse the GUID string
	FGuid TargetGuid;
	if (!FGuid::Parse(NodeGuidString, TargetGuid))
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("DeleteNodeByGuid: Invalid GUID string '%s'"), *NodeGuidString);
		return false;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("DeleteNodeByGuid: Looking for node with GUID %s in %s"),
		*NodeGuidString, *Blueprint->GetName());

	// Search all graphs for the node
	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			if (Node->NodeGuid == TargetGuid)
			{
				FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();

				UE_LOG(LogBlueprintFixer, Log, TEXT("  Found node '%s' in graph '%s', deleting..."),
					*NodeTitle, *Graph->GetName());

				// Remove the node
				Graph->RemoveNode(Node);

				FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

				UE_LOG(LogBlueprintFixer, Log, TEXT("  Successfully deleted node"));
				return true;
			}
		}
	}

	UE_LOG(LogBlueprintFixer, Warning, TEXT("DeleteNodeByGuid: Node with GUID %s not found"), *NodeGuidString);
	return false;
}

TArray<FString> UBlueprintFixerLibrary::GetBrokenNodeGuids(UBlueprint* Blueprint)
{
	TArray<FString> Results;

	if (!Blueprint)
	{
		Results.Add(TEXT("ERROR: Blueprint is null"));
		return Results;
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("GetBrokenNodeGuids: Analyzing %s"), *Blueprint->GetName());

	// Keywords that indicate broken nodes
	TArray<FString> BrokenKeywords;
	BrokenKeywords.Add(TEXT("<unknown struct>"));
	BrokenKeywords.Add(TEXT("bad enum"));
	BrokenKeywords.Add(TEXT("Wildcard"));

	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
			FString NodeClass = Node->GetClass()->GetName();

			bool bIsBroken = false;
			for (const FString& Keyword : BrokenKeywords)
			{
				if (NodeTitle.Contains(Keyword) || NodeClass.Contains(Keyword))
				{
					bIsBroken = true;
					break;
				}
			}

			// Also check for BreakStruct/MakeStruct nodes without valid struct
			if (NodeClass.Contains(TEXT("BreakStruct")) || NodeClass.Contains(TEXT("MakeStruct")))
			{
				// Check if the title contains "unknown"
				if (NodeTitle.Contains(TEXT("unknown")) || NodeTitle.Contains(TEXT("<")))
				{
					bIsBroken = true;
				}
			}

			if (bIsBroken)
			{
				FString GuidStr = Node->NodeGuid.ToString(EGuidFormats::DigitsWithHyphens);
				// Convert to no-hyphen format for easier use
				GuidStr = Node->NodeGuid.ToString(EGuidFormats::Digits);

				FString Info = FString::Printf(TEXT("GUID=%s, Graph=%s, Class=%s, Title=%s"),
					*GuidStr, *Graph->GetName(), *NodeClass, *NodeTitle);

				Results.Add(Info);
				UE_LOG(LogBlueprintFixer, Log, TEXT("  Broken: %s"), *Info);
			}
		}
	}

	UE_LOG(LogBlueprintFixer, Log, TEXT("GetBrokenNodeGuids: Found %d broken nodes"), Results.Num());
	return Results;
}

int32 UBlueprintFixerLibrary::DeleteNodesByGuids(UBlueprint* Blueprint, TArray<FString> NodeGuids)
{
	if (!Blueprint)
	{
		UE_LOG(LogBlueprintFixer, Error, TEXT("DeleteNodesByGuids: Blueprint is null"));
		return 0;
	}

	int32 DeletedCount = 0;

	for (const FString& GuidStr : NodeGuids)
	{
		if (DeleteNodeByGuid(Blueprint, GuidStr))
		{
			DeletedCount++;
		}
	}

	if (DeletedCount > 0)
	{
		UE_LOG(LogBlueprintFixer, Log, TEXT("DeleteNodesByGuids: Deleted %d nodes from %s"),
			DeletedCount, *Blueprint->GetName());
	}

	return DeletedCount;
}
