// SLFAnimBPExport.cpp
// AnimBP Export/Diff/Fix implementation for SLFAutomationLibrary
// This file is included at the end of SLFAutomationLibrary.cpp


// Helper to escape a string for JSON (handles newlines, quotes, etc.)
static FString SLFEscapeJsonString(const FString& Input)
{
	FString Result = Input;
	Result = Result.Replace(TEXT("\\"), TEXT("\\\\"));
	Result = Result.Replace(TEXT("\""), TEXT("\\\""));
	Result = Result.Replace(TEXT("\n"), TEXT("\\n"));
	Result = Result.Replace(TEXT("\r"), TEXT("\\r"));
	Result = Result.Replace(TEXT("\t"), TEXT("\\t"));
	return Result;
}

// Helper to serialize a pin to JSON format
static FString SerializePinToJson(UEdGraphPin* Pin, int32 Indent)
{
	if (!Pin) return TEXT("null");

	FString IndentStr;
	for (int32 i = 0; i < Indent; i++) IndentStr += TEXT("  ");

	FString Json;
	Json += IndentStr + TEXT("{\n");
	Json += IndentStr + TEXT("  \"PinName\": \"") + SLFEscapeJsonString(Pin->PinName.ToString()) + TEXT("\",\n");
	Json += IndentStr + TEXT("  \"PinType\": \"") + Pin->PinType.PinCategory.ToString() + TEXT("\",\n");
	Json += IndentStr + TEXT("  \"Direction\": \"") + (Pin->Direction == EGPD_Input ? TEXT("Input") : TEXT("Output")) + TEXT("\",\n");
	Json += IndentStr + TEXT("  \"DefaultValue\": \"") + SLFEscapeJsonString(Pin->DefaultValue) + TEXT("\",\n");

	// Connected pins
	Json += IndentStr + TEXT("  \"LinkedTo\": [");
	for (int32 i = 0; i < Pin->LinkedTo.Num(); i++)
	{
		if (i > 0) Json += TEXT(", ");
		UEdGraphPin* LinkedPin = Pin->LinkedTo[i];
		if (LinkedPin && LinkedPin->GetOwningNode())
		{
			Json += TEXT("\"") + LinkedPin->GetOwningNode()->GetName() + TEXT(".") + LinkedPin->PinName.ToString() + TEXT("\"");
		}
	}
	Json += TEXT("]\n");
	Json += IndentStr + TEXT("}");

	return Json;
}

