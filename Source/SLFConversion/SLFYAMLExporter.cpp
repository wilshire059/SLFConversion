// SLFYAMLExporter.cpp
// Complete implementation for YAML asset documentation

#include "SLFYAMLExporter.h"
#if WITH_EDITOR
#include "Engine/Blueprint.h"
#include "Engine/DataTable.h"
#include "Engine/UserDefinedStruct.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "UObject/UnrealType.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "WidgetBlueprint.h"
#include "Components/Widget.h"
#include "Components/PanelWidget.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"
#include "GameplayTagContainer.h"

FString USLFYAMLExporter::GetIndent(int32 Level)
{
	return FString::ChrN(Level * 2, ' ');
}

FString USLFYAMLExporter::EscapeYAMLString(const FString& Input)
{
	FString Output = Input;
	Output.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
	Output.ReplaceInline(TEXT("\""), TEXT("\\\""));
	Output.ReplaceInline(TEXT("\n"), TEXT("\\n"));
	Output.ReplaceInline(TEXT("\r"), TEXT("\\r"));
	Output.ReplaceInline(TEXT("\t"), TEXT("\\t"));

	// If string contains special YAML chars, quote it
	if (Output.Contains(TEXT(":")) || Output.Contains(TEXT("#")) ||
		Output.Contains(TEXT("[")) || Output.Contains(TEXT("]")) ||
		Output.Contains(TEXT("{")) || Output.Contains(TEXT("}")) ||
		Output.StartsWith(TEXT(" ")) || Output.EndsWith(TEXT(" ")))
	{
		return FString::Printf(TEXT("\"%s\""), *Output);
	}
	return Output;
}

FString USLFYAMLExporter::FTextToString(const FText& Text)
{
	if (Text.IsEmpty())
	{
		return TEXT("");
	}
	return Text.ToString();
}

