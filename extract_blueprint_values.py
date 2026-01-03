# extract_blueprint_values.py
# Extracts CDO (Class Default Object) values from Blueprints BEFORE migration
# Run this on the BACKUP Blueprint content to capture all property values
#
# Usage:
#   1. Restore backup content first: cp -r Backups/blueprint_only/Content Content/
#   2. Run: UnrealEditor-Cmd.exe Project.uproject -run=pythonscript
#           -script="extract_blueprint_values.py" -stdout -unattended -nosplash

import unreal
import json
import os

# Output file for extracted values
OUTPUT_FILE = "C:/scripts/SLFConversion/blueprint_cdo_values.json"

# Blueprint paths to extract (same as run_migration.py PATH_OVERRIDES)
BLUEPRINTS_TO_EXTRACT = {
    # Game Framework - CRITICAL for input contexts
    "PC_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework",
    "PC_Menu_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/MainMenu/PC_Menu_SoulslikeFramework",
    "GM_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework",
    "GM_Menu_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/MainMenu/GM_Menu_SoulslikeFramework",
    "GS_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/GS_SoulslikeFramework",
    "PS_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/PS_SoulslikeFramework",
    "GI_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework",
    # Characters
    "B_BaseCharacter": "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
    "B_Soulslike_Character": "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
    "B_Soulslike_Enemy": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "B_Soulslike_Boss": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
    "B_Soulslike_NPC": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    # AI Controller
    "AIC_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework",
}

# Properties we're most interested in (asset references, configs)
PRIORITY_PROPERTY_PATTERNS = [
    "MappingContext",
    "InputMapping",
    "DefaultPawn",
    "PlayerController",
    "GameMode",
    "GameState",
    "Widget",
    "Mesh",
    "Animation",
    "Animset",
    "Action",
    "Stat",
    "Buff",
    "Item",
    "Config",
    "Default",
]


def get_property_value_as_string(obj, prop_name):
    """Get a property value and convert to string representation"""
    try:
        val = obj.get_editor_property(prop_name)
        if val is None:
            return None

        # Handle different types
        val_type = type(val).__name__

        # Asset references (UObject-derived)
        if hasattr(val, 'get_path_name'):
            path = val.get_path_name()
            if path:
                return {"type": "object", "path": str(path)}

        # Soft object references
        if hasattr(val, 'get_asset_path'):
            try:
                path = str(val.get_asset_path())
                if path and path != "None":
                    return {"type": "soft_object", "path": path}
            except:
                pass

        # Class references
        if hasattr(val, 'get_class_path_name'):
            try:
                path = str(val.get_class_path_name())
                if path:
                    return {"type": "class", "path": path}
            except:
                pass

        # Primitive types
        if isinstance(val, bool):
            return {"type": "bool", "value": val}
        elif isinstance(val, int):
            return {"type": "int", "value": val}
        elif isinstance(val, float):
            return {"type": "float", "value": val}
        elif isinstance(val, str):
            return {"type": "string", "value": val}

        # Structs
        if hasattr(val, '__dict__'):
            # Try to serialize struct
            try:
                struct_data = {}
                for attr in dir(val):
                    if not attr.startswith('_'):
                        try:
                            attr_val = getattr(val, attr)
                            if not callable(attr_val):
                                if hasattr(attr_val, 'get_path_name'):
                                    struct_data[attr] = str(attr_val.get_path_name())
                                else:
                                    struct_data[attr] = str(attr_val)
                        except:
                            pass
                if struct_data:
                    return {"type": "struct", "data": struct_data}
            except:
                pass

        # Fallback: convert to string
        str_val = str(val)
        if str_val and str_val != "None":
            return {"type": "unknown", "string": str_val, "python_type": val_type}

        return None
    except Exception as e:
        return None