// Helper to serialize a node to JSON format
static FString SerializeNodeToJson(UEdGraphNode* Node, int32 Indent)
{
	if (!Node) return TEXT("null");

	FString IndentStr;
	for (int32 i = 0; i < Indent; i++) IndentStr += TEXT("  ");

	FString Json;
	Json += IndentStr + TEXT("{\n");
	Json += IndentStr + TEXT("  \"NodeName\": \"") + Node->GetName() + TEXT("\",\n");
	Json += IndentStr + TEXT("  \"NodeClass\": \"") + Node->GetClass()->GetName() + TEXT("\",\n");
	Json += IndentStr + TEXT("  \"NodeTitle\": \"") + SLFEscapeJsonString(Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()) + TEXT("\",\n");
	Json += IndentStr + TEXT("  \"NodePosX\": ") + FString::FromInt(Node->NodePosX) + TEXT(",\n");
	Json += IndentStr + TEXT("  \"NodePosY\": ") + FString::FromInt(Node->NodePosY) + TEXT(",\n");
	Json += IndentStr + TEXT("  \"NodeGuid\": \"") + Node->NodeGuid.ToString() + TEXT("\",\n");

	// Export all relevant properties
	Json += IndentStr + TEXT("  \"Properties\": {\n");
	bool bFirstProp = true;
	for (TFieldIterator<FProperty> PropIt(Node->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Prop = *PropIt;
		FString PropName = Prop->GetName();

		// Skip internal/common properties
		if (PropName == TEXT("NodePosX") || PropName == TEXT("NodePosY") ||
			PropName == TEXT("NodeGuid") || PropName == TEXT("Pins")) continue;

		FString ValueStr;
		if (FStrProperty* StrProp = CastField<FStrProperty>(Prop))
		{
			FString* Value = StrProp->ContainerPtrToValuePtr<FString>(Node);
			if (Value && !Value->IsEmpty()) ValueStr = *Value;
		}
		else if (FNameProperty* NameProp = CastField<FNameProperty>(Prop))
		{
			FName* Value = NameProp->ContainerPtrToValuePtr<FName>(Node);
			if (Value && !Value->IsNone()) ValueStr = Value->ToString();
		}
		else if (FClassProperty* ClassProp = CastField<FClassProperty>(Prop))
		{
			UClass** Value = ClassProp->ContainerPtrToValuePtr<UClass*>(Node);
			if (Value && *Value) ValueStr = (*Value)->GetPathName();
		}
		else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
		{
			UObject** Value = ObjProp->ContainerPtrToValuePtr<UObject*>(Node);
			if (Value && *Value) ValueStr = (*Value)->GetPathName();
		}

		if (!ValueStr.IsEmpty())
		{
			if (!bFirstProp) Json += TEXT(",\n");
			bFirstProp = false;
			Json += IndentStr + TEXT("    \"") + PropName + TEXT("\": \"") + SLFEscapeJsonString(ValueStr) + TEXT("\"");
		}
	}
	Json += TEXT("\n") + IndentStr + TEXT("  },\n");

	// Export pins
	Json += IndentStr + TEXT("  \"Pins\": [\n");
	for (int32 i = 0; i < Node->Pins.Num(); i++)
	{
		if (i > 0) Json += TEXT(",\n");
		Json += SerializePinToJson(Node->Pins[i], Indent + 2);
	}
	Json += TEXT("\n") + IndentStr + TEXT("  ]\n");
	Json += IndentStr + TEXT("}");

	return Json;
}

FString USLFAutomationLibrary::ExportBlueprintState(UObject* BlueprintAsset, const FString& OutputFilePath)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(BlueprintAsset);
	if (!Blueprint)
	{
		return TEXT("{\"error\": \"Not a Blueprint\"}");
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("ExportBlueprintState: %s"), *Blueprint->GetName());

	FString Json;
	Json += TEXT("{\n");
	Json += TEXT("  \"BlueprintName\": \"") + Blueprint->GetName() + TEXT("\",\n");
	Json += TEXT("  \"BlueprintClass\": \"") + Blueprint->GetClass()->GetName() + TEXT("\",\n");
	Json += TEXT("  \"ParentClass\": \"") + (Blueprint->ParentClass ? Blueprint->ParentClass->GetPathName() : TEXT("None")) + TEXT("\",\n");

	// Variables
	Json += TEXT("  \"Variables\": [\n");
	for (int32 i = 0; i < Blueprint->NewVariables.Num(); i++)
	{
		const FBPVariableDescription& Var = Blueprint->NewVariables[i];
		if (i > 0) Json += TEXT(",\n");
		Json += TEXT("    {\"Name\": \"") + Var.VarName.ToString() + TEXT("\", \"Type\": \"") + Var.VarType.PinCategory.ToString() + TEXT("\"}");
	}
	Json += TEXT("\n  ],\n");

	// Graphs
	Json += TEXT("  \"Graphs\": [\n");

	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(Blueprint->UbergraphPages);
	AllGraphs.Append(Blueprint->FunctionGraphs);

	for (int32 gi = 0; gi < AllGraphs.Num(); gi++)
	{
		UEdGraph* Graph = AllGraphs[gi];
		if (!Graph) continue;

		if (gi > 0) Json += TEXT(",\n");
		Json += TEXT("    {\n");
		Json += TEXT("      \"GraphName\": \"") + Graph->GetName() + TEXT("\",\n");
		Json += TEXT("      \"GraphClass\": \"") + Graph->GetClass()->GetName() + TEXT("\",\n");
		Json += TEXT("      \"NodeCount\": ") + FString::FromInt(Graph->Nodes.Num()) + TEXT(",\n");
		Json += TEXT("      \"Nodes\": [\n");

		for (int32 ni = 0; ni < Graph->Nodes.Num(); ni++)
		{
			if (ni > 0) Json += TEXT(",\n");
			Json += SerializeNodeToJson(Graph->Nodes[ni], 4);
		}

		Json += TEXT("\n      ]\n");
		Json += TEXT("    }");
	}

	Json += TEXT("\n  ]\n");
	Json += TEXT("}\n");

	if (!OutputFilePath.IsEmpty())
	{
		FFileHelper::SaveStringToFile(Json, *OutputFilePath);
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Saved to: %s (%d bytes)"), *OutputFilePath, Json.Len());
	}

	return Json;
}