void USLFYAMLExporter::PropertyToYAML(FProperty* Property, const void* ValuePtr, FString& OutYAML, int32 IndentLevel)
{
	if (!Property || !ValuePtr) return;

	FString Indent = GetIndent(IndentLevel);
	FString PropName = Property->GetName();

	// Handle different property types
	if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		OutYAML += FString::Printf(TEXT("%s%s:\n"), *Indent, *PropName);
		StructToYAML(ValuePtr, StructProp->Struct, OutYAML, IndentLevel + 1);
	}
	else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
	{
		UObject* ObjValue = ObjProp->GetObjectPropertyValue(ValuePtr);
		if (ObjValue)
		{
			OutYAML += FString::Printf(TEXT("%s%s:\n"), *Indent, *PropName);
			OutYAML += FString::Printf(TEXT("%s  class: %s\n"), *Indent, *ObjValue->GetClass()->GetName());
			OutYAML += FString::Printf(TEXT("%s  name: %s\n"), *Indent, *ObjValue->GetName());
			OutYAML += FString::Printf(TEXT("%s  path: %s\n"), *Indent, *EscapeYAMLString(ObjValue->GetPathName()));
		}
		else
		{
			OutYAML += FString::Printf(TEXT("%s%s: null\n"), *Indent, *PropName);
		}
	}
	else if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Property))
	{
		FSoftObjectPtr SoftPtr = SoftObjProp->GetPropertyValue(ValuePtr);
		if (SoftPtr.IsNull())
		{
			OutYAML += FString::Printf(TEXT("%s%s: null\n"), *Indent, *PropName);
		}
		else
		{
			OutYAML += FString::Printf(TEXT("%s%s: %s\n"), *Indent, *PropName, *EscapeYAMLString(SoftPtr.ToString()));
		}
	}
	else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
	{
		FScriptArrayHelper ArrayHelper(ArrayProp, ValuePtr);
		OutYAML += FString::Printf(TEXT("%s%s:\n"), *Indent, *PropName);
		OutYAML += FString::Printf(TEXT("%s  count: %d\n"), *Indent, ArrayHelper.Num());
		OutYAML += FString::Printf(TEXT("%s  items:\n"), *Indent);

		for (int32 i = 0; i < ArrayHelper.Num(); i++)
		{
			const void* ElementPtr = ArrayHelper.GetRawPtr(i);
			OutYAML += FString::Printf(TEXT("%s    - index: %d\n"), *Indent, i);

			if (FStructProperty* InnerStruct = CastField<FStructProperty>(ArrayProp->Inner))
			{
				StructToYAML(ElementPtr, InnerStruct->Struct, OutYAML, IndentLevel + 3);
			}
			else
			{
				FString ElementStr;
				ArrayProp->Inner->ExportTextItem_Direct(ElementStr, ElementPtr, nullptr, nullptr, PPF_None);
				OutYAML += FString::Printf(TEXT("%s      value: %s\n"), *Indent, *EscapeYAMLString(ElementStr));
			}
		}
	}
	else if (FMapProperty* MapProp = CastField<FMapProperty>(Property))
	{
		FScriptMapHelper MapHelper(MapProp, ValuePtr);
		OutYAML += FString::Printf(TEXT("%s%s:\n"), *Indent, *PropName);
		OutYAML += FString::Printf(TEXT("%s  count: %d\n"), *Indent, MapHelper.Num());
		OutYAML += FString::Printf(TEXT("%s  entries:\n"), *Indent);

		int32 EntryIndex = 0;
		for (auto It = MapHelper.CreateIterator(); It; ++It, ++EntryIndex)
		{
			FString KeyStr, ValueStr;
			MapProp->KeyProp->ExportTextItem_Direct(KeyStr, MapHelper.GetKeyPtr(It.GetInternalIndex()), nullptr, nullptr, PPF_None);
			MapProp->ValueProp->ExportTextItem_Direct(ValueStr, MapHelper.GetValuePtr(It.GetInternalIndex()), nullptr, nullptr, PPF_None);
			OutYAML += FString::Printf(TEXT("%s    - key: %s\n"), *Indent, *EscapeYAMLString(KeyStr));
			OutYAML += FString::Printf(TEXT("%s      value: %s\n"), *Indent, *EscapeYAMLString(ValueStr));
		}
	}
	else if (FTextProperty* TextProp = CastField<FTextProperty>(Property))
	{
		FText TextValue = TextProp->GetPropertyValue(ValuePtr);
		OutYAML += FString::Printf(TEXT("%s%s: %s\n"), *Indent, *PropName, *EscapeYAMLString(FTextToString(TextValue)));
	}
	else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
	{
		FString StrValue = StrProp->GetPropertyValue(ValuePtr);
		OutYAML += FString::Printf(TEXT("%s%s: %s\n"), *Indent, *PropName, *EscapeYAMLString(StrValue));
	}
	else if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
	{
		FName NameValue = NameProp->GetPropertyValue(ValuePtr);
		OutYAML += FString::Printf(TEXT("%s%s: %s\n"), *Indent, *PropName, *EscapeYAMLString(NameValue.ToString()));
	}
	else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
	{
		bool BoolValue = BoolProp->GetPropertyValue(ValuePtr);
		OutYAML += FString::Printf(TEXT("%s%s: %s\n"), *Indent, *PropName, BoolValue ? TEXT("true") : TEXT("false"));
	}
	else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
	{
		int32 IntValue = IntProp->GetPropertyValue(ValuePtr);
		OutYAML += FString::Printf(TEXT("%s%s: %d\n"), *Indent, *PropName, IntValue);
	}
	else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	{
		float FloatValue = FloatProp->GetPropertyValue(ValuePtr);
		OutYAML += FString::Printf(TEXT("%s%s: %f\n"), *Indent, *PropName, FloatValue);
	}
	else if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
	{
		double DoubleValue = DoubleProp->GetPropertyValue(ValuePtr);
		OutYAML += FString::Printf(TEXT("%s%s: %f\n"), *Indent, *PropName, DoubleValue);
	}
	else if (FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
	{
		FString EnumStr;
		EnumProp->ExportTextItem_Direct(EnumStr, ValuePtr, nullptr, nullptr, PPF_None);
		OutYAML += FString::Printf(TEXT("%s%s: %s\n"), *Indent, *PropName, *EscapeYAMLString(EnumStr));
	}
	else if (FByteProperty* ByteProp = CastField<FByteProperty>(Property))
	{
		if (ByteProp->Enum)
		{
			uint8 ByteValue = ByteProp->GetPropertyValue(ValuePtr);
			FString EnumName = ByteProp->Enum->GetNameStringByValue(ByteValue);
			OutYAML += FString::Printf(TEXT("%s%s: %s\n"), *Indent, *PropName, *EscapeYAMLString(EnumName));
		}
		else
		{
			uint8 ByteValue = ByteProp->GetPropertyValue(ValuePtr);
			OutYAML += FString::Printf(TEXT("%s%s: %d\n"), *Indent, *PropName, ByteValue);
		}
	}
	else
	{
		// Generic fallback using ExportTextItem
		FString ValueStr;
		Property->ExportTextItem_Direct(ValueStr, ValuePtr, nullptr, nullptr, PPF_None);
		OutYAML += FString::Printf(TEXT("%s%s: %s\n"), *Indent, *PropName, *EscapeYAMLString(ValueStr));
	}
}

