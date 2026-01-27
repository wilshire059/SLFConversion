# test_weapon_offset.py
# Test different Z offsets to find the correct weapon grip position

import unreal

def log(msg):
    print(f"[TestOffset] {msg}")
    unreal.log_warning(f"[TestOffset] {msg}")

# Test with SwordExample01
BP_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01"

# Try different Z values to find the right one
# Original bp_only: Z=36.49
# If wrist level means too high, try negative
TEST_Z_VALUES = [0.0, -10.0, -20.0, -36.49]

def main():
    log("=" * 70)
    log("TESTING WEAPON Z OFFSETS")
    log("=" * 70)

    bp = unreal.load_asset(BP_PATH)
    if not bp:
        log("Could not load Blueprint")
        return

    gen_class = bp.generated_class()
    cdo = unreal.get_default_object(gen_class)

    # Get current values
    current_loc = cdo.get_editor_property('default_mesh_relative_location')
    current_rot = cdo.get_editor_property('default_mesh_relative_rotation')

    log(f"Current Location: X={current_loc.x:.4f}, Y={current_loc.y:.4f}, Z={current_loc.z:.4f}")
    log(f"Current Rotation: P={current_rot.pitch:.2f}, Y={current_rot.yaw:.2f}, R={current_rot.roll:.2f}")

    # Try Z=0 to see baseline
    log("\n--- Setting Z=0 (baseline test) ---")
    new_loc = unreal.Vector(0.0, 0.0, 0.0)
    cdo.set_editor_property('default_mesh_relative_location', new_loc)

    # Keep the roll=180 rotation
    new_rot = unreal.Rotator(180.0, 0.0, 0.0)  # Roll=180
    cdo.set_editor_property('default_mesh_relative_rotation', new_rot)

    # Verify
    check_loc = cdo.get_editor_property('default_mesh_relative_location')
    check_rot = cdo.get_editor_property('default_mesh_relative_rotation')
    log(f"New Location: X={check_loc.x:.4f}, Y={check_loc.y:.4f}, Z={check_loc.z:.4f}")
    log(f"New Rotation: P={check_rot.pitch:.2f}, Y={check_rot.yaw:.2f}, R={check_rot.roll:.2f}")

    # Save
    unreal.EditorAssetLibrary.save_asset(BP_PATH)
    log("[SAVED] - Test with Z=0 in PIE to see if weapon moves")

if __name__ == "__main__":
    main()
