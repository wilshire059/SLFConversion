# compare_weapon_configs.py
# Run on BOTH bp_only and SLFConversion to compare ALL weapon properties
# Outputs detailed JSON with component transforms, attachment settings, and hierarchy

import unreal
import json
import os

def log(msg):
    unreal.log_warning(str(msg))

WEAPON_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

BASE_WEAPON_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon"

def get_rotator_dict(rot):
    """Convert rotator to dict with normalized values"""
    return {
        "pitch": rot.pitch if hasattr(rot, 'pitch') else 0.0,
        "yaw": rot.yaw if hasattr(rot, 'yaw') else 0.0,
        "roll": rot.roll if hasattr(rot, 'roll') else 0.0
    }

def get_vector_dict(vec):
    """Convert vector to dict"""
    return {
        "x": vec.x if hasattr(vec, 'x') else 0.0,
        "y": vec.y if hasattr(vec, 'y') else 0.0,
        "z": vec.z if hasattr(vec, 'z') else 0.0
    }

def export_component_data(component):
    """Extract all relevant data from a component"""
    data = {
        "class": type(component).__name__ if component else "None",
        "name": component.get_name() if component else "None"
    }

    if component:
        try:
            rel_loc = component.get_editor_property("relative_location")
            data["relative_location"] = get_vector_dict(rel_loc)
        except:
            data["relative_location"] = "N/A"

        try:
            rel_rot = component.get_editor_property("relative_rotation")
            data["relative_rotation"] = get_rotator_dict(rel_rot)
        except:
            data["relative_rotation"] = "N/A"

        try:
            rel_scale = component.get_editor_property("relative_scale3d")
            data["relative_scale"] = get_vector_dict(rel_scale)
        except:
            data["relative_scale"] = "N/A"

        try:
            visibility = component.get_editor_property("visible")
            data["visible"] = visibility
        except:
            data["visible"] = "N/A"

        # For StaticMeshComponent, get the mesh
        if hasattr(component, 'get_editor_property'):
            try:
                mesh = component.get_editor_property("static_mesh")
                if mesh:
                    data["static_mesh"] = mesh.get_name()
            except:
                pass

    return data