void USLFYAMLExporter::StructToYAML(const void* StructData, const UStruct* StructType, FString& OutYAML, int32 IndentLevel)
{
	if (!StructData || !StructType) return;

	for (TFieldIterator<FProperty> PropIt(StructType); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		if (!Property) continue;

		// Skip deprecated and transient
		if (Property->HasAnyPropertyFlags(CPF_Deprecated | CPF_Transient)) continue;

		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructData);
		PropertyToYAML(Property, ValuePtr, OutYAML, IndentLevel);
	}
}

void USLFYAMLExporter::ObjectToYAML(UObject* Object, FString& OutYAML, int32 IndentLevel)
{
	if (!Object) return;

	FString Indent = GetIndent(IndentLevel);

	// Object header
	OutYAML += FString::Printf(TEXT("%sclass: %s\n"), *Indent, *Object->GetClass()->GetName());
	OutYAML += FString::Printf(TEXT("%sname: %s\n"), *Indent, *Object->GetName());
	OutYAML += FString::Printf(TEXT("%spath: %s\n"), *Indent, *EscapeYAMLString(Object->GetPathName()));
	OutYAML += FString::Printf(TEXT("%sproperties:\n"), *Indent);

	// Export all properties
	for (TFieldIterator<FProperty> PropIt(Object->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		if (!Property) continue;

		// Skip deprecated, transient, and editor-only
		if (Property->HasAnyPropertyFlags(CPF_Deprecated | CPF_Transient)) continue;

		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);
		PropertyToYAML(Property, ValuePtr, OutYAML, IndentLevel + 1);
	}
}

