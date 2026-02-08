// SLFAssetValidator.cpp
// Complete asset property export for validation

#include "SLFAssetValidator.h"
#include "Engine/Blueprint.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/DataTable.h"
#include "Engine/UserDefinedStruct.h"
#include "UObject/UnrealType.h"
#include "UObject/PropertyIterator.h"
#include "Components/Widget.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "WidgetBlueprint.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/UObjectIterator.h"

void USLFAssetValidator::ExportObjectProperties(UObject* Object, FString& OutText, int32 IndentLevel)
{
	if (!Object)
	{
		OutText += TEXT("null\n");
		return;
	}

	FString Indent = FString::ChrN(IndentLevel * 2, ' ');

	// Object header
	OutText += FString::Printf(TEXT("%sObject: %s (Class: %s)\n"),
		*Indent, *Object->GetName(), *Object->GetClass()->GetName());

	// Iterate all properties
	for (TFieldIterator<FProperty> PropIt(Object->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		if (!Property) continue;

		// Skip deprecated and transient properties
		if (Property->HasAnyPropertyFlags(CPF_Deprecated | CPF_Transient)) continue;

		FString PropName = Property->GetName();
		FString PropType = Property->GetCPPType();

		// Get property value as string
		FString ValueStr;
		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);

		if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
		{
			OutText += FString::Printf(TEXT("%s  %s (%s):\n"), *Indent, *PropName, *PropType);
			ExportStructProperties(ValuePtr, StructProp->Struct, OutText, IndentLevel + 2);
		}
		else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
		{
			UObject* ObjValue = ObjProp->GetObjectPropertyValue(ValuePtr);
			if (ObjValue)
			{
				OutText += FString::Printf(TEXT("%s  %s = %s (Path: %s)\n"),
					*Indent, *PropName, *ObjValue->GetName(), *ObjValue->GetPathName());
			}
			else
			{
				OutText += FString::Printf(TEXT("%s  %s = null\n"), *Indent, *PropName);
			}
		}
		else if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Property))
		{
			FSoftObjectPtr SoftPtr = SoftObjProp->GetPropertyValue(ValuePtr);
			OutText += FString::Printf(TEXT("%s  %s = %s\n"),
				*Indent, *PropName, *SoftPtr.ToString());
		}
		else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
		{
			FScriptArrayHelper ArrayHelper(ArrayProp, ValuePtr);
			OutText += FString::Printf(TEXT("%s  %s (Array, Count: %d):\n"),
				*Indent, *PropName, ArrayHelper.Num());

			for (int32 i = 0; i < FMath::Min(ArrayHelper.Num(), 100); i++) // Limit to 100 entries
			{
				const void* ElementPtr = ArrayHelper.GetRawPtr(i);
				if (FStructProperty* InnerStruct = CastField<FStructProperty>(ArrayProp->Inner))
				{
					OutText += FString::Printf(TEXT("%s    [%d]:\n"), *Indent, i);
					ExportStructProperties(ElementPtr, InnerStruct->Struct, OutText, IndentLevel + 3);
				}
				else
				{
					FString ElementStr;
					ArrayProp->Inner->ExportTextItem_Direct(ElementStr, ElementPtr, nullptr, nullptr, PPF_None);
					OutText += FString::Printf(TEXT("%s    [%d] = %s\n"), *Indent, i, *ElementStr);
				}
			}
		}
		else if (FMapProperty* MapProp = CastField<FMapProperty>(Property))
		{
			FScriptMapHelper MapHelper(MapProp, ValuePtr);
			OutText += FString::Printf(TEXT("%s  %s (Map, Count: %d):\n"),
				*Indent, *PropName, MapHelper.Num());

			int32 Count = 0;
			for (auto It = MapHelper.CreateIterator(); It && Count < 100; ++It, ++Count)
			{
				FString KeyStr, ValueStr2;
				MapProp->KeyProp->ExportTextItem_Direct(KeyStr, MapHelper.GetKeyPtr(It.GetInternalIndex()), nullptr, nullptr, PPF_None);
				MapProp->ValueProp->ExportTextItem_Direct(ValueStr2, MapHelper.GetValuePtr(It.GetInternalIndex()), nullptr, nullptr, PPF_None);
				OutText += FString::Printf(TEXT("%s    [%s] = %s\n"), *Indent, *KeyStr, *ValueStr2);
			}
		}
		else
		{
			Property->ExportTextItem_Direct(ValueStr, ValuePtr, nullptr, nullptr, PPF_None);
			OutText += FString::Printf(TEXT("%s  %s = %s\n"), *Indent, *PropName, *ValueStr);
		}
	}
}