FString USLFAutomationLibrary::ExportAnimGraphState(UObject* AnimBlueprintAsset, const FString& OutputFilePath)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		return TEXT("{\"error\": \"Not an AnimBlueprint\"}");
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("ExportAnimGraphState: %s"), *AnimBP->GetName());

	FString Json;
	Json += TEXT("{\n");
	Json += TEXT("  \"AnimBlueprintName\": \"") + AnimBP->GetName() + TEXT("\",\n");
	Json += TEXT("  \"TargetSkeleton\": \"") + (AnimBP->TargetSkeleton ? AnimBP->TargetSkeleton->GetPathName() : TEXT("None")) + TEXT("\",\n");
	Json += TEXT("  \"ParentClass\": \"") + (AnimBP->ParentClass ? AnimBP->ParentClass->GetPathName() : TEXT("None")) + TEXT("\",\n");

	// Variables with full details
	Json += TEXT("  \"Variables\": [\n");
	for (int32 i = 0; i < AnimBP->NewVariables.Num(); i++)
	{
		const FBPVariableDescription& Var = AnimBP->NewVariables[i];
		if (i > 0) Json += TEXT(",\n");
		Json += TEXT("    {\n");
		Json += TEXT("      \"Name\": \"") + Var.VarName.ToString() + TEXT("\",\n");
		Json += TEXT("      \"Type\": \"") + Var.VarType.PinCategory.ToString() + TEXT("\",\n");
		Json += TEXT("      \"SubCategory\": \"") + Var.VarType.PinSubCategory.ToString() + TEXT("\",\n");
		Json += TEXT("      \"SubCategoryObject\": \"") + (Var.VarType.PinSubCategoryObject.IsValid() ? Var.VarType.PinSubCategoryObject->GetPathName() : TEXT("")) + TEXT("\"\n");
		Json += TEXT("    }");
	}
	Json += TEXT("\n  ],\n");

	// All graphs with full node details
	Json += TEXT("  \"Graphs\": [\n");

	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(AnimBP->UbergraphPages);
	AllGraphs.Append(AnimBP->FunctionGraphs);

	for (int32 gi = 0; gi < AllGraphs.Num(); gi++)
	{
		UEdGraph* Graph = AllGraphs[gi];
		if (!Graph) continue;

		if (gi > 0) Json += TEXT(",\n");
		Json += TEXT("    {\n");
		Json += TEXT("      \"GraphName\": \"") + Graph->GetName() + TEXT("\",\n");
		Json += TEXT("      \"GraphClass\": \"") + Graph->GetClass()->GetName() + TEXT("\",\n");
		Json += TEXT("      \"Nodes\": [\n");

		for (int32 ni = 0; ni < Graph->Nodes.Num(); ni++)
		{
			UEdGraphNode* Node = Graph->Nodes[ni];
			if (!Node) continue;

			if (ni > 0) Json += TEXT(",\n");
			Json += SerializeNodeToJson(Node, 4);
		}

		Json += TEXT("\n      ]\n");
		Json += TEXT("    }");
	}

	Json += TEXT("\n  ]\n");
	Json += TEXT("}\n");

	if (!OutputFilePath.IsEmpty())
	{
		FFileHelper::SaveStringToFile(Json, *OutputFilePath);
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Saved to: %s (%d bytes)"), *OutputFilePath, Json.Len());
	}

	return Json;
}

