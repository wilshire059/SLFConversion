"""
Export dialog system configuration from both bp_only and SLFConversion for comparison.
Run this FIRST on bp_only, then on SLFConversion.
"""
import unreal
import json
import os

# Determine which project we're in
project_name = "SLFConversion" if "SLFConversion" in unreal.Paths.project_dir() else "bp_only"
OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/dialog_comparison"
OUTPUT_FILE = f"{OUTPUT_DIR}/{project_name}_dialog_export.json"

def get_struct_info(ustruct):
    """Get detailed struct information"""
    if not ustruct:
        return None

    info = {
        "name": ustruct.get_name() if hasattr(ustruct, 'get_name') else str(ustruct),
        "path": ustruct.get_path_name() if hasattr(ustruct, 'get_path_name') else str(ustruct),
    }
    return info

def export_datatable_info(dt_path):
    """Export DataTable structure and content info"""
    dt = unreal.EditorAssetLibrary.load_asset(dt_path)
    if not dt:
        return {"error": f"Could not load {dt_path}"}

    info = {
        "path": dt_path,
        "class": dt.get_class().get_name(),
    }

    # Get row struct type
    try:
        row_struct = dt.get_editor_property('row_struct')
        if row_struct:
            info["row_struct"] = get_struct_info(row_struct)
    except Exception as e:
        info["row_struct_error"] = str(e)

    # Get row names
    try:
        row_names = dt.get_row_names()
        info["row_count"] = len(row_names)
        info["row_names"] = [str(r) for r in row_names]
    except Exception as e:
        info["row_names_error"] = str(e)

    return info

def export_data_asset_info(asset_path):
    """Export PDA dialog/vendor asset info"""
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        return {"error": f"Could not load {asset_path}"}

    info = {
        "path": asset_path,
        "class": asset.get_class().get_name(),
        "class_path": asset.get_class().get_path_name(),
    }

    # Get parent class chain
    try:
        class_obj = asset.get_class()
        parent_chain = []
        current = class_obj
        for _ in range(10):  # Prevent infinite loop
            parent_name = current.get_name()
            parent_chain.append(parent_name)
            # Try to get outer
            outer = current.get_outer()
            if outer and hasattr(outer, 'get_name'):
                current = outer
            else:
                break
        info["class_chain"] = parent_chain
    except Exception as e:
        info["class_chain_error"] = str(e)

    # Get specific properties
    properties_to_check = [
        'default_dialog_table',
        'requirement',
        'items',
        'default_slot_count',
        'default_slots_per_row',
    ]

    info["properties"] = {}
    for prop_name in properties_to_check:
        try:
            value = asset.get_editor_property(prop_name)
            if value is not None:
                if hasattr(value, 'get_path_name'):
                    info["properties"][prop_name] = value.get_path_name()
                elif hasattr(value, 'to_soft_object_path'):
                    info["properties"][prop_name] = str(value.to_soft_object_path())
                elif isinstance(value, (list, tuple)):
                    info["properties"][prop_name] = f"Array[{len(value)}]"
                else:
                    info["properties"][prop_name] = str(value)
        except:
            pass  # Property doesn't exist

    return info

def export_widget_info(widget_path):
    """Export widget Blueprint info"""
    widget = unreal.EditorAssetLibrary.load_asset(widget_path)
    if not widget:
        return {"error": f"Could not load {widget_path}"}

    info = {
        "path": widget_path,
        "class": widget.get_class().get_name(),
    }

    # Get generated class
    try:
        gen_class = widget.generated_class()
        if gen_class:
            info["generated_class"] = gen_class.get_name()
    except:
        pass

    return info

def export_blueprint_info(bp_path):
    """Export Blueprint parent class and component info"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        return {"error": f"Could not load {bp_path}"}

    info = {
        "path": bp_path,
        "class": bp.get_class().get_name(),
    }

    # Get generated class and parent
    try:
        gen_class = bp.generated_class()
        if gen_class:
            info["generated_class"] = gen_class.get_name()
            info["generated_class_path"] = gen_class.get_path_name()
    except:
        pass

    return info

def main():
    unreal.log_warning(f"\n{'='*70}")
    unreal.log_warning(f"EXPORTING DIALOG SYSTEM CONFIG FROM: {project_name}")
    unreal.log_warning(f"{'='*70}\n")

    data = {
        "project": project_name,
        "datatables": {},
        "dialog_assets": {},
        "vendor_assets": {},
        "widgets": {},
        "blueprints": {},
    }

    # DataTables to check
    datatables = [
        "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_InProgress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Complete",
    ]

    unreal.log_warning("=== Exporting DataTables ===")
    for dt_path in datatables:
        name = dt_path.split("/")[-1]
        unreal.log_warning(f"  {name}")
        data["datatables"][name] = export_datatable_info(dt_path)

    # Dialog/Vendor assets
    dialog_assets = [
        "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor",
    ]

    unreal.log_warning("\n=== Exporting Dialog Assets ===")
    for asset_path in dialog_assets:
        name = asset_path.split("/")[-1]
        unreal.log_warning(f"  {name}")
        data["dialog_assets"][name] = export_data_asset_info(asset_path)

    vendor_assets = [
        "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor",
        "/Game/SoulslikeFramework/Data/Vendor/ShowcaseVendorNpc/DA_ExampleVendor",
    ]

    unreal.log_warning("\n=== Exporting Vendor Assets ===")
    for asset_path in vendor_assets:
        name = asset_path.split("/")[-1]
        unreal.log_warning(f"  {name}")
        data["vendor_assets"][name] = export_data_asset_info(asset_path)

    # Widgets
    widgets = [
        "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
        "/Game/SoulslikeFramework/Widgets/Dialog/W_Dialog",
        "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window",
        "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor",
    ]

    unreal.log_warning("\n=== Exporting Widgets ===")
    for widget_path in widgets:
        name = widget_path.split("/")[-1]
        unreal.log_warning(f"  {name}")
        data["widgets"][name] = export_widget_info(widget_path)

    # Blueprints
    blueprints = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager",
    ]

    unreal.log_warning("\n=== Exporting Blueprints ===")
    for bp_path in blueprints:
        name = bp_path.split("/")[-1]
        unreal.log_warning(f"  {name}")
        data["blueprints"][name] = export_blueprint_info(bp_path)

    # Save output
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(data, f, indent=2)

    unreal.log_warning(f"\n{'='*70}")
    unreal.log_warning(f"SAVED TO: {OUTPUT_FILE}")
    unreal.log_warning(f"{'='*70}\n")

if __name__ == "__main__":
    main()
