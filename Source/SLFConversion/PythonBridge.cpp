// PythonBridge.cpp - Comprehensive Blueprint Export and Migration Utilities

#include "PythonBridge.h"

#if WITH_EDITOR
#include "Engine/Blueprint.h"
#include "Engine/UserDefinedEnum.h"
#include "Engine/UserDefinedStruct.h"
#include "Engine/DataAsset.h"
#include "Animation/AnimBlueprint.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node.h"
#include "K2Node_Event.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "K2Node_Composite.h"
#include "K2Node_Tunnel.h"
#include "AnimGraphNode_Base.h"
#include "AnimGraphNode_StateMachine.h"
#include "AnimationStateMachineGraph.h"
#include "AnimStateNode.h"
#include "AnimStateTransitionNode.h"
#include "AnimStateConduitNode.h"
#include "AnimationGraph.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/PropertyIterator.h"

// ============================================================================
// JSON HELPER FUNCTIONS
// ============================================================================

static FString JsonObjectToString(TSharedPtr<FJsonObject> JsonObject)
{
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	return OutputString;
}

static TSharedPtr<FJsonObject> CreateTypeObject(const FString& Category, const FString& SubCategory = TEXT("None"),
	const FString& SubCategoryObject = TEXT(""), const FString& SubCategoryObjectPath = TEXT(""),
	bool bIsArray = false, bool bIsSet = false, bool bIsMap = false,
	bool bIsReference = false, bool bIsConst = false)
{
	TSharedPtr<FJsonObject> TypeObj = MakeShareable(new FJsonObject());
	TypeObj->SetStringField(TEXT("Category"), Category);
	TypeObj->SetStringField(TEXT("SubCategory"), SubCategory);
	if (!SubCategoryObject.IsEmpty())
	{
		TypeObj->SetStringField(TEXT("SubCategoryObject"), SubCategoryObject);
	}
	if (!SubCategoryObjectPath.IsEmpty())
	{
		TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), SubCategoryObjectPath);
	}
	TypeObj->SetBoolField(TEXT("IsArray"), bIsArray);
	TypeObj->SetBoolField(TEXT("IsSet"), bIsSet);
	TypeObj->SetBoolField(TEXT("IsMap"), bIsMap);
	TypeObj->SetBoolField(TEXT("IsReference"), bIsReference);
	TypeObj->SetBoolField(TEXT("IsConst"), bIsConst);
	return TypeObj;
}

// ============================================================================
// PROPERTY TYPE SERIALIZATION - THE CRITICAL PART FOR TMAP KEY TYPES
// ============================================================================

TSharedPtr<FJsonObject> UPythonBridge::ExtractMapKeyValueTypes(FMapProperty* MapProperty)
{
	TSharedPtr<FJsonObject> MapTypeObj = MakeShareable(new FJsonObject());

	if (!MapProperty)
	{
		return MapTypeObj;
	}

	// Get the key property
	FProperty* KeyProp = MapProperty->KeyProp;
	if (KeyProp)
	{
		MapTypeObj->SetObjectField(TEXT("KeyType"), SerializePropertyType(KeyProp));
	}

	// Get the value property
	FProperty* ValueProp = MapProperty->ValueProp;
	if (ValueProp)
	{
		MapTypeObj->SetObjectField(TEXT("ValueType"), SerializePropertyType(ValueProp));
	}

	return MapTypeObj;
}

TSharedPtr<FJsonObject> UPythonBridge::SerializePropertyType(FProperty* Property)
{
	TSharedPtr<FJsonObject> TypeObj = MakeShareable(new FJsonObject());

	if (!Property)
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("unknown"));
		return TypeObj;
	}

	// Handle containers first
	if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
	{
		TSharedPtr<FJsonObject> InnerType = SerializePropertyType(ArrayProp->Inner);
		// Copy inner type fields
		for (auto& Pair : InnerType->Values)
		{
			TypeObj->SetField(Pair.Key, Pair.Value);
		}
		TypeObj->SetBoolField(TEXT("IsArray"), true);
		TypeObj->SetBoolField(TEXT("IsSet"), false);
		TypeObj->SetBoolField(TEXT("IsMap"), false);
		return TypeObj;
	}

	if (FSetProperty* SetProp = CastField<FSetProperty>(Property))
	{
		TSharedPtr<FJsonObject> ElementType = SerializePropertyType(SetProp->ElementProp);
		for (auto& Pair : ElementType->Values)
		{
			TypeObj->SetField(Pair.Key, Pair.Value);
		}
		TypeObj->SetBoolField(TEXT("IsArray"), false);
		TypeObj->SetBoolField(TEXT("IsSet"), true);
		TypeObj->SetBoolField(TEXT("IsMap"), false);
		return TypeObj;
	}

	// *** THE KEY FIX: Properly extract TMap key and value types ***
	if (FMapProperty* MapProp = CastField<FMapProperty>(Property))
	{
		// Get value type as the main type (for backwards compatibility)
		TSharedPtr<FJsonObject> ValueType = SerializePropertyType(MapProp->ValueProp);
		for (auto& Pair : ValueType->Values)
		{
			TypeObj->SetField(Pair.Key, Pair.Value);
		}

		// *** NEW: Add KeyType as a separate field ***
		TSharedPtr<FJsonObject> KeyType = SerializePropertyType(MapProp->KeyProp);
		TypeObj->SetObjectField(TEXT("KeyType"), KeyType);

		TypeObj->SetBoolField(TEXT("IsArray"), false);
		TypeObj->SetBoolField(TEXT("IsSet"), false);
		TypeObj->SetBoolField(TEXT("IsMap"), true);
		return TypeObj;
	}

	// Initialize defaults
	TypeObj->SetBoolField(TEXT("IsArray"), false);
	TypeObj->SetBoolField(TEXT("IsSet"), false);
	TypeObj->SetBoolField(TEXT("IsMap"), false);
	TypeObj->SetBoolField(TEXT("IsReference"), Property->HasAnyPropertyFlags(CPF_ReferenceParm | CPF_OutParm));
	TypeObj->SetBoolField(TEXT("IsConst"), Property->HasAnyPropertyFlags(CPF_ConstParm));

	// Handle specific property types
	if (CastField<FBoolProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("bool"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
	}
	else if (FByteProperty* ByteProp = CastField<FByteProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("byte"));
		if (ByteProp->Enum)
		{
			TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
			TypeObj->SetStringField(TEXT("SubCategoryObject"), ByteProp->Enum->GetName());
			TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), ByteProp->Enum->GetPathName());
		}
		else
		{
			TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		}
	}
	else if (FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("byte"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		if (EnumProp->GetEnum())
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), EnumProp->GetEnum()->GetName());
			TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), EnumProp->GetEnum()->GetPathName());
		}
	}
	else if (CastField<FIntProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("int"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
	}
	else if (CastField<FInt64Property>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("int64"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
	}
	else if (CastField<FFloatProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("real"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("float"));
	}
	else if (CastField<FDoubleProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("real"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("double"));
	}
	else if (CastField<FStrProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("string"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
	}
	else if (CastField<FNameProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("name"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
	}
	else if (CastField<FTextProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("text"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
	}
	else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("struct"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		if (StructProp->Struct)
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), StructProp->Struct->GetName());
			TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), StructProp->Struct->GetPathName());
		}
	}
	else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("object"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		if (ObjProp->PropertyClass)
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), ObjProp->PropertyClass->GetName());
			TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), ObjProp->PropertyClass->GetPathName());
		}
	}
	else if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("softobject"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		if (SoftObjProp->PropertyClass)
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), SoftObjProp->PropertyClass->GetName());
			TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), SoftObjProp->PropertyClass->GetPathName());
		}
	}
	else if (FClassProperty* ClassProp = CastField<FClassProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("class"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		if (ClassProp->MetaClass)
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), ClassProp->MetaClass->GetName());
			TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), ClassProp->MetaClass->GetPathName());
		}
	}
	else if (FSoftClassProperty* SoftClassProp = CastField<FSoftClassProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("softclass"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		if (SoftClassProp->MetaClass)
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), SoftClassProp->MetaClass->GetName());
			TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), SoftClassProp->MetaClass->GetPathName());
		}
	}
	else if (FInterfaceProperty* InterfaceProp = CastField<FInterfaceProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("interface"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		if (InterfaceProp->InterfaceClass)
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), InterfaceProp->InterfaceClass->GetName());
			TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), InterfaceProp->InterfaceClass->GetPathName());
		}
	}
	else if (FDelegateProperty* DelegateProp = CastField<FDelegateProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("delegate"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		if (DelegateProp->SignatureFunction)
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), DelegateProp->SignatureFunction->GetName());
		}
	}
	else if (FMulticastDelegateProperty* MCDelegateProp = CastField<FMulticastDelegateProperty>(Property))
	{
		TypeObj->SetStringField(TEXT("Category"), TEXT("mcdelegate"));
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("None"));
		if (MCDelegateProp->SignatureFunction)
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), MCDelegateProp->SignatureFunction->GetName());
		}
	}
	else
	{
		// Fallback
		TypeObj->SetStringField(TEXT("Category"), Property->GetCPPType());
		TypeObj->SetStringField(TEXT("SubCategory"), TEXT("Unknown"));
	}

	return TypeObj;
}