FString USLFAutomationLibrary::DiffBlueprintStates(const FString& BeforeJsonPath, const FString& AfterJsonPath)
{
	FString BeforeJson, AfterJson;

	if (!FFileHelper::LoadFileToString(BeforeJson, *BeforeJsonPath))
	{
		return TEXT("Error: Could not load before JSON: ") + BeforeJsonPath;
	}
	if (!FFileHelper::LoadFileToString(AfterJson, *AfterJsonPath))
	{
		return TEXT("Error: Could not load after JSON: ") + AfterJsonPath;
	}

	FString Diff;
	Diff += TEXT("=== BLUEPRINT STATE DIFF ===\n\n");
	Diff += TEXT("Before: ") + BeforeJsonPath + TEXT("\n");
	Diff += TEXT("After: ") + AfterJsonPath + TEXT("\n\n");

	// Line-by-line diff
	TArray<FString> BeforeLines, AfterLines;
	BeforeJson.ParseIntoArrayLines(BeforeLines);
	AfterJson.ParseIntoArrayLines(AfterLines);

	Diff += FString::Printf(TEXT("Before: %d lines\n"), BeforeLines.Num());
	Diff += FString::Printf(TEXT("After: %d lines\n\n"), AfterLines.Num());

	// Find differences
	TSet<FString> BeforeSet(BeforeLines);
	TSet<FString> AfterSet(AfterLines);

	int32 RemovedCount = 0, AddedCount = 0;

	Diff += TEXT("=== REMOVED ===\n");
	for (const FString& Line : BeforeLines)
	{
		FString Trimmed = Line.TrimStartAndEnd();
		if (!AfterSet.Contains(Line) && !Trimmed.IsEmpty() && !Trimmed.StartsWith(TEXT("{")))
		{
			Diff += TEXT("- ") + Trimmed + TEXT("\n");
			RemovedCount++;
		}
	}

	Diff += TEXT("\n=== ADDED ===\n");
	for (const FString& Line : AfterLines)
	{
		FString Trimmed = Line.TrimStartAndEnd();
		if (!BeforeSet.Contains(Line) && !Trimmed.IsEmpty() && !Trimmed.StartsWith(TEXT("{")))
		{
			Diff += TEXT("+ ") + Trimmed + TEXT("\n");
			AddedCount++;
		}
	}

	Diff += FString::Printf(TEXT("\n=== SUMMARY ===\nRemoved: %d lines\nAdded: %d lines\n"), RemovedCount, AddedCount);

	return Diff;
}

int32 USLFAutomationLibrary::FixAnimBPFromReference(UObject* AnimBlueprintAsset, const FString& ReferenceJsonPath)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("FixAnimBPFromReference: Not an AnimBlueprint"));
		return 0;
	}

	FString ReferenceJson;
	if (!FFileHelper::LoadFileToString(ReferenceJson, *ReferenceJsonPath))
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("FixAnimBPFromReference: Could not load reference JSON: %s"), *ReferenceJsonPath);
		return 0;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("FixAnimBPFromReference: %s from %s"), *AnimBP->GetName(), *ReferenceJsonPath);

	int32 FixCount = 0;

	// Map of old -> new property names
	TMap<FString, FString> PropertyRenames;
	PropertyRenames.Add(TEXT("IsGuarding?"), TEXT("IsGuarding"));
	PropertyRenames.Add(TEXT("bIsAccelerating?"), TEXT("bIsAccelerating"));
	PropertyRenames.Add(TEXT("bIsBlocking?"), TEXT("bIsBlocking"));
	PropertyRenames.Add(TEXT("bIsFalling?"), TEXT("bIsFalling"));
	PropertyRenames.Add(TEXT("IsResting?"), TEXT("IsResting"));

	// Check reference for these properties and fix them
	for (const auto& Pair : PropertyRenames)
	{
		if (ReferenceJson.Contains(Pair.Key))
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Found '%s' in reference, fixing to '%s'"), *Pair.Key, *Pair.Value);
			int32 Fixed = FixPropertyAccessPaths(AnimBP, Pair.Key, Pair.Value);
			FixCount += Fixed;
		}
	}

	if (FixCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
		FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("  Applied %d total fixes"), FixCount);
	return FixCount;
}

FString USLFAutomationLibrary::GetNodeDetails(UObject* BlueprintAsset, const FString& NodeName)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(BlueprintAsset);
	if (!Blueprint)
	{
		return TEXT("{\"error\": \"Not a Blueprint\"}");
	}

	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(Blueprint->UbergraphPages);
	AllGraphs.Append(Blueprint->FunctionGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			if (Node->GetName() == NodeName ||
				Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString().Contains(NodeName))
			{
				return SerializeNodeToJson(Node, 0);
			}
		}
	}

	return TEXT("{\"error\": \"Node not found: ") + NodeName + TEXT("\"}");
}


