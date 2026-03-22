# compare_weapon_scs.py
# Compare weapon Blueprint SCS (SimpleConstructionScript) component transforms
# between bp_only and SLFConversion
# These are the built-in default transforms stored in the Blueprint

import unreal
import json
import os

# Weapons to test
WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
]

# Determine project
project_path = unreal.Paths.project_dir()
if "bp_only" in project_path.lower():
    PROJECT_NAME = "bp_only"
    OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/weapon_scs_bp_only.json"
else:
    PROJECT_NAME = "SLFConversion"
    OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/weapon_scs_slfconversion.json"

results = {"project": PROJECT_NAME, "weapons": {}}

def log(msg):
    print(f"[SCS:{PROJECT_NAME}] {msg}")
    unreal.log_warning(f"[SCS:{PROJECT_NAME}] {msg}")

def get_vector_dict(v):
    return {"x": round(v.x, 4), "y": round(v.y, 4), "z": round(v.z, 4)}

def get_rotator_dict(r):
    return {"pitch": round(r.pitch, 4), "yaw": round(r.yaw, 4), "roll": round(r.roll, 4)}

def main():
    log("=" * 70)
    log(f"WEAPON SCS COMPONENT TRANSFORM COMPARISON - {PROJECT_NAME}")
    log("=" * 70)

    for weapon_path in WEAPONS:
        weapon_name = weapon_path.split("/")[-1]
        log(f"")
        log(f"--- {weapon_name} ---")

        weapon_data = {"path": weapon_path, "components": {}}

        try:
            # Load Blueprint
            bp = unreal.load_asset(weapon_path)
            if not bp:
                log(f"  ERROR: Could not load Blueprint")
                weapon_data["error"] = "Could not load Blueprint"
                results["weapons"][weapon_name] = weapon_data
                continue

            gen_class = bp.generated_class()
            if not gen_class:
                log(f"  ERROR: No generated class")
                weapon_data["error"] = "No generated class"
                results["weapons"][weapon_name] = weapon_data
                continue

            # Get CDO
            cdo = unreal.get_default_object(gen_class)
            if not cdo:
                log(f"  ERROR: No CDO")
                weapon_data["error"] = "No CDO"
                results["weapons"][weapon_name] = weapon_data
                continue

            log(f"  Class: {gen_class.get_name()}")
            weapon_data["class"] = gen_class.get_name()

            # Check for C++ properties (SLFConversion only)
            try:
                default_loc = cdo.get_editor_property('default_mesh_relative_location')
                default_rot = cdo.get_editor_property('default_mesh_relative_rotation')
                weapon_data["cpp_default_mesh_relative_location"] = get_vector_dict(default_loc)
                weapon_data["cpp_default_mesh_relative_rotation"] = get_rotator_dict(default_rot)
                log(f"  C++ DefaultMeshRelativeLocation: {default_loc}")
                log(f"  C++ DefaultMeshRelativeRotation: {default_rot}")
            except:
                log(f"  (No C++ DefaultMesh properties - this is bp_only)")

            # Spawn actor to check actual component transforms
            world = unreal.EditorLevelLibrary.get_editor_world()
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
                gen_class,
                unreal.Vector(0, 0, 1000),
                unreal.Rotator(0, 0, 0)
            )

            if actor:
                log(f"  Spawned: {actor.get_name()}")

                # Get all components
                all_comps = actor.get_components_by_class(unreal.SceneComponent)
                for comp in all_comps:
                    comp_name = comp.get_name()
                    comp_class = comp.get_class().get_name()

                    # Get relative transform
                    rel_loc = comp.get_editor_property('relative_location')
                    rel_rot = comp.get_editor_property('relative_rotation')

                    comp_data = {
                        "class": comp_class,
                        "relative_location": get_vector_dict(rel_loc),
                        "relative_rotation": get_rotator_dict(rel_rot)
                    }

                    # For StaticMeshComponent, also get mesh name
                    if "StaticMesh" in comp_class:
                        try:
                            mesh = comp.get_editor_property('static_mesh')
                            if mesh:
                                comp_data["static_mesh"] = mesh.get_name()
                        except:
                            pass

                    weapon_data["components"][comp_name] = comp_data

                    # Log if this is a mesh component
                    if "Mesh" in comp_name or "StaticMesh" in comp_class:
                        log(f"  {comp_name} ({comp_class}):")
                        log(f"    Relative Location: {rel_loc}")
                        log(f"    Relative Rotation: {rel_rot}")

                actor.destroy_actor()
            else:
                log(f"  ERROR: Could not spawn actor")

        except Exception as e:
            log(f"  EXCEPTION: {e}")
            weapon_data["error"] = str(e)

        results["weapons"][weapon_name] = weapon_data

    # Save results
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(results, f, indent=2)

    log("")
    log(f"Results saved to: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
