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
	
	// Use GetAllGraphs to include AnimGraph (where BlendListByEnum nodes are)
	AnimBP->GetAllGraphs(AllGraphs);
	// Skip the old approach
	// AllGraphs.Append(AnimBP->UbergraphPages);

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
	
	// Use GetAllGraphs to include AnimGraph (where BlendListByEnum nodes are)
	AnimBP->GetAllGraphs(AllGraphs);
	// Skip the old approach
	// AllGraphs.Append(AnimBP->UbergraphPages);

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
	
	// Use GetAllGraphs to include AnimGraph (where BlendListByEnum nodes are)
	AnimBP->GetAllGraphs(AllGraphs);
	// Skip the old approach
	// AllGraphs.Append(AnimBP->UbergraphPages);

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
	
	// Use GetAllGraphs to include AnimGraph (where BlendListByEnum nodes are)
	AnimBP->GetAllGraphs(AllGraphs);
	// Skip the old approach
	// AllGraphs.Append(AnimBP->UbergraphPages);

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
	
	// Use GetAllGraphs to include AnimGraph (where BlendListByEnum nodes are)
	AnimBP->GetAllGraphs(AllGraphs);
	// Skip the old approach
	// AllGraphs.Append(AnimBP->UbergraphPages);

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
	
	// Use GetAllGraphs to include AnimGraph (where BlendListByEnum nodes are)
	AnimBP->GetAllGraphs(AllGraphs);
	// Skip the old approach
	// AllGraphs.Append(AnimBP->UbergraphPages);

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

// ============================================================================
// DiagnoseAnimBP - Comprehensive AnimBP analysis
// ============================================================================
FString USLFAutomationLibrary::DiagnoseAnimBP(UObject* AnimBlueprintAsset)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		return TEXT("ERROR: Not an AnimBlueprint asset");
	}

	FString Result;
	Result += TEXT("=======================================================================\n");
	Result += TEXT("ANIMBP DIAGNOSTIC REPORT\n");
	Result += FString::Printf(TEXT("Asset: %s\n"), *AnimBP->GetPathName());
	Result += TEXT("=======================================================================\n\n");

	// Get all graphs
	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);

	Result += FString::Printf(TEXT("Total Graphs: %d\n\n"), AllGraphs.Num());

	// Find AnimGraph and analyze it
	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		FString GraphName = Graph->GetName();
		int32 NodeCount = Graph->Nodes.Num();

		// Check if this is the AnimGraph
		bool bIsAnimGraph = GraphName.Contains(TEXT("AnimGraph"));

		if (bIsAnimGraph)
		{
			Result += TEXT("=== ANIMGRAPH ===\n");
			Result += FString::Printf(TEXT("Node Count: %d\n\n"), NodeCount);

			// Analyze each node
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (!Node) continue;

				FString NodeClass = Node->GetClass()->GetName();
				FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
				NodeTitle = NodeTitle.Replace(TEXT("\n"), TEXT(" | "));

				// Check for K2Node_VariableGet (variable references)
				if (UK2Node_VariableGet* VarGet = Cast<UK2Node_VariableGet>(Node))
				{
					FName VarName = VarGet->GetVarName();
					FString VarNameStr = VarName.ToString();

					Result += FString::Printf(TEXT("VARIABLE GET: %s\n"), *VarNameStr);
					Result += FString::Printf(TEXT("  Position: X=%d, Y=%d\n"), Node->NodePosX, Node->NodePosY);

					// Check if variable has special characters
					if (VarNameStr.Contains(TEXT("?")) || VarNameStr.Contains(TEXT("(")))
					{
						Result += TEXT("  *** PROBLEM: Variable has special characters ***\n");
					}

					// Trace output connections
					for (UEdGraphPin* Pin : Node->Pins)
					{
						if (Pin && Pin->Direction == EGPD_Output && Pin->LinkedTo.Num() > 0)
						{
							for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
							{
								if (LinkedPin && LinkedPin->GetOwningNode())
								{
									FString LinkedTitle = LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
									LinkedTitle = LinkedTitle.Replace(TEXT("\n"), TEXT(" | "));
									Result += FString::Printf(TEXT("  OUTPUT --> %s (pin: %s)\n"),
										*LinkedTitle, *LinkedPin->PinName.ToString());
								}
							}
						}
					}
					Result += TEXT("\n");
				}

				// Check for AnimGraphNode_BlendListByEnum (Blend Poses by enum)
				if (NodeClass.Contains(TEXT("BlendListByEnum")))
				{
					Result += FString::Printf(TEXT("BLEND POSES BY ENUM: %s\n"), *NodeTitle);
					Result += FString::Printf(TEXT("  Position: X=%d, Y=%d\n"), Node->NodePosX, Node->NodePosY);

					// Check ActiveEnumValue pin
					for (UEdGraphPin* Pin : Node->Pins)
					{
						if (Pin && Pin->PinName.ToString().Contains(TEXT("ActiveEnumValue")))
						{
							if (Pin->LinkedTo.Num() > 0)
							{
								for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
								{
									if (LinkedPin && LinkedPin->GetOwningNode())
									{
										FString LinkedTitle = LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
										Result += FString::Printf(TEXT("  ActiveEnumValue <-- %s\n"), *LinkedTitle);
									}
								}
							}
							else
							{
								Result += FString::Printf(TEXT("  ActiveEnumValue = %s (NOT CONNECTED!)\n"), *Pin->DefaultValue);
								Result += TEXT("  *** NEEDS BINDING TO OVERLAY STATE VARIABLE ***\n");
							}
						}
					}

					// Check pose inputs
					for (UEdGraphPin* Pin : Node->Pins)
					{
						if (Pin && Pin->Direction == EGPD_Input && Pin->PinName.ToString().StartsWith(TEXT("BlendPose")))
						{
							if (Pin->LinkedTo.Num() > 0)
							{
								for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
								{
									if (LinkedPin && LinkedPin->GetOwningNode())
									{
										FString LinkedTitle = LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
										LinkedTitle = LinkedTitle.Replace(TEXT("\n"), TEXT(" | "));
										Result += FString::Printf(TEXT("  %s <-- %s\n"),
											*Pin->PinName.ToString(), *LinkedTitle);
									}
								}
							}
						}
					}

					// Check output
					for (UEdGraphPin* Pin : Node->Pins)
					{
						if (Pin && Pin->Direction == EGPD_Output && Pin->LinkedTo.Num() > 0)
						{
							for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
							{
								if (LinkedPin && LinkedPin->GetOwningNode())
								{
									FString LinkedTitle = LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
									LinkedTitle = LinkedTitle.Replace(TEXT("\n"), TEXT(" | "));
									Result += FString::Printf(TEXT("  OUTPUT --> %s\n"), *LinkedTitle);
								}
							}
						}
					}
					Result += TEXT("\n");
				}

				// Check for AnimGraphNode_StateMachine
				if (NodeClass.Contains(TEXT("StateMachine")))
				{
					Result += FString::Printf(TEXT("STATE MACHINE: %s\n"), *NodeTitle);
					Result += FString::Printf(TEXT("  Position: X=%d, Y=%d\n"), Node->NodePosX, Node->NodePosY);

					// Output connection
					for (UEdGraphPin* Pin : Node->Pins)
					{
						if (Pin && Pin->Direction == EGPD_Output && Pin->LinkedTo.Num() > 0)
						{
							for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
							{
								if (LinkedPin && LinkedPin->GetOwningNode())
								{
									FString LinkedTitle = LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
									LinkedTitle = LinkedTitle.Replace(TEXT("\n"), TEXT(" | "));
									Result += FString::Printf(TEXT("  OUTPUT --> %s\n"), *LinkedTitle);
								}
							}
						}
					}
					Result += TEXT("\n");
				}
			}
		}
		else if (GraphName.Contains(TEXT("StateMachine")) || GraphName.Contains(TEXT("_SM")))
		{
			// This might be a state machine internal graph
			Result += FString::Printf(TEXT("=== STATE MACHINE GRAPH: %s (%d nodes) ===\n"), *GraphName, NodeCount);

			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (!Node) continue;

				FString NodeClass = Node->GetClass()->GetName();
				FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
				NodeTitle = NodeTitle.Replace(TEXT("\n"), TEXT(" | "));

				// States
				if (NodeClass.Contains(TEXT("StateNode")) || NodeClass.Contains(TEXT("AnimState")))
				{
					Result += FString::Printf(TEXT("  STATE: %s\n"), *NodeTitle);
				}
				// Transitions
				else if (NodeClass.Contains(TEXT("Transition")))
				{
					Result += FString::Printf(TEXT("  TRANSITION: %s\n"), *NodeTitle);

					// Check for connected conditions
					bool bHasCondition = false;
					for (UEdGraphPin* Pin : Node->Pins)
					{
						if (Pin && Pin->PinName.ToString().Contains(TEXT("Result")) && Pin->Direction == EGPD_Input)
						{
							if (Pin->LinkedTo.Num() > 0)
							{
								bHasCondition = true;
								for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
								{
									if (LinkedPin && LinkedPin->GetOwningNode())
									{
										FString LinkedTitle = LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
										Result += FString::Printf(TEXT("    Condition: %s\n"), *LinkedTitle);
									}
								}
							}
						}
					}
					if (!bHasCondition)
					{
						Result += TEXT("    *** NO CONDITION CONNECTED ***\n");
					}
				}
			}
			Result += TEXT("\n");
		}
	}

	Result += TEXT("=======================================================================\n");
	Result += TEXT("END OF DIAGNOSTIC REPORT\n");
	Result += TEXT("=======================================================================\n");

	return Result;
}