// ============================================================================
// VARIABLE SERIALIZATION
// ============================================================================

TSharedPtr<FJsonObject> UPythonBridge::SerializeVariable(const FBPVariableDescription& Variable, UBlueprint* Blueprint)
{
	TSharedPtr<FJsonObject> VarObj = MakeShareable(new FJsonObject());

	VarObj->SetStringField(TEXT("Name"), Variable.VarName.ToString());
	VarObj->SetStringField(TEXT("Category"), Variable.Category.ToString());
	VarObj->SetStringField(TEXT("DefaultValue"), Variable.DefaultValue);
	VarObj->SetBoolField(TEXT("IsInstanceEditable"), Variable.PropertyFlags & CPF_Edit);
	VarObj->SetBoolField(TEXT("IsBlueprintReadOnly"), Variable.PropertyFlags & CPF_BlueprintReadOnly);
	VarObj->SetBoolField(TEXT("IsExposeOnSpawn"), Variable.PropertyFlags & CPF_ExposeOnSpawn);
	VarObj->SetBoolField(TEXT("IsPrivate"), Variable.PropertyFlags & CPF_DisableEditOnInstance);
	VarObj->SetBoolField(TEXT("IsReplicated"), Variable.RepNotifyFunc != NAME_None || (Variable.PropertyFlags & CPF_Net));

	// Get the actual property for complete type info (including TMap key types!)
	FProperty* ActualProperty = nullptr;
	if (Blueprint->GeneratedClass)
	{
		ActualProperty = Blueprint->GeneratedClass->FindPropertyByName(Variable.VarName);
	}
	if (!ActualProperty && Blueprint->SkeletonGeneratedClass)
	{
		ActualProperty = Blueprint->SkeletonGeneratedClass->FindPropertyByName(Variable.VarName);
	}

	if (ActualProperty)
	{
		// Use the actual property for full type info (this gets TMap key types!)
		VarObj->SetObjectField(TEXT("Type"), SerializePropertyType(ActualProperty));
	}
	else
	{
		// Fallback to the pin type info from the variable description
		TSharedPtr<FJsonObject> TypeObj = MakeShareable(new FJsonObject());

		FString Category = Variable.VarType.PinCategory.ToString();
		TypeObj->SetStringField(TEXT("Category"), Category);
		TypeObj->SetStringField(TEXT("SubCategory"), Variable.VarType.PinSubCategory.ToString());

		if (Variable.VarType.PinSubCategoryObject.IsValid())
		{
			TypeObj->SetStringField(TEXT("SubCategoryObject"), Variable.VarType.PinSubCategoryObject->GetName());
			TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), Variable.VarType.PinSubCategoryObject->GetPathName());
		}

		TypeObj->SetBoolField(TEXT("IsArray"), Variable.VarType.IsArray());
		TypeObj->SetBoolField(TEXT("IsSet"), Variable.VarType.IsSet());
		TypeObj->SetBoolField(TEXT("IsMap"), Variable.VarType.IsMap());
		TypeObj->SetBoolField(TEXT("IsReference"), Variable.VarType.bIsReference);
		TypeObj->SetBoolField(TEXT("IsConst"), Variable.VarType.bIsConst);

		// Try to get key type for maps
		if (Variable.VarType.IsMap() && Variable.VarType.PinValueType.TerminalCategory != NAME_None)
		{
			TSharedPtr<FJsonObject> KeyTypeObj = MakeShareable(new FJsonObject());
			KeyTypeObj->SetStringField(TEXT("Category"), Variable.VarType.PinValueType.TerminalCategory.ToString());
			KeyTypeObj->SetStringField(TEXT("SubCategory"), Variable.VarType.PinValueType.TerminalSubCategory.ToString());
			if (Variable.VarType.PinValueType.TerminalSubCategoryObject.IsValid())
			{
				KeyTypeObj->SetStringField(TEXT("SubCategoryObject"), Variable.VarType.PinValueType.TerminalSubCategoryObject->GetName());
				KeyTypeObj->SetStringField(TEXT("SubCategoryObjectPath"), Variable.VarType.PinValueType.TerminalSubCategoryObject->GetPathName());
			}
			TypeObj->SetObjectField(TEXT("KeyType"), KeyTypeObj);
		}

		VarObj->SetObjectField(TEXT("Type"), TypeObj);
	}

	return VarObj;
}

// ============================================================================
// PIN SERIALIZATION
// ============================================================================

TSharedPtr<FJsonObject> UPythonBridge::SerializePin(UEdGraphPin* Pin)
{
	TSharedPtr<FJsonObject> PinObj = MakeShareable(new FJsonObject());

	if (!Pin)
	{
		return PinObj;
	}

	PinObj->SetStringField(TEXT("Name"), Pin->PinName.ToString());
	PinObj->SetStringField(TEXT("Direction"), Pin->Direction == EGPD_Input ? TEXT("Input") : TEXT("Output"));

	// Type info
	TSharedPtr<FJsonObject> TypeObj = MakeShareable(new FJsonObject());
	TypeObj->SetStringField(TEXT("Category"), Pin->PinType.PinCategory.ToString());
	TypeObj->SetStringField(TEXT("SubCategory"), Pin->PinType.PinSubCategory.ToString());
	if (Pin->PinType.PinSubCategoryObject.IsValid())
	{
		TypeObj->SetStringField(TEXT("SubCategoryObject"), Pin->PinType.PinSubCategoryObject->GetName());
		TypeObj->SetStringField(TEXT("SubCategoryObjectPath"), Pin->PinType.PinSubCategoryObject->GetPathName());
	}
	TypeObj->SetBoolField(TEXT("IsArray"), Pin->PinType.IsArray());
	TypeObj->SetBoolField(TEXT("IsSet"), Pin->PinType.IsSet());
	TypeObj->SetBoolField(TEXT("IsMap"), Pin->PinType.IsMap());
	TypeObj->SetBoolField(TEXT("IsReference"), Pin->PinType.bIsReference);
	TypeObj->SetBoolField(TEXT("IsConst"), Pin->PinType.bIsConst);

	// For Map pins, include key type
	if (Pin->PinType.IsMap() && Pin->PinType.PinValueType.TerminalCategory != NAME_None)
	{
		TSharedPtr<FJsonObject> KeyTypeObj = MakeShareable(new FJsonObject());
		KeyTypeObj->SetStringField(TEXT("Category"), Pin->PinType.PinValueType.TerminalCategory.ToString());
		KeyTypeObj->SetStringField(TEXT("SubCategory"), Pin->PinType.PinValueType.TerminalSubCategory.ToString());
		if (Pin->PinType.PinValueType.TerminalSubCategoryObject.IsValid())
		{
			KeyTypeObj->SetStringField(TEXT("SubCategoryObject"), Pin->PinType.PinValueType.TerminalSubCategoryObject->GetName());
			KeyTypeObj->SetStringField(TEXT("SubCategoryObjectPath"), Pin->PinType.PinValueType.TerminalSubCategoryObject->GetPathName());
		}
		TypeObj->SetObjectField(TEXT("KeyType"), KeyTypeObj);
	}

	PinObj->SetObjectField(TEXT("Type"), TypeObj);

	// Default values
	PinObj->SetStringField(TEXT("DefaultValue"), Pin->DefaultValue);
	PinObj->SetStringField(TEXT("DefaultObject"), Pin->DefaultObject ? Pin->DefaultObject->GetPathName() : TEXT(""));
	PinObj->SetStringField(TEXT("DefaultTextValue"), Pin->DefaultTextValue.ToString());
	PinObj->SetBoolField(TEXT("IsHidden"), Pin->bHidden);

	// Connections
	TArray<TSharedPtr<FJsonValue>> LinkedToArray;
	for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
	{
		if (LinkedPin && LinkedPin->GetOwningNode())
		{
			TSharedPtr<FJsonObject> LinkObj = MakeShareable(new FJsonObject());
			LinkObj->SetStringField(TEXT("NodeGuid"), LinkedPin->GetOwningNode()->NodeGuid.ToString());
			LinkObj->SetStringField(TEXT("NodeTitle"), LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString());
			LinkObj->SetStringField(TEXT("PinName"), LinkedPin->PinName.ToString());
			LinkedToArray.Add(MakeShareable(new FJsonValueObject(LinkObj)));
		}
	}
	PinObj->SetArrayField(TEXT("LinkedTo"), LinkedToArray);

	return PinObj;
}