bool USLFYAMLExporter::ExportBlueprintToYAML(const FString& BlueprintPath, const FString& OutputPath)
{
	UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *BlueprintPath);
	if (!Blueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("[YAMLExporter] Failed to load Blueprint: %s"), *BlueprintPath);
		return false;
	}

	FString YAML;
	YAML += TEXT("# Blueprint Export - AUTO-GENERATED\n");
	YAML += TEXT("# DO NOT EDIT - This is the source of truth from bp_only\n\n");

	YAML += TEXT("blueprint:\n");
	YAML += FString::Printf(TEXT("  path: %s\n"), *EscapeYAMLString(BlueprintPath));
	YAML += FString::Printf(TEXT("  class: %s\n"), *Blueprint->GetClass()->GetName());
	YAML += FString::Printf(TEXT("  parent_class: %s\n"), Blueprint->ParentClass ? *Blueprint->ParentClass->GetName() : TEXT("None"));
	YAML += FString::Printf(TEXT("  generated_class: %s\n"), Blueprint->GeneratedClass ? *Blueprint->GeneratedClass->GetName() : TEXT("None"));
	YAML += TEXT("\n");

	// Blueprint Variables
	YAML += TEXT("variables:\n");
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		YAML += FString::Printf(TEXT("  - name: %s\n"), *Var.VarName.ToString());
		YAML += FString::Printf(TEXT("    type: %s\n"), *Var.VarType.PinCategory.ToString());
		YAML += FString::Printf(TEXT("    subcategory: %s\n"), *Var.VarType.PinSubCategory.ToString());
		YAML += FString::Printf(TEXT("    category: %s\n"), *EscapeYAMLString(Var.Category.ToString()));
		YAML += FString::Printf(TEXT("    replication: %d\n"), (int32)Var.ReplicationCondition);
		if (!Var.DefaultValue.IsEmpty())
		{
			YAML += FString::Printf(TEXT("    default_value: %s\n"), *EscapeYAMLString(Var.DefaultValue));
		}
		YAML += TEXT("\n");
	}

	// Class Default Object
	if (Blueprint->GeneratedClass)
	{
		UObject* CDO = Blueprint->GeneratedClass->GetDefaultObject();
		if (CDO)
		{
			YAML += TEXT("class_default_object:\n");
			ObjectToYAML(CDO, YAML, 1);
			YAML += TEXT("\n");
		}
	}

	// Components (SCS)
	if (Blueprint->SimpleConstructionScript)
	{
		YAML += TEXT("components:\n");
		TArray<USCS_Node*> AllNodes = Blueprint->SimpleConstructionScript->GetAllNodes();
		for (USCS_Node* Node : AllNodes)
		{
			if (Node && Node->ComponentTemplate)
			{
				YAML += FString::Printf(TEXT("  - variable_name: %s\n"), *Node->GetVariableName().ToString());
				YAML += FString::Printf(TEXT("    component_class: %s\n"), *Node->ComponentTemplate->GetClass()->GetName());
				YAML += FString::Printf(TEXT("    parent_variable: %s\n"), Node->ParentComponentOrVariableName.IsNone() ? TEXT("None") : *Node->ParentComponentOrVariableName.ToString());
				YAML += TEXT("    properties:\n");
				ObjectToYAML(Node->ComponentTemplate, YAML, 3);
				YAML += TEXT("\n");
			}
		}
	}

	// Save
	if (FFileHelper::SaveStringToFile(YAML, *OutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[YAMLExporter] Exported Blueprint to: %s"), *OutputPath);
		return true;
	}

	return false;
}