void USLFAssetValidator::ExportStructProperties(const void* StructData, const UStruct* StructType, FString& OutText, int32 IndentLevel)
{
	if (!StructData || !StructType) return;

	FString Indent = FString::ChrN(IndentLevel * 2, ' ');

	for (TFieldIterator<FProperty> PropIt(StructType); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		if (!Property) continue;

		FString PropName = Property->GetName();
		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructData);

		if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
		{
			OutText += FString::Printf(TEXT("%s%s:\n"), *Indent, *PropName);
			ExportStructProperties(ValuePtr, StructProp->Struct, OutText, IndentLevel + 1);
		}
		else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
		{
			UObject* ObjValue = ObjProp->GetObjectPropertyValue(ValuePtr);
			if (ObjValue)
			{
				OutText += FString::Printf(TEXT("%s%s = %s\n"), *Indent, *PropName, *ObjValue->GetPathName());
			}
			else
			{
				OutText += FString::Printf(TEXT("%s%s = null\n"), *Indent, *PropName);
			}
		}
		else if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Property))
		{
			FSoftObjectPtr SoftPtr = SoftObjProp->GetPropertyValue(ValuePtr);
			OutText += FString::Printf(TEXT("%s%s = %s\n"), *Indent, *PropName, *SoftPtr.ToString());
		}
		else
		{
			FString ValueStr;
			Property->ExportTextItem_Direct(ValueStr, ValuePtr, nullptr, nullptr, PPF_None);
			OutText += FString::Printf(TEXT("%s%s = %s\n"), *Indent, *PropName, *ValueStr);
		}
	}
}

bool USLFAssetValidator::ExportBlueprintProperties(const FString& BlueprintPath, const FString& OutputPath)
{
	UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *BlueprintPath);
	if (!Blueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("[AssetValidator] Failed to load Blueprint: %s"), *BlueprintPath);
		return false;
	}

	FString ExportText;
	ExportText += FString::Printf(TEXT("=== BLUEPRINT EXPORT ===\n"));
	ExportText += FString::Printf(TEXT("Path: %s\n"), *BlueprintPath);
	ExportText += FString::Printf(TEXT("Class: %s\n"), *Blueprint->GetClass()->GetName());
	ExportText += FString::Printf(TEXT("ParentClass: %s\n"), Blueprint->ParentClass ? *Blueprint->ParentClass->GetName() : TEXT("None"));
	ExportText += FString::Printf(TEXT("GeneratedClass: %s\n"), Blueprint->GeneratedClass ? *Blueprint->GeneratedClass->GetName() : TEXT("None"));
	ExportText += TEXT("\n");

	// Export CDO (Class Default Object) properties
	if (Blueprint->GeneratedClass)
	{
		UObject* CDO = Blueprint->GeneratedClass->GetDefaultObject();
		if (CDO)
		{
			ExportText += TEXT("=== CLASS DEFAULT OBJECT ===\n");
			ExportObjectProperties(CDO, ExportText, 0);
			ExportText += TEXT("\n");
		}
	}

	// Export Blueprint variables
	ExportText += TEXT("=== BLUEPRINT VARIABLES ===\n");
	for (const FBPVariableDescription& Var : Blueprint->NewVariables)
	{
		ExportText += FString::Printf(TEXT("  %s (Type: %s, Category: %s)\n"),
			*Var.VarName.ToString(),
			*Var.VarType.PinCategory.ToString(),
			*Var.Category.ToString());

		// Export default value if present
		if (!Var.DefaultValue.IsEmpty())
		{
			ExportText += FString::Printf(TEXT("    DefaultValue: %s\n"), *Var.DefaultValue);
		}
	}
	ExportText += TEXT("\n");

	// Export SimpleConstructionScript (Components)
	if (Blueprint->SimpleConstructionScript)
	{
		ExportText += TEXT("=== COMPONENTS (SCS) ===\n");
		TArray<USCS_Node*> AllNodes = Blueprint->SimpleConstructionScript->GetAllNodes();
		for (USCS_Node* Node : AllNodes)
		{
			if (Node && Node->ComponentTemplate)
			{
				ExportText += FString::Printf(TEXT("  Component: %s (Class: %s)\n"),
					*Node->GetVariableName().ToString(),
					*Node->ComponentTemplate->GetClass()->GetName());

				// Export component properties
				ExportObjectProperties(Node->ComponentTemplate, ExportText, 2);
			}
		}
		ExportText += TEXT("\n");
	}

	// Save to file
	if (FFileHelper::SaveStringToFile(ExportText, *OutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[AssetValidator] Exported Blueprint to: %s"), *OutputPath);
		return true;
	}

	return false;
}