// ============================================================================
// NODE SERIALIZATION
// ============================================================================

TSharedPtr<FJsonObject> UPythonBridge::SerializeNode(UEdGraphNode* Node)
{
	TSharedPtr<FJsonObject> NodeObj = MakeShareable(new FJsonObject());

	if (!Node)
	{
		return NodeObj;
	}

	NodeObj->SetStringField(TEXT("NodeClass"), Node->GetClass()->GetName());
	NodeObj->SetStringField(TEXT("NodeTitle"), Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString());
	NodeObj->SetStringField(TEXT("NodeGuid"), Node->NodeGuid.ToString());
	NodeObj->SetNumberField(TEXT("PosX"), Node->NodePosX);
	NodeObj->SetNumberField(TEXT("PosY"), Node->NodePosY);
	NodeObj->SetStringField(TEXT("NodeComment"), Node->NodeComment);

	// K2Node-specific info
	if (UK2Node* K2Node = Cast<UK2Node>(Node))
	{
		// Check for variable nodes
		if (UK2Node_VariableGet* VarGet = Cast<UK2Node_VariableGet>(Node))
		{
			NodeObj->SetStringField(TEXT("VariableName"), VarGet->GetVarName().ToString());
		}
		else if (UK2Node_VariableSet* VarSet = Cast<UK2Node_VariableSet>(Node))
		{
			NodeObj->SetStringField(TEXT("VariableName"), VarSet->GetVarName().ToString());
		}

		// Check for function calls
		if (UK2Node_CallFunction* CallFunc = Cast<UK2Node_CallFunction>(Node))
		{
			UFunction* Func = CallFunc->GetTargetFunction();
			if (Func)
			{
				NodeObj->SetStringField(TEXT("FunctionName"), Func->GetName());
				NodeObj->SetStringField(TEXT("FunctionClass"), Func->GetOwnerClass() ? Func->GetOwnerClass()->GetName() : TEXT("Unknown"));
			}
		}

		// Check for events
		if (UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node))
		{
			NodeObj->SetStringField(TEXT("EventName"), EventNode->GetFunctionName().ToString());
		}
		if (UK2Node_CustomEvent* CustomEvent = Cast<UK2Node_CustomEvent>(Node))
		{
			NodeObj->SetStringField(TEXT("CustomEventName"), CustomEvent->CustomFunctionName.ToString());
		}

		// *** CRITICAL: Extract PropertyAccess paths from AnimGraph nodes ***
		if (UAnimGraphNode_Base* AnimGraphNode = Cast<UAnimGraphNode_Base>(Node))
		{
			// Access the deprecated PropertyBindings map which contains PropertyPath info
			// Use reflection to access PropertyBindings_DEPRECATED since it's a UPROPERTY
			TArray<TSharedPtr<FJsonValue>> BindingsArray;
			
			FMapProperty* BindingsMap = CastField<FMapProperty>(
				UAnimGraphNode_Base::StaticClass()->FindPropertyByName(TEXT("PropertyBindings_DEPRECATED")));
			if (BindingsMap)
			{
				FScriptMapHelper MapHelper(BindingsMap, BindingsMap->ContainerPtrToValuePtr<void>(AnimGraphNode));
				for (int32 i = 0; i < MapHelper.Num(); ++i)
				{
					if (MapHelper.IsValidIndex(i))
					{
						// Get the binding struct
						FAnimGraphNodePropertyBinding* Binding = (FAnimGraphNodePropertyBinding*)MapHelper.GetValuePtr(i);
						if (Binding && Binding->PropertyPath.Num() > 0)
						{
							TSharedPtr<FJsonObject> BindingObj = MakeShareable(new FJsonObject());
							BindingObj->SetStringField(TEXT("PropertyName"), Binding->PropertyName.ToString());
							BindingObj->SetStringField(TEXT("PropertyPath"), FString::Join(Binding->PropertyPath, TEXT(".")));
							BindingObj->SetBoolField(TEXT("IsBound"), Binding->bIsBound);
							
							TArray<TSharedPtr<FJsonValue>> PathSegments;
							for (const FString& Seg : Binding->PropertyPath)
							{
								PathSegments.Add(MakeShareable(new FJsonValueString(Seg)));
							}
							BindingObj->SetArrayField(TEXT("PathSegments"), PathSegments);
							
							BindingsArray.Add(MakeShareable(new FJsonValueObject(BindingObj)));
						}
					}
				}
			}
			if (BindingsArray.Num() > 0)
			{
				NodeObj->SetArrayField(TEXT("PropertyBindings"), BindingsArray);
			}
		}
		
		// *** Extract path from K2Node_PropertyAccess via reflection ***
		// The node class may be K2Node_PropertyAccess but header isn't public
		FString NodeClassName = Node->GetClass()->GetName();
		if (NodeClassName.Contains(TEXT("PropertyAccess")))
		{
			// Try to find a Path property using reflection
			for (TFieldIterator<FProperty> PropIt(Node->GetClass()); PropIt; ++PropIt)
			{
				FProperty* Prop = *PropIt;
				FString PropName = Prop->GetName();
				
				// Look for path-related properties
				if (PropName.Contains(TEXT("Path")) || PropName.Contains(TEXT("Resolve")))
				{
					// Handle array of strings (typical path representation)
					if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Prop))
					{
						if (FStrProperty* InnerStrProp = CastField<FStrProperty>(ArrayProp->Inner))
						{
							FScriptArrayHelper ArrayHelper(ArrayProp, Prop->ContainerPtrToValuePtr<void>(Node));
							TArray<TSharedPtr<FJsonValue>> PathArray;
							for (int32 i = 0; i < ArrayHelper.Num(); ++i)
							{
								FString* StrPtr = reinterpret_cast<FString*>(ArrayHelper.GetRawPtr(i));
								if (StrPtr)
								{
									PathArray.Add(MakeShareable(new FJsonValueString(*StrPtr)));
								}
							}
							if (PathArray.Num() > 0)
							{
								NodeObj->SetArrayField(TEXT("PropertyAccessPath"), PathArray);
								NodeObj->SetStringField(TEXT("PropertyAccessPathJoined"), 
									FString::JoinBy(PathArray, TEXT("."), [](const TSharedPtr<FJsonValue>& Val) {
										return Val->AsString();
									}));
							}
						}
					}
					// Handle single string
					else if (FStrProperty* StrProp = CastField<FStrProperty>(Prop))
					{
						FString Value = StrProp->GetPropertyValue_InContainer(Node);
						if (!Value.IsEmpty())
						{
							NodeObj->SetStringField(FString::Printf(TEXT("PropertyAccess_%s"), *PropName), Value);
						}
					}
				}
			}
		}
	}

	// Collapsed graph (tunnel/composite nodes)
	if (UK2Node_Composite* CompositeNode = Cast<UK2Node_Composite>(Node))
	{
		if (CompositeNode->BoundGraph)
		{
			NodeObj->SetObjectField(TEXT("CollapsedGraph"), SerializeGraph(CompositeNode->BoundGraph));
		}
	}

	// Serialize pins
	TArray<TSharedPtr<FJsonValue>> PinsArray;
	for (UEdGraphPin* Pin : Node->Pins)
	{
		PinsArray.Add(MakeShareable(new FJsonValueObject(SerializePin(Pin))));
	}
	NodeObj->SetArrayField(TEXT("Pins"), PinsArray);

	return NodeObj;
}