bool USLFYAMLExporter::ExportWidgetToYAML(const FString& WidgetPath, const FString& OutputPath)
{
	UWidgetBlueprint* WidgetBP = LoadObject<UWidgetBlueprint>(nullptr, *WidgetPath);
	if (!WidgetBP)
	{
		UE_LOG(LogTemp, Error, TEXT("[YAMLExporter] Failed to load Widget: %s"), *WidgetPath);
		return false;
	}

	FString YAML;
	YAML += TEXT("# Widget Blueprint Export - AUTO-GENERATED\n");
	YAML += TEXT("# DO NOT EDIT - This is the source of truth from bp_only\n\n");

	YAML += TEXT("widget:\n");
	YAML += FString::Printf(TEXT("  path: %s\n"), *EscapeYAMLString(WidgetPath));
	YAML += FString::Printf(TEXT("  class: %s\n"), *WidgetBP->GetClass()->GetName());
	YAML += FString::Printf(TEXT("  parent_class: %s\n"), WidgetBP->ParentClass ? *WidgetBP->ParentClass->GetName() : TEXT("None"));
	YAML += TEXT("\n");

	// Variables
	YAML += TEXT("variables:\n");
	for (const FBPVariableDescription& Var : WidgetBP->NewVariables)
	{
		YAML += FString::Printf(TEXT("  - name: %s\n"), *Var.VarName.ToString());
		YAML += FString::Printf(TEXT("    type: %s\n"), *Var.VarType.PinCategory.ToString());
		YAML += FString::Printf(TEXT("    subcategory: %s\n"), *Var.VarType.PinSubCategory.ToString());
		if (Var.VarType.PinSubCategoryObject.IsValid())
		{
			YAML += FString::Printf(TEXT("    subcategory_object: %s\n"), *Var.VarType.PinSubCategoryObject->GetName());
		}
		if (!Var.DefaultValue.IsEmpty())
		{
			YAML += FString::Printf(TEXT("    default_value: %s\n"), *EscapeYAMLString(Var.DefaultValue));
		}
		YAML += TEXT("\n");
	}

	// Widget Tree
	YAML += TEXT("widget_tree:\n");
	if (WidgetBP->WidgetTree)
	{
		TArray<UWidget*> AllWidgets;
		WidgetBP->WidgetTree->ForEachWidget([&AllWidgets](UWidget* Widget)
		{
			if (Widget)
			{
				AllWidgets.Add(Widget);
			}
		});

		for (UWidget* Widget : AllWidgets)
		{
			YAML += FString::Printf(TEXT("  - name: %s\n"), *Widget->GetName());
			YAML += FString::Printf(TEXT("    class: %s\n"), *Widget->GetClass()->GetName());
			YAML += FString::Printf(TEXT("    visibility: %d\n"), (int32)Widget->GetVisibility());
			YAML += FString::Printf(TEXT("    is_visible: %s\n"), Widget->IsVisible() ? TEXT("true") : TEXT("false"));

			// Check if it's a panel with children
			if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
			{
				YAML += FString::Printf(TEXT("    child_count: %d\n"), Panel->GetChildrenCount());
			}

			// Export widget-specific properties
			YAML += TEXT("    properties:\n");
			for (TFieldIterator<FProperty> PropIt(Widget->GetClass()); PropIt; ++PropIt)
			{
				FProperty* Property = *PropIt;
				if (!Property) continue;
				if (Property->HasAnyPropertyFlags(CPF_Deprecated | CPF_Transient)) continue;

				// Only export properties defined on this class, not inherited
				if (Property->GetOwnerClass() == Widget->GetClass())
				{
					const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Widget);
					PropertyToYAML(Property, ValuePtr, YAML, 3);
				}
			}
			YAML += TEXT("\n");
		}
	}

	// CDO
	if (WidgetBP->GeneratedClass)
	{
		UObject* CDO = WidgetBP->GeneratedClass->GetDefaultObject();
		if (CDO)
		{
			YAML += TEXT("class_default_object:\n");
			ObjectToYAML(CDO, YAML, 1);
		}
	}

	if (FFileHelper::SaveStringToFile(YAML, *OutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[YAMLExporter] Exported Widget to: %s"), *OutputPath);
		return true;
	}

	return false;
}

bool USLFYAMLExporter::ExportDataTableToYAML(const FString& TablePath, const FString& OutputPath)
{
	UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *TablePath);
	if (!DataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[YAMLExporter] Failed to load DataTable: %s"), *TablePath);
		return false;
	}

	FString YAML;
	YAML += TEXT("# DataTable Export - AUTO-GENERATED\n");
	YAML += TEXT("# DO NOT EDIT - This is the source of truth from bp_only\n\n");

	YAML += TEXT("datatable:\n");
	YAML += FString::Printf(TEXT("  path: %s\n"), *EscapeYAMLString(TablePath));
	YAML += FString::Printf(TEXT("  row_struct: %s\n"), DataTable->GetRowStruct() ? *DataTable->GetRowStruct()->GetName() : TEXT("None"));

	TArray<FName> RowNames = DataTable->GetRowNames();
	YAML += FString::Printf(TEXT("  row_count: %d\n"), RowNames.Num());
	YAML += TEXT("\n");

	YAML += TEXT("rows:\n");
	for (const FName& RowName : RowNames)
	{
		YAML += FString::Printf(TEXT("  - row_name: %s\n"), *RowName.ToString());

		const uint8* RowData = DataTable->FindRowUnchecked(RowName);
		if (RowData && DataTable->GetRowStruct())
		{
			YAML += TEXT("    columns:\n");
			StructToYAML(RowData, DataTable->GetRowStruct(), YAML, 3);
		}
		YAML += TEXT("\n");
	}

	if (FFileHelper::SaveStringToFile(YAML, *OutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[YAMLExporter] Exported DataTable to: %s"), *OutputPath);
		return true;
	}

	return false;
}