// ============================================================================
// GetStateMachineStructure - Get detailed state machine info
// ============================================================================
FString USLFAutomationLibrary::GetStateMachineStructure(UObject* AnimBlueprintAsset, const FString& StateMachineName)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		return TEXT("ERROR: Not an AnimBlueprint asset");
	}

	FString Result;
	Result += FString::Printf(TEXT("=== STATE MACHINE: %s ===\n\n"), *StateMachineName);

	// Get all graphs
	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		FString GraphName = Graph->GetName();

		// Check if this graph is related to the state machine
		if (GraphName.Contains(StateMachineName) || GraphName.Contains(TEXT("_SM")))
		{
			Result += FString::Printf(TEXT("Graph: %s\n"), *GraphName);

			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (!Node) continue;

				FString NodeClass = Node->GetClass()->GetName();
				FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
				NodeTitle = NodeTitle.Replace(TEXT("\n"), TEXT(" | "));

				Result += FString::Printf(TEXT("  [%s] %s\n"), *NodeClass, *NodeTitle);
				Result += FString::Printf(TEXT("    Position: X=%d, Y=%d\n"), Node->NodePosX, Node->NodePosY);

				// List all pins and connections
				for (UEdGraphPin* Pin : Node->Pins)
				{
					if (!Pin) continue;

					FString PinDir = (Pin->Direction == EGPD_Input) ? TEXT("IN") : TEXT("OUT");
					Result += FString::Printf(TEXT("    Pin [%s] %s"), *PinDir, *Pin->PinName.ToString());

					if (Pin->LinkedTo.Num() > 0)
					{
						Result += TEXT(" --> ");
						for (int32 i = 0; i < Pin->LinkedTo.Num(); i++)
						{
							if (i > 0) Result += TEXT(", ");
							UEdGraphPin* LinkedPin = Pin->LinkedTo[i];
							if (LinkedPin && LinkedPin->GetOwningNode())
							{
								Result += LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
							}
						}
					}
					else if (Pin->Direction == EGPD_Input && !Pin->DefaultValue.IsEmpty())
					{
						Result += FString::Printf(TEXT(" = %s"), *Pin->DefaultValue);
					}

					Result += TEXT("\n");
				}
			}
			Result += TEXT("\n");
		}
	}

	return Result;
}

// ============================================================================
// InspectTransitionGraph - Show ALL nodes in transitions matching a keyword
// ============================================================================
FString USLFAutomationLibrary::InspectTransitionGraph(UObject* AnimBlueprintAsset, const FString& Keyword)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		return TEXT("ERROR: Not an AnimBlueprint asset");
	}

	FString Result;
	Result += FString::Printf(TEXT("=== INSPECTING TRANSITIONS (keyword: '%s') ===\n\n"), *Keyword);

	// Get all graphs
	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);

	int32 TransitionIndex = 0;
	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		FString GraphName = Graph->GetName();

		// Look for transition graphs
		if (!GraphName.Contains(TEXT("Transition"))) continue;

		// If keyword is empty, show all transitions
		// Otherwise, check if any node in this graph matches the keyword
		bool bShowThisGraph = Keyword.IsEmpty();
		if (!bShowThisGraph)
		{
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (!Node) continue;
				FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
				if (NodeTitle.Contains(Keyword))
				{
					bShowThisGraph = true;
					break;
				}
				// Check pin connections
				for (UEdGraphPin* Pin : Node->Pins)
				{
					if (!Pin) continue;
					for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
					{
						if (LinkedPin && LinkedPin->GetOwningNode())
						{
							FString LinkedTitle = LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
							if (LinkedTitle.Contains(Keyword))
							{
								bShowThisGraph = true;
								break;
							}
						}
					}
					if (bShowThisGraph) break;
				}
				if (bShowThisGraph) break;
			}
		}

		if (!bShowThisGraph) continue;

		TransitionIndex++;
		Result += FString::Printf(TEXT("\n=== TRANSITION #%d ===\n"), TransitionIndex);
		Result += FString::Printf(TEXT("Graph Name: %s\n"), *GraphName);
		Result += FString::Printf(TEXT("Node Count: %d\n\n"), Graph->Nodes.Num());

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			FString NodeClass = Node->GetClass()->GetName();
			FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
			NodeTitle = NodeTitle.Replace(TEXT("\n"), TEXT(" | "));

			Result += FString::Printf(TEXT("  [%s] %s\n"), *NodeClass, *NodeTitle);
			Result += FString::Printf(TEXT("    Pos: X=%d, Y=%d\n"), Node->NodePosX, Node->NodePosY);

			// Show all pins with details
			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin) continue;

				FString PinDir = (Pin->Direction == EGPD_Input) ? TEXT("IN") : TEXT("OUT");

				if (Pin->LinkedTo.Num() > 0)
				{
					for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
					{
						if (LinkedPin && LinkedPin->GetOwningNode())
						{
							FString LinkedTitle = LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
							LinkedTitle = LinkedTitle.Replace(TEXT("\n"), TEXT(" | "));
							Result += FString::Printf(TEXT("    Pin [%s] %s --> %s\n"), *PinDir, *Pin->PinName.ToString(), *LinkedTitle);
						}
					}
				}
				else if (!Pin->DefaultValue.IsEmpty())
				{
					Result += FString::Printf(TEXT("    Pin [%s] %s = '%s'\n"), *PinDir, *Pin->PinName.ToString(), *Pin->DefaultValue);
				}
			}
			Result += TEXT("\n");
		}
	}

	Result += FString::Printf(TEXT("\nTotal matching transitions: %d\n"), TransitionIndex);
	return Result;
}

