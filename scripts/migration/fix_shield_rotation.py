# fix_shield_rotation.py
# Fix shield rotation to face outward during guard
# The shield's "outside" should point away from the character (toward enemies)
# Currently it points inward. Add 180 degrees to yaw.

import unreal

def log(msg):
    unreal.log_warning(str(msg))

def make_rotator(pitch, yaw, roll):
    r = unreal.Rotator()
    r.pitch = pitch
    r.yaw = yaw
    r.roll = roll
    return r

SHIELD_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield"

def fix_shield():
    log("=" * 60)
    log("FIXING SHIELD ROTATION")
    log("=" * 60)

    # Load Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(SHIELD_PATH)
    if not bp:
        log("ERROR: Could not load Shield Blueprint")
        return False

    # Get generated class and CDO
    gen_class = bp.generated_class()
    if not gen_class:
        log("ERROR: No generated class")
        return False

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("ERROR: Could not get CDO")
        return False

    try:
        # Get current rotation
        current_rot = cdo.get_editor_property("default_mesh_relative_rotation")
        log(f"Current rotation: P={current_rot.pitch}, Y={current_rot.yaw}, R={current_rot.roll}")

        # Add 180 to yaw to flip the shield to face outward
        new_yaw = current_rot.yaw + 180.0
        new_rotation = make_rotator(current_rot.pitch, new_yaw, current_rot.roll)

        log(f"New rotation: P={new_rotation.pitch}, Y={new_rotation.yaw}, R={new_rotation.roll}")

        # Apply the new rotation
        cdo.set_editor_property("default_mesh_relative_rotation", new_rotation)

        # Save
        unreal.EditorAssetLibrary.save_asset(SHIELD_PATH)
        log("SAVED: Shield rotation fixed")
        return True

    except Exception as e:
        log(f"ERROR: {e}")
        return False

if __name__ == "__main__":
    fix_shield()
    log("\nSHIELD ROTATION FIX COMPLETE")