bool USLFYAMLExporter::ExportDataAssetToYAML(const FString& AssetPath, const FString& OutputPath)
{
	UObject* Asset = LoadObject<UObject>(nullptr, *AssetPath);
	if (!Asset)
	{
		UE_LOG(LogTemp, Error, TEXT("[YAMLExporter] Failed to load DataAsset: %s"), *AssetPath);
		return false;
	}

	FString YAML;
	YAML += TEXT("# Data Asset Export - AUTO-GENERATED\n");
	YAML += TEXT("# DO NOT EDIT - This is the source of truth from bp_only\n\n");

	YAML += TEXT("data_asset:\n");
	YAML += FString::Printf(TEXT("  path: %s\n"), *EscapeYAMLString(AssetPath));
	YAML += FString::Printf(TEXT("  class: %s\n"), *Asset->GetClass()->GetName());
	YAML += TEXT("\n");

	YAML += TEXT("properties:\n");
	ObjectToYAML(Asset, YAML, 1);

	if (FFileHelper::SaveStringToFile(YAML, *OutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[YAMLExporter] Exported DataAsset to: %s"), *OutputPath);
		return true;
	}

	return false;
}

bool USLFYAMLExporter::ExportStructToYAML(const FString& StructPath, const FString& OutputPath)
{
	UUserDefinedStruct* Struct = LoadObject<UUserDefinedStruct>(nullptr, *StructPath);
	if (!Struct)
	{
		UE_LOG(LogTemp, Error, TEXT("[YAMLExporter] Failed to load Struct: %s"), *StructPath);
		return false;
	}

	FString YAML;
	YAML += TEXT("# Struct Export - AUTO-GENERATED\n");
	YAML += TEXT("# DO NOT EDIT - This is the source of truth from bp_only\n\n");

	YAML += TEXT("struct:\n");
	YAML += FString::Printf(TEXT("  path: %s\n"), *EscapeYAMLString(StructPath));
	YAML += FString::Printf(TEXT("  name: %s\n"), *Struct->GetName());
	YAML += TEXT("\n");

	YAML += TEXT("fields:\n");
	for (TFieldIterator<FProperty> PropIt(Struct); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		if (!Property) continue;

		YAML += FString::Printf(TEXT("  - name: %s\n"), *Property->GetName());
		YAML += FString::Printf(TEXT("    type: %s\n"), *Property->GetCPPType());
		YAML += FString::Printf(TEXT("    offset: %d\n"), Property->GetOffset_ForInternal());
		YAML += FString::Printf(TEXT("    size: %d\n"), Property->GetSize());
		YAML += TEXT("\n");
	}

	if (FFileHelper::SaveStringToFile(YAML, *OutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[YAMLExporter] Exported Struct to: %s"), *OutputPath);
		return true;
	}

	return false;
}

int32 USLFYAMLExporter::ExportNPCSystemToYAML(const FString& OutputFolder)
{
	int32 ExportCount = 0;

	// Create subdirectories
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectoryTree(*FPaths::Combine(OutputFolder, TEXT("blueprints")));
	PlatformFile.CreateDirectoryTree(*FPaths::Combine(OutputFolder, TEXT("components")));
	PlatformFile.CreateDirectoryTree(*FPaths::Combine(OutputFolder, TEXT("data")));
	PlatformFile.CreateDirectoryTree(*FPaths::Combine(OutputFolder, TEXT("datatables")));
	PlatformFile.CreateDirectoryTree(*FPaths::Combine(OutputFolder, TEXT("widgets")));
	PlatformFile.CreateDirectoryTree(*FPaths::Combine(OutputFolder, TEXT("structures")));

	// ═══════════════════════════════════════════════════════════════════
	// NPC BLUEPRINTS
	// ═══════════════════════════════════════════════════════════════════
	TArray<FString> NPCBlueprints = {
		TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC"),
		TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide"),
		TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor")
	};

	for (const FString& BPPath : NPCBlueprints)
	{
		FString FileName = FPaths::GetBaseFilename(BPPath) + TEXT(".yaml");
		FString OutPath = FPaths::Combine(OutputFolder, TEXT("blueprints"), FileName);
		if (ExportBlueprintToYAML(BPPath, OutPath)) ExportCount++;
	}

	// ═══════════════════════════════════════════════════════════════════
	// COMPONENTS
	// ═══════════════════════════════════════════════════════════════════
	TArray<FString> Components = {
		TEXT("/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager")
	};

	for (const FString& CompPath : Components)
	{
		FString FileName = FPaths::GetBaseFilename(CompPath) + TEXT(".yaml");
		FString OutPath = FPaths::Combine(OutputFolder, TEXT("components"), FileName);
		if (ExportBlueprintToYAML(CompPath, OutPath)) ExportCount++;
	}

	// ═══════════════════════════════════════════════════════════════════
	// DATA ASSETS
	// ═══════════════════════════════════════════════════════════════════
	TArray<FString> DataAssets = {
		TEXT("/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor"),
		TEXT("/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor"),
		TEXT("/Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor")
	};

	for (const FString& AssetPath : DataAssets)
	{
		FString FileName = FPaths::GetBaseFilename(AssetPath) + TEXT(".yaml");
		FString OutPath = FPaths::Combine(OutputFolder, TEXT("data"), FileName);
		if (ExportDataAssetToYAML(AssetPath, OutPath)) ExportCount++;
	}

	// ═══════════════════════════════════════════════════════════════════
	// DATATABLES
	// ═══════════════════════════════════════════════════════════════════
	TArray<FString> DataTables = {
		TEXT("/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic")
	};

	for (const FString& TablePath : DataTables)
	{
		FString FileName = FPaths::GetBaseFilename(TablePath) + TEXT(".yaml");
		FString OutPath = FPaths::Combine(OutputFolder, TEXT("datatables"), FileName);
		if (ExportDataTableToYAML(TablePath, OutPath)) ExportCount++;
	}

	// ═══════════════════════════════════════════════════════════════════
	// WIDGETS
	// ═══════════════════════════════════════════════════════════════════
	TArray<FString> Widgets = {
		TEXT("/Game/SoulslikeFramework/Widgets/HUD/W_Dialog"),
		TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window"),
		TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor"),
		TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot"),
		TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_VendorAction")
	};

	for (const FString& WidgetPath : Widgets)
	{
		FString FileName = FPaths::GetBaseFilename(WidgetPath) + TEXT(".yaml");
		FString OutPath = FPaths::Combine(OutputFolder, TEXT("widgets"), FileName);
		if (ExportWidgetToYAML(WidgetPath, OutPath)) ExportCount++;
	}

	// ═══════════════════════════════════════════════════════════════════
	// STRUCTURES
	// ═══════════════════════════════════════════════════════════════════
	TArray<FString> Structures = {
		TEXT("/Game/SoulslikeFramework/Structures/Dialog/FDialogEntry"),
		TEXT("/Game/SoulslikeFramework/Structures/Dialog/FDialogGameplayEvent"),
		TEXT("/Game/SoulslikeFramework/Structures/Dialog/FDialogProgress"),
		TEXT("/Game/SoulslikeFramework/Structures/Dialog/FDialogRequirement"),
		TEXT("/Game/SoulslikeFramework/Structures/Vendor/FVendorItems")
	};

	for (const FString& StructPath : Structures)
	{
		FString FileName = FPaths::GetBaseFilename(StructPath) + TEXT(".yaml");
		FString OutPath = FPaths::Combine(OutputFolder, TEXT("structures"), FileName);
		if (ExportStructToYAML(StructPath, OutPath)) ExportCount++;
	}

	UE_LOG(LogTemp, Log, TEXT("[YAMLExporter] Exported %d NPC system assets to: %s"), ExportCount, *OutputFolder);
	return ExportCount;
}

#endif // WITH_EDITOR
