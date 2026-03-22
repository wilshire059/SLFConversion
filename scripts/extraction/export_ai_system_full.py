# export_ai_system_full.py
# Comprehensive export of ALL AI-related assets for comparison
# Run on BOTH bp_only and SLFConversion projects

import unreal
import json
import os

# Output directory - will be different per project
PROJECT_NAME = "SLFConversion"  # Change to "bp_only" when running on backup
OUTPUT_DIR = f"C:/scripts/SLFConversion/migration_cache/ai_export_{PROJECT_NAME}"

# Assets to export
ASSETS = {
    "behavior_trees": [
        "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable",
    ],
    "bt_tasks": [
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ClearKey",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetCurrentLocation",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPoint",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPointNearStartPosition",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetStrafePointAroundTarget",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetKey",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetMovementMode",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SimpleMoveTo",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchState",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchToPreviousState",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ToggleFocus",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_TryExecuteAbility",
    ],
    "bt_services": [
        "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_ChaseBounds",
        "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_IsTargetDead",
    ],
    "bt_decorators": [
        "/Game/SoulslikeFramework/Blueprints/_AI/Decorators/BTD_StateEquals",
    ],
    "blackboard": [
        "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard",
    ],
    "ai_controller": [
        "/Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework",
    ],
    "enemies": [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    ],
    "components": [
        "/Game/SoulslikeFramework/Blueprints/_Components/AI/AC_AI_BehaviorManager",
        "/Game/SoulslikeFramework/Blueprints/_Components/AI/AC_AI_CombatManager",
    ],
    "enums": [
        "/Game/SoulslikeFramework/Enums/E_AI_States",
    ],
}

def ensure_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)

def export_blueprint_full(bp_path, output_file):
    """Export full Blueprint info using export_text and SLFAutomationLibrary"""
    result = {
        "path": bp_path,
        "exists": False,
        "parent_class": None,
        "variables": [],
        "functions": [],
        "has_eventgraph": False,
        "export_text": None,
        "error": None,
    }

    try:
        bp = unreal.load_asset(bp_path)
        if not bp:
            result["error"] = "Asset not found"
            return result

        result["exists"] = True
        result["name"] = bp.get_name()

        # Get parent class
        try:
            parent_path = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            result["parent_class"] = parent_path
        except:
            result["parent_class"] = "Error getting parent"

        # Get export text (raw Blueprint data)
        try:
            export_text = unreal.EditorAssetLibrary.export_text(bp_path)
            result["export_text"] = export_text[:50000] if len(export_text) > 50000 else export_text  # Limit size
        except Exception as e:
            result["export_text"] = f"Error: {e}"

        # Try to get variable/function count
        try:
            if hasattr(bp, 'new_variables'):
                result["variable_count"] = len(bp.new_variables) if bp.new_variables else 0
            if hasattr(bp, 'function_graphs'):
                result["function_count"] = len(bp.function_graphs) if bp.function_graphs else 0
        except:
            pass

    except Exception as e:
        result["error"] = str(e)

    return result

def export_behavior_tree(bt_path, output_file):
    """Export BT structure using SLFAutomationLibrary"""
    result = {
        "path": bt_path,
        "exists": False,
        "structure": None,
        "error": None,
    }

    try:
        bt = unreal.load_asset(bt_path)
        if not bt:
            result["error"] = "Asset not found"
            return result

        result["exists"] = True
        result["name"] = bt.get_name()

        # Get full BT structure with decorator values
        try:
            structure = unreal.SLFAutomationLibrary.export_bt_all_decorator_int_values(bt_path)
            result["structure"] = structure
        except Exception as e:
            result["structure"] = f"Error: {e}"

        # Get raw export text
        try:
            export_text = unreal.EditorAssetLibrary.export_text(bt_path)
            result["export_text"] = export_text[:100000] if len(export_text) > 100000 else export_text
        except Exception as e:
            result["export_text"] = f"Error: {e}"

    except Exception as e:
        result["error"] = str(e)

    return result

def export_blackboard(bb_path, output_file):
    """Export Blackboard definition"""
    result = {
        "path": bb_path,
        "exists": False,
        "keys": [],
        "error": None,
    }

    try:
        bb = unreal.load_asset(bb_path)
        if not bb:
            result["error"] = "Asset not found"
            return result

        result["exists"] = True
        result["name"] = bb.get_name()

        # Get blackboard diagnosis
        try:
            diagnosis = unreal.SLFAutomationLibrary.diagnose_blackboard_state_key(bb_path)
            result["diagnosis"] = diagnosis
        except Exception as e:
            result["diagnosis"] = f"Error: {e}"

        # Get raw export text
        try:
            export_text = unreal.EditorAssetLibrary.export_text(bb_path)
            result["export_text"] = export_text
        except Exception as e:
            result["export_text"] = f"Error: {e}"

    except Exception as e:
        result["error"] = str(e)

    return result