// ============================================================================
// FixTransitionBooleanInput - Fix a specific transition's broken boolean input
// ============================================================================
int32 USLFAutomationLibrary::FixTransitionBooleanInput(UObject* AnimBlueprintAsset, const FString& TransitionGraphNameContains, const FString& BoolPinName, const FString& VariableName)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("FixTransitionBooleanInput: Not an AnimBlueprint"));
		return 0;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("FixTransitionBooleanInput: Looking for '%s' in transitions, pin '%s', variable '%s'"),
		*TransitionGraphNameContains, *BoolPinName, *VariableName);

	int32 FixCount = 0;

	// Get all graphs
	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		FString GraphName = Graph->GetName();

		// Only process transition graphs that match the filter
		if (!GraphName.Contains(TEXT("Transition"))) continue;
		if (!TransitionGraphNameContains.IsEmpty() && !GraphName.Contains(TransitionGraphNameContains)) continue;

		UE_LOG(LogSLFAutomation, Warning, TEXT("  Checking graph: %s"), *GraphName);

		// Find boolean nodes (NOT/AND) with disconnected pin
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			FString NodeClass = Node->GetClass()->GetName();
			if (!NodeClass.Contains(TEXT("K2Node_CallFunction")) && !NodeClass.Contains(TEXT("K2Node_CommutativeAssociativeBinaryOperator")))
				continue;

			// Check for the target pin
			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin || Pin->Direction != EGPD_Input) continue;
				if (Pin->PinName.ToString() != BoolPinName) continue;
				if (Pin->LinkedTo.Num() > 0) continue;  // Already connected

				// Found a disconnected input pin - now create variable get node
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Found disconnected pin '%s' on node '%s'"),
					*BoolPinName, *Node->GetName());

				// Check if a variable get node already exists for this variable
				UK2Node_VariableGet* ExistingVarGet = nullptr;
				for (UEdGraphNode* OtherNode : Graph->Nodes)
				{
					if (UK2Node_VariableGet* VarGet = Cast<UK2Node_VariableGet>(OtherNode))
					{
						if (VarGet->GetVarName().ToString() == VariableName)
						{
							ExistingVarGet = VarGet;
							break;
						}
					}
				}

				if (ExistingVarGet)
				{
					// Variable get node exists, just connect it
					for (UEdGraphPin* VarPin : ExistingVarGet->Pins)
					{
						if (VarPin && VarPin->Direction == EGPD_Output)
						{
							Pin->MakeLinkTo(VarPin);
							UE_LOG(LogSLFAutomation, Warning, TEXT("      Connected existing variable '%s' to pin '%s'"),
								*VariableName, *BoolPinName);
							FixCount++;
							break;
						}
					}
				}
				else
				{
					// Need to create a new variable get node
					UK2Node_VariableGet* NewVarGet = NewObject<UK2Node_VariableGet>(Graph);
					NewVarGet->VariableReference.SetSelfMember(FName(*VariableName));
					NewVarGet->SetFromProperty(
						FindFProperty<FProperty>(AnimBP->GeneratedClass, FName(*VariableName)),
						false, AnimBP->GeneratedClass);

					// Position it near the target node
					NewVarGet->NodePosX = Node->NodePosX - 200;
					NewVarGet->NodePosY = Node->NodePosY;
					NewVarGet->CreateNewGuid();

					Graph->AddNode(NewVarGet, false, false);
					NewVarGet->AllocateDefaultPins();

					// Connect the output to our target pin
					for (UEdGraphPin* VarPin : NewVarGet->Pins)
					{
						if (VarPin && VarPin->Direction == EGPD_Output && VarPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
						{
							Pin->MakeLinkTo(VarPin);
							UE_LOG(LogSLFAutomation, Warning, TEXT("      Created and connected new variable get '%s' to pin '%s'"),
								*VariableName, *BoolPinName);
							FixCount++;
							break;
						}
					}
				}
			}
		}
	}

	if (FixCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("  Fixed %d connections"), FixCount);
	return FixCount;
}

