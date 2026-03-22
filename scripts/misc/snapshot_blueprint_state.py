#!/usr/bin/env python3
"""
snapshot_blueprint_state.py
Capture component TMap values BEFORE migration for validation

Usage:
  Run via UnrealEditor-Cmd.exe -run=pythonscript -script=snapshot_blueprint_state.py

Output:
  migration_snapshot.json - Complete component state snapshot
"""

import unreal
import json
import os

# Output path
SNAPSHOT_PATH = "C:/scripts/SLFConversion/migration_snapshot.json"

# Components to snapshot with their key TMap/Array properties
COMPONENTS_TO_SNAPSHOT = {
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager": {
        "class_name": "AC_ActionManager",
        "properties": ["Actions", "AvailableActions", "ActionAssetsCache", "ActionTagsCache"]
    },
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager": {
        "class_name": "AC_StatManager",
        "properties": ["Stats", "StatTable"]
    },
    "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager": {
        "class_name": "AC_EquipmentManager",
        "properties": ["EquippedItems", "EquipmentSlots"]
    },
    "/Game/SoulslikeFramework/Blueprints/Components/AC_InventoryManager": {
        "class_name": "AC_InventoryManager",
        "properties": ["Inventory", "InventoryItems"]
    },
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager": {
        "class_name": "AC_CombatManager",
        "properties": ["CurrentTarget", "LockedTarget"]
    },
    "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer": {
        "class_name": "AC_InputBuffer",
        "properties": ["BufferQueue", "BufferOpen"]
    }
}


def get_property_value_safe(obj, prop_name):
    """Safely get a property value and convert to JSON-serializable format"""
    try:
        value = obj.get_editor_property(prop_name)
        return serialize_value(value, prop_name)
    except Exception as e:
        return {"error": str(e), "property": prop_name}


def serialize_value(value, prop_name=""):
    """Convert Unreal values to JSON-serializable format"""
    if value is None:
        return None

    # Handle basic types
    if isinstance(value, (bool, int, float, str)):
        return value

    # Handle GameplayTag
    if hasattr(value, 'tag_name'):
        return {"type": "GameplayTag", "value": str(value.tag_name)}

    # Handle UObject references
    if isinstance(value, unreal.Object):
        return {
            "type": "object",
            "class": value.get_class().get_name() if value else None,
            "path": value.get_path_name() if value else None,
            "name": value.get_name() if value else None
        }

    # Handle TMap (appears as dict in Python)
    if isinstance(value, dict):
        serialized = {"type": "TMap", "count": len(value), "entries": {}}
        for k, v in value.items():
            key_str = str(k.tag_name) if hasattr(k, 'tag_name') else str(k)
            serialized["entries"][key_str] = serialize_value(v)
        return serialized

    # Handle TArray (appears as list in Python)
    if isinstance(value, (list, tuple)):
        return {
            "type": "TArray",
            "count": len(value),
            "items": [serialize_value(item) for item in value]
        }

    # Handle class references (TSubclassOf)
    if hasattr(value, 'get_name') and hasattr(value, 'get_path_name'):
        return {
            "type": "class",
            "name": value.get_name(),
            "path": value.get_path_name()
        }

    # Fallback
    return {"type": "unknown", "repr": repr(value)}


def snapshot_blueprint(bp_path, config):
    """Snapshot a single Blueprint's CDO properties"""
    try:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            return {"error": f"Could not load Blueprint: {bp_path}"}

        # Get generated class and CDO
        gen_class = bp.generated_class()
        if not gen_class:
            return {"error": f"No generated class for: {bp_path}"}

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            return {"error": f"Could not get CDO for: {bp_path}"}

        # Snapshot each property
        snapshot = {
            "path": bp_path,
            "class_name": config["class_name"],
            "parent_class": str(gen_class.get_super_class().get_name()) if gen_class.get_super_class() else None,
            "properties": {}
        }

        for prop_name in config["properties"]:
            snapshot["properties"][prop_name] = get_property_value_safe(cdo, prop_name)

        return snapshot

    except Exception as e:
        return {"error": str(e), "path": bp_path}


def capture_full_snapshot():
    """Capture snapshot of all configured components"""
    unreal.log_warning("═══════════════════════════════════════════════════════════════")
    unreal.log_warning("[Snapshot] Capturing pre-migration Blueprint state")
    unreal.log_warning("═══════════════════════════════════════════════════════════════")

    snapshot = {
        "version": "1.0",
        "timestamp": str(unreal.Paths.get_project_file_path()),
        "components": {}
    }

    for bp_path, config in COMPONENTS_TO_SNAPSHOT.items():
        unreal.log_warning(f"[Snapshot] Processing: {config['class_name']}")
        snapshot["components"][config["class_name"]] = snapshot_blueprint(bp_path, config)

    # Write snapshot to file
    with open(SNAPSHOT_PATH, 'w', encoding='utf-8') as f:
        json.dump(snapshot, f, indent=2)

    unreal.log_warning(f"[Snapshot] Saved to: {SNAPSHOT_PATH}")
    unreal.log_warning("═══════════════════════════════════════════════════════════════")

    return snapshot


# Entry point when run as script
if __name__ == "__main__":
    capture_full_snapshot()