// ============================================================================
// GRAPH SERIALIZATION
// ============================================================================

TSharedPtr<FJsonObject> UPythonBridge::SerializeGraph(UEdGraph* Graph)
{
	TSharedPtr<FJsonObject> GraphObj = MakeShareable(new FJsonObject());

	if (!Graph)
	{
		return GraphObj;
	}

	GraphObj->SetStringField(TEXT("GraphName"), Graph->GetName());

	// Determine graph type
	FString GraphType = TEXT("Unknown");
	if (Graph->GetSchema())
	{
		GraphType = Graph->GetSchema()->GetClass()->GetName();
	}
	GraphObj->SetStringField(TEXT("GraphType"), GraphType);

	// Serialize nodes
	TArray<TSharedPtr<FJsonValue>> NodesArray;
	for (UEdGraphNode* Node : Graph->Nodes)
	{
		NodesArray.Add(MakeShareable(new FJsonValueObject(SerializeNode(Node))));
	}
	GraphObj->SetArrayField(TEXT("Nodes"), NodesArray);

	return GraphObj;
}

// ============================================================================
// FUNCTION SERIALIZATION
// ============================================================================

TSharedPtr<FJsonObject> UPythonBridge::SerializeFunction(UFunction* Function, UBlueprint* Blueprint)
{
	TSharedPtr<FJsonObject> FuncObj = MakeShareable(new FJsonObject());

	if (!Function)
	{
		return FuncObj;
	}

	FuncObj->SetStringField(TEXT("Name"), Function->GetName());

	// Access specifier
	FString AccessSpecifier = TEXT("Public");
	if (Function->HasAnyFunctionFlags(FUNC_Private))
	{
		AccessSpecifier = TEXT("Private");
	}
	else if (Function->HasAnyFunctionFlags(FUNC_Protected))
	{
		AccessSpecifier = TEXT("Protected");
	}
	FuncObj->SetStringField(TEXT("AccessSpecifier"), AccessSpecifier);

	// Function flags
	FuncObj->SetBoolField(TEXT("IsPure"), Function->HasAnyFunctionFlags(FUNC_BlueprintPure));
	FuncObj->SetBoolField(TEXT("IsConst"), Function->HasAnyFunctionFlags(FUNC_Const));
	FuncObj->SetBoolField(TEXT("IsStatic"), Function->HasAnyFunctionFlags(FUNC_Static));
	FuncObj->SetBoolField(TEXT("IsEvent"), Function->HasAnyFunctionFlags(FUNC_Event));
	FuncObj->SetBoolField(TEXT("IsNative"), Function->HasAnyFunctionFlags(FUNC_Native));
	FuncObj->SetBoolField(TEXT("IsNetReplicatedFunc"), Function->HasAnyFunctionFlags(FUNC_Net));

	// Get category from metadata
	FString Category = Function->GetMetaData(TEXT("Category"));
	FuncObj->SetStringField(TEXT("Category"), Category);

	// Parameters (inputs and outputs)
	TArray<TSharedPtr<FJsonValue>> InputsArray;
	TArray<TSharedPtr<FJsonValue>> OutputsArray;

	for (TFieldIterator<FProperty> PropIt(Function); PropIt; ++PropIt)
	{
		FProperty* Param = *PropIt;

		TSharedPtr<FJsonObject> ParamObj = MakeShareable(new FJsonObject());
		ParamObj->SetStringField(TEXT("Name"), Param->GetName());
		ParamObj->SetObjectField(TEXT("Type"), SerializePropertyType(Param));

		// Get default value if available
		FString DefaultValue;
		if (Function->HasMetaData(*FString::Printf(TEXT("CPP_Default_%s"), *Param->GetName())))
		{
			DefaultValue = Function->GetMetaData(*FString::Printf(TEXT("CPP_Default_%s"), *Param->GetName()));
		}
		ParamObj->SetStringField(TEXT("DefaultValue"), DefaultValue);

		if (Param->HasAnyPropertyFlags(CPF_ReturnParm | CPF_OutParm))
		{
			OutputsArray.Add(MakeShareable(new FJsonValueObject(ParamObj)));
		}
		else
		{
			InputsArray.Add(MakeShareable(new FJsonValueObject(ParamObj)));
		}
	}

	FuncObj->SetArrayField(TEXT("Inputs"), InputsArray);
	FuncObj->SetArrayField(TEXT("Outputs"), OutputsArray);

	// Get local variables from function graph
	TArray<TSharedPtr<FJsonValue>> LocalsArray;
	if (Blueprint)
	{
		for (UEdGraph* Graph : Blueprint->FunctionGraphs)
		{
			if (Graph && Graph->GetFName() == Function->GetFName())
			{
				// Find local variable nodes
				for (UEdGraphNode* Node : Graph->Nodes)
				{
					if (UK2Node_FunctionEntry* EntryNode = Cast<UK2Node_FunctionEntry>(Node))
					{
						for (const FBPVariableDescription& LocalVar : EntryNode->LocalVariables)
						{
							TSharedPtr<FJsonObject> LocalObj = MakeShareable(new FJsonObject());
							LocalObj->SetStringField(TEXT("Name"), LocalVar.VarName.ToString());
							LocalObj->SetObjectField(TEXT("Type"), SerializeVariable(LocalVar, Blueprint)->GetObjectField(TEXT("Type")));
							LocalObj->SetStringField(TEXT("DefaultValue"), LocalVar.DefaultValue);
							LocalsArray.Add(MakeShareable(new FJsonValueObject(LocalObj)));
						}
					}
				}
			}
		}
	}
	FuncObj->SetArrayField(TEXT("LocalVariables"), LocalsArray);

	return FuncObj;
}

// ============================================================================
// MAIN EXPORT FUNCTION
// ============================================================================