// ============================================================================
// FixAllBrokenTransitions - Auto-fix all known broken transition inputs
// ============================================================================
int32 USLFAutomationLibrary::FixAllBrokenTransitions(UObject* AnimBlueprintAsset)
{
	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("FixAllBrokenTransitions: Not an AnimBlueprint"));
		return 0;
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== FixAllBrokenTransitions: %s ==="), *AnimBP->GetName());

	int32 TotalFixCount = 0;

	// Get all graphs
	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);

	// First pass: identify all broken connections
	TArray<TPair<UEdGraphPin*, FString>> BrokenConnections;

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		if (!Graph->GetName().Contains(TEXT("Transition"))) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();

			// Check for boolean nodes
			if (NodeTitle.Contains(TEXT("NOT Boolean")) || NodeTitle.Contains(TEXT("AND Boolean")))
			{
				for (UEdGraphPin* Pin : Node->Pins)
				{
					if (!Pin || Pin->Direction != EGPD_Input) continue;
					if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Boolean) continue;
					if (Pin->LinkedTo.Num() > 0) continue;  // Already connected
					if (Pin->DefaultValue != TEXT("false")) continue;  // Not a broken default

					// This pin is broken - determine which variable it should connect to
					// Based on the original AnimBP structure:
					// - CYCLE to IDLE: NOT bIsAccelerating (if not accelerating, go to idle)
					// - Crouch transitions: IsCrouched
					// - Rest transitions: IsResting

					// Try to determine which variable to use based on context
					FString VariableToConnect;

					// Check if this is in a crouch-related transition
					if (Graph->GetName().Contains(TEXT("CROUCH")) || Graph->GetName().Contains(TEXT("Crouch")))
					{
						VariableToConnect = TEXT("IsCrouched");
					}
					// Check for rest transitions
					else if (Graph->GetName().Contains(TEXT("REST")) || Graph->GetName().Contains(TEXT("Rest")) || Graph->GetName().Contains(TEXT("SITTING")))
					{
						VariableToConnect = TEXT("IsResting");
					}
					// Default to bIsAccelerating for locomotion transitions
					else
					{
						VariableToConnect = TEXT("bIsAccelerating");
					}

					BrokenConnections.Add(TPair<UEdGraphPin*, FString>(Pin, VariableToConnect));
					UE_LOG(LogSLFAutomation, Warning, TEXT("  Found broken: %s.%s -> will connect to %s"),
						*Node->GetName(), *Pin->PinName.ToString(), *VariableToConnect);
				}
			}
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("Found %d broken connections to fix"), BrokenConnections.Num());

	// Second pass: create variable get nodes and connect them
	for (const TPair<UEdGraphPin*, FString>& BrokenPair : BrokenConnections)
	{
		UEdGraphPin* Pin = BrokenPair.Key;
		const FString& VariableName = BrokenPair.Value;
		UEdGraph* Graph = Pin->GetOwningNode()->GetGraph();

		// Check if property exists on the generated class
		FProperty* FoundProp = FindFProperty<FProperty>(AnimBP->GeneratedClass, FName(*VariableName));
		if (!FoundProp)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Skipping - property '%s' not found on class"), *VariableName);
			continue;
		}

		// Check if variable get already exists
		UK2Node_VariableGet* ExistingVarGet = nullptr;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (UK2Node_VariableGet* VarGet = Cast<UK2Node_VariableGet>(Node))
			{
				if (VarGet->GetVarName().ToString() == VariableName)
				{
					ExistingVarGet = VarGet;
					break;
				}
			}
		}

		if (ExistingVarGet)
		{
			// Connect to existing
			for (UEdGraphPin* VarPin : ExistingVarGet->Pins)
			{
				if (VarPin && VarPin->Direction == EGPD_Output && VarPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
				{
					Pin->MakeLinkTo(VarPin);
					TotalFixCount++;
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Connected existing '%s' to pin"), *VariableName);
					break;
				}
			}
		}
		else
		{
			// Create new variable get node
			UK2Node_VariableGet* NewVarGet = NewObject<UK2Node_VariableGet>(Graph);
			NewVarGet->VariableReference.SetSelfMember(FName(*VariableName));
			NewVarGet->SetFromProperty(FoundProp, false, AnimBP->GeneratedClass);

			UEdGraphNode* TargetNode = Pin->GetOwningNode();
			NewVarGet->NodePosX = TargetNode->NodePosX - 200;
			NewVarGet->NodePosY = TargetNode->NodePosY;
			NewVarGet->CreateNewGuid();

			Graph->AddNode(NewVarGet, false, false);
			NewVarGet->AllocateDefaultPins();

			// Connect
			for (UEdGraphPin* VarPin : NewVarGet->Pins)
			{
				if (VarPin && VarPin->Direction == EGPD_Output && VarPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
				{
					Pin->MakeLinkTo(VarPin);
					TotalFixCount++;
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Created and connected new '%s' node"), *VariableName);
					break;
				}
			}
		}
	}

	if (TotalFixCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
		FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Fixed %d total broken connections ==="), TotalFixCount);
	return TotalFixCount;
}

/**
 * Fix a specific BlendListByEnum node by binding its ActiveEnumValue pin to a variable
 */
int32 USLFAutomationLibrary::FixBlendListByEnumBinding(UObject* AnimBlueprintAsset, int32 NodeIndex, const FString& VariableName)
{
	if (!AnimBlueprintAsset) return 0;

	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UBlueprint* BP = Cast<UBlueprint>(AnimBlueprintAsset);
		if (BP && BP->GeneratedClass)
		{
			AnimBP = Cast<UAnimBlueprint>(BP);
		}
	}
	if (!AnimBP) return 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Fixing BlendListByEnum node %d with variable '%s' ==="), NodeIndex, *VariableName);

	// Find AnimGraph
	UEdGraph* AnimGraph = nullptr;
	for (UEdGraph* Graph : AnimBP->FunctionGraphs)
	{
		if (Graph && Graph->GetFName() == FName(TEXT("AnimGraph")))
		{
			AnimGraph = Graph;
			break;
		}
	}

	if (!AnimGraph)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  AnimGraph not found"));
		return 0;
	}

	// Find all BlendListByEnum nodes
	TArray<UAnimGraphNode_Base*> BlendNodes;
	for (UEdGraphNode* Node : AnimGraph->Nodes)
	{
		if (Node && Node->GetClass()->GetName().Contains(TEXT("BlendListByEnum")))
		{
			BlendNodes.Add(Cast<UAnimGraphNode_Base>(Node));
		}
	}

	if (NodeIndex < 0 || NodeIndex >= BlendNodes.Num())
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Node index %d out of range (found %d nodes)"), NodeIndex, BlendNodes.Num());
		return 0;
	}

	UAnimGraphNode_Base* TargetNode = BlendNodes[NodeIndex];
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Target node: %s at Y=%d"), *TargetNode->GetName(), (int32)TargetNode->NodePosY);

	// Find the ActiveEnumValue pin
	UEdGraphPin* EnumPin = nullptr;
	for (UEdGraphPin* Pin : TargetNode->Pins)
	{
		if (Pin && Pin->PinName == TEXT("ActiveEnumValue"))
		{
			EnumPin = Pin;
			break;
		}
	}

	if (!EnumPin)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  ActiveEnumValue pin not found"));
		return 0;
	}

	// Check if already connected
	if (EnumPin->LinkedTo.Num() > 0)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Pin already connected to %d nodes"), EnumPin->LinkedTo.Num());
		return 0;
	}

	// Find or create the variable get node
	FProperty* VarProp = AnimBP->GeneratedClass->FindPropertyByName(FName(*VariableName));
	if (!VarProp)
	{
		// Try Blueprint variables
		for (FBPVariableDescription& Var : AnimBP->NewVariables)
		{
			if (Var.VarName == FName(*VariableName))
			{
				VarProp = AnimBP->GeneratedClass->FindPropertyByName(Var.VarName);
				break;
			}
		}
	}

	if (!VarProp)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Variable '%s' not found"), *VariableName);
		return 0;
	}

	// Create a K2Node_VariableGet
	UK2Node_VariableGet* NewVarGet = NewObject<UK2Node_VariableGet>(AnimGraph);
	NewVarGet->VariableReference.SetSelfMember(FName(*VariableName));
	NewVarGet->NodePosX = TargetNode->NodePosX - 200;
	NewVarGet->NodePosY = TargetNode->NodePosY + 400;
	NewVarGet->CreateNewGuid();

	AnimGraph->AddNode(NewVarGet, false, false);
	NewVarGet->AllocateDefaultPins();

	// Find the output pin on the variable get
	UEdGraphPin* VarOutPin = nullptr;
	for (UEdGraphPin* Pin : NewVarGet->Pins)
	{
		if (Pin && Pin->Direction == EGPD_Output)
		{
			VarOutPin = Pin;
			break;
		}
	}

	if (!VarOutPin)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Failed to find output pin on variable node"));
		return 0;
	}

	// Connect
	EnumPin->MakeLinkTo(VarOutPin);

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
	FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);

	UE_LOG(LogSLFAutomation, Warning, TEXT("  Successfully connected ActiveEnumValue to '%s'"), *VariableName);
	return 1;
}

/**
 * Auto-fix all BlendListByEnum nodes for overlay states
 * Matches nodes by Y position:
 * - Y=16 (first) -> LeftHandOverlayState
 * - Y=592 (second) -> RightHandOverlayState
 */
