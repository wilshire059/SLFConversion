"""
extract_tracking_json.py

Extracts all migration-relevant information from Blueprint JSON exports
and creates tracking JSON files for the 3-pass validation system.

Usage:
    python extract_tracking_json.py <blueprint_json_path> [output_dir]

Or to extract all Blueprints in a category:
    python extract_tracking_json.py --all-components
    python extract_tracking_json.py --all-interfaces
    python extract_tracking_json.py --all-blueprints
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Any, Optional

# Paths - Updated to use v2 exports with TMap KeyTypes
BP_EXPORT_DIR = Path(r"C:\scripts\SLFConversion\Exports\BlueprintDNA_v2")
TRACKING_DIR = Path(r"C:\scripts\SLFConversion\MigrationTracking")

# Type mapping from Blueprint JSON to C++
TYPE_MAP = {
    ("bool", None): "bool",
    ("byte", None): "uint8",
    ("int", None): "int32",
    ("int64", None): "int64",
    ("real", "float"): "float",
    ("real", "double"): "double",
    ("name", None): "FName",
    ("string", None): "FString",
    ("text", None): "FText",
    ("struct", "Vector"): "FVector",
    ("struct", "Vector2D"): "FVector2D",
    ("struct", "Rotator"): "FRotator",
    ("struct", "Transform"): "FTransform",
    ("struct", "LinearColor"): "FLinearColor",
    ("struct", "Color"): "FColor",
    ("struct", "GameplayTag"): "FGameplayTag",
    ("struct", "GameplayTagContainer"): "FGameplayTagContainer",
    ("struct", "Guid"): "FGuid",
    ("struct", "Key"): "FKey",
    ("struct", "TimerHandle"): "FTimerHandle",
    ("object", "Actor"): "AActor*",
    ("object", "ActorComponent"): "UActorComponent*",
    ("object", "SceneComponent"): "USceneComponent*",
    ("object", "AnimMontage"): "UAnimMontage*",
    ("object", "AnimSequenceBase"): "UAnimSequenceBase*",
    ("object", "BlendSpace"): "UBlendSpace*",
    ("object", "DataTable"): "UDataTable*",
    ("object", "PrimaryDataAsset"): "UPrimaryDataAsset*",
    ("object", "Texture2D"): "UTexture2D*",
    ("object", "StaticMesh"): "UStaticMesh*",
    ("object", "SkeletalMesh"): "USkeletalMesh*",
    ("object", "NiagaraSystem"): "UNiagaraSystem*",
    ("object", "SoundBase"): "USoundBase*",
    ("object", "LevelSequence"): "ULevelSequence*",
    ("object", "Object"): "UObject*",
    ("object", "Class"): "UClass*",
    ("class", None): "TSubclassOf<UObject>",
    ("softobject", None): "TSoftObjectPtr<UObject>",
    ("softclass", None): "TSoftClassPtr<UObject>",
}

# Enum mappings (Blueprint enum name -> C++ enum)
ENUM_MAP = {
    "E_Direction": "ESLFDirection",
    "E_ValueType": "ESLFValueType",
    "E_AI_States": "ESLFAIStates",
    "E_AI_StateHandle": "ESLFAIStateHandle",
    "E_AttackPower": "ESLFAttackPower",
    "E_ItemCategory": "ESLFItemCategory",
    "E_ItemSubCategory": "ESLFItemSubCategory",
    "E_StatCategory": "ESLFStatCategory",
    "E_StatScaling": "ESLFStatScaling",
    "E_ExecutionType": "ESLFExecutionType",
    "E_HitReactType": "ESLFHitReactType",
    "E_OverlayState": "ESLFOverlayState",
    "E_MovementType": "ESLFMovementType",
    "E_ActionWeaponSlot": "ESLFActionWeaponSlot",
    "E_TraceType": "ESLFTraceType",
    "E_Progress": "ESLFProgress",
    "E_FadeTypes": "ESLFFadeTypes",
    "E_SaveBehavior": "ESLFSaveBehavior",
    "E_SettingCategory": "ESLFSettingCategory",
    "E_SettingEntry": "ESLFSettingEntry",
    "E_VendorType": "ESLFVendorType",
    "E_LadderClimbState": "ESLFLadderClimbState",
    "E_AI_BossEncounterType": "ESLFAIBossEncounterType",
    "E_AI_Senses": "ESLFAISenses",
    "E_AI_StrafeLocations": "ESLFAIStrafeLocations",
    "E_CalculationType": "ESLFCalculationType",
    "E_DebugWindowScaleType": "ESLFDebugWindowScaleType",
    "E_DotProductThreshold": "ESLFDotProductThreshold",
    "E_EnclosureLevel": "ESLFEnclosureLevel",
    "E_InputAction": "ESLFInputAction",
    "E_InventoryAmountedActionType": "ESLFInventoryAmountedActionType",
    "E_InventorySlotType": "ESLFInventorySlotType",
    "E_KeyType": "ESLFKeyType",
    "E_LightningMode": "ESLFLightningMode",
    "E_MontageSection": "ESLFMontageSection",
    "E_NpcState": "ESLFNpcState",
    "E_RoomLighting": "ESLFRoomLighting",
    "E_WeaponAbilityHandle": "ESLFWeaponAbilityHandle",
    "E_WorldMeshStyle": "ESLFWorldMeshStyle",
}

# Struct mappings (Blueprint struct name -> C++ struct)
STRUCT_MAP = {
    "FStatInfo": "FSLFStatInfo",
    "FStatOverride": "FSLFStatOverride",
    "FRegen": "FSLFRegen",
    "FStatBehavior": "FSLFStatBehavior",
    "FAffectedStat": "FSLFAffectedStat",
    "FAffectedStats": "FSLFAffectedStats",
    "FStatChange": "FStatChange",
    "FStatChangePercent": "FStatChangePercent",
    "FStatEntry": "FSLFStatEntry",
    "FDodgeMontages": "FSLFDodgeMontages",
    "FActionsData": "FSLFActionsData",
    "FAnimationData": "FSLFAnimationData",
    "FMontage": "FSLFMontage",
    "FExecutionInfo": "FSLFExecutionInfo",
    "FExecutionAnimInfo": "FSLFExecutionAnimInfo",
    "FWeaponAttackPower": "FSLFWeaponAttackPower",
    "FSprintCost": "FSLFSprintCost",
    "FItemInfo": "FSLFItemInfo",
    "FItemInfoCount": "FSLFItemInfoCount",
    "FItemCategory": "FSLFItemCategoryData",
    "FEquipmentInfo": "FSLFEquipmentInfo",
    "FEquipmentSlot": "FSLFEquipmentSlot",
    "FEquipmentStat": "FSLFEquipmentStat",
    "FEquipmentSocketInfo": "FSLFEquipmentSocketInfo",
    "FEquipmentWeaponStatInfo": "FSLFEquipmentWeaponStatInfo",
    "FCurrentEquipment": "FSLFCurrentEquipment",
    "FInventoryCategory": "FSLFInventoryCategory",
    "FLootItem": "FSLFLootItem",
    "FWeightedLoot": "FSLFWeightedLoot",
    "FFlaskData": "FSLFFlaskData",
    "FCraftingInfo": "FSLFCraftingInfo",
    "FVendorItems": "FSLFVendorItems",
    "FAiRuleDistance": "FSLFAiRuleDistance",
    "FAiRuleStat": "FSLFAiRuleStat",
    "FAiAttackEntry": "FSLFAiAttackEntry",
    "FAiBossPhase": "FSLFAiBossPhase",
    "FAiPatrolPathInfo": "FSLFAiPatrolPathInfo",
    "FAiSenseLocationInfo": "FSLFAiSenseLocationInfo",
    "FAiSenseTargetInfo": "FSLFAiSenseTargetInfo",
    "FAiStrafeInfo": "FSLFAiStrafeInfo",
    "FDialogEntry": "FSLFDialogEntry",
    "FDialogProgress": "FSLFDialogProgress",
    "FDialogRequirement": "FSLFDialogRequirement",
    "FDialogGameplayEvent": "FSLFDialogGameplayEvent",
    "FSaveData": "FSLFSaveData",
    "FSaveGameInfo": "FSLFSaveGameInfo",
    "FClassSaveInfo": "FSLFClassSaveInfo",
    "FWorldSaveInfo": "FSLFWorldSaveInfo",
    "FProgressSaveInfo": "FSLFProgressSaveInfo",
    "FInventoryItemsSaveInfo": "FSLFInventoryItemsSaveInfo",
    "FEquipmentItemsSaveInfo": "FSLFEquipmentItemsSaveInfo",
    "FItemWheelSaveInfo": "FSLFItemWheelSaveInfo",
    "FInteractableStateSaveInfo": "FSLFInteractableStateSaveInfo",
    "FNpcSaveInfo": "FSLFNpcSaveInfo",
    "FNpcVendorSaveInfo": "FSLFNpcVendorSaveInfo",
    "FSpawnedActorSaveInfo": "FSLFSpawnedActorSaveInfo",
    "FDoorLockInfo": "FSLFDoorLockInfo",
    "FStatusEffectApplication": "FSLFStatusEffectApplication",
    "FStatusEffectRankInfo": "FSLFStatusEffectRankInfo",
    "FStatusEffectVfxInfo": "FSLFStatusEffectVfxInfo",
    "FStatusEffectTick": "FStatusEffectTick",
    "FStatusEffectStatChanges": "FStatusEffectStatChanges",
    "FStatusEffectOneShotAndTick": "FStatusEffectOneShotAndTick",
    "FDayNightInfo": "FSLFDayNightInfo",
    "FLevelChangeData": "FLevelChangeData",
    "FSkeletalMeshData": "FSLFSkeletalMeshData",
    "FKeyMappingCorrelation": "FSLFKeyMappingCorrelation",
    "FLoadingScreenTip": "FSLFLoadingScreenTip",
    "FCreditsEntry": "FSLFCreditsEntry",
    "FCreditsNames": "FSLFCreditsNames",
    "FCreditsExtra": "FSLFCreditsExtra",
    "FWorldMeshInfo": "FSLFWorldMeshInfo",
    "FCardinalData": "FSLFCardinalData",
    "FItemWheelNextSlotInfo": "FSLFItemWheelNextSlotInfo",
    "S_Colour": "FSLFColour",
    "S_RoomSettings": "FSLFRoomSettings",
    "FRequiredCurrencyForLevel": "FSLFRequiredCurrencyForLevel",
}


def map_type(type_info: Dict) -> str:
    """Convert Blueprint type info to C++ type string."""
    category = type_info.get("Category", "")
    sub_category = type_info.get("SubCategory")
    sub_obj = type_info.get("SubCategoryObject")
    is_array = type_info.get("IsArray", False)
    is_set = type_info.get("IsSet", False)
    is_map = type_info.get("IsMap", False)

    # Get base type
    base_type = None

    # Check if it's an enum
    if sub_obj and sub_obj in ENUM_MAP:
        base_type = ENUM_MAP[sub_obj]
    # Check if it's a struct
    elif sub_obj and sub_obj in STRUCT_MAP:
        base_type = STRUCT_MAP[sub_obj]
    # Check if it's a known type
    elif (category, sub_obj) in TYPE_MAP:
        base_type = TYPE_MAP[(category, sub_obj)]
    elif (category, sub_category) in TYPE_MAP:
        base_type = TYPE_MAP[(category, sub_category)]
    elif (category, None) in TYPE_MAP:
        base_type = TYPE_MAP[(category, None)]
    else:
        # Unknown type - use UObject* as fallback
        if category == "object":
            base_type = f"UObject* /* {sub_obj} */"
        elif category == "struct":
            base_type = f"F{sub_obj}" if sub_obj else "FUnknownStruct"
        elif category == "byte" and sub_obj:
            # Likely an enum we don't have mapped
            base_type = f"uint8 /* {sub_obj} */"
        else:
            base_type = f"/* UNKNOWN: {category}/{sub_category}/{sub_obj} */"

    # Handle containers
    if is_array:
        return f"TArray<{base_type}>"
    elif is_set:
        return f"TSet<{base_type}>"
    elif is_map:
        key_type_info = type_info.get("KeyType", {})
        if key_type_info and key_type_info.get("Category"):
            key_type = map_type(key_type_info)
        else:
            key_type = "FUnknownKey"
        return f"TMap<{key_type}, {base_type}>"

    return base_type


def extract_variable(var_data: Dict) -> Dict:
    """Extract variable information for tracking."""
    return {
        "name": var_data.get("Name", "Unknown"),
        "type": map_type(var_data.get("Type", {})),
        "type_raw": var_data.get("Type", {}),
        "category": var_data.get("Category", "Default"),
        "default_value": var_data.get("DefaultValue", ""),
        "is_instance_editable": var_data.get("IsInstanceEditable", False),
        "blueprint_read_only": var_data.get("IsBlueprintReadOnly", False),
        "blueprint_read_write": not var_data.get("IsBlueprintReadOnly", False),
        "is_expose_on_spawn": var_data.get("IsExposeOnSpawn", False),
        "is_private": var_data.get("IsPrivate", False),
        "is_replicated": var_data.get("IsReplicated", False),
        "pass1_complete": False,
        "pass2_validated": False,
        "pass3_complete": False,
        "notes": ""
    }


def extract_function(func_data: Dict) -> Dict:
    """Extract function information for tracking."""
    # Handle both old format (Parameters) and new format (Inputs/Outputs)
    params = []

    # New format: Inputs array
    for param in func_data.get("Inputs", []):
        param_name = param.get("Name", "Unknown")
        param_type = param.get("Type", {})
        if isinstance(param_type, str):
            params.append({
                "name": param_name,
                "type": param_type,
                "type_raw": {"Category": param_type}
            })
        else:
            params.append({
                "name": param_name,
                "type": map_type(param_type) if param_type else "Unknown",
                "type_raw": param_type
            })

    # Old format: Parameters array
    for param in func_data.get("Parameters", []):
        params.append({
            "name": param.get("Name", "Unknown"),
            "type": map_type(param.get("Type", {})),
            "type_raw": param.get("Type", {})
        })

    # Handle return type from Outputs or ReturnType
    return_type = "void"
    return_type_raw = {}

    outputs = func_data.get("Outputs", [])
    if outputs:
        for output in outputs:
            if output.get("Name") == "ReturnValue":
                output_type = output.get("Type", {})
                if isinstance(output_type, str):
                    return_type = output_type
                    return_type_raw = {"Category": output_type}
                else:
                    return_type = map_type(output_type) if output_type else "void"
                    return_type_raw = output_type
                break
    elif func_data.get("ReturnType"):
        return_type_raw = func_data.get("ReturnType", {})
        return_type = map_type(return_type_raw) if return_type_raw else "void"

    return {
        "name": func_data.get("Name", "Unknown"),
        "return_type": return_type,
        "return_type_raw": return_type_raw,
        "parameters": params,
        "category": func_data.get("Category", "Default"),
        "access_specifier": func_data.get("AccessSpecifier", "Public"),
        "is_pure": func_data.get("IsPure", False),
        "is_const": func_data.get("IsConst", False),
        "is_static": func_data.get("IsStatic", False),
        "is_blueprint_native_event": False,  # Determined by graph analysis
        "is_blueprint_pure": func_data.get("IsPure", False),
        "is_blueprint_callable": True,  # Assume callable unless proven otherwise
        "has_exec_pins": not func_data.get("IsPure", False),
        "local_variables": [v.get("Name") for v in func_data.get("LocalVariables", [])],
        "graph_node_count": 0,  # Will be populated during graph analysis
        "graph_summary": "",
        "pass1_complete": False,
        "pass2_validated": False,
        "pass3_complete": False,
        "implementation_summary": "",
        "validation_notes": ""
    }


def extract_event_dispatcher(ed_data: Dict) -> Dict:
    """Extract event dispatcher information for tracking."""
    params = []
    for param in ed_data.get("Parameters", []):
        params.append({
            "name": param.get("Name", "Unknown"),
            "type": map_type(param.get("Type", {})) if param.get("Type") else "Unknown",
            "type_raw": param.get("Type", {})
        })

    return {
        "name": ed_data.get("Name", "Unknown"),
        "parameters": params,
        "pass1_complete": False,
        "pass2_validated": False,
        "pass3_complete": False,
        "notes": ""
    }


def analyze_graph(graph_data: Dict) -> Dict:
    """Analyze a graph for tracking."""
    nodes = graph_data.get("Nodes", [])
    node_count = len(nodes)

    # Find entry points
    entry_points = []
    has_branches = False
    has_loops = False
    has_timers = False
    function_calls = []

    for node in nodes:
        node_class = node.get("NodeClass", node.get("Class", ""))
        node_title = node.get("NodeTitle", "")

        if "K2Node_Event" in node_class or "K2Node_CustomEvent" in node_class:
            event_name = node.get("EventName", node.get("CustomFunctionName", node_title))
            if event_name and event_name != "Unknown":
                entry_points.append(event_name)

        if "K2Node_IfThenElse" in node_class or "Branch" in node_title:
            has_branches = True

        if "K2Node_MacroInstance" in node_class:
            macro_name = node.get("MacroGraphName", "")
            if "ForEach" in macro_name or "While" in macro_name:
                has_loops = True

        if "Timer" in str(node):
            has_timers = True

        if "K2Node_CallFunction" in node_class:
            func_name = node.get("FunctionName", node_title)
            if func_name:
                function_calls.append(func_name)

    # Handle GraphName vs Name
    graph_name = graph_data.get("GraphName", graph_data.get("Name", "Unknown"))
    graph_type = graph_data.get("GraphType", graph_data.get("Type", "unknown")).lower()

    return {
        "name": graph_name,
        "type": graph_type,
        "entry_points": entry_points,
        "node_count": node_count,
        "has_branches": has_branches,
        "has_loops": has_loops,
        "has_timers": has_timers,
        "function_calls": function_calls[:20],  # Limit to avoid bloat
        "pass1_complete": False,
        "pass2_validated": False,
        "pass3_complete": False,
        "implementation_summary": ""
    }


def extract_dependencies(bp_data: Dict) -> Dict:
    """Extract all dependencies from Blueprint data."""
    deps = {
        "structs": set(),
        "enums": set(),
        "interfaces": [],
        "components": [],
        "blueprints": [],
        "data_assets": []
    }

    # Extract from variables
    for var in bp_data.get("Variables", {}).get("List", []):
        type_info = var.get("Type", {})
        sub_obj = type_info.get("SubCategoryObject", "")
        sub_path = type_info.get("SubCategoryObjectPath", "")

        if sub_obj:
            if "Enum" in sub_path or sub_obj.startswith("E_"):
                deps["enums"].add(sub_obj)
            elif "Struct" in sub_path or sub_obj.startswith("F"):
                deps["structs"].add(sub_obj)

    # Extract from interfaces
    for iface in bp_data.get("Interfaces", {}).get("List", []):
        deps["interfaces"].append(iface.get("Name", "Unknown"))

    # Extract from components
    for comp in bp_data.get("Components", {}).get("Hierarchy", []):
        comp_class = comp.get("Class", "")
        if comp_class and "_C" in comp_class:
            deps["components"].append(comp_class.replace("_C", ""))

    return {
        "structs": list(deps["structs"]),
        "enums": list(deps["enums"]),
        "interfaces": deps["interfaces"],
        "components": deps["components"],
        "blueprints": deps["blueprints"],
        "data_assets": deps["data_assets"]
    }


def generate_tracking_json(bp_json_path: str) -> Dict:
    """Generate a tracking JSON from a Blueprint JSON export."""
    with open(bp_json_path, 'r', encoding='utf-8') as f:
        bp_data = json.load(f)

    bp_name = bp_data.get("Name", "Unknown")
    bp_path = bp_data.get("Path", "")
    parent_class = bp_data.get("ParentClass", "Object")
    parent_path = bp_data.get("ParentClassPath", "")

    # Determine C++ base class name
    if "ActorComponent" in parent_class or "ActorComponent" in parent_path:
        cpp_base = "UActorComponent"
    elif "Character" in parent_class or "Character" in parent_path:
        cpp_base = "ACharacter"
    elif "Actor" in parent_class or "Actor" in parent_path:
        cpp_base = "AActor"
    elif "_C" in parent_class:
        # It's a Blueprint parent
        cpp_base = f"U{parent_class.replace('_C', '')}"
    else:
        cpp_base = "UObject"

    # Extract all items
    variables = []
    for var in bp_data.get("Variables", {}).get("List", []):
        variables.append(extract_variable(var))

    # Functions can be in different locations depending on export format
    functions = []
    # Try FunctionSignatures.Functions first (newer format)
    func_list = bp_data.get("FunctionSignatures", {}).get("Functions", [])
    # Fall back to Functions (older format)
    if not func_list:
        func_list = bp_data.get("Functions", [])
    for func in func_list:
        functions.append(extract_function(func))

    event_dispatchers = []
    for ed in bp_data.get("EventDispatchers", {}).get("List", []):
        event_dispatchers.append(extract_event_dispatcher(ed))

    # Graphs can be in different locations depending on export format
    graphs = []
    # Try Logic.AllGraphs first (newer format)
    graph_list = bp_data.get("Logic", {}).get("AllGraphs", [])
    # Fall back to Graphs (older format)
    if not graph_list:
        graph_list = bp_data.get("Graphs", [])
    for graph in graph_list:
        graphs.append(analyze_graph(graph))

    # Check for custom events in EventGraph
    for graph in graphs:
        if graph["type"] == "eventgraph":
            for entry in graph["entry_points"]:
                # Custom events should be BlueprintNativeEvent
                for func in functions:
                    if func["name"] == entry:
                        func["is_blueprint_native_event"] = True

    # Extract dependencies
    dependencies = extract_dependencies(bp_data)

    # Determine implemented interfaces
    implemented_interfaces = []
    for iface in bp_data.get("Interfaces", {}).get("List", []):
        iface_name = iface.get("Name", "").replace("_C", "")
        implemented_interfaces.append(iface_name)

    # Generate header/source file names
    if bp_name.startswith("AC_"):
        cpp_name = bp_name.replace("AC_", "") + "Component"
    elif bp_name.startswith("B_"):
        cpp_name = "SLF" + bp_name.replace("B_", "")
    elif bp_name.startswith("BPI_"):
        cpp_name = "SLF" + bp_name.replace("BPI_", "")
    else:
        cpp_name = bp_name

    tracking = {
        "blueprint_name": bp_name,
        "blueprint_path": bp_path,
        "json_export_path": str(bp_json_path),
        "cpp_header": f"{cpp_name}.h",
        "cpp_source": f"{cpp_name}.cpp",
        "parent_class": cpp_base,
        "parent_class_raw": parent_class,
        "implemented_interfaces": implemented_interfaces,

        "migration_status": {
            "pass1_complete": False,
            "pass2_validated": False,
            "pass3_complete": False,
            "overall_status": "not_started"
        },

        "summary": {
            "variable_count": len(variables),
            "function_count": len(functions),
            "event_dispatcher_count": len(event_dispatchers),
            "graph_count": len(graphs),
            "total_items": len(variables) + len(functions) + len(event_dispatchers) + len(graphs)
        },

        "variables": variables,
        "functions": functions,
        "event_dispatchers": event_dispatchers,
        "graphs": graphs,
        "collapsed_graphs": [],  # Would need deeper analysis to extract

        "dependencies": dependencies,

        "final_summary": "",
        "known_issues": [],
        "debug_notes": [],
        "test_results": []
    }

    return tracking


def get_layer_for_blueprint(bp_name: str, bp_path: str) -> str:
    """Determine which layer a Blueprint belongs to."""
    if "Interface" in bp_path:
        return "Layer1_Interfaces"
    elif "Component" in bp_path or bp_name.startswith("AC_"):
        return "Layer3_Components"
    elif any(x in bp_name for x in ["Character", "Enemy", "Boss", "NPC"]):
        return "Layer5_Characters"
    elif any(x in bp_name for x in ["GI_", "GM_", "PC_", "GS_", "PS_"]):
        return "Layer6_GameFramework"
    elif "DataAsset" in bp_path or bp_name.startswith("PDA_"):
        return "Layer2_DataAssets"
    else:
        return "Layer4_CoreBlueprints"


def process_blueprint(bp_json_path: str, output_dir: Optional[str] = None) -> str:
    """Process a single Blueprint and save tracking JSON."""
    bp_json_path = Path(bp_json_path)

    if not bp_json_path.exists():
        raise FileNotFoundError(f"Blueprint JSON not found: {bp_json_path}")

    print(f"Processing: {bp_json_path.name}")

    tracking = generate_tracking_json(str(bp_json_path))

    # Determine output location
    bp_name = tracking["blueprint_name"]
    bp_path = tracking["blueprint_path"]
    layer = get_layer_for_blueprint(bp_name, bp_path)

    if output_dir:
        out_dir = Path(output_dir)
    else:
        out_dir = TRACKING_DIR / layer

    out_dir.mkdir(parents=True, exist_ok=True)
    out_path = out_dir / f"{bp_name}.json"

    with open(out_path, 'w', encoding='utf-8') as f:
        json.dump(tracking, f, indent=2)

    print(f"  -> Saved: {out_path}")
    print(f"     Variables: {tracking['summary']['variable_count']}")
    print(f"     Functions: {tracking['summary']['function_count']}")
    print(f"     Event Dispatchers: {tracking['summary']['event_dispatcher_count']}")
    print(f"     Graphs: {tracking['summary']['graph_count']}")

    return str(out_path)


def process_all_in_category(category: str):
    """Process all Blueprints in a category."""
    category_dir = BP_EXPORT_DIR / category

    if not category_dir.exists():
        print(f"Category directory not found: {category_dir}")
        return

    json_files = list(category_dir.glob("*.json"))
    print(f"\nProcessing {len(json_files)} files in {category}/")
    print("=" * 60)

    for json_file in sorted(json_files):
        try:
            process_blueprint(str(json_file))
        except Exception as e:
            print(f"  ERROR: {e}")

    print(f"\nCompleted processing {len(json_files)} files")


def main():
    """Main entry point."""
    if len(sys.argv) < 2:
        print(__doc__)
        return

    arg = sys.argv[1]

    if arg == "--all-components":
        process_all_in_category("Component")
    elif arg == "--all-interfaces":
        process_all_in_category("Interface")
    elif arg == "--all-blueprints":
        process_all_in_category("Blueprint")
    elif arg == "--all-dataassets":
        process_all_in_category("DataAsset")
    elif arg == "--all-gameframework":
        process_all_in_category("GameFramework")
    elif arg == "--all":
        for category in ["Interface", "Component", "DataAsset", "Blueprint", "GameFramework"]:
            process_all_in_category(category)
    else:
        # Single file
        output_dir = sys.argv[2] if len(sys.argv) > 2 else None
        process_blueprint(arg, output_dir)


if __name__ == "__main__":
    main()