FString UPythonBridge::ExportBlueprintDNA(UBlueprint* Blueprint)
{
	if (!Blueprint)
	{
		return TEXT("{}");
	}

	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());

	// Basic info
	RootObj->SetStringField(TEXT("Name"), Blueprint->GetName());
	RootObj->SetStringField(TEXT("Path"), Blueprint->GetPathName());
	RootObj->SetStringField(TEXT("BlueprintClass"), Blueprint->GetClass()->GetName());

	// Parent class info
	if (Blueprint->ParentClass)
	{
		RootObj->SetStringField(TEXT("ParentClass"), Blueprint->ParentClass->GetName());
		RootObj->SetStringField(TEXT("ParentClassPath"), Blueprint->ParentClass->GetPathName());
	}

	// Blueprint type
	FString BPType;
	switch (Blueprint->BlueprintType)
	{
	case BPTYPE_Normal: BPType = TEXT("BPTYPE_Normal"); break;
	case BPTYPE_Const: BPType = TEXT("BPTYPE_Const"); break;
	case BPTYPE_MacroLibrary: BPType = TEXT("BPTYPE_MacroLibrary"); break;
	case BPTYPE_Interface: BPType = TEXT("BPTYPE_Interface"); break;
	case BPTYPE_LevelScript: BPType = TEXT("BPTYPE_LevelScript"); break;
	case BPTYPE_FunctionLibrary: BPType = TEXT("BPTYPE_FunctionLibrary"); break;
	default: BPType = TEXT("Unknown"); break;
	}
	RootObj->SetStringField(TEXT("BlueprintType"), BPType);

	// Variables
	TSharedPtr<FJsonObject> VariablesObj = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> VariablesArray;
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		VariablesArray.Add(MakeShareable(new FJsonValueObject(SerializeVariable(Var, Blueprint))));
	}
	VariablesObj->SetArrayField(TEXT("List"), VariablesArray);
	RootObj->SetObjectField(TEXT("Variables"), VariablesObj);

	// Event Dispatchers (multicast delegates)
	TSharedPtr<FJsonObject> DispatchersObj = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> DispatchersArray;
	if (Blueprint->GeneratedClass)
	{
		for (TFieldIterator<FMulticastDelegateProperty> PropIt(Blueprint->GeneratedClass, EFieldIteratorFlags::ExcludeSuper); PropIt; ++PropIt)
		{
			FMulticastDelegateProperty* DelegateProp = *PropIt;
			TSharedPtr<FJsonObject> DispatcherObj = MakeShareable(new FJsonObject());
			DispatcherObj->SetStringField(TEXT("Name"), DelegateProp->GetName());

			// Get parameters from signature function
			TArray<TSharedPtr<FJsonValue>> ParamsArray;
			if (DelegateProp->SignatureFunction)
			{
				for (TFieldIterator<FProperty> ParamIt(DelegateProp->SignatureFunction); ParamIt; ++ParamIt)
				{
					FProperty* Param = *ParamIt;
					if (!Param->HasAnyPropertyFlags(CPF_ReturnParm))
					{
						TSharedPtr<FJsonObject> ParamObj = MakeShareable(new FJsonObject());
						ParamObj->SetStringField(TEXT("Name"), Param->GetName());
						ParamObj->SetObjectField(TEXT("Type"), SerializePropertyType(Param));
						ParamsArray.Add(MakeShareable(new FJsonValueObject(ParamObj)));
					}
				}
			}
			DispatcherObj->SetArrayField(TEXT("Parameters"), ParamsArray);
			DispatchersArray.Add(MakeShareable(new FJsonValueObject(DispatcherObj)));
		}
	}
	DispatchersObj->SetArrayField(TEXT("List"), DispatchersArray);
	RootObj->SetObjectField(TEXT("EventDispatchers"), DispatchersObj);

	// Implemented Interfaces
	TSharedPtr<FJsonObject> InterfacesObj = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> InterfacesArray;
	for (const FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
	{
		if (Interface.Interface)
		{
			TSharedPtr<FJsonObject> InterfaceObj = MakeShareable(new FJsonObject());
			InterfaceObj->SetStringField(TEXT("Name"), Interface.Interface->GetName());
			InterfaceObj->SetStringField(TEXT("Path"), Interface.Interface->GetPathName());

			// List interface functions
			TArray<TSharedPtr<FJsonValue>> InterfaceFuncsArray;
			for (TFieldIterator<UFunction> FuncIt(Interface.Interface); FuncIt; ++FuncIt)
			{
				InterfaceFuncsArray.Add(MakeShareable(new FJsonValueString((*FuncIt)->GetName())));
			}
			InterfaceObj->SetArrayField(TEXT("Functions"), InterfaceFuncsArray);

			InterfacesArray.Add(MakeShareable(new FJsonValueObject(InterfaceObj)));
		}
	}
	InterfacesObj->SetArrayField(TEXT("List"), InterfacesArray);
	RootObj->SetObjectField(TEXT("Interfaces"), InterfacesObj);

	// Components (for Actor blueprints)
	TSharedPtr<FJsonObject> ComponentsObj = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> ComponentsArray;
	for (USCS_Node* SCSNode : Blueprint->SimpleConstructionScript ? Blueprint->SimpleConstructionScript->GetAllNodes() : TArray<USCS_Node*>())
	{
		if (SCSNode && SCSNode->ComponentTemplate)
		{
			TSharedPtr<FJsonObject> CompObj = MakeShareable(new FJsonObject());
			CompObj->SetStringField(TEXT("Name"), SCSNode->GetVariableName().ToString());
			CompObj->SetStringField(TEXT("Class"), SCSNode->ComponentTemplate->GetClass()->GetName());
			CompObj->SetStringField(TEXT("ClassPath"), SCSNode->ComponentTemplate->GetClass()->GetPathName());

			// Parent component
			if (SCSNode->ParentComponentOrVariableName != NAME_None)
			{
				CompObj->SetStringField(TEXT("AttachedTo"), SCSNode->ParentComponentOrVariableName.ToString());
			}

			ComponentsArray.Add(MakeShareable(new FJsonValueObject(CompObj)));
		}
	}
	ComponentsObj->SetArrayField(TEXT("Hierarchy"), ComponentsArray);
	RootObj->SetObjectField(TEXT("Components"), ComponentsObj);

	// Function Signatures
	TSharedPtr<FJsonObject> FunctionsObj = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> FunctionsArray;
	if (Blueprint->GeneratedClass)
	{
		for (TFieldIterator<UFunction> FuncIt(Blueprint->GeneratedClass, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
		{
			UFunction* Func = *FuncIt;
			// Skip delegate signatures
			if (!Func->HasAnyFunctionFlags(FUNC_Delegate))
			{
				FunctionsArray.Add(MakeShareable(new FJsonValueObject(SerializeFunction(Func, Blueprint))));
			}
		}
	}
	FunctionsObj->SetArrayField(TEXT("Functions"), FunctionsArray);
	RootObj->SetObjectField(TEXT("FunctionSignatures"), FunctionsObj);

	// All Graphs (Logic section)
	TSharedPtr<FJsonObject> LogicObj = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> AllGraphsArray;

	// Event graphs
	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		AllGraphsArray.Add(MakeShareable(new FJsonValueObject(SerializeGraph(Graph))));
	}

	// Function graphs
	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
	{
		AllGraphsArray.Add(MakeShareable(new FJsonValueObject(SerializeGraph(Graph))));
	}

	// Macro graphs
	for (UEdGraph* Graph : Blueprint->MacroGraphs)
	{
		AllGraphsArray.Add(MakeShareable(new FJsonValueObject(SerializeGraph(Graph))));
	}

	// Delegate graphs
	for (UEdGraph* Graph : Blueprint->DelegateSignatureGraphs)
	{
		AllGraphsArray.Add(MakeShareable(new FJsonValueObject(SerializeGraph(Graph))));
	}

	LogicObj->SetArrayField(TEXT("AllGraphs"), AllGraphsArray);
	RootObj->SetObjectField(TEXT("Logic"), LogicObj);

	return JsonObjectToString(RootObj);
}

// ============================================================================
// OTHER EXPORT FUNCTIONS
// ============================================================================

FString UPythonBridge::ExportWidgetBlueprintDNA(UWidgetBlueprint* WidgetBlueprint)
{
	if (!WidgetBlueprint)
	{
		return TEXT("{}");
	}

	// Start with base Blueprint export
	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());

	// Parse the base Blueprint export
	FString BaseDNA = ExportBlueprintDNA(WidgetBlueprint);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BaseDNA);
	FJsonSerializer::Deserialize(Reader, RootObj);

	// Add widget-specific info
	if (WidgetBlueprint->WidgetTree)
	{
		TSharedPtr<FJsonObject> WidgetTreeObj = MakeShareable(new FJsonObject());

		// Root widget
		if (WidgetBlueprint->WidgetTree->RootWidget)
		{
			WidgetTreeObj->SetStringField(TEXT("RootWidgetClass"), WidgetBlueprint->WidgetTree->RootWidget->GetClass()->GetName());
			WidgetTreeObj->SetStringField(TEXT("RootWidgetName"), WidgetBlueprint->WidgetTree->RootWidget->GetName());
		}

		// All widgets
		TArray<TSharedPtr<FJsonValue>> WidgetsArray;
		WidgetBlueprint->WidgetTree->ForEachWidget([&WidgetsArray](UWidget* Widget)
		{
			if (Widget)
			{
				TSharedPtr<FJsonObject> WidgetObj = MakeShareable(new FJsonObject());
				WidgetObj->SetStringField(TEXT("Name"), Widget->GetName());
				WidgetObj->SetStringField(TEXT("Class"), Widget->GetClass()->GetName());
				WidgetObj->SetBoolField(TEXT("IsVariable"), Widget->bIsVariable);
				WidgetsArray.Add(MakeShareable(new FJsonValueObject(WidgetObj)));
			}
		});
		WidgetTreeObj->SetArrayField(TEXT("AllWidgets"), WidgetsArray);

		RootObj->SetObjectField(TEXT("WidgetTree"), WidgetTreeObj);
	}

	// Animations
	TArray<TSharedPtr<FJsonValue>> AnimationsArray;
	for (UWidgetAnimation* Animation : WidgetBlueprint->Animations)
	{
		if (Animation)
		{
			TSharedPtr<FJsonObject> AnimObj = MakeShareable(new FJsonObject());
			AnimObj->SetStringField(TEXT("Name"), Animation->GetName());
			AnimObj->SetNumberField(TEXT("Length"), Animation->GetEndTime() - Animation->GetStartTime());
			AnimationsArray.Add(MakeShareable(new FJsonValueObject(AnimObj)));
		}
	}
	RootObj->SetArrayField(TEXT("Animations"), AnimationsArray);

	return JsonObjectToString(RootObj);
}

