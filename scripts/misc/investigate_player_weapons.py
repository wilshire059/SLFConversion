# investigate_player_weapons.py
# Comprehensive investigation of player weapon setup

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

def get_class_hierarchy(gen_class):
    """Get full parent class chain"""
    chain = []
    current = gen_class
    while current:
        chain.append(current.get_name())
        try:
            current = unreal.SystemLibrary.get_super_class(current)
        except:
            break
    return chain

def investigate_weapons():
    log("")
    log("=" * 80)
    log("COMPREHENSIVE PLAYER WEAPON INVESTIGATION")
    log("=" * 80)

    # Find ALL weapon blueprints
    weapon_paths = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    ]

    # Also search for any other weapons
    all_assets = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/", recursive=True)
    for asset in all_assets:
        if asset.endswith("_C"):
            continue
        clean_path = asset.replace(".uasset", "").split(".")[0]
        if clean_path not in weapon_paths and "B_Item_Weapon" in clean_path:
            weapon_paths.append(clean_path)

    log(f"Found {len(weapon_paths)} weapon blueprints")

    results = {}

    for bp_path in sorted(weapon_paths):
        bp_name = bp_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{bp_name}]")
        log(f"{'='*60}")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        weapon_data = {"path": bp_path}

        # Get class hierarchy
        hierarchy = get_class_hierarchy(gen_class)
        log(f"  CLASS HIERARCHY: {' -> '.join(hierarchy[:6])}")
        weapon_data["hierarchy"] = hierarchy[:6]

        # Check if it inherits from SLFWeaponBase
        has_slf_weapon_base = "SLFWeaponBase" in hierarchy
        log(f"  INHERITS SLFWeaponBase: {has_slf_weapon_base}")
        weapon_data["has_slf_weapon_base"] = has_slf_weapon_base

        # Get CDO
        try:
            cdo = unreal.get_default_object(gen_class)
        except:
            cdo = None

        if cdo:
            # Check for transform properties
            try:
                loc = cdo.get_editor_property("default_mesh_relative_location")
                log(f"  CDO Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
                weapon_data["cdo_location"] = f"({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})"
            except Exception as e:
                log(f"  CDO Location: PROPERTY NOT FOUND - {e}")
                weapon_data["cdo_location"] = "NOT FOUND"

            try:
                rot = cdo.get_editor_property("default_mesh_relative_rotation")
                log(f"  CDO Rotation: (P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f})")
                weapon_data["cdo_rotation"] = f"(P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f})"
            except Exception as e:
                log(f"  CDO Rotation: PROPERTY NOT FOUND - {e}")
                weapon_data["cdo_rotation"] = "NOT FOUND"

            try:
                mesh = cdo.get_editor_property("default_weapon_mesh")
                mesh_name = mesh.get_name() if mesh else "None"
                log(f"  CDO Mesh: {mesh_name}")
                weapon_data["cdo_mesh"] = mesh_name
            except Exception as e:
                log(f"  CDO Mesh: PROPERTY NOT FOUND - {e}")
                weapon_data["cdo_mesh"] = "NOT FOUND"

        # Spawn actor to check components
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            spawn_loc = unreal.Vector(-100000, -100000, -100000)
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc)

            if actor:
                log(f"  SPAWNED ACTOR COMPONENTS:")
                mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
                weapon_data["components"] = []

                for comp in mesh_comps:
                    comp_name = comp.get_name()
                    rel_loc = comp.get_editor_property("relative_location")
                    rel_rot = comp.get_editor_property("relative_rotation")
                    mesh = comp.get_editor_property("static_mesh")
                    mesh_name = mesh.get_name() if mesh else "NONE"

                    log(f"    [{comp_name}]")
                    log(f"      Location: ({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})")
                    log(f"      Rotation: (P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f})")
                    log(f"      Mesh: {mesh_name}")

                    weapon_data["components"].append({
                        "name": comp_name,
                        "location": f"({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})",
                        "rotation": f"(P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f})",
                        "mesh": mesh_name
                    })

                actor.destroy_actor()
        except Exception as e:
            log(f"  SPAWN ERROR: {e}")

        results[bp_name] = weapon_data

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/player_weapon_investigation.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)
    log(f"\n\nSaved to: {output_path}")

if __name__ == "__main__":
    investigate_weapons()