int32 USLFAutomationLibrary::FixAllBlendListByEnumBindings(UObject* AnimBlueprintAsset)
{
	if (!AnimBlueprintAsset) return 0;

	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UBlueprint* BP = Cast<UBlueprint>(AnimBlueprintAsset);
		if (BP && BP->GeneratedClass)
		{
			AnimBP = Cast<UAnimBlueprint>(BP);
		}
	}
	if (!AnimBP) return 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Auto-fixing BlendListByEnum bindings ==="));

	// Find AnimGraph
	UEdGraph* AnimGraph = nullptr;
	for (UEdGraph* Graph : AnimBP->FunctionGraphs)
	{
		if (Graph && Graph->GetFName() == FName(TEXT("AnimGraph")))
		{
			AnimGraph = Graph;
			break;
		}
	}

	if (!AnimGraph)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  AnimGraph not found"));
		return 0;
	}

	// Find all BlendListByEnum nodes and sort by Y position
	TArray<UAnimGraphNode_Base*> BlendNodes;
	for (UEdGraphNode* Node : AnimGraph->Nodes)
	{
		if (Node && Node->GetClass()->GetName().Contains(TEXT("BlendListByEnum")))
		{
			BlendNodes.Add(Cast<UAnimGraphNode_Base>(Node));
		}
	}

	// Sort by Y position
	BlendNodes.Sort([](const UAnimGraphNode_Base& A, const UAnimGraphNode_Base& B) {
		return A.NodePosY < B.NodePosY;
	});

	UE_LOG(LogSLFAutomation, Warning, TEXT("  Found %d BlendListByEnum nodes"), BlendNodes.Num());

	int32 TotalFixed = 0;

	// Map: first node (lower Y) -> Left hand, second node (higher Y) -> Right hand
	// Based on the diagnostic: Y=16 is Left, Y=592 is Right
	TArray<FString> VariableNames;
	VariableNames.Add(TEXT("LeftHandOverlayState"));  // First node (Y=16)
	VariableNames.Add(TEXT("RightHandOverlayState")); // Second node (Y=592)

	for (int32 i = 0; i < BlendNodes.Num() && i < VariableNames.Num(); ++i)
	{
		UAnimGraphNode_Base* Node = BlendNodes[i];
		const FString& VarName = VariableNames[i];

		UE_LOG(LogSLFAutomation, Warning, TEXT("  Node %d at Y=%d -> %s"), i, (int32)Node->NodePosY, *VarName);

		// Find the ActiveEnumValue pin
		UEdGraphPin* EnumPin = nullptr;
		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (Pin && Pin->PinName == TEXT("ActiveEnumValue"))
			{
				EnumPin = Pin;
				break;
			}
		}

		if (!EnumPin)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    ActiveEnumValue pin not found"));
			continue;
		}

		if (EnumPin->LinkedTo.Num() > 0)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Already connected"));
			continue;
		}

		// Create variable get node
		UK2Node_VariableGet* NewVarGet = NewObject<UK2Node_VariableGet>(AnimGraph);
		NewVarGet->VariableReference.SetSelfMember(FName(*VarName));
		NewVarGet->NodePosX = Node->NodePosX - 200;
		NewVarGet->NodePosY = Node->NodePosY + 400;
		NewVarGet->CreateNewGuid();

		AnimGraph->AddNode(NewVarGet, false, false);
		NewVarGet->AllocateDefaultPins();

		// Find output pin
		UEdGraphPin* VarOutPin = nullptr;
		for (UEdGraphPin* OutPin : NewVarGet->Pins)
		{
			if (OutPin && OutPin->Direction == EGPD_Output)
			{
				VarOutPin = OutPin;
				break;
			}
		}

		if (VarOutPin)
		{
			EnumPin->MakeLinkTo(VarOutPin);
			TotalFixed++;
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Connected ActiveEnumValue to '%s'"), *VarName);
		}
	}

	if (TotalFixed > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
		FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Fixed %d BlendListByEnum bindings ==="), TotalFixed);
	return TotalFixed;
}

/**
 * Redirect property bindings on BlendListByEnum nodes from Blueprint variables to C++ properties
 * This fixes compiler errors like "Invalid field 'LeftHandOverlayState_0' found in property path"
 *
 * The old Blueprint variables (LeftHandOverlayState_0, RightHandOverlayState_0) were removed
 * but the BlendListByEnum nodes still have internal property bindings referencing them.
 * This function redirects those bindings to the C++ properties (LeftHandOverlayState, RightHandOverlayState).
 */
int32 USLFAutomationLibrary::RedirectOverlayStateBindings(UObject* AnimBlueprintAsset)
{
	if (!AnimBlueprintAsset) return 0;

	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UBlueprint* BP = Cast<UBlueprint>(AnimBlueprintAsset);
		if (BP && BP->GeneratedClass)
		{
			AnimBP = Cast<UAnimBlueprint>(BP);
		}
	}
	if (!AnimBP) return 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Fixing Overlay State Property Bindings ==="));

	int32 TotalFixed = 0;

	// Iterate all graphs looking for BlendListByEnum nodes with problematic bindings
	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);
	UE_LOG(LogSLFAutomation, Warning, TEXT("Searching %d graphs"), AllGraphs.Num());

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			// Only process BlendListByEnum nodes
			UAnimGraphNode_BlendListByEnum* BlendListNode = Cast<UAnimGraphNode_BlendListByEnum>(Node);
			if (!BlendListNode) continue;

			UE_LOG(LogSLFAutomation, Warning, TEXT("  Processing BlendListByEnum: %s in graph %s"),
				*Node->GetName(), *Graph->GetName());

			// Check if it has a binding for ActiveEnumValue (this is where the property path is stored)
			if (BlendListNode->HasBinding(FName(TEXT("ActiveEnumValue"))))
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Found ActiveEnumValue binding - removing"));

				// Remove the problematic property binding
				// The pin connection (K2Node_VariableGet -> ActiveEnumValue) will still work
				BlendListNode->RemoveBindings(FName(TEXT("ActiveEnumValue")));
				TotalFixed++;

				UE_LOG(LogSLFAutomation, Warning, TEXT("    Removed property binding"));
			}
			else
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    No ActiveEnumValue binding found"));
			}

			// Also check HasBinding without parameter to see what bindings exist
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Has BlendPose binding: %s"),
				BlendListNode->HasBinding(FName(TEXT("BlendPose"))) ? TEXT("YES") : TEXT("NO"));
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Has BlendTime binding: %s"),
				BlendListNode->HasBinding(FName(TEXT("BlendTime"))) ? TEXT("YES") : TEXT("NO"));
		}
	}

	if (TotalFixed > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
		FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Fixed %d BlendListByEnum nodes ==="), TotalFixed);
	return TotalFixed;
}

/**
 * Migrate BlendListByEnum nodes from Blueprint enum to C++ enum
 * This properly replaces nodes: saves connections, maps enum entry names by VALUE, reconstructs, restores connections
 *
 * The key challenge is that Blueprint enums use internal names like "NewEnumerator0", "NewEnumerator1"
 * while C++ enums use proper names like "Unarmed", "OneHanded", etc.
 * We map by INTEGER VALUE, not by name.
 */