FString UPythonBridge::ExportAnimBlueprintDNA(UAnimBlueprint* AnimBlueprint)
{
	if (!AnimBlueprint)
	{
		return TEXT("{}");
	}

	// Start with base Blueprint export
	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());

	FString BaseDNA = ExportBlueprintDNA(AnimBlueprint);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BaseDNA);
	FJsonSerializer::Deserialize(Reader, RootObj);

	// Add anim-specific info
	if (AnimBlueprint->TargetSkeleton)
	{
		RootObj->SetStringField(TEXT("TargetSkeleton"), AnimBlueprint->TargetSkeleton->GetPathName());
	}

	// *** ENHANCED: Export AnimGraph structure ***
	TSharedPtr<FJsonObject> AnimGraphObj = MakeShareable(new FJsonObject());

	// Export all animation graphs (includes AnimGraph and state machine graphs)
	TArray<TSharedPtr<FJsonValue>> AnimGraphsArray;
	for (UEdGraph* Graph : AnimBlueprint->FunctionGraphs)
	{
		if (!Graph) continue;

		TSharedPtr<FJsonObject> GraphInfoObj = MakeShareable(new FJsonObject());
		GraphInfoObj->SetStringField(TEXT("Name"), Graph->GetName());
		GraphInfoObj->SetStringField(TEXT("SchemaClass"), Graph->GetSchema() ? Graph->GetSchema()->GetClass()->GetName() : TEXT("Unknown"));

		// Check if this is an animation state machine graph
		if (UAnimationStateMachineGraph* StateMachineGraph = Cast<UAnimationStateMachineGraph>(Graph))
		{
			GraphInfoObj->SetStringField(TEXT("Type"), TEXT("StateMachine"));

			// Export states
			TArray<TSharedPtr<FJsonValue>> StatesArray;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (UAnimStateNode* StateNode = Cast<UAnimStateNode>(Node))
				{
					TSharedPtr<FJsonObject> StateObj = MakeShareable(new FJsonObject());
					StateObj->SetStringField(TEXT("Name"), StateNode->GetStateName());
					StateObj->SetStringField(TEXT("NodeClass"), TEXT("AnimStateNode"));

					// If the state has a bound graph (the actual state logic), serialize it
					if (StateNode->BoundGraph)
					{
						// Extract PropertyBindings from AnimGraph nodes in the state graph
						TArray<TSharedPtr<FJsonValue>> PropertyAccessArray;
						for (UEdGraphNode* InnerNode : StateNode->BoundGraph->Nodes)
						{
							if (UAnimGraphNode_Base* AnimNode = Cast<UAnimGraphNode_Base>(InnerNode))
							{
								// Access PropertyBindings_DEPRECATED via reflection
								FMapProperty* BindingsMap = CastField<FMapProperty>(
									UAnimGraphNode_Base::StaticClass()->FindPropertyByName(TEXT("PropertyBindings_DEPRECATED")));
								if (BindingsMap)
								{
									FScriptMapHelper MapHelper(BindingsMap, BindingsMap->ContainerPtrToValuePtr<void>(AnimNode));
									for (int32 i = 0; i < MapHelper.Num(); ++i)
									{
										if (MapHelper.IsValidIndex(i))
										{
											FAnimGraphNodePropertyBinding* Binding = (FAnimGraphNodePropertyBinding*)MapHelper.GetValuePtr(i);
											if (Binding && Binding->PropertyPath.Num() > 0)
											{
												TSharedPtr<FJsonObject> PropAccessObj = MakeShareable(new FJsonObject());
												PropAccessObj->SetStringField(TEXT("Path"), FString::Join(Binding->PropertyPath, TEXT(".")));
												PropertyAccessArray.Add(MakeShareable(new FJsonValueObject(PropAccessObj)));
											}
										}
									}
								}
							}
						}
						if (PropertyAccessArray.Num() > 0)
						{
							StateObj->SetArrayField(TEXT("PropertyAccessNodes"), PropertyAccessArray);
						}
					}

					StatesArray.Add(MakeShareable(new FJsonValueObject(StateObj)));
				}
				else if (UAnimStateTransitionNode* TransNode = Cast<UAnimStateTransitionNode>(Node))
				{
					TSharedPtr<FJsonObject> TransObj = MakeShareable(new FJsonObject());
					TransObj->SetStringField(TEXT("NodeClass"), TEXT("AnimStateTransitionNode"));
					if (TransNode->GetPreviousState())
					{
						TransObj->SetStringField(TEXT("FromState"), TransNode->GetPreviousState()->GetStateName());
					}
					if (TransNode->GetNextState())
					{
						TransObj->SetStringField(TEXT("ToState"), TransNode->GetNextState()->GetStateName());
					}
					StatesArray.Add(MakeShareable(new FJsonValueObject(TransObj)));
				}
			}
			GraphInfoObj->SetArrayField(TEXT("States"), StatesArray);
		}
		else
		{
			GraphInfoObj->SetStringField(TEXT("Type"), TEXT("AnimGraph"));

			// Scan for PropertyBindings in AnimGraph nodes
			TArray<TSharedPtr<FJsonValue>> PropertyAccessArray;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (UAnimGraphNode_Base* AnimNode = Cast<UAnimGraphNode_Base>(Node))
				{
					FMapProperty* BindingsMap = CastField<FMapProperty>(
						UAnimGraphNode_Base::StaticClass()->FindPropertyByName(TEXT("PropertyBindings_DEPRECATED")));
					if (BindingsMap)
					{
						FScriptMapHelper MapHelper(BindingsMap, BindingsMap->ContainerPtrToValuePtr<void>(AnimNode));
						for (int32 i = 0; i < MapHelper.Num(); ++i)
						{
							if (MapHelper.IsValidIndex(i))
							{
								FAnimGraphNodePropertyBinding* Binding = (FAnimGraphNodePropertyBinding*)MapHelper.GetValuePtr(i);
								if (Binding && Binding->PropertyPath.Num() > 0)
								{
									TSharedPtr<FJsonObject> PropAccessObj = MakeShareable(new FJsonObject());
									PropAccessObj->SetStringField(TEXT("Path"), FString::Join(Binding->PropertyPath, TEXT(".")));
									PropertyAccessArray.Add(MakeShareable(new FJsonValueObject(PropAccessObj)));
								}
							}
						}
					}
				}
			}
			if (PropertyAccessArray.Num() > 0)
			{
				GraphInfoObj->SetArrayField(TEXT("PropertyAccessNodes"), PropertyAccessArray);
			}
		}

		AnimGraphsArray.Add(MakeShareable(new FJsonValueObject(GraphInfoObj)));
	}
	AnimGraphObj->SetArrayField(TEXT("Graphs"), AnimGraphsArray);

	// *** CRITICAL: Collect ALL PropertyAccess paths from the entire AnimBP ***
	TArray<TSharedPtr<FJsonValue>> AllPropertyAccessArray;
	TSet<FString> SeenPaths;  // Deduplicate

	// Helper lambda to extract PropertyBindings from AnimGraph nodes
	auto ExtractPropertyAccess = [&](UEdGraph* Graph) {
		if (!Graph) return;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (UAnimGraphNode_Base* AnimNode = Cast<UAnimGraphNode_Base>(Node))
			{
				FMapProperty* BindingsMap = CastField<FMapProperty>(
					UAnimGraphNode_Base::StaticClass()->FindPropertyByName(TEXT("PropertyBindings_DEPRECATED")));
				if (BindingsMap)
				{
					FScriptMapHelper MapHelper(BindingsMap, BindingsMap->ContainerPtrToValuePtr<void>(AnimNode));
					for (int32 i = 0; i < MapHelper.Num(); ++i)
					{
						if (MapHelper.IsValidIndex(i))
						{
							FAnimGraphNodePropertyBinding* Binding = (FAnimGraphNodePropertyBinding*)MapHelper.GetValuePtr(i);
							if (Binding && Binding->PropertyPath.Num() > 0)
							{
								FString FullPath = FString::Join(Binding->PropertyPath, TEXT("."));
								if (!SeenPaths.Contains(FullPath))
								{
									SeenPaths.Add(FullPath);
									TSharedPtr<FJsonObject> PropObj = MakeShareable(new FJsonObject());
									PropObj->SetStringField(TEXT("Path"), FullPath);
									PropObj->SetStringField(TEXT("GraphName"), Graph->GetName());
									PropObj->SetStringField(TEXT("NodeName"), AnimNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString());
									AllPropertyAccessArray.Add(MakeShareable(new FJsonValueObject(PropObj)));
								}
							}
						}
					}
				}
			}
		}
	};

	// Scan all graphs
	for (UEdGraph* Graph : AnimBlueprint->FunctionGraphs)
	{
		ExtractPropertyAccess(Graph);

		// Also scan nested graphs (state machine states have their own graphs)
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (UAnimStateNode* StateNode = Cast<UAnimStateNode>(Node))
			{
				ExtractPropertyAccess(StateNode->BoundGraph);
			}
		}
	}
	for (UEdGraph* Graph : AnimBlueprint->UbergraphPages)
	{
		ExtractPropertyAccess(Graph);
	}

	AnimGraphObj->SetArrayField(TEXT("AllPropertyAccessPaths"), AllPropertyAccessArray);
	RootObj->SetObjectField(TEXT("AnimGraphInfo"), AnimGraphObj);

	return JsonObjectToString(RootObj);
}

