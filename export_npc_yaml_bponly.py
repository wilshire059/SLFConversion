"""
Export ALL NPC system assets from bp_only to YAML.
This is the SOURCE OF TRUTH for what we need to implement.
"""
import unreal
import os
import json

OUTPUT_DIR = "C:/scripts/SLFConversion/Exports/NPC_System/bp_only"

# ALL NPC-related assets to export
NPC_ASSETS = {
    # Characters
    "B_Soulslike_NPC": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "B_Soulslike_NPC_ShowcaseGuide": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "B_Soulslike_NPC_ShowcaseVendor": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",

    # Components
    "AC_AI_InteractionManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager",

    # Data Assets
    "PDA_Dialog": "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog",
    "PDA_Vendor": "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor",
    "DA_ExampleDialog": "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog",
    "DA_ExampleDialog_Vendor": "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor",
    "DA_ExampleVendor": "/Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor",

    # Widgets
    "W_Dialog": "/Game/SoulslikeFramework/Widgets/HUD/W_Dialog",
    "W_NPC_Window": "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window",
    "W_NPC_Window_Vendor": "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor",
    "W_VendorSlot": "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot",
    "W_VendorAction": "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorAction",
}

def get_class_hierarchy(obj):
    """Get the full class hierarchy."""
    hierarchy = []
    cls = obj.get_class()
    while cls:
        hierarchy.append(cls.get_name())
        cls = cls.get_super_class()
    return hierarchy

def get_all_properties(obj):
    """Extract all properties from an object."""
    props = {}
    try:
        # Get all property names via iteration
        for prop in dir(obj):
            if prop.startswith('_'):
                continue
            try:
                val = getattr(obj, prop)
                if callable(val):
                    continue
                # Convert to string representation
                props[prop] = str(val)
            except:
                pass
    except Exception as e:
        props["_error"] = str(e)
    return props

def export_blueprint_details(bp, bp_name):
    """Export detailed Blueprint information."""
    data = {
        "name": bp_name,
        "class": bp.get_class().get_name(),
        "hierarchy": get_class_hierarchy(bp),
        "parent_class": None,
        "variables": [],
        "functions": [],
        "components": [],
        "interfaces": [],
        "event_dispatchers": [],
    }

    # Get generated class
    gen_class = None
    try:
        gen_class = bp.generated_class()
        if gen_class:
            data["generated_class"] = gen_class.get_name()
            parent = gen_class.get_super_class()
            if parent:
                data["parent_class"] = parent.get_name()
    except:
        pass

    # Get CDO properties
    if gen_class:
        try:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                data["cdo_properties"] = get_all_properties(cdo)
        except Exception as e:
            data["cdo_error"] = str(e)

    # Try to get Blueprint-specific info
    try:
        # Variables
        var_names = unreal.BlueprintEditorLibrary.get_blueprint_variable_names(bp)
        for var_name in var_names:
            var_info = {"name": var_name}
            try:
                meta = unreal.BlueprintEditorLibrary.get_blueprint_variable_metadata(bp, var_name, "Category")
                var_info["category"] = meta
            except:
                pass
            data["variables"].append(var_info)
    except Exception as e:
        data["variables_error"] = str(e)

    # Try to get functions
    try:
        func_names = unreal.BlueprintEditorLibrary.get_blueprint_function_names(bp)
        data["functions"] = list(func_names)
    except Exception as e:
        data["functions_error"] = str(e)

    # Try to get interfaces
    try:
        interfaces = unreal.BlueprintEditorLibrary.get_blueprint_implemented_interfaces(bp)
        data["interfaces"] = [str(i) for i in interfaces]
    except Exception as e:
        data["interfaces_error"] = str(e)

    return data

def export_component_details(bp, bp_name):
    """Export component Blueprint details - similar to regular BP but for ActorComponents."""
    return export_blueprint_details(bp, bp_name)

def export_widget_details(widget_bp, bp_name):
    """Export Widget Blueprint details."""
    data = export_blueprint_details(widget_bp, bp_name)
    data["widget_type"] = "WidgetBlueprint"

    # Try to get widget tree info
    try:
        # Widget tree is in the WidgetTree property
        widget_tree = widget_bp.get_editor_property("widget_tree")
        if widget_tree:
            root = widget_tree.get_editor_property("root_widget")
            if root:
                data["root_widget"] = root.get_name()
                data["root_widget_class"] = root.get_class().get_name()
    except Exception as e:
        data["widget_tree_error"] = str(e)

    return data

def export_data_asset(asset, asset_name):
    """Export Data Asset properties."""
    data = {
        "name": asset_name,
        "class": asset.get_class().get_name(),
        "hierarchy": get_class_hierarchy(asset),
        "properties": get_all_properties(asset),
    }
    return data

def export_to_yaml(data, filename):
    """Export data to YAML-like format (using JSON for simplicity)."""
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    filepath = os.path.join(OUTPUT_DIR, f"{filename}.yaml")

    # Use JSON but with nice formatting
    with open(filepath, 'w', encoding='utf-8') as f:
        # Write as pseudo-YAML for readability
        f.write(f"# {filename} - Exported from bp_only\n")
        f.write(f"# Source of truth for NPC system migration\n\n")
        json.dump(data, f, indent=2, default=str)

    return filepath

def main():
    unreal.log_warning("\n" + "="*60)
    unreal.log_warning("EXPORTING NPC SYSTEM FROM BP_ONLY (SOURCE OF TRUTH)")
    unreal.log_warning("="*60 + "\n")

    exported = 0
    failed = 0

    for name, path in NPC_ASSETS.items():
        unreal.log_warning(f"Processing: {name}")

        asset = unreal.EditorAssetLibrary.load_asset(path)
        if not asset:
            unreal.log_warning(f"  [MISSING] {path}")
            failed += 1
            continue

        asset_class = asset.get_class().get_name()
        unreal.log_warning(f"  Class: {asset_class}")

        try:
            if "WidgetBlueprint" in asset_class:
                data = export_widget_details(asset, name)
            elif "Blueprint" in asset_class:
                data = export_blueprint_details(asset, name)
            elif "DataAsset" in asset_class or "PrimaryDataAsset" in asset_class:
                data = export_data_asset(asset, name)
            else:
                data = export_data_asset(asset, name)

            filepath = export_to_yaml(data, name)
            unreal.log_warning(f"  [OK] Exported to: {filepath}")
            exported += 1

        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")
            failed += 1

    unreal.log_warning("\n" + "="*60)
    unreal.log_warning(f"EXPORT COMPLETE: {exported} succeeded, {failed} failed")
    unreal.log_warning(f"Output: {OUTPUT_DIR}")
    unreal.log_warning("="*60 + "\n")

if __name__ == "__main__":
    main()