def export_enum(enum_path, output_file):
    """Export Enum definition"""
    result = {
        "path": enum_path,
        "exists": False,
        "values": [],
        "error": None,
    }

    try:
        enum_asset = unreal.load_asset(enum_path)
        if not enum_asset:
            result["error"] = "Asset not found"
            return result

        result["exists"] = True
        result["name"] = enum_asset.get_name()

        # Get raw export text - this shows the enum values
        try:
            export_text = unreal.EditorAssetLibrary.export_text(enum_path)
            result["export_text"] = export_text
        except Exception as e:
            result["export_text"] = f"Error: {e}"

    except Exception as e:
        result["error"] = str(e)

    return result

def main():
    ensure_dir(OUTPUT_DIR)

    all_exports = {}

    # Export Behavior Trees
    print(f"\n=== Exporting Behavior Trees ===")
    all_exports["behavior_trees"] = {}
    for bt_path in ASSETS["behavior_trees"]:
        name = bt_path.split("/")[-1]
        print(f"  {name}...")
        result = export_behavior_tree(bt_path, f"{OUTPUT_DIR}/bt_{name}.json")
        all_exports["behavior_trees"][name] = result

        # Also save individual file
        with open(f"{OUTPUT_DIR}/bt_{name}.json", "w") as f:
            json.dump(result, f, indent=2, default=str)

    # Export BT Tasks
    print(f"\n=== Exporting BT Tasks ===")
    all_exports["bt_tasks"] = {}
    for bp_path in ASSETS["bt_tasks"]:
        name = bp_path.split("/")[-1]
        print(f"  {name}...")
        result = export_blueprint_full(bp_path, f"{OUTPUT_DIR}/task_{name}.json")
        all_exports["bt_tasks"][name] = result

        with open(f"{OUTPUT_DIR}/task_{name}.json", "w") as f:
            json.dump(result, f, indent=2, default=str)

    # Export BT Services
    print(f"\n=== Exporting BT Services ===")
    all_exports["bt_services"] = {}
    for bp_path in ASSETS["bt_services"]:
        name = bp_path.split("/")[-1]
        print(f"  {name}...")
        result = export_blueprint_full(bp_path, f"{OUTPUT_DIR}/service_{name}.json")
        all_exports["bt_services"][name] = result

        with open(f"{OUTPUT_DIR}/service_{name}.json", "w") as f:
            json.dump(result, f, indent=2, default=str)

    # Export Blackboard
    print(f"\n=== Exporting Blackboard ===")
    all_exports["blackboard"] = {}
    for bb_path in ASSETS["blackboard"]:
        name = bb_path.split("/")[-1]
        print(f"  {name}...")
        result = export_blackboard(bb_path, f"{OUTPUT_DIR}/bb_{name}.json")
        all_exports["blackboard"][name] = result

        with open(f"{OUTPUT_DIR}/bb_{name}.json", "w") as f:
            json.dump(result, f, indent=2, default=str)

    # Export AI Controller
    print(f"\n=== Exporting AI Controller ===")
    all_exports["ai_controller"] = {}
    for bp_path in ASSETS["ai_controller"]:
        name = bp_path.split("/")[-1]
        print(f"  {name}...")
        result = export_blueprint_full(bp_path, f"{OUTPUT_DIR}/aic_{name}.json")
        all_exports["ai_controller"][name] = result

        with open(f"{OUTPUT_DIR}/aic_{name}.json", "w") as f:
            json.dump(result, f, indent=2, default=str)

    # Export Enemies
    print(f"\n=== Exporting Enemies ===")
    all_exports["enemies"] = {}
    for bp_path in ASSETS["enemies"]:
        name = bp_path.split("/")[-1]
        print(f"  {name}...")
        result = export_blueprint_full(bp_path, f"{OUTPUT_DIR}/enemy_{name}.json")
        all_exports["enemies"][name] = result

        with open(f"{OUTPUT_DIR}/enemy_{name}.json", "w") as f:
            json.dump(result, f, indent=2, default=str)

    # Export Components
    print(f"\n=== Exporting Components ===")
    all_exports["components"] = {}
    for bp_path in ASSETS["components"]:
        name = bp_path.split("/")[-1]
        print(f"  {name}...")
        result = export_blueprint_full(bp_path, f"{OUTPUT_DIR}/comp_{name}.json")
        all_exports["components"][name] = result

        with open(f"{OUTPUT_DIR}/comp_{name}.json", "w") as f:
            json.dump(result, f, indent=2, default=str)

    # Export Enums
    print(f"\n=== Exporting Enums ===")
    all_exports["enums"] = {}
    for enum_path in ASSETS["enums"]:
        name = enum_path.split("/")[-1]
        print(f"  {name}...")
        result = export_enum(enum_path, f"{OUTPUT_DIR}/enum_{name}.json")
        all_exports["enums"][name] = result

        with open(f"{OUTPUT_DIR}/enum_{name}.json", "w") as f:
            json.dump(result, f, indent=2, default=str)

    # Save master export
    with open(f"{OUTPUT_DIR}/FULL_EXPORT.json", "w") as f:
        json.dump(all_exports, f, indent=2, default=str)

    print(f"\n=== Export Complete ===")
    print(f"Output directory: {OUTPUT_DIR}")
    print(f"Files exported: {sum(len(v) for v in all_exports.values())}")

if __name__ == "__main__":
    main()
