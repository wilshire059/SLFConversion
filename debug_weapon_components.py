# debug_weapon_components.py
# Check actual component structure on weapon Blueprints in SLFConversion
# Run on SLFConversion to see what components exist after reparenting

import unreal
import json
import os

def log(msg):
    unreal.log_warning(str(msg))

WEAPON_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/weapon_debug"

def get_vector_str(vec):
    return f"({vec.x:.4f}, {vec.y:.4f}, {vec.z:.4f})"

def get_rotator_str(rot):
    return f"(P={rot.pitch:.4f}, Y={rot.yaw:.4f}, R={rot.roll:.4f})"

def analyze_blueprint(bp_path):
    """Analyze Blueprint components and CDO state"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        return {"error": "Could not load Blueprint"}

    bp_name = bp_path.split("/")[-1]
    log(f"\n{'='*60}")
    log(f"Analyzing: {bp_name}")
    log(f"{'='*60}")

    results = {
        "blueprint": bp_name,
        "path": bp_path,
    }

    # Get parent class
    try:
        parent = bp.get_editor_property("parent_class")
        if parent:
            results["parent_class"] = parent.get_name()
            results["parent_class_path"] = parent.get_path_name()
            log(f"Parent class: {results['parent_class']}")
    except:
        results["parent_class"] = "Unknown"

    # Get generated class and CDO
    gen_class = bp.generated_class()
    if not gen_class:
        results["error"] = "No generated class"
        return results

    results["generated_class"] = gen_class.get_name()
    log(f"Generated class: {results['generated_class']}")

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        results["error"] = "No CDO"
        return results

    # Get CDO properties for transforms
    cdo_props = {}
    try:
        loc = cdo.get_editor_property("default_mesh_relative_location")
        cdo_props["default_mesh_relative_location"] = get_vector_str(loc)
        log(f"CDO DefaultMeshRelativeLocation: {cdo_props['default_mesh_relative_location']}")
    except Exception as e:
        cdo_props["default_mesh_relative_location"] = str(e)

    try:
        rot = cdo.get_editor_property("default_mesh_relative_rotation")
        cdo_props["default_mesh_relative_rotation"] = get_rotator_str(rot)
        log(f"CDO DefaultMeshRelativeRotation: {cdo_props['default_mesh_relative_rotation']}")
    except Exception as e:
        cdo_props["default_mesh_relative_rotation"] = str(e)

    try:
        mesh = cdo.get_editor_property("default_weapon_mesh")
        if mesh and not mesh.is_null():
            loaded_mesh = mesh.load_synchronous()
            cdo_props["default_weapon_mesh"] = loaded_mesh.get_name() if loaded_mesh else "None"
        else:
            cdo_props["default_weapon_mesh"] = "None/Null"
        log(f"CDO DefaultWeaponMesh: {cdo_props['default_weapon_mesh']}")
    except Exception as e:
        cdo_props["default_weapon_mesh"] = str(e)

    results["cdo_properties"] = cdo_props

    # Get ALL StaticMeshComponents on CDO
    log(f"\nStaticMeshComponents on CDO:")
    components_info = []
    try:
        all_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
        log(f"  Found {len(all_comps)} StaticMeshComponent(s)")

        for comp in all_comps:
            comp_info = {
                "name": comp.get_name(),
            }

            # Get static mesh
            try:
                mesh = comp.get_editor_property("static_mesh")
                comp_info["static_mesh"] = mesh.get_name() if mesh else "None"
            except:
                comp_info["static_mesh"] = "N/A"

            # Get relative location
            try:
                loc = comp.get_editor_property("relative_location")
                comp_info["relative_location"] = get_vector_str(loc)
            except:
                comp_info["relative_location"] = "N/A"

            # Get relative rotation
            try:
                rot = comp.get_editor_property("relative_rotation")
                comp_info["relative_rotation"] = get_rotator_str(rot)
            except:
                comp_info["relative_rotation"] = "N/A"

            # Get visibility
            try:
                visible = comp.get_editor_property("visible")
                comp_info["visible"] = visible
            except:
                comp_info["visible"] = "N/A"

            # Get attachment parent
            try:
                parent_comp = comp.get_attach_parent()
                comp_info["attach_parent"] = parent_comp.get_name() if parent_comp else "None"
            except:
                comp_info["attach_parent"] = "N/A"

            components_info.append(comp_info)
            log(f"  [{comp_info['name']}]")
            log(f"    Mesh: {comp_info['static_mesh']}")
            log(f"    Location: {comp_info['relative_location']}")
            log(f"    Rotation: {comp_info['relative_rotation']}")
            log(f"    Visible: {comp_info['visible']}")
            log(f"    Parent: {comp_info['attach_parent']}")
    except Exception as e:
        log(f"  Error: {e}")

    results["static_mesh_components"] = components_info

    # Check WeaponMesh property directly
    log(f"\nWeaponMesh property on CDO:")
    try:
        weapon_mesh = cdo.get_editor_property("weapon_mesh")
        if weapon_mesh:
            wm_info = {
                "name": weapon_mesh.get_name(),
            }
            try:
                mesh = weapon_mesh.get_editor_property("static_mesh")
                wm_info["static_mesh"] = mesh.get_name() if mesh else "None"
            except:
                wm_info["static_mesh"] = "N/A"
            try:
                loc = weapon_mesh.get_editor_property("relative_location")
                wm_info["relative_location"] = get_vector_str(loc)
            except:
                wm_info["relative_location"] = "N/A"
            try:
                rot = weapon_mesh.get_editor_property("relative_rotation")
                wm_info["relative_rotation"] = get_rotator_str(rot)
            except:
                wm_info["relative_rotation"] = "N/A"

            results["weapon_mesh_property"] = wm_info
            log(f"  Name: {wm_info['name']}")
            log(f"  Mesh: {wm_info['static_mesh']}")
            log(f"  Location: {wm_info['relative_location']}")
            log(f"  Rotation: {wm_info['relative_rotation']}")
        else:
            results["weapon_mesh_property"] = "None"
            log(f"  None")
    except Exception as e:
        results["weapon_mesh_property"] = str(e)
        log(f"  Error: {e}")

    return results

def run_debug():
    log("=" * 70)
    log("WEAPON COMPONENT DEBUG ANALYSIS")
    log("=" * 70)

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    all_results = {}

    for bp_path in WEAPON_PATHS:
        bp_name = bp_path.split("/")[-1]
        results = analyze_blueprint(bp_path)
        all_results[bp_name] = results

    # Write combined results
    output_path = f"{OUTPUT_DIR}/analysis.json"
    with open(output_path, 'w') as f:
        json.dump(all_results, f, indent=2)

    log(f"\n\nAnalysis saved to: {output_path}")

if __name__ == "__main__":
    run_debug()
    log("\nDEBUG COMPLETE")
