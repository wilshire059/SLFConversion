# reparent_player_weapons.py
# Force reparent player weapons to SLFWeaponBase and apply transforms

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Player weapons that need reparenting
PLAYER_WEAPONS = {
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01": {
        "location": unreal.Vector(0.0, 0.0, 36.49),
        "rotation": unreal.Rotator(180.0, 0.0, 0.0),
        "mesh": "/Game/SoulslikeFramework/Demo/Meshes/Weapons/SM_ExampleSword01"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02": {
        "location": unreal.Vector(0.0, 0.0, 50.09),
        "rotation": unreal.Rotator(-130.0, 0.0, 0.0),
        "mesh": "/Game/SoulslikeFramework/Demo/Meshes/Weapons/SM_ExampleSword02"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword": {
        "location": unreal.Vector(0.0, 0.0, 70.0),
        "rotation": unreal.Rotator(180.0, 0.0, 0.0),
        "mesh": "/Game/SoulslikeFramework/Demo/Meshes/Weapons/SM_Greatsword"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana": {
        "location": unreal.Vector(0.0, 0.0, 50.0),
        "rotation": unreal.Rotator(180.0, 0.0, 0.0),
        "mesh": "/Game/SoulslikeFramework/Demo/Meshes/Weapons/SM_Katana"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield": {
        "location": unreal.Vector(0.0, 0.0, 0.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
        "mesh": "/Game/SoulslikeFramework/Demo/Meshes/Weapons/SM_Shield"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace": {
        "location": unreal.Vector(0.0, 0.0, 60.0),
        "rotation": unreal.Rotator(180.0, 0.0, 0.0),
        "mesh": "/Game/SoulslikeFramework/Demo/Meshes/Weapons/SM_BossMace"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword": {
        "location": unreal.Vector(0.0, 0.0, 40.0),
        "rotation": unreal.Rotator(180.0, 0.0, 0.0),
        "mesh": "/Game/SoulslikeFramework/Demo/Meshes/Weapons/SM_PoisonSword"
    },
}

# Base weapon Blueprint (parent for all player weapons originally)
BASE_WEAPON_BP = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon"

def reparent_and_apply():
    log("=" * 70)
    log("REPARENTING PLAYER WEAPONS TO SLFWeaponBase")
    log("=" * 70)

    # Load C++ parent class
    cpp_parent = unreal.load_class(None, "/Script/SLFConversion.SLFWeaponBase")
    if not cpp_parent:
        log("ERROR: Could not load SLFWeaponBase C++ class!")
        return

    log(f"C++ Parent Class: {cpp_parent.get_name()}")

    # First, reparent the base weapon Blueprint
    log(f"\n--- Reparenting Base Weapon ---")
    base_bp = unreal.EditorAssetLibrary.load_asset(BASE_WEAPON_BP)
    if base_bp:
        try:
            # Check current parent
            current_parent = base_bp.get_editor_property("parent_class")
            log(f"  Current parent: {current_parent.get_name() if current_parent else 'None'}")

            # Reparent to C++
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(base_bp, cpp_parent)
            log(f"  Reparent result: {result}")

            if result:
                unreal.EditorAssetLibrary.save_asset(BASE_WEAPON_BP)
                log(f"  Saved B_Item_Weapon")
        except Exception as e:
            log(f"  ERROR reparenting base: {e}")

    # Now process each player weapon
    for bp_path, transform_data in PLAYER_WEAPONS.items():
        bp_name = bp_path.split("/")[-1]
        log(f"\n--- {bp_name} ---")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        # Check current parent
        try:
            current_parent = bp.get_editor_property("parent_class")
            parent_name = current_parent.get_name() if current_parent else "None"
            log(f"  Current parent: {parent_name}")
        except:
            parent_name = "Unknown"
            log(f"  Could not get current parent")

        # Only reparent if not already child of SLFWeaponBase
        gen_class = bp.generated_class()
        if gen_class:
            # Check inheritance
            is_slf_weapon = False
            try:
                # Try to cast CDO to see if it's SLFWeaponBase
                cdo = unreal.get_default_object(gen_class)
                # Check for SLFWeaponBase properties
                try:
                    cdo.get_editor_property("default_mesh_relative_location")
                    is_slf_weapon = True
                except:
                    is_slf_weapon = False
            except:
                pass

            log(f"  Has SLFWeaponBase properties: {is_slf_weapon}")

            if not is_slf_weapon:
                # Need to reparent
                try:
                    # Reparent to B_Item_Weapon (which is now SLFWeaponBase child)
                    # Or directly to SLFWeaponBase
                    base_bp_gen = None
                    if base_bp:
                        base_bp_gen = base_bp.generated_class()

                    if base_bp_gen:
                        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, base_bp_gen)
                        log(f"  Reparented to B_Item_Weapon: {result}")
                    else:
                        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_parent)
                        log(f"  Reparented to SLFWeaponBase: {result}")

                    if result:
                        unreal.EditorAssetLibrary.save_asset(bp_path)
                except Exception as e:
                    log(f"  ERROR reparenting: {e}")

        # Reload and apply transforms
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            continue

        try:
            cdo = unreal.get_default_object(gen_class)

            # Apply location
            cdo.set_editor_property("default_mesh_relative_location", transform_data["location"])
            log(f"  Set location: {transform_data['location']}")

            # Apply rotation
            cdo.set_editor_property("default_mesh_relative_rotation", transform_data["rotation"])
            log(f"  Set rotation: {transform_data['rotation']}")

            # Apply mesh
            mesh = unreal.EditorAssetLibrary.load_asset(transform_data["mesh"])
            if mesh:
                cdo.set_editor_property("default_weapon_mesh", mesh)
                log(f"  Set mesh: {mesh.get_name()}")
            else:
                log(f"  WARNING: Could not load mesh {transform_data['mesh']}")

            # Save
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  SAVED")

        except Exception as e:
            log(f"  ERROR applying transforms: {e}")

    log("\n" + "=" * 70)
    log("REPARENTING COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    reparent_and_apply()