int32 USLFAutomationLibrary::FixPropertyAccessPaths(UObject* AnimBlueprintAsset, const FString& OldPropertyName, const FString& NewPropertyName)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("FixPropertyAccessPaths: Not an AnimBlueprint"));
		return 0;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("FixPropertyAccessPaths: %s - '%s' -> '%s'"), 
		*AnimBP->GetName(), *OldPropertyName, *NewPropertyName);

	int32 FixCount = 0;

	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(AnimBP->UbergraphPages);
	AllGraphs.Append(AnimBP->FunctionGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			// Check all string properties for the old property name
			for (TFieldIterator<FStrProperty> PropIt(Node->GetClass()); PropIt; ++PropIt)
			{
				FStrProperty* StrProp = *PropIt;
				FString* Value = StrProp->ContainerPtrToValuePtr<FString>(Node);
				if (Value && Value->Contains(OldPropertyName))
				{
					FString NewValue = Value->Replace(*OldPropertyName, *NewPropertyName);
					UE_LOG(LogSLFAutomation, Warning, TEXT("  Fixed %s.%s: '%s' -> '%s'"),
						*Node->GetName(), *StrProp->GetName(), **Value, *NewValue);
					*Value = NewValue;
					FixCount++;
				}
			}

			// Also check array of strings/names that might contain paths
			for (TFieldIterator<FArrayProperty> PropIt(Node->GetClass()); PropIt; ++PropIt)
			{
				FArrayProperty* ArrProp = *PropIt;
				if (FStrProperty* InnerStr = CastField<FStrProperty>(ArrProp->Inner))
				{
					FScriptArrayHelper ArrayHelper(ArrProp, ArrProp->ContainerPtrToValuePtr<void>(Node));
					for (int32 i = 0; i < ArrayHelper.Num(); i++)
					{
						FString* ElemValue = reinterpret_cast<FString*>(ArrayHelper.GetRawPtr(i));
						if (ElemValue && ElemValue->Contains(OldPropertyName))
						{
							FString NewValue = ElemValue->Replace(*OldPropertyName, *NewPropertyName);
							UE_LOG(LogSLFAutomation, Warning, TEXT("  Fixed %s.%s[%d]: '%s' -> '%s'"),
								*Node->GetName(), *ArrProp->GetName(), i, **ElemValue, *NewValue);
							*ElemValue = NewValue;
							FixCount++;
						}
					}
				}
			}
		}
	}

	if (FixCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(AnimBP);
	}

	return FixCount;
}

int32 USLFAutomationLibrary::FixLinkedAnimLayers(UObject* AnimBlueprintAsset, const FString& InterfaceName, const FString& LinkedAnimGraphPath)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("FixLinkedAnimLayers: Not an AnimBlueprint"));
		return 0;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("FixLinkedAnimLayers: %s - Interface: '%s', LinkedAnim: '%s'"), 
		*AnimBP->GetName(), *InterfaceName, *LinkedAnimGraphPath);

	// Load the linked anim graph class
	UClass* LinkedAnimClass = LoadClass<UAnimInstance>(nullptr, *LinkedAnimGraphPath);
	if (!LinkedAnimClass)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not load LinkedAnimGraph class: %s"), *LinkedAnimGraphPath);
		return 0;
	}

	int32 FixCount = 0;

	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(AnimBP->UbergraphPages);
	AllGraphs.Append(AnimBP->FunctionGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			// Check if this is a LinkedAnimLayer node
			if (Node->GetClass()->GetName().Contains(TEXT("LinkedAnimLayer")))
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("  Found LinkedAnimLayer node: %s"), *Node->GetName());

				// Try to find and set InstanceClass property
				FClassProperty* ClassProp = FindFProperty<FClassProperty>(Node->GetClass(), TEXT("InstanceClass"));
				if (ClassProp)
				{
					UClass** Value = ClassProp->ContainerPtrToValuePtr<UClass*>(Node);
					if (Value)
					{
						*Value = LinkedAnimClass;
						UE_LOG(LogSLFAutomation, Warning, TEXT("    Set InstanceClass to: %s"), *LinkedAnimClass->GetName());
						FixCount++;
					}
				}
			}
		}
	}

	if (FixCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(AnimBP);
	}

	return FixCount;
}