def extract_cdo_properties(bp_asset):
    """Extract all property values from a Blueprint's CDO"""
    properties = {}

    try:
        # Get the generated class
        gen_class = bp_asset.generated_class()
        if not gen_class:
            return properties

        # Get the CDO (Class Default Object)
        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            return properties

        # Get list of properties from the class
        # We'll try common property names and pattern-based discovery

        # Method 1: Try to get property names from the class
        try:
            # Use reflection to get property names
            for attr in dir(cdo):
                if attr.startswith('_'):
                    continue
                if attr.startswith('get_') or attr.startswith('set_'):
                    continue
                if attr in ['static_class', 'cast', 'get_class', 'get_outer', 'get_name', 'get_fname', 'get_path_name']:
                    continue

                # Skip methods
                try:
                    attr_val = getattr(cdo, attr)
                    if callable(attr_val):
                        continue
                except:
                    continue

                # Try to get property value
                val = get_property_value_as_string(cdo, attr)
                if val is not None:
                    properties[attr] = val
        except Exception as e:
            unreal.log_warning(f"  Reflection failed: {e}")

        # Method 2: Use get_editor_property with known patterns
        known_props = [
            # Input
            "GameplayMappingContext",
            "UIInputMappingContext",
            "MenuInputMappingContext",
            "InputMappingContext",
            "DefaultInputMappingContext",
            # Widgets
            "W_HUD",
            "HUDWidgetClass",
            "InventoryWidgetClass",
            "EquipmentWidgetClass",
            # Game Mode
            "DefaultPawnClass",
            "PlayerControllerClass",
            "GameStateClass",
            "PlayerStateClass",
            "HUDClass",
            # Character
            "Mesh",
            "AnimClass",
            "AnimInstance",
            "SkeletalMesh",
            # General
            "bAutoRegisterWithSubsystem",
            "InputPriority",
        ]

        for prop in known_props:
            if prop not in properties:
                val = get_property_value_as_string(cdo, prop)
                if val is not None:
                    properties[prop] = val

    except Exception as e:
        unreal.log_error(f"  Error extracting CDO: {e}")

    return properties


def extract_component_defaults(bp_asset):
    """Extract default values for components in the Blueprint"""
    components = {}

    try:
        # Get SimpleConstructionScript for components
        gen_class = bp_asset.generated_class()
        if not gen_class:
            return components

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            return components

        # Try to get components
        try:
            all_components = cdo.get_components_by_class(unreal.ActorComponent)
            for comp in all_components:
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()

                comp_props = {}
                # Extract key properties from component
                for attr in dir(comp):
                    if attr.startswith('_'):
                        continue
                    try:
                        val = get_property_value_as_string(comp, attr)
                        if val is not None:
                            comp_props[attr] = val
                    except:
                        pass

                if comp_props:
                    components[comp_name] = {
                        "class": comp_class,
                        "properties": comp_props
                    }
        except:
            pass

    except Exception as e:
        unreal.log_warning(f"  Error extracting components: {e}")

    return components


def run():
    print("=" * 60)
    print("BLUEPRINT CDO VALUE EXTRACTION")
    print("=" * 60)
    print("")

    all_values = {}
    extracted_count = 0

    for name, path in BLUEPRINTS_TO_EXTRACT.items():
        print(f"Processing: {name}")

        try:
            # Check if asset exists
            if not unreal.EditorAssetLibrary.does_asset_exist(path):
                print(f"  SKIP: Asset not found at {path}")
                continue

            # Load the Blueprint
            bp = unreal.EditorAssetLibrary.load_asset(path)
            if not bp:
                print(f"  SKIP: Could not load asset")
                continue

            # Extract CDO properties
            cdo_props = extract_cdo_properties(bp)

            # Extract component defaults (for Actors)
            components = extract_component_defaults(bp)

            if cdo_props or components:
                all_values[name] = {
                    "path": path,
                    "properties": cdo_props,
                    "components": components
                }
                extracted_count += 1
                print(f"  OK: {len(cdo_props)} properties, {len(components)} components")
            else:
                print(f"  WARN: No properties extracted")

        except Exception as e:
            print(f"  ERROR: {e}")

    # Save to JSON
    print("")
    print(f"Saving to {OUTPUT_FILE}...")

    try:
        with open(OUTPUT_FILE, 'w') as f:
            json.dump(all_values, f, indent=2, default=str)
        print(f"SUCCESS: Extracted {extracted_count} Blueprints")
    except Exception as e:
        print(f"ERROR: Could not save JSON: {e}")

    # Print summary of what was found
    print("")
    print("=" * 60)
    print("EXTRACTED VALUES SUMMARY")
    print("=" * 60)

    for name, data in all_values.items():
        print(f"\n{name}:")
        for prop_name, prop_val in data.get("properties", {}).items():
            if prop_val.get("type") == "object":
                print(f"  {prop_name} = {prop_val.get('path', 'None')}")


run()