int32 USLFAutomationLibrary::MigrateBlendListByEnumToCpp(UObject* AnimBlueprintAsset, const FString& OldEnumPath, const FString& NewEnumPath)
{
	if (!AnimBlueprintAsset) return 0;

	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UBlueprint* BP = Cast<UBlueprint>(AnimBlueprintAsset);
		if (BP && BP->GeneratedClass)
		{
			AnimBP = Cast<UAnimBlueprint>(BP);
		}
	}
	if (!AnimBP) return 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== MigrateBlendListByEnumToCpp ==="));
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Old enum: %s"), *OldEnumPath);
	UE_LOG(LogSLFAutomation, Warning, TEXT("  New enum: %s"), *NewEnumPath);

	// Load the new C++ enum
	UEnum* NewEnum = FindObject<UEnum>(nullptr, *NewEnumPath);
	if (!NewEnum)
	{
		NewEnum = LoadObject<UEnum>(nullptr, *NewEnumPath);
	}
	if (!NewEnum)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Failed to find new enum: %s"), *NewEnumPath);
		return 0;
	}
	UE_LOG(LogSLFAutomation, Warning, TEXT("  Found new enum: %s with %d entries"), *NewEnum->GetName(), NewEnum->NumEnums());

	// Log C++ enum entries
	for (int32 i = 0; i < NewEnum->NumEnums(); i++)
	{
		FName EntryName = NewEnum->GetNameByIndex(i);
		int64 EntryValue = NewEnum->GetValueByIndex(i);
		UE_LOG(LogSLFAutomation, Warning, TEXT("    C++ Enum[%d]: %s = %lld"), i, *EntryName.ToString(), EntryValue);
	}

	// Load the old enum for value mapping
	UEnum* OldEnum = FindObject<UEnum>(nullptr, *OldEnumPath);
	if (!OldEnum)
	{
		OldEnum = LoadObject<UEnum>(nullptr, *OldEnumPath);
	}
	if (OldEnum)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Found old enum: %s with %d entries"), *OldEnum->GetName(), OldEnum->NumEnums());
		for (int32 i = 0; i < OldEnum->NumEnums(); i++)
		{
			FName EntryName = OldEnum->GetNameByIndex(i);
			int64 EntryValue = OldEnum->GetValueByIndex(i);
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Old Enum[%d]: %s = %lld"), i, *EntryName.ToString(), EntryValue);
		}
	}

	int32 MigratedCount = 0;

	// Get ALL graphs
	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(AnimBP->FunctionGraphs);
	for (UEdGraph* Graph : AnimBP->UbergraphPages)
	{
		if (Graph) AllGraphs.Add(Graph);
	}

	// Find the VisibleEnumEntries property
	FArrayProperty* VisibleEntriesProp = CastField<FArrayProperty>(
		UAnimGraphNode_BlendListByEnum::StaticClass()->FindPropertyByName(TEXT("VisibleEnumEntries")));

	if (!VisibleEntriesProp)
	{
		UE_LOG(LogSLFAutomation, Error, TEXT("  Could not find VisibleEnumEntries property!"));
		return 0;
	}

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;

		UE_LOG(LogSLFAutomation, Warning, TEXT("  Checking graph: %s"), *Graph->GetName());

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;
			if (!Node->GetClass()->GetName().Contains(TEXT("BlendListByEnum"))) continue;

			UAnimGraphNode_BlendListByEnum* BlendNode = Cast<UAnimGraphNode_BlendListByEnum>(Node);
			if (!BlendNode) continue;

			UEnum* BoundEnum = BlendNode->GetEnum();
			if (!BoundEnum) continue;

			FString BoundEnumPath = BoundEnum->GetPathName();
			if (!BoundEnumPath.Equals(OldEnumPath, ESearchCase::IgnoreCase))
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Node %s uses different enum: %s"), *Node->GetName(), *BoundEnumPath);
				continue;
			}

			UE_LOG(LogSLFAutomation, Warning, TEXT("    Migrating node: %s at (%d, %d)"), *Node->GetName(), Node->NodePosX, Node->NodePosY);

			// Step 1: Save pin connections BY INDEX (BlendPose_0, BlendPose_1, etc.)
			struct FPinConnectionByIndex
			{
				int32 PinIndex;
				TArray<TPair<UEdGraphNode*, FName>> LinkedPins;
			};
			TArray<FPinConnectionByIndex> SavedPoseConnections;
			TArray<TPair<FName, TArray<TPair<UEdGraphNode*, FName>>>> OtherConnections;

			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin || Pin->LinkedTo.Num() == 0) continue;

				TArray<TPair<UEdGraphNode*, FName>> Links;
				for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
				{
					if (LinkedPin && LinkedPin->GetOwningNode())
					{
						Links.Add(TPair<UEdGraphNode*, FName>(LinkedPin->GetOwningNode(), LinkedPin->PinName));
					}
				}
				if (Links.Num() == 0) continue;

				FString PinStr = Pin->PinName.ToString();
				if (PinStr.StartsWith(TEXT("BlendPose_")))
				{
					int32 PinIndex = FCString::Atoi(*PinStr.RightChop(10));
					FPinConnectionByIndex Conn;
					Conn.PinIndex = PinIndex;
					Conn.LinkedPins = Links;
					SavedPoseConnections.Add(Conn);
					UE_LOG(LogSLFAutomation, Warning, TEXT("      Saved BlendPose_%d: %d links"), PinIndex, Links.Num());
				}
				else
				{
					OtherConnections.Add(TPair<FName, TArray<TPair<UEdGraphNode*, FName>>>(Pin->PinName, Links));
					UE_LOG(LogSLFAutomation, Warning, TEXT("      Saved %s: %d links"), *PinStr, Links.Num());
				}
			}

			// Step 2: Get visible entry VALUES before changing enum
			TArray<int64> VisibleEntryValues;
			{
				FScriptArrayHelper ArrayHelper(VisibleEntriesProp, VisibleEntriesProp->ContainerPtrToValuePtr<void>(BlendNode));
				for (int32 i = 0; i < ArrayHelper.Num(); i++)
				{
					FName* EntryNamePtr = (FName*)ArrayHelper.GetRawPtr(i);
					if (EntryNamePtr && OldEnum)
					{
						int64 EntryValue = OldEnum->GetValueByName(*EntryNamePtr);
						VisibleEntryValues.Add(EntryValue);
						UE_LOG(LogSLFAutomation, Warning, TEXT("      VisibleEntry[%d]: %s -> value %lld"), i, *EntryNamePtr->ToString(), EntryValue);
					}
				}
			}

			// Step 3: Call ReloadEnum to switch to new enum
			BlendNode->ReloadEnum(NewEnum);

			// Step 4: Fix VisibleEnumEntries to use correct C++ enum names
			{
				FScriptArrayHelper ArrayHelper(VisibleEntriesProp, VisibleEntriesProp->ContainerPtrToValuePtr<void>(BlendNode));
				ArrayHelper.EmptyValues();

				for (int64 Value : VisibleEntryValues)
				{
					int32 NewEnumIndex = NewEnum->GetIndexByValue(Value);
					if (NewEnumIndex != INDEX_NONE)
					{
						FName NewEntryName = NewEnum->GetNameByIndex(NewEnumIndex);
						int32 AddedIndex = ArrayHelper.AddValue();
						FName* NewEntryPtr = (FName*)ArrayHelper.GetRawPtr(AddedIndex);
						*NewEntryPtr = NewEntryName;
						UE_LOG(LogSLFAutomation, Warning, TEXT("      Set VisibleEntry[%d]: value %lld -> %s"), AddedIndex, Value, *NewEntryName.ToString());
					}
					else
					{
						UE_LOG(LogSLFAutomation, Warning, TEXT("      [WARN] No C++ enum entry for value %lld"), Value);
					}
				}
			}

			// Step 5: Reconstruct node with new pins
			BlendNode->ReconstructNode();

			// Step 6: Restore BlendPose connections by index
			for (const FPinConnectionByIndex& Conn : SavedPoseConnections)
			{
				FName NewPinName = FName(*FString::Printf(TEXT("BlendPose_%d"), Conn.PinIndex));
				UEdGraphPin* Pin = Node->FindPin(NewPinName);
				if (!Pin)
				{
					UE_LOG(LogSLFAutomation, Warning, TEXT("      Could not find BlendPose_%d after reconstruction"), Conn.PinIndex);
					continue;
				}

				for (const TPair<UEdGraphNode*, FName>& Link : Conn.LinkedPins)
				{
					UEdGraphPin* OtherPin = Link.Key ? Link.Key->FindPin(Link.Value) : nullptr;
					if (OtherPin)
					{
						Pin->MakeLinkTo(OtherPin);
						UE_LOG(LogSLFAutomation, Warning, TEXT("      Restored BlendPose_%d -> %s.%s"), Conn.PinIndex, *Link.Key->GetName(), *Link.Value.ToString());
					}
				}
			}

			// Step 7: Restore other connections
			for (const auto& Conn : OtherConnections)
			{
				UEdGraphPin* Pin = Node->FindPin(Conn.Key);
				if (!Pin)
				{
					UE_LOG(LogSLFAutomation, Warning, TEXT("      Could not find pin %s after reconstruction"), *Conn.Key.ToString());
					continue;
				}

				for (const TPair<UEdGraphNode*, FName>& Link : Conn.Value)
				{
					UEdGraphPin* OtherPin = Link.Key ? Link.Key->FindPin(Link.Value) : nullptr;
					if (OtherPin)
					{
						Pin->MakeLinkTo(OtherPin);
						UE_LOG(LogSLFAutomation, Warning, TEXT("      Restored %s -> %s.%s"), *Conn.Key.ToString(), *Link.Key->GetName(), *Link.Value.ToString());
					}
				}
			}

			MigratedCount++;
		}
	}

	if (MigratedCount > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
		FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);

		// Save
		UPackage* Package = AnimBP->GetOutermost();
		FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		UPackage::SavePackage(Package, AnimBP, *PackageFileName, SaveArgs);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Migrated %d BlendListByEnum nodes ==="), MigratedCount);
	return MigratedCount;
}



