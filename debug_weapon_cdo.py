# debug_weapon_cdo.py
# Check if DefaultMeshRelativeLocation/Rotation are set on weapon CDOs

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
]

def debug_weapon_cdos():
    log("")
    log("=" * 70)
    log("DEBUGGING WEAPON CDO PROPERTIES")
    log("=" * 70)

    for bp_path in WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  No generated class")
            continue

        # Get parent class
        try:
            parent = unreal.SystemLibrary.get_super_class(gen_class)
            log(f"  Parent: {parent.get_name() if parent else 'None'}")
        except:
            log(f"  Could not get parent")

        try:
            cdo = unreal.get_default_object(gen_class)
        except:
            cdo = None

        if not cdo:
            log(f"  Could not get CDO")
            continue

        # Check DefaultMeshRelativeLocation
        try:
            loc = cdo.get_editor_property("default_mesh_relative_location")
            log(f"  DefaultMeshRelativeLocation: ({loc.x}, {loc.y}, {loc.z})")
        except Exception as e:
            log(f"  DefaultMeshRelativeLocation ERROR: {e}")

        # Check DefaultMeshRelativeRotation
        try:
            rot = cdo.get_editor_property("default_mesh_relative_rotation")
            log(f"  DefaultMeshRelativeRotation: (P={rot.pitch}, Y={rot.yaw}, R={rot.roll})")
        except Exception as e:
            log(f"  DefaultMeshRelativeRotation ERROR: {e}")

        # Check DefaultWeaponMesh
        try:
            mesh = cdo.get_editor_property("default_weapon_mesh")
            log(f"  DefaultWeaponMesh: {mesh.get_name() if mesh else 'None'}")
        except Exception as e:
            log(f"  DefaultWeaponMesh ERROR: {e}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    debug_weapon_cdos()