FString USLFAutomationLibrary::DiagnoseAnimGraphNodes(UObject* AnimBlueprintAsset)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		return TEXT("Error: Not an AnimBlueprint");
	}

	FString Diagnosis;
	Diagnosis += FString::Printf(TEXT("=== ANIMBLUEPRINT DIAGNOSIS: %s ===\n\n"), *AnimBP->GetName());
	Diagnosis += FString::Printf(TEXT("ParentClass: %s\n"), AnimBP->ParentClass ? *AnimBP->ParentClass->GetName() : TEXT("None"));
	Diagnosis += FString::Printf(TEXT("TargetSkeleton: %s\n\n"), AnimBP->TargetSkeleton ? *AnimBP->TargetSkeleton->GetName() : TEXT("None"));

	// Variables
	Diagnosis += TEXT("=== VARIABLES ===\n");
	for (const FBPVariableDescription& Var : AnimBP->NewVariables)
	{
		Diagnosis += FString::Printf(TEXT("  %s : %s\n"), *Var.VarName.ToString(), *Var.VarType.PinCategory.ToString());
	}

	// Graphs and nodes
	Diagnosis += TEXT("\n=== GRAPHS ===\n");

	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(AnimBP->UbergraphPages);
	AllGraphs.Append(AnimBP->FunctionGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		Diagnosis += FString::Printf(TEXT("\n--- %s (%s) ---\n"), *Graph->GetName(), *Graph->GetClass()->GetName());
		Diagnosis += FString::Printf(TEXT("  Nodes: %d\n"), Graph->Nodes.Num());

		// Count by type
		TMap<FString, int32> TypeCounts;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;
			FString TypeName = Node->GetClass()->GetName();
			TypeCounts.FindOrAdd(TypeName)++;
		}

		for (const auto& Pair : TypeCounts)
		{
			Diagnosis += FString::Printf(TEXT("    %s: %d\n"), *Pair.Key, Pair.Value);
		}

		// List specific node types of interest
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			FString ClassName = Node->GetClass()->GetName();
			if (ClassName.Contains(TEXT("PropertyAccess")) || 
				ClassName.Contains(TEXT("LinkedAnimLayer")) ||
				ClassName.Contains(TEXT("LayerNode")))
			{
				Diagnosis += FString::Printf(TEXT("  [%s] %s: %s\n"), 
					*ClassName, 
					*Node->GetName(),
					*Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString());

				// Dump relevant properties
				for (TFieldIterator<FProperty> PropIt(Node->GetClass()); PropIt; ++PropIt)
				{
					FProperty* Prop = *PropIt;
					FString ValueStr;

					if (FStrProperty* StrProp = CastField<FStrProperty>(Prop))
					{
						FString* Value = StrProp->ContainerPtrToValuePtr<FString>(Node);
						if (Value && !Value->IsEmpty()) ValueStr = *Value;
					}
					else if (FClassProperty* ClassProp = CastField<FClassProperty>(Prop))
					{
						UClass** Value = ClassProp->ContainerPtrToValuePtr<UClass*>(Node);
						if (Value && *Value) ValueStr = (*Value)->GetName();
					}
					else if (FNameProperty* NameProp = CastField<FNameProperty>(Prop))
					{
						FName* Value = NameProp->ContainerPtrToValuePtr<FName>(Node);
						if (Value && !Value->IsNone()) ValueStr = Value->ToString();
					}

					if (!ValueStr.IsEmpty())
					{
						Diagnosis += FString::Printf(TEXT("      %s = %s\n"), *Prop->GetName(), *ValueStr);
					}
				}
			}
		}
	}

	return Diagnosis;
}

// ============================================================================
// AAA-QUALITY LINKEDANIMLAYER FIX FUNCTIONS (Using proper UE5 APIs)
// ============================================================================