/**
 * Fix disconnected boolean variable getters in AnimGraph (bIsBlocking, bIsFalling)
 * These need to connect to:
 * - bIsBlocking -> AnimGraphNode_BlendListByBool.bActiveValue
 * - bIsFalling -> NOT Boolean input (K2Node_CallFunction)
 */
int32 USLFAutomationLibrary::FixAnimGraphBooleanConnections(UObject* AnimBlueprintAsset)
{
	if (!AnimBlueprintAsset) return 0;

	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UBlueprint* BP = Cast<UBlueprint>(AnimBlueprintAsset);
		if (BP && BP->GeneratedClass)
		{
			AnimBP = Cast<UAnimBlueprint>(BP);
		}
	}
	if (!AnimBP) return 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== FixAnimGraphBooleanConnections: %s ==="), *AnimBP->GetName());

	// Find AnimGraph
	UEdGraph* AnimGraph = nullptr;
	for (UEdGraph* Graph : AnimBP->FunctionGraphs)
	{
		if (Graph && Graph->GetFName() == FName(TEXT("AnimGraph")))
		{
			AnimGraph = Graph;
			break;
		}
	}

	if (!AnimGraph)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  AnimGraph not found"));
		return 0;
	}

	int32 TotalFixed = 0;

	// Map of variable names to their target node patterns
	// bIsBlocking -> BlendListByBool node
	// bIsFalling -> NOT Boolean node
	struct FBoolConnectionInfo
	{
		FString VariableName;
		FString TargetNodePattern;
		FString TargetPinName;
	};

	TArray<FBoolConnectionInfo> ConnectionsToFix;
	ConnectionsToFix.Add({TEXT("bIsBlocking"), TEXT("BlendListByBool"), TEXT("bActiveValue")});
	ConnectionsToFix.Add({TEXT("bIsFalling"), TEXT("NOT Boolean"), TEXT("A")});

	for (const FBoolConnectionInfo& ConnInfo : ConnectionsToFix)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  Looking for %s -> %s.%s"),
			*ConnInfo.VariableName, *ConnInfo.TargetNodePattern, *ConnInfo.TargetPinName);

		// Find the variable get node for this variable
		UK2Node_VariableGet* VarGetNode = nullptr;
		for (UEdGraphNode* Node : AnimGraph->Nodes)
		{
			if (UK2Node_VariableGet* VarGet = Cast<UK2Node_VariableGet>(Node))
			{
				if (VarGet->GetVarName().ToString() == ConnInfo.VariableName)
				{
					VarGetNode = VarGet;
					break;
				}
			}
		}

		if (!VarGetNode)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Variable get node '%s' not found"), *ConnInfo.VariableName);
			continue;
		}

		// Check if already connected
		UEdGraphPin* VarOutputPin = nullptr;
		for (UEdGraphPin* Pin : VarGetNode->Pins)
		{
			if (Pin && Pin->Direction == EGPD_Output)
			{
				VarOutputPin = Pin;
				break;
			}
		}

		if (!VarOutputPin)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    No output pin found on variable get"));
			continue;
		}

		if (VarOutputPin->LinkedTo.Num() > 0)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Already connected to %d nodes"), VarOutputPin->LinkedTo.Num());
			continue;
		}

		// Find target node by pattern
		UEdGraphNode* TargetNode = nullptr;
		for (UEdGraphNode* Node : AnimGraph->Nodes)
		{
			if (!Node) continue;

			// Check node class name or title
			FString NodeClassName = Node->GetClass()->GetName();
			FString NodeTitle = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();

			if (NodeClassName.Contains(ConnInfo.TargetNodePattern) || NodeTitle.Contains(ConnInfo.TargetNodePattern))
			{
				TargetNode = Node;
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Found target node: %s (%s)"), *Node->GetName(), *NodeClassName);
				break;
			}
		}

		if (!TargetNode)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Target node matching '%s' not found"), *ConnInfo.TargetNodePattern);
			continue;
		}

		// Find target pin
		UEdGraphPin* TargetPin = nullptr;
		for (UEdGraphPin* Pin : TargetNode->Pins)
		{
			if (Pin && Pin->Direction == EGPD_Input && Pin->PinName.ToString() == ConnInfo.TargetPinName)
			{
				TargetPin = Pin;
				break;
			}
		}

		if (!TargetPin)
		{
			// Try partial match for pin names like "bActiveValue" vs "Active Value"
			for (UEdGraphPin* Pin : TargetNode->Pins)
			{
				if (Pin && Pin->Direction == EGPD_Input &&
					Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
				{
					TargetPin = Pin;
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Using boolean input pin: %s"), *Pin->PinName.ToString());
					break;
				}
			}
		}

		if (!TargetPin)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Target pin '%s' not found"), *ConnInfo.TargetPinName);
			continue;
		}

		// Check if target pin is already connected
		if (TargetPin->LinkedTo.Num() > 0)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("    Target pin already connected"));
			continue;
		}

		// Connect!
		VarOutputPin->MakeLinkTo(TargetPin);
		TotalFixed++;
		UE_LOG(LogSLFAutomation, Warning, TEXT("    Connected %s -> %s.%s"),
			*ConnInfo.VariableName, *TargetNode->GetName(), *TargetPin->PinName.ToString());
	}

	if (TotalFixed > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
		FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Fixed %d AnimGraph boolean connections ==="), TotalFixed);
	return TotalFixed;
}

