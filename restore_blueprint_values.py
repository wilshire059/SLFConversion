# restore_blueprint_values.py
# Restores CDO values to migrated Blueprints AFTER reparenting
# This sets the C++ UPROPERTY values from the saved JSON
#
# Usage:
#   1. Run migration first (run_migration.py)
#   2. Run: UnrealEditor-Cmd.exe Project.uproject -run=pythonscript
#           -script="restore_blueprint_values.py" -stdout -unattended -nosplash

import unreal
import json
import os

# Input file with extracted values
INPUT_FILE = "C:/scripts/SLFConversion/blueprint_cdo_values.json"

# Mapping of old Blueprint property names to C++ property names (if different)
PROPERTY_NAME_MAP = {
    # Input contexts - these are the critical ones causing crashes
    "GameplayMappingContext": "GameplayMappingContext",
    "UIInputMappingContext": "UIInputMappingContext",
    "MenuInputMappingContext": "MenuInputMappingContext",
}


def load_asset_reference(path_str):
    """Load an asset from a path string"""
    if not path_str or path_str == "None":
        return None

    try:
        # Clean up the path
        path = str(path_str)

        # Handle different path formats
        if "." in path and not path.startswith("/"):
            # Format: Package.AssetName
            parts = path.rsplit(".", 1)
            if len(parts) == 2:
                path = parts[0]

        # Try loading
        asset = unreal.load_asset(path)
        return asset
    except Exception as e:
        unreal.log_warning(f"  Could not load asset: {path_str} - {e}")
        return None


def set_property_from_value(obj, prop_name, value_data):
    """Set a property on an object from extracted value data"""
    if value_data is None:
        return False

    val_type = value_data.get("type", "unknown")

    try:
        if val_type == "object":
            # Load and set object reference
            path = value_data.get("path")
            if path:
                asset = load_asset_reference(path)
                if asset:
                    obj.set_editor_property(prop_name, asset)
                    return True

        elif val_type == "soft_object":
            # Set soft object reference
            path = value_data.get("path")
            if path:
                # Create soft object path
                try:
                    soft_path = unreal.SoftObjectPath(path)
                    obj.set_editor_property(prop_name, soft_path)
                    return True
                except:
                    # Try loading directly
                    asset = load_asset_reference(path)
                    if asset:
                        obj.set_editor_property(prop_name, asset)
                        return True

        elif val_type == "class":
            # Load class reference
            path = value_data.get("path")
            if path:
                cls = unreal.load_class(None, path)
                if cls:
                    obj.set_editor_property(prop_name, cls)
                    return True

        elif val_type == "bool":
            obj.set_editor_property(prop_name, bool(value_data.get("value", False)))
            return True

        elif val_type == "int":
            obj.set_editor_property(prop_name, int(value_data.get("value", 0)))
            return True

        elif val_type == "float":
            obj.set_editor_property(prop_name, float(value_data.get("value", 0.0)))
            return True

        elif val_type == "string":
            obj.set_editor_property(prop_name, str(value_data.get("value", "")))
            return True

    except Exception as e:
        unreal.log_warning(f"  Failed to set {prop_name}: {e}")
        return False

    return False


def restore_blueprint_values(bp_asset, saved_data):
    """Restore saved values to a Blueprint's CDO"""
    restored_count = 0

    try:
        # Get the generated class and CDO
        gen_class = bp_asset.generated_class()
        if not gen_class:
            unreal.log_warning("  No generated class")
            return 0

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            unreal.log_warning("  No CDO")
            return 0

        # Restore properties
        properties = saved_data.get("properties", {})
        for prop_name, value_data in properties.items():
            # Check for property name mapping
            cpp_prop_name = PROPERTY_NAME_MAP.get(prop_name, prop_name)

            if set_property_from_value(cdo, cpp_prop_name, value_data):
                restored_count += 1
                unreal.log(f"  Restored: {cpp_prop_name}")
            else:
                # Try alternate property name (with underscore prefix for private)
                if set_property_from_value(cdo, f"_{cpp_prop_name}", value_data):
                    restored_count += 1
                    unreal.log(f"  Restored: _{cpp_prop_name}")

    except Exception as e:
        unreal.log_error(f"  Error restoring values: {e}")

    return restored_count


def run():
    print("=" * 60)
    print("BLUEPRINT CDO VALUE RESTORATION")
    print("=" * 60)
    print("")

    # Load saved values
    if not os.path.exists(INPUT_FILE):
        print(f"ERROR: Input file not found: {INPUT_FILE}")
        print("Run extract_blueprint_values.py first!")
        return

    try:
        with open(INPUT_FILE, 'r') as f:
            all_values = json.load(f)
    except Exception as e:
        print(f"ERROR: Could not load JSON: {e}")
        return

    print(f"Loaded {len(all_values)} Blueprint value sets")
    print("")

    total_restored = 0
    blueprints_processed = 0

    for name, data in all_values.items():
        path = data.get("path")
        if not path:
            continue

        print(f"Processing: {name}")

        try:
            # Check if asset exists
            if not unreal.EditorAssetLibrary.does_asset_exist(path):
                print(f"  SKIP: Asset not found")
                continue

            # Load the Blueprint
            bp = unreal.EditorAssetLibrary.load_asset(path)
            if not bp:
                print(f"  SKIP: Could not load asset")
                continue

            # Restore values
            restored = restore_blueprint_values(bp, data)
            total_restored += restored
            blueprints_processed += 1

            if restored > 0:
                # Save the asset
                unreal.EditorAssetLibrary.save_loaded_asset(bp)
                print(f"  OK: {restored} properties restored and saved")
            else:
                print(f"  WARN: No properties restored")

        except Exception as e:
            print(f"  ERROR: {e}")

    print("")
    print("=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print(f"Blueprints processed: {blueprints_processed}")
    print(f"Total properties restored: {total_restored}")


run()
