# verify_sword01_cdo.py
# Check SwordExample01 CDO transform values

import unreal

WEAPON_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01"

def log(msg):
    print(f"[CDOCheck] {msg}")
    unreal.log_warning(f"[CDOCheck] {msg}")

def main():
    log("=" * 70)
    log("CHECKING SwordExample01 CDO TRANSFORM VALUES")
    log("=" * 70)

    bp = unreal.load_asset(WEAPON_PATH)
    if not bp:
        log("ERROR: Could not load Blueprint")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        log("ERROR: No generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("ERROR: No CDO")
        return

    log(f"Blueprint: {WEAPON_PATH}")
    log(f"Class: {gen_class.get_name()}")

    # Check DefaultMeshRelativeLocation
    try:
        loc = cdo.get_editor_property('default_mesh_relative_location')
        log(f"DefaultMeshRelativeLocation: X={loc.x:.4f}, Y={loc.y:.4f}, Z={loc.z:.4f}")
    except Exception as e:
        log(f"ERROR getting DefaultMeshRelativeLocation: {e}")

    # Check DefaultMeshRelativeRotation
    try:
        rot = cdo.get_editor_property('default_mesh_relative_rotation')
        log(f"DefaultMeshRelativeRotation: P={rot.pitch:.4f}, Y={rot.yaw:.4f}, R={rot.roll:.4f}")
    except Exception as e:
        log(f"ERROR getting DefaultMeshRelativeRotation: {e}")

    # Check DefaultWeaponMesh
    try:
        mesh = cdo.get_editor_property('default_weapon_mesh')
        log(f"DefaultWeaponMesh: {mesh.asset_name if mesh else 'NULL'}")
    except Exception as e:
        log(f"ERROR getting DefaultWeaponMesh: {e}")

    # Expected values from bp_only
    log("")
    log("EXPECTED VALUES (from bp_only):")
    log("  Location: X=0.000, Y=0.000, Z=36.494")
    log("  Rotation: P=0.000, Y=0.000, R=180.000")

if __name__ == "__main__":
    main()