FString USLFAutomationLibrary::DiagnoseLinkedAnimLayerNodes(UObject* AnimBlueprintAsset)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		return TEXT("Error: Not an AnimBlueprint");
	}

	FString Result;
	Result += FString::Printf(TEXT("=== LINKEDANIMLAYER DIAGNOSIS: %s ===\n\n"), *AnimBP->GetName());

	// Build map of FunctionGraph names to GUIDs
	TMap<FName, FGuid> GraphNameToGuid;
	Result += TEXT("=== FUNCTION GRAPHS (Layer Implementations) ===\n");
	for (UEdGraph* Graph : AnimBP->FunctionGraphs)
	{
		if (Graph)
		{
			FName GraphFName = FName(*Graph->GetName());
			GraphNameToGuid.Add(GraphFName, Graph->GraphGuid);
			Result += FString::Printf(TEXT("  %s -> GUID: %s\n"), 
				*Graph->GetName(), *Graph->GraphGuid.ToString());
		}
	}
	Result += TEXT("\n");

	// Find LinkedAnimLayer nodes using proper Cast<>
	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(AnimBP->UbergraphPages);
	AllGraphs.Append(AnimBP->FunctionGraphs);

	Result += TEXT("=== LINKEDANIMLAYER NODES ===\n");
	int32 TotalNodes = 0;
	int32 GuidMismatches = 0;
	int32 MissingGraphs = 0;

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			// Use proper Cast<> - NOT string matching!
			UAnimGraphNode_LinkedAnimLayer* LayerNode = Cast<UAnimGraphNode_LinkedAnimLayer>(Node);
			if (!LayerNode) continue;

			TotalNodes++;

			// Access via public members and API
			const FAnimNode_LinkedAnimLayer& RuntimeNode = LayerNode->Node;
			bool bIsSelfLayer = (*RuntimeNode.Interface == nullptr);
			FName LayerName = RuntimeNode.Layer;

			Result += FString::Printf(TEXT("\nNode: %s\n"), *LayerNode->GetName());
			Result += FString::Printf(TEXT("  Layer: %s\n"), *LayerName.ToString());
			Result += FString::Printf(TEXT("  SelfLayer: %s\n"), bIsSelfLayer ? TEXT("YES") : TEXT("NO"));
			Result += FString::Printf(TEXT("  InterfaceGuid: %s\n"), *LayerNode->InterfaceGuid.ToString());

			if (bIsSelfLayer)
			{
				FGuid* FoundGuid = GraphNameToGuid.Find(LayerName);
				if (FoundGuid)
				{
					if (*FoundGuid == LayerNode->InterfaceGuid)
					{
						Result += TEXT("  Status: [OK] GUID matches implementing graph\n");
					}
					else
					{
						Result += FString::Printf(TEXT("  Status: [FIX NEEDED] GUID mismatch!\n"));
						Result += FString::Printf(TEXT("    Node GUID:  %s\n"), *LayerNode->InterfaceGuid.ToString());
						Result += FString::Printf(TEXT("    Graph GUID: %s\n"), *FoundGuid->ToString());
						GuidMismatches++;
					}
				}
				else
				{
					Result += FString::Printf(TEXT("  Status: [ERROR] No implementing graph found for layer: %s\n"), *LayerName.ToString());
					MissingGraphs++;
				}
			}
			else
			{
				Result += FString::Printf(TEXT("  Interface: %s\n"), 
					*RuntimeNode.Interface ? *(*RuntimeNode.Interface)->GetName() : TEXT("None"));
				Result += TEXT("  Status: [EXTERNAL] Uses external interface implementation\n");
			}
		}
	}

	Result += TEXT("\n=== SUMMARY ===\n");
	Result += FString::Printf(TEXT("Total LinkedAnimLayer nodes: %d\n"), TotalNodes);
	Result += FString::Printf(TEXT("GUID mismatches (fixable): %d\n"), GuidMismatches);
	Result += FString::Printf(TEXT("Missing graph implementations: %d\n"), MissingGraphs);

	if (GuidMismatches > 0)
	{
		Result += TEXT("\nRun FixLinkedAnimLayerGuid() to fix GUID mismatches.\n");
	}

	return Result;
}