bool USLFAssetValidator::ExportWidgetProperties(const FString& WidgetPath, const FString& OutputPath)
{
	UBlueprint* WidgetBP = LoadObject<UBlueprint>(nullptr, *WidgetPath);
	if (!WidgetBP)
	{
		UE_LOG(LogTemp, Error, TEXT("[AssetValidator] Failed to load Widget: %s"), *WidgetPath);
		return false;
	}

	FString ExportText;
	ExportText += FString::Printf(TEXT("=== WIDGET BLUEPRINT EXPORT ===\n"));
	ExportText += FString::Printf(TEXT("Path: %s\n"), *WidgetPath);
	ExportText += FString::Printf(TEXT("ParentClass: %s\n"), WidgetBP->ParentClass ? *WidgetBP->ParentClass->GetName() : TEXT("None"));
	ExportText += TEXT("\n");

	// Export CDO
	if (WidgetBP->GeneratedClass)
	{
		UObject* CDO = WidgetBP->GeneratedClass->GetDefaultObject();
		if (CDO)
		{
			ExportText += TEXT("=== CLASS DEFAULT OBJECT ===\n");
			ExportObjectProperties(CDO, ExportText, 0);
			ExportText += TEXT("\n");
		}

		// Try to get widget tree from the WidgetBlueprint itself (not generated class)
		if (UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(WidgetBP))
		{
			ExportText += TEXT("=== WIDGET TREE ===\n");
			if (WidgetBlueprint->WidgetTree)
			{
				WidgetBlueprint->WidgetTree->ForEachWidget([&ExportText](UWidget* Widget)
				{
					if (Widget)
					{
						ExportText += FString::Printf(TEXT("  Widget: %s (Class: %s)\n"),
							*Widget->GetName(), *Widget->GetClass()->GetName());

						// Export key widget properties
						ExportText += FString::Printf(TEXT("    Visibility: %d\n"), (int32)Widget->GetVisibility());
						ExportText += FString::Printf(TEXT("    IsVisible: %s\n"), Widget->IsVisible() ? TEXT("true") : TEXT("false"));
					}
				});
			}
			ExportText += TEXT("\n");
		}
	}

	// Export variables
	ExportText += TEXT("=== WIDGET VARIABLES ===\n");
	for (const FBPVariableDescription& Var : WidgetBP->NewVariables)
	{
		ExportText += FString::Printf(TEXT("  %s (Type: %s)\n"),
			*Var.VarName.ToString(),
			*Var.VarType.PinCategory.ToString());
		if (!Var.DefaultValue.IsEmpty())
		{
			ExportText += FString::Printf(TEXT("    DefaultValue: %s\n"), *Var.DefaultValue);
		}
	}

	if (FFileHelper::SaveStringToFile(ExportText, *OutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[AssetValidator] Exported Widget to: %s"), *OutputPath);
		return true;
	}
	return false;
}

bool USLFAssetValidator::ExportDataAssetProperties(const FString& AssetPath, const FString& OutputPath)
{
	UObject* Asset = LoadObject<UObject>(nullptr, *AssetPath);
	if (!Asset)
	{
		UE_LOG(LogTemp, Error, TEXT("[AssetValidator] Failed to load DataAsset: %s"), *AssetPath);
		return false;
	}

	FString ExportText;
	ExportText += FString::Printf(TEXT("=== DATA ASSET EXPORT ===\n"));
	ExportText += FString::Printf(TEXT("Path: %s\n"), *AssetPath);
	ExportText += FString::Printf(TEXT("Class: %s\n"), *Asset->GetClass()->GetName());
	ExportText += TEXT("\n");

	ExportText += TEXT("=== PROPERTIES ===\n");
	ExportObjectProperties(Asset, ExportText, 0);

	if (FFileHelper::SaveStringToFile(ExportText, *OutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[AssetValidator] Exported DataAsset to: %s"), *OutputPath);
		return true;
	}
	return false;
}

bool USLFAssetValidator::ExportDataTableProperties(const FString& TablePath, const FString& OutputPath)
{
	UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *TablePath);
	if (!DataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[AssetValidator] Failed to load DataTable: %s"), *TablePath);
		return false;
	}

	FString ExportText;
	ExportText += FString::Printf(TEXT("=== DATA TABLE EXPORT ===\n"));
	ExportText += FString::Printf(TEXT("Path: %s\n"), *TablePath);
	ExportText += FString::Printf(TEXT("RowStruct: %s\n"), DataTable->GetRowStruct() ? *DataTable->GetRowStruct()->GetName() : TEXT("None"));
	ExportText += TEXT("\n");

	// Export all rows
	ExportText += TEXT("=== ROWS ===\n");
	TArray<FName> RowNames = DataTable->GetRowNames();
	ExportText += FString::Printf(TEXT("Row Count: %d\n\n"), RowNames.Num());

	for (const FName& RowName : RowNames)
	{
		ExportText += FString::Printf(TEXT("--- Row: %s ---\n"), *RowName.ToString());

		const uint8* RowData = DataTable->FindRowUnchecked(RowName);
		if (RowData && DataTable->GetRowStruct())
		{
			ExportStructProperties(RowData, DataTable->GetRowStruct(), ExportText, 1);
		}
		ExportText += TEXT("\n");
	}

	if (FFileHelper::SaveStringToFile(ExportText, *OutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[AssetValidator] Exported DataTable to: %s"), *OutputPath);
		return true;
	}
	return false;
}

bool USLFAssetValidator::CompareExports(const FString& File1Path, const FString& File2Path, const FString& DiffOutputPath)
{
	FString Content1, Content2;

	if (!FFileHelper::LoadFileToString(Content1, *File1Path))
	{
		UE_LOG(LogTemp, Error, TEXT("[AssetValidator] Failed to load: %s"), *File1Path);
		return false;
	}

	if (!FFileHelper::LoadFileToString(Content2, *File2Path))
	{
		UE_LOG(LogTemp, Error, TEXT("[AssetValidator] Failed to load: %s"), *File2Path);
		return false;
	}

	// Split into lines and compare
	TArray<FString> Lines1, Lines2;
	Content1.ParseIntoArrayLines(Lines1);
	Content2.ParseIntoArrayLines(Lines2);

	FString DiffText;
	DiffText += FString::Printf(TEXT("=== DIFF REPORT ===\n"));
	DiffText += FString::Printf(TEXT("File1: %s (%d lines)\n"), *File1Path, Lines1.Num());
	DiffText += FString::Printf(TEXT("File2: %s (%d lines)\n"), *File2Path, Lines2.Num());
	DiffText += TEXT("\n");

	int32 DiffCount = 0;
	int32 MaxLines = FMath::Max(Lines1.Num(), Lines2.Num());

	for (int32 i = 0; i < MaxLines; i++)
	{
		FString Line1 = i < Lines1.Num() ? Lines1[i] : TEXT("<missing>");
		FString Line2 = i < Lines2.Num() ? Lines2[i] : TEXT("<missing>");

		if (Line1 != Line2)
		{
			DiffCount++;
			DiffText += FString::Printf(TEXT("Line %d:\n"), i + 1);
			DiffText += FString::Printf(TEXT("  File1: %s\n"), *Line1);
			DiffText += FString::Printf(TEXT("  File2: %s\n"), *Line2);
			DiffText += TEXT("\n");
		}
	}

	DiffText += FString::Printf(TEXT("\n=== SUMMARY ===\n"));
	DiffText += FString::Printf(TEXT("Total differences: %d\n"), DiffCount);

	if (FFileHelper::SaveStringToFile(DiffText, *DiffOutputPath))
	{
		UE_LOG(LogTemp, Log, TEXT("[AssetValidator] Diff saved to: %s (Found %d differences)"), *DiffOutputPath, DiffCount);
		return true;
	}
	return false;
}

int32 USLFAssetValidator::ExportAllDialogAssets(const FString& OutputFolder)
{
	int32 ExportCount = 0;

	// Ensure output folder exists
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectoryTree(*OutputFolder);

	// Dialog Data Assets
	TArray<FString> DialogAssets = {
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog")
	};

	for (const FString& AssetPath : DialogAssets)
	{
		FString FileName = FPaths::GetBaseFilename(AssetPath);
		FString OutPath = FPaths::Combine(OutputFolder, FileName + TEXT("_DataAsset.txt"));
		if (ExportDataAssetProperties(AssetPath, OutPath))
		{
			ExportCount++;
		}
	}

	// Dialog DataTables
	TArray<FString> DialogTables = {
		TEXT("/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed"),
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic")
	};

	for (const FString& TablePath : DialogTables)
	{
		FString FileName = FPaths::GetBaseFilename(TablePath);
		FString OutPath = FPaths::Combine(OutputFolder, FileName + TEXT("_DataTable.txt"));
		if (ExportDataTableProperties(TablePath, OutPath))
		{
			ExportCount++;
		}
	}

	// Dialog-related Widgets
	TArray<FString> DialogWidgets = {
		TEXT("/Game/SoulslikeFramework/Widgets/HUD/W_HUD"),
		TEXT("/Game/SoulslikeFramework/Widgets/HUD/W_Dialog"),
		TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window"),
		TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor")
	};

	for (const FString& WidgetPath : DialogWidgets)
	{
		FString FileName = FPaths::GetBaseFilename(WidgetPath);
		FString OutPath = FPaths::Combine(OutputFolder, FileName + TEXT("_Widget.txt"));
		if (ExportWidgetProperties(WidgetPath, OutPath))
		{
			ExportCount++;
		}
	}

	// NPC Blueprints
	TArray<FString> NPCBlueprints = {
		TEXT("/Game/SoulslikeFramework/Blueprints/Characters/NPCs/B_Soulslike_NPC"),
		TEXT("/Game/SoulslikeFramework/Blueprints/Characters/NPCs/B_Soulslike_NPC_ShowcaseGuide"),
		TEXT("/Game/SoulslikeFramework/Blueprints/Characters/NPCs/B_Soulslike_NPC_ShowcaseVendor")
	};

	for (const FString& BPPath : NPCBlueprints)
	{
		FString FileName = FPaths::GetBaseFilename(BPPath);
		FString OutPath = FPaths::Combine(OutputFolder, FileName + TEXT("_Blueprint.txt"));
		if (ExportBlueprintProperties(BPPath, OutPath))
		{
			ExportCount++;
		}
	}

	// Components
	TArray<FString> Components = {
		TEXT("/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager")
	};

	for (const FString& CompPath : Components)
	{
		FString FileName = FPaths::GetBaseFilename(CompPath);
		FString OutPath = FPaths::Combine(OutputFolder, FileName + TEXT("_Component.txt"));
		if (ExportBlueprintProperties(CompPath, OutPath))
		{
			ExportCount++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[AssetValidator] Exported %d dialog-related assets to: %s"), ExportCount, *OutputFolder);
	return ExportCount;
}
