# extract_bp_only_scs.py
# Extract the actual SCS component transforms from bp_only Blueprints

import unreal
import json

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
]

def log(msg):
    print(f"[Extract] {msg}")
    unreal.log_warning(f"[Extract] {msg}")

def main():
    log("=" * 70)
    log("EXTRACTING BP_ONLY SCS COMPONENT TRANSFORMS")
    log("=" * 70)

    results = {}

    for bp_path in WEAPONS:
        bp_name = bp_path.split('/')[-1]
        log(f"\n--- {bp_name} ---")

        bp = unreal.load_asset(bp_path)
        if not bp:
            log(f"  Could not load: {bp_path}")
            continue

        results[bp_name] = {
            "path": bp_path,
            "scs_components": [],
            "cdo_properties": {}
        }

        # Check CDO properties
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    loc = cdo.get_editor_property('default_mesh_relative_location')
                    rot = cdo.get_editor_property('default_mesh_relative_rotation')
                    results[bp_name]["cdo_properties"]["default_mesh_relative_location"] = {
                        "x": loc.x, "y": loc.y, "z": loc.z
                    }
                    results[bp_name]["cdo_properties"]["default_mesh_relative_rotation"] = {
                        "pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll
                    }
                    log(f"  CDO Property - Location: Z={loc.z:.4f}")
                    log(f"  CDO Property - Rotation: R={rot.roll:.2f}")
                except Exception as e:
                    log(f"  CDO property error: {e}")

                # Also check actual WeaponMesh component on CDO
                try:
                    mesh_comp = cdo.get_editor_property('weapon_mesh')
                    if mesh_comp:
                        comp_loc = mesh_comp.get_editor_property('relative_location')
                        comp_rot = mesh_comp.get_editor_property('relative_rotation')
                        log(f"  CDO WeaponMesh Component - Location: Z={comp_loc.z:.4f}")
                        log(f"  CDO WeaponMesh Component - Rotation: R={comp_rot.roll:.2f}")
                        results[bp_name]["cdo_weapon_mesh"] = {
                            "location": {"x": comp_loc.x, "y": comp_loc.y, "z": comp_loc.z},
                            "rotation": {"pitch": comp_rot.pitch, "yaw": comp_rot.yaw, "roll": comp_rot.roll}
                        }
                except Exception as e:
                    log(f"  WeaponMesh error: {e}")

        # Check SCS
        try:
            scs = bp.get_editor_property('simple_construction_script')
            if scs:
                all_nodes = scs.get_all_nodes()
                log(f"  SCS has {len(all_nodes)} nodes")
                for node in all_nodes:
                    comp_template = node.get_editor_property('component_template')
                    if comp_template:
                        comp_name = comp_template.get_name()
                        comp_class = comp_template.get_class().get_name()
                        log(f"    SCS Node: {comp_name} ({comp_class})")

                        if 'StaticMesh' in comp_class or 'Mesh' in comp_name:
                            try:
                                loc = comp_template.get_editor_property('relative_location')
                                rot = comp_template.get_editor_property('relative_rotation')
                                log(f"      SCS Location: Z={loc.z:.4f}")
                                log(f"      SCS Rotation: R={rot.roll:.2f}")
                                results[bp_name]["scs_components"].append({
                                    "name": comp_name,
                                    "class": comp_class,
                                    "location": {"x": loc.x, "y": loc.y, "z": loc.z},
                                    "rotation": {"pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll}
                                })
                            except Exception as e:
                                log(f"      Transform error: {e}")
        except Exception as e:
            log(f"  SCS error: {e}")

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/bp_only_scs_extract.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