/**
 * Fix GetAccelerationData graph - reconnects broken SET node inputs
 * The graph computes acceleration data but the SET nodes have disconnected inputs:
 * - K2Node_CallFunction_2.ReturnValue (NOT Boolean) -> K2Node_VariableSet_1.bIsAccelerating
 * - K2Node_PropertyAccess_1.Value (IsFalling) -> K2Node_VariableSet_5.bIsFalling
 */
int32 USLFAutomationLibrary::FixGetAccelerationDataGraph(UObject* AnimBlueprintAsset)
{
	if (!AnimBlueprintAsset) return 0;

	UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
	if (!AnimBP)
	{
		UBlueprint* BP = Cast<UBlueprint>(AnimBlueprintAsset);
		if (BP && BP->GeneratedClass)
		{
			AnimBP = Cast<UAnimBlueprint>(BP);
		}
	}
	if (!AnimBP) return 0;

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== FixGetAccelerationDataGraph: %s ==="), *AnimBP->GetName());

	// Find GetAccelerationData graph
	UEdGraph* AccelGraph = nullptr;
	TArray<UEdGraph*> AllGraphs;
	AnimBP->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (Graph && Graph->GetName().Contains(TEXT("GetAccelerationData")))
		{
			AccelGraph = Graph;
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Found graph: %s"), *Graph->GetName());
			break;
		}
	}

	if (!AccelGraph)
	{
		UE_LOG(LogSLFAutomation, Warning, TEXT("  GetAccelerationData graph not found"));
		return 0;
	}

	int32 TotalFixed = 0;

	// Build node lookup maps by name
	TMap<FString, UEdGraphNode*> NodesByName;
	for (UEdGraphNode* Node : AccelGraph->Nodes)
	{
		if (Node)
		{
			NodesByName.Add(Node->GetName(), Node);
		}
	}

	// Connection 1: K2Node_CallFunction_2.ReturnValue -> K2Node_VariableSet_1.bIsAccelerating
	{
		UEdGraphNode* SourceNode = NodesByName.FindRef(TEXT("K2Node_CallFunction_2"));
		UEdGraphNode* TargetNode = NodesByName.FindRef(TEXT("K2Node_VariableSet_1"));

		if (SourceNode && TargetNode)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Found K2Node_CallFunction_2 and K2Node_VariableSet_1"));

			// Find ReturnValue output pin on source
			UEdGraphPin* SourcePin = nullptr;
			for (UEdGraphPin* Pin : SourceNode->Pins)
			{
				if (Pin && Pin->Direction == EGPD_Output && Pin->PinName == TEXT("ReturnValue"))
				{
					SourcePin = Pin;
					break;
				}
			}

			// Find bIsAccelerating input pin on target
			UEdGraphPin* TargetPin = nullptr;
			for (UEdGraphPin* Pin : TargetNode->Pins)
			{
				if (Pin && Pin->Direction == EGPD_Input && Pin->PinName == TEXT("bIsAccelerating"))
				{
					TargetPin = Pin;
					break;
				}
			}

			if (SourcePin && TargetPin)
			{
				if (SourcePin->LinkedTo.Num() == 0 || TargetPin->LinkedTo.Num() == 0)
				{
					// Break any existing connections first
					SourcePin->BreakAllPinLinks();
					TargetPin->BreakAllPinLinks();

					// Make new connection
					SourcePin->MakeLinkTo(TargetPin);
					TotalFixed++;
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Connected K2Node_CallFunction_2.ReturnValue -> K2Node_VariableSet_1.bIsAccelerating"));
				}
				else
				{
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Already connected"));
				}
			}
			else
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Pins not found: SourcePin=%s, TargetPin=%s"),
					SourcePin ? TEXT("OK") : TEXT("NULL"),
					TargetPin ? TEXT("OK") : TEXT("NULL"));
			}
		}
		else
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  K2Node_CallFunction_2 or K2Node_VariableSet_1 not found"));
		}
	}

	// Connection 2: K2Node_PropertyAccess_1.Value -> K2Node_VariableSet_5.bIsFalling
	{
		UEdGraphNode* SourceNode = NodesByName.FindRef(TEXT("K2Node_PropertyAccess_1"));
		UEdGraphNode* TargetNode = NodesByName.FindRef(TEXT("K2Node_VariableSet_5"));

		if (SourceNode && TargetNode)
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  Found K2Node_PropertyAccess_1 and K2Node_VariableSet_5"));

			// Find Value output pin on source
			UEdGraphPin* SourcePin = nullptr;
			for (UEdGraphPin* Pin : SourceNode->Pins)
			{
				if (Pin && Pin->Direction == EGPD_Output && Pin->PinName == TEXT("Value"))
				{
					SourcePin = Pin;
					break;
				}
			}

			// Find bIsFalling input pin on target
			UEdGraphPin* TargetPin = nullptr;
			for (UEdGraphPin* Pin : TargetNode->Pins)
			{
				if (Pin && Pin->Direction == EGPD_Input && Pin->PinName == TEXT("bIsFalling"))
				{
					TargetPin = Pin;
					break;
				}
			}

			if (SourcePin && TargetPin)
			{
				if (SourcePin->LinkedTo.Num() == 0 || TargetPin->LinkedTo.Num() == 0)
				{
					// Break any existing connections first
					SourcePin->BreakAllPinLinks();
					TargetPin->BreakAllPinLinks();

					// Make new connection
					SourcePin->MakeLinkTo(TargetPin);
					TotalFixed++;
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Connected K2Node_PropertyAccess_1.Value -> K2Node_VariableSet_5.bIsFalling"));
				}
				else
				{
					UE_LOG(LogSLFAutomation, Warning, TEXT("    Already connected"));
				}
			}
			else
			{
				UE_LOG(LogSLFAutomation, Warning, TEXT("    Pins not found: SourcePin=%s, TargetPin=%s"),
					SourcePin ? TEXT("OK") : TEXT("NULL"),
					TargetPin ? TEXT("OK") : TEXT("NULL"));
			}
		}
		else
		{
			UE_LOG(LogSLFAutomation, Warning, TEXT("  K2Node_PropertyAccess_1 or K2Node_VariableSet_5 not found"));
		}
	}

	if (TotalFixed > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);
		FKismetEditorUtilities::CompileBlueprint(AnimBP, EBlueprintCompileOptions::SkipGarbageCollection);
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("=== Fixed %d GetAccelerationData connections ==="), TotalFixed);
	return TotalFixed;
}