FString UPythonBridge::ExportUserDefinedEnum(UUserDefinedEnum* Enum)
{
	if (!Enum)
	{
		return TEXT("{}");
	}

	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());
	RootObj->SetStringField(TEXT("Name"), Enum->GetName());
	RootObj->SetStringField(TEXT("Path"), Enum->GetPathName());

	TArray<TSharedPtr<FJsonValue>> ValuesArray;
	for (int32 i = 0; i < Enum->NumEnums() - 1; ++i) // -1 to skip MAX
	{
		TSharedPtr<FJsonObject> ValueObj = MakeShareable(new FJsonObject());
		ValueObj->SetStringField(TEXT("Name"), Enum->GetNameStringByIndex(i));
		ValueObj->SetStringField(TEXT("DisplayName"), Enum->GetDisplayNameTextByIndex(i).ToString());
		ValueObj->SetNumberField(TEXT("Value"), Enum->GetValueByIndex(i));
		ValuesArray.Add(MakeShareable(new FJsonValueObject(ValueObj)));
	}
	RootObj->SetArrayField(TEXT("Values"), ValuesArray);

	return JsonObjectToString(RootObj);
}

FString UPythonBridge::ExportUserDefinedStruct(UUserDefinedStruct* Struct)
{
	if (!Struct)
	{
		return TEXT("{}");
	}

	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());
	RootObj->SetStringField(TEXT("Name"), Struct->GetName());
	RootObj->SetStringField(TEXT("Path"), Struct->GetPathName());

	TArray<TSharedPtr<FJsonValue>> PropertiesArray;
	for (TFieldIterator<FProperty> PropIt(Struct); PropIt; ++PropIt)
	{
		FProperty* Prop = *PropIt;
		TSharedPtr<FJsonObject> PropObj = MakeShareable(new FJsonObject());
		PropObj->SetStringField(TEXT("Name"), Prop->GetName());
		PropObj->SetObjectField(TEXT("Type"), SerializePropertyType(Prop));
		PropertiesArray.Add(MakeShareable(new FJsonValueObject(PropObj)));
	}
	RootObj->SetArrayField(TEXT("Properties"), PropertiesArray);

	return JsonObjectToString(RootObj);
}

FString UPythonBridge::ExportDataAsset(UDataAsset* DataAsset)
{
	if (!DataAsset)
	{
		return TEXT("{}");
	}

	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());
	RootObj->SetStringField(TEXT("Name"), DataAsset->GetName());
	RootObj->SetStringField(TEXT("Path"), DataAsset->GetPathName());
	RootObj->SetStringField(TEXT("Class"), DataAsset->GetClass()->GetName());

	TArray<TSharedPtr<FJsonValue>> PropertiesArray;
	for (TFieldIterator<FProperty> PropIt(DataAsset->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Prop = *PropIt;
		if (Prop->HasAnyPropertyFlags(CPF_Edit | CPF_BlueprintVisible))
		{
			TSharedPtr<FJsonObject> PropObj = MakeShareable(new FJsonObject());
			PropObj->SetStringField(TEXT("Name"), Prop->GetName());
			PropObj->SetObjectField(TEXT("Type"), SerializePropertyType(Prop));
			PropertiesArray.Add(MakeShareable(new FJsonValueObject(PropObj)));
		}
	}
	RootObj->SetArrayField(TEXT("Properties"), PropertiesArray);

	return JsonObjectToString(RootObj);
}

FString UPythonBridge::ExportAssetDNA(UObject* Asset)
{
	if (!Asset)
	{
		return TEXT("{}");
	}

	if (UBlueprint* BP = Cast<UBlueprint>(Asset))
	{
		if (UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Asset))
		{
			return ExportWidgetBlueprintDNA(WidgetBP);
		}
		if (UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(Asset))
		{
			return ExportAnimBlueprintDNA(AnimBP);
		}
		return ExportBlueprintDNA(BP);
	}
	if (UUserDefinedEnum* Enum = Cast<UUserDefinedEnum>(Asset))
	{
		return ExportUserDefinedEnum(Enum);
	}
	if (UUserDefinedStruct* Struct = Cast<UUserDefinedStruct>(Asset))
	{
		return ExportUserDefinedStruct(Struct);
	}
	if (UDataAsset* DA = Cast<UDataAsset>(Asset))
	{
		return ExportDataAsset(DA);
	}

	// Fallback - basic info
	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());
	RootObj->SetStringField(TEXT("Name"), Asset->GetName());
	RootObj->SetStringField(TEXT("Path"), Asset->GetPathName());
	RootObj->SetStringField(TEXT("Class"), Asset->GetClass()->GetName());
	return JsonObjectToString(RootObj);
}

// ============================================================================
// BLUEPRINT MANIPULATION FUNCTIONS
// ============================================================================

bool UPythonBridge::ReparentBlueprint(UBlueprint* Blueprint, UClass* NewParentClass)
{
	if (!Blueprint || !NewParentClass)
	{
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Reparenting '%s' from '%s' to '%s'"),
		*Blueprint->GetName(),
		Blueprint->ParentClass ? *Blueprint->ParentClass->GetName() : TEXT("null"),
		*NewParentClass->GetName());

	Blueprint->ParentClass = NewParentClass;
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	FKismetEditorUtilities::CompileBlueprint(Blueprint);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	return true;
}

bool UPythonBridge::DeleteBlueprintVariable(UBlueprint* Blueprint, FName VariableName)
{
	if (!Blueprint)
	{
		return false;
	}

	bool bFound = false;
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		if (Var.VarName == VariableName)
		{
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Variable '%s' not found in '%s'"),
			*VariableName.ToString(), *Blueprint->GetName());
		return false;
	}

	FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, VariableName);
	UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Deleted variable '%s' from '%s'"),
		*VariableName.ToString(), *Blueprint->GetName());

	return true;
}

bool UPythonBridge::DeleteBlueprintFunction(UBlueprint* Blueprint, FName FunctionName)
{
	if (!Blueprint)
	{
		return false;
	}

	UEdGraph* GraphToRemove = nullptr;
	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
	{
		if (Graph && Graph->GetFName() == FunctionName)
		{
			GraphToRemove = Graph;
			break;
		}
	}

	if (GraphToRemove)
	{
		FBlueprintEditorUtils::RemoveGraph(Blueprint, GraphToRemove, EGraphRemoveFlags::Default);
		UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Deleted function '%s' from '%s'"),
			*FunctionName.ToString(), *Blueprint->GetName());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Function '%s' not found in '%s'"),
		*FunctionName.ToString(), *Blueprint->GetName());
	return false;
}

