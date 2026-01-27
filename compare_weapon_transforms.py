# compare_weapon_transforms.py
# Compare weapon mesh transforms and socket settings between projects

import unreal
import json

def log(msg):
    print(f"[WeaponCmp] {msg}")
    unreal.log_warning(f"[WeaponCmp] {msg}")

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/weapon_transform_comparison.json"

WEAPONS_TO_CHECK = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def get_weapon_info(weapon_path):
    """Extract weapon mesh and socket transform info"""
    info = {"path": weapon_path, "exists": False}

    bp = unreal.load_asset(weapon_path)
    if not bp:
        return info

    info["exists"] = True
    info["class"] = bp.get_class().get_name()

    gen_class = None
    if hasattr(bp, 'generated_class'):
        gen_class = bp.generated_class()

    if not gen_class:
        return info

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        return info

    # Check C++ properties for mesh transforms
    props_to_check = [
        'default_mesh_relative_location',
        'default_mesh_relative_rotation',
        'default_weapon_mesh',
        'weapon_mesh',
        'item_info',
        'attachment_rotation_offset',
        'weapon_socket_name',
        'right_hand_socket',
        'left_hand_socket',
    ]

    for prop in props_to_check:
        try:
            val = cdo.get_editor_property(prop)
            if val is not None:
                if hasattr(val, 'x'):  # Vector/Rotator
                    info[prop] = f"({val.x}, {val.y}, {val.z})"
                elif hasattr(val, 'get_name'):
                    info[prop] = val.get_name()
                elif hasattr(val, 'get_path_name'):
                    info[prop] = val.get_path_name()
                else:
                    info[prop] = str(val)
        except:
            pass

    # Check components for mesh and transform settings
    try:
        components = cdo.get_components_by_class(unreal.StaticMeshComponent)
        for i, comp in enumerate(components):
            comp_info = {}
            comp_info["name"] = comp.get_name()

            # Get relative transform
            rel_loc = comp.get_editor_property('relative_location')
            rel_rot = comp.get_editor_property('relative_rotation')

            if rel_loc:
                comp_info["relative_location"] = f"({rel_loc.x}, {rel_loc.y}, {rel_loc.z})"
            if rel_rot:
                comp_info["relative_rotation"] = f"({rel_rot.pitch}, {rel_rot.yaw}, {rel_rot.roll})"

            # Get mesh
            mesh = comp.get_editor_property('static_mesh')
            if mesh:
                comp_info["mesh"] = mesh.get_name()

            info[f"component_{i}"] = comp_info
    except Exception as e:
        info["component_error"] = str(e)

    return info

def main():
    log("=" * 80)
    log("WEAPON TRANSFORM COMPARISON")
    log("=" * 80)

    results = {}

    for weapon_path in WEAPONS_TO_CHECK:
        weapon_name = weapon_path.split("/")[-1]
        log(f"\n--- {weapon_name} ---")

        info = get_weapon_info(weapon_path)
        results[weapon_name] = info

        for key, val in info.items():
            if key != "path":
                log(f"  {key}: {val}")

    # Save results
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {OUTPUT_PATH}")

    log("=" * 80)

if __name__ == "__main__":
    main()