def export_weapon_details(bp_path):
    """Export all details from a weapon Blueprint"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        return {"error": "Could not load Blueprint"}

    bp_name = bp_path.split("/")[-1]
    data = {
        "path": bp_path,
        "name": bp_name,
    }

    # Get parent class info
    try:
        parent = bp.get_editor_property("parent_class")
        if parent:
            data["parent_class"] = parent.get_name()
            data["parent_class_path"] = parent.get_path_name()
    except:
        data["parent_class"] = "Unknown"

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        data["error"] = "No generated class"
        return data

    data["generated_class"] = gen_class.get_name()

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        data["error"] = "No CDO"
        return data

    # Export CDO properties
    cdo_props = {}

    # Check for C++ properties (SLFWeaponBase)
    try:
        val = cdo.get_editor_property("default_mesh_relative_location")
        cdo_props["default_mesh_relative_location"] = get_vector_dict(val)
    except Exception as e:
        cdo_props["default_mesh_relative_location"] = f"N/A: {e}"

    try:
        val = cdo.get_editor_property("default_mesh_relative_rotation")
        cdo_props["default_mesh_relative_rotation"] = get_rotator_dict(val)
    except Exception as e:
        cdo_props["default_mesh_relative_rotation"] = f"N/A: {e}"

    try:
        val = cdo.get_editor_property("default_weapon_mesh")
        if val:
            cdo_props["default_weapon_mesh"] = val.get_name() if hasattr(val, 'get_name') else str(val)
        else:
            cdo_props["default_weapon_mesh"] = "None"
    except Exception as e:
        cdo_props["default_weapon_mesh"] = f"N/A: {e}"

    try:
        val = cdo.get_editor_property("default_attachment_rotation_offset")
        cdo_props["default_attachment_rotation_offset"] = get_rotator_dict(val)
    except Exception as e:
        cdo_props["default_attachment_rotation_offset"] = f"N/A: {e}"

    data["cdo_properties"] = cdo_props

    # Get SCS components
    scs_components = {}
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            root_nodes = scs.get_all_nodes()
            for node in root_nodes:
                if node:
                    comp_template = node.component_template
                    if comp_template:
                        comp_name = comp_template.get_name()
                        scs_components[comp_name] = export_component_data(comp_template)
    except Exception as e:
        scs_components["error"] = str(e)

    data["scs_components"] = scs_components

    # Check for WeaponMesh component directly on CDO
    try:
        weapon_mesh = cdo.get_editor_property("weapon_mesh")
        if weapon_mesh:
            data["weapon_mesh_component"] = export_component_data(weapon_mesh)
    except:
        data["weapon_mesh_component"] = "N/A"

    return data

def export_base_weapon_details():
    """Export details from B_Item_Weapon base class"""
    bp = unreal.EditorAssetLibrary.load_asset(BASE_WEAPON_PATH)
    if not bp:
        return {"error": "Could not load base weapon Blueprint"}

    data = {
        "path": BASE_WEAPON_PATH,
        "name": "B_Item_Weapon"
    }

    # Get parent class
    try:
        parent = bp.get_editor_property("parent_class")
        if parent:
            data["parent_class"] = parent.get_name()
            data["parent_class_path"] = parent.get_path_name()
    except:
        pass

    # Get SCS components from base class
    scs_components = {}
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            root_nodes = scs.get_all_nodes()
            for node in root_nodes:
                if node:
                    comp_template = node.component_template
                    if comp_template:
                        comp_name = comp_template.get_name()
                        scs_components[comp_name] = export_component_data(comp_template)
    except Exception as e:
        scs_components["error"] = str(e)

    data["scs_components"] = scs_components

    return data

def run_export():
    log("=" * 70)
    log("COMPREHENSIVE WEAPON CONFIGURATION EXPORT")
    log("=" * 70)

    all_data = {
        "project": "",  # Will be filled based on which project runs this
        "base_weapon": export_base_weapon_details(),
        "weapons": {}
    }

    # Determine which project based on parent class
    test_bp = unreal.EditorAssetLibrary.load_asset(WEAPON_PATHS[0])
    if test_bp:
        try:
            parent = test_bp.get_editor_property("parent_class")
            if parent:
                parent_name = parent.get_name()
                if "SLF" in parent_name or "SLFWeaponBase" in parent_name:
                    all_data["project"] = "SLFConversion"
                else:
                    all_data["project"] = "bp_only"
        except:
            all_data["project"] = "unknown"

    log(f"Project detected: {all_data['project']}")

    # Export each weapon
    for bp_path in WEAPON_PATHS:
        bp_name = bp_path.split("/")[-1]
        log(f"\nExporting: {bp_name}")
        weapon_data = export_weapon_details(bp_path)
        all_data["weapons"][bp_name] = weapon_data

        # Log key values
        if "cdo_properties" in weapon_data:
            props = weapon_data["cdo_properties"]
            log(f"  CDO Location: {props.get('default_mesh_relative_location', 'N/A')}")
            log(f"  CDO Rotation: {props.get('default_mesh_relative_rotation', 'N/A')}")

    # Determine output path
    if all_data["project"] == "SLFConversion":
        output_path = "C:/scripts/SLFConversion/migration_cache/slfconversion_weapon_config.json"
    else:
        output_path = "C:/scripts/SLFConversion/migration_cache/bp_only_weapon_config.json"

    # Ensure directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    with open(output_path, 'w') as f:
        json.dump(all_data, f, indent=2)

    log(f"\n\nExported to: {output_path}")
    return all_data

if __name__ == "__main__":
    run_export()
    log("\nEXPORT COMPLETE")