bool UPythonBridge::DeleteBlueprintComponent(UBlueprint* Blueprint, FName ComponentName)
{
	if (!Blueprint || !Blueprint->SimpleConstructionScript)
	{
		return false;
	}

	USCS_Node* NodeToRemove = nullptr;
	for (USCS_Node* Node : Blueprint->SimpleConstructionScript->GetAllNodes())
	{
		if (Node && Node->GetVariableName() == ComponentName)
		{
			NodeToRemove = Node;
			break;
		}
	}

	if (NodeToRemove)
	{
		Blueprint->SimpleConstructionScript->RemoveNode(NodeToRemove);
		UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Deleted component '%s' from '%s'"),
			*ComponentName.ToString(), *Blueprint->GetName());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Component '%s' not found in '%s'"),
		*ComponentName.ToString(), *Blueprint->GetName());
	return false;
}

bool UPythonBridge::RenameBlueprintVariable(UBlueprint* Blueprint, FName OldName, FName NewName)
{
	if (!Blueprint)
	{
		return false;
	}

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
		UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Variable '%s' not found in '%s'"),
			*OldName.ToString(), *Blueprint->GetName());
		return false;
	}

	FBlueprintEditorUtils::RenameMemberVariable(Blueprint, OldName, NewName);
	UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Renamed variable '%s' -> '%s' in '%s'"),
		*OldName.ToString(), *NewName.ToString(), *Blueprint->GetName());

	return true;
}

bool UPythonBridge::RenameBlueprintComponent(UBlueprint* Blueprint, FName OldName, FName NewName)
{
	if (!Blueprint || !Blueprint->SimpleConstructionScript)
	{
		return false;
	}

	USCS_Node* NodeToRename = nullptr;
	for (USCS_Node* Node : Blueprint->SimpleConstructionScript->GetAllNodes())
	{
		if (Node && Node->GetVariableName() == OldName)
		{
			NodeToRename = Node;
			break;
		}
	}

	if (NodeToRename)
	{
		FBlueprintEditorUtils::RenameComponentMemberVariable(Blueprint, NodeToRename, NewName);
		UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Renamed component '%s' -> '%s' in '%s'"),
			*OldName.ToString(), *NewName.ToString(), *Blueprint->GetName());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Component '%s' not found in '%s'"),
		*OldName.ToString(), *Blueprint->GetName());
	return false;
}

bool UPythonBridge::RemoveBlueprintInterface(UBlueprint* Blueprint, UClass* InterfaceClass)
{
	if (!Blueprint || !InterfaceClass)
	{
		return false;
	}

	int32 IndexToRemove = -1;
	for (int32 i = 0; i < Blueprint->ImplementedInterfaces.Num(); ++i)
	{
		if (Blueprint->ImplementedInterfaces[i].Interface == InterfaceClass)
		{
			IndexToRemove = i;
			break;
		}
	}

	if (IndexToRemove >= 0)
	{
		// Use FTopLevelAssetPath for the new API
		FTopLevelAssetPath InterfacePath(InterfaceClass->GetPackage()->GetFName(), InterfaceClass->GetFName());
		FBlueprintEditorUtils::RemoveInterface(Blueprint, InterfacePath);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Removed interface '%s' from '%s'"),
			*InterfaceClass->GetName(), *Blueprint->GetName());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Interface '%s' not found in '%s'"),
		*InterfaceClass->GetName(), *Blueprint->GetName());
	return false;
}

int32 UPythonBridge::ClearEventGraph(UBlueprint* Blueprint)
{
	if (!Blueprint)
	{
		return 0;
	}

	int32 NodesRemoved = 0;
	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		if (Graph)
		{
			NodesRemoved += Graph->Nodes.Num();
			Graph->Nodes.Empty();
		}
	}

	if (NodesRemoved > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Cleared %d nodes from EventGraph in '%s'"),
			NodesRemoved, *Blueprint->GetName());
	}

	return NodesRemoved;
}

TArray<FString> UPythonBridge::PrepareBlueprintForReparenting(UBlueprint* Blueprint, UClass* NewParentClass)
{
	TArray<FString> RenamedVariables;

	if (!Blueprint || !NewParentClass)
	{
		return RenamedVariables;
	}

	// Find variables that would conflict with the new parent's properties
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		FProperty* ConflictingProp = NewParentClass->FindPropertyByName(Var.VarName);
		if (ConflictingProp)
		{
			FName OldName = Var.VarName;
			FName NewName = FName(*FString::Printf(TEXT("BP_%s"), *Var.VarName.ToString()));

			FBlueprintEditorUtils::RenameMemberVariable(Blueprint, OldName, NewName);
			RenamedVariables.Add(FString::Printf(TEXT("%s -> %s"), *OldName.ToString(), *NewName.ToString()));

			UE_LOG(LogTemp, Warning, TEXT("PythonBridge: Renamed conflicting variable '%s' -> '%s'"),
				*OldName.ToString(), *NewName.ToString());
		}
	}

	return RenamedVariables;
}

// ============================================================================
// DEPENDENCY ANALYSIS
// ============================================================================

TArray<UBlueprint*> UPythonBridge::GetChildBlueprints(UClass* ParentClass)
{
	TArray<UBlueprint*> ChildBlueprints;

	if (!ParentClass)
	{
		return ChildBlueprints;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> BlueprintAssets;
	AssetRegistry.GetAssetsByClass(UBlueprint::StaticClass()->GetClassPathName(), BlueprintAssets, true);

	for (const FAssetData& Asset : BlueprintAssets)
	{
		if (UBlueprint* BP = Cast<UBlueprint>(Asset.GetAsset()))
		{
			if (BP->GeneratedClass && BP->GeneratedClass->IsChildOf(ParentClass))
			{
				ChildBlueprints.Add(BP);
			}
		}
	}

	return ChildBlueprints;
}

TArray<FString> UPythonBridge::GetAssetDependencies(UObject* Asset)
{
	TArray<FString> Dependencies;

	if (!Asset)
	{
		return Dependencies;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetIdentifier> Deps;
	AssetRegistry.GetDependencies(FAssetIdentifier(FName(*Asset->GetPathName())), Deps);

	for (const FAssetIdentifier& Dep : Deps)
	{
		Dependencies.Add(Dep.ToString());
	}

	return Dependencies;
}

TArray<FString> UPythonBridge::GetAssetReferencers(UObject* Asset)
{
	TArray<FString> Referencers;

	if (!Asset)
	{
		return Referencers;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetIdentifier> Refs;
	AssetRegistry.GetReferencers(FAssetIdentifier(FName(*Asset->GetPathName())), Refs);

	for (const FAssetIdentifier& Ref : Refs)
	{
		Referencers.Add(Ref.ToString());
	}

	return Referencers;
}

FString UPythonBridge::GetAssetDependencyReport(UObject* Asset)
{
	if (!Asset)
	{
		return TEXT("{}");
	}

	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());
	RootObj->SetStringField(TEXT("Asset"), Asset->GetPathName());

	TArray<FString> Deps = GetAssetDependencies(Asset);
	TArray<TSharedPtr<FJsonValue>> DepsArray;
	for (const FString& Dep : Deps)
	{
		DepsArray.Add(MakeShareable(new FJsonValueString(Dep)));
	}
	RootObj->SetArrayField(TEXT("Dependencies"), DepsArray);

	TArray<FString> Refs = GetAssetReferencers(Asset);
	TArray<TSharedPtr<FJsonValue>> RefsArray;
	for (const FString& Ref : Refs)
	{
		RefsArray.Add(MakeShareable(new FJsonValueString(Ref)));
	}
	RootObj->SetArrayField(TEXT("Referencers"), RefsArray);

	return JsonObjectToString(RootObj);
}

// ============================================================================
// WIDGET HELPERS
// ============================================================================

UObject* UPythonBridge::GetWidgetTreeFromBP(UWidgetBlueprint* WidgetBlueprint)
{
	if (!WidgetBlueprint)
	{
		return nullptr;
	}
	return WidgetBlueprint->WidgetTree;
}

UObject* UPythonBridge::GetRootWidgetFromBP(UWidgetBlueprint* WidgetBlueprint)
{
	if (!WidgetBlueprint || !WidgetBlueprint->WidgetTree)
	{
		return nullptr;
	}
	return WidgetBlueprint->WidgetTree->RootWidget;
}

#endif // WITH_EDITOR