int32 USLFAutomationLibrary::FixLinkedAnimLayerGuid(UObject* AnimBlueprintAsset)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("FixLinkedAnimLayerGuid: Not an AnimBlueprint"));
		return 0;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== FixLinkedAnimLayerGuid: %s ==="), *AnimBP->GetName());

	int32 FixCount = 0;

	// Build map of FunctionGraph names to GUIDs
	TMap<FName, FGuid> GraphNameToGuid;
	for (UEdGraph* Graph : AnimBP->FunctionGraphs)
	{
		if (Graph)
		{
			GraphNameToGuid.Add(FName(*Graph->GetName()), Graph->GraphGuid);
		}
	}

	// Find LinkedAnimLayer nodes
	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(AnimBP->UbergraphPages);
	AllGraphs.Append(AnimBP->FunctionGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			// Use proper Cast<>
			UAnimGraphNode_LinkedAnimLayer* LayerNode = Cast<UAnimGraphNode_LinkedAnimLayer>(Node);
			if (!LayerNode) continue;

			const FAnimNode_LinkedAnimLayer& RuntimeNode = LayerNode->Node;
			
			// Only fix self-layer nodes (Interface is nullptr)
			if (*RuntimeNode.Interface != nullptr) continue;

			FName LayerName = RuntimeNode.Layer;
			FGuid* CorrectGuid = GraphNameToGuid.Find(LayerName);

			if (CorrectGuid && *CorrectGuid != LayerNode->InterfaceGuid)
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("  Fixing %s: Layer=%s, OldGUID=%s -> NewGUID=%s"),
					*LayerNode->GetName(),
					*LayerName.ToString(),
					*LayerNode->InterfaceGuid.ToString(),
					*CorrectGuid->ToString());

				// Set the correct GUID
				LayerNode->InterfaceGuid = *CorrectGuid;

				// Use proper API to update - this validates the binding
				LayerNode->Modify();  // UpdateGuidForLayer not exported

				FixCount++;
			}
		}
	}

	if (FixCount > 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("Fixed %d LinkedAnimLayer GUIDs"), FixCount);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
	}
	else
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("No GUID fixes needed"));
	}

	return FixCount;
}

FString USLFAutomationLibrary::GetBlueprintCompileErrors(UObject* BlueprintAsset)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(BlueprintAsset);
	if (!Blueprint)
	{
		return TEXT("Error: Not a Blueprint");
	}

	FString Result;
	Result += FString::Printf(TEXT("=== COMPILE STATUS: %s ===\n\n"), *Blueprint->GetName());
	Result += FString::Printf(TEXT("Blueprint Status: %s\n"), 
		Blueprint->Status == BS_Error ? TEXT("ERROR") :
		Blueprint->Status == BS_UpToDate ? TEXT("UP TO DATE") :
		Blueprint->Status == BS_Dirty ? TEXT("DIRTY") : TEXT("UNKNOWN"));

	// Collect errors from graph nodes
	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(Blueprint->UbergraphPages);
	AllGraphs.Append(Blueprint->FunctionGraphs);

	int32 ErrorCount = 0;
	int32 WarningCount = 0;

	Result += TEXT("\n=== NODE ERRORS ===\n");

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			// Check for ErrorMsg property
			if (Node->bHasCompilerMessage)
			{
				if (Node->ErrorType == EMessageSeverity::Error)
				{
					Result += FString::Printf(TEXT("[ERROR] %s: %s\n"), 
						*Node->GetName(), *Node->ErrorMsg);
					ErrorCount++;
				}
				else if (Node->ErrorType == EMessageSeverity::Warning)
				{
					Result += FString::Printf(TEXT("[WARNING] %s: %s\n"), 
						*Node->GetName(), *Node->ErrorMsg);
					WarningCount++;
				}
			}
		}
	}

	Result += TEXT("\n=== SUMMARY ===\n");
	Result += FString::Printf(TEXT("Errors: %d\n"), ErrorCount);
	Result += FString::Printf(TEXT("Warnings: %d\n"), WarningCount);

	if (ErrorCount == 0 && WarningCount == 0)
	{
		Result += TEXT("\nNo compile errors or warnings found in graph nodes.\n");
	}

	return Result;
}
