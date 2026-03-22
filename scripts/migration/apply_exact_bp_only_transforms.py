# apply_exact_bp_only_transforms.py
# Apply EXACT transforms from bp_only - no modifications, no guessing
# Run on SLFConversion project

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

# Load the bp_only transforms
BP_ONLY_DATA_FILE = "C:/scripts/SLFConversion/migration_cache/bp_only_weapon_transforms.json"

WEAPON_PATH_MAP = {
    "B_Item_Weapon_SwordExample01": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "B_Item_Weapon_Greatsword": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "B_Item_Weapon_Katana": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "B_Item_Weapon_Shield": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
}

def make_rotator(pitch, yaw, roll):
    """Create rotator with explicit property assignment"""
    r = unreal.Rotator()
    r.pitch = pitch
    r.yaw = yaw
    r.roll = roll
    return r

def apply_transforms():
    log("=" * 70)
    log("APPLYING EXACT BP_ONLY TRANSFORMS TO SLFCONVERSION")
    log("=" * 70)

    # Load bp_only data
    with open(BP_ONLY_DATA_FILE, 'r') as f:
        bp_only_data = json.load(f)

    log(f"Loaded {len(bp_only_data)} weapons from bp_only cache")

    for weapon_name, transform_data in bp_only_data.items():
        if weapon_name not in WEAPON_PATH_MAP:
            log(f"\n[{weapon_name}] SKIP - not in path map")
            continue

        bp_path = WEAPON_PATH_MAP[weapon_name]
        log(f"\n{'='*60}")
        log(f"[{weapon_name}]")
        log(f"{'='*60}")

        # Load Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        # Get generated class
        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            log(f"  ERROR: Could not get CDO")
            continue

        try:
            mesh_data = transform_data.get("StaticMesh", {})
            loc_data = mesh_data.get("location", {})
            rot_data = mesh_data.get("rotation", {})

            # Extract EXACT values from bp_only
            loc_x = loc_data.get("x", 0.0)
            loc_y = loc_data.get("y", 0.0)
            loc_z = loc_data.get("z", 0.0)

            rot_pitch = rot_data.get("pitch", 0.0)
            rot_yaw = rot_data.get("yaw", 0.0)
            rot_roll = rot_data.get("roll", 0.0)

            log(f"  bp_only Location: ({loc_x:.6f}, {loc_y:.6f}, {loc_z:.6f})")
            log(f"  bp_only Rotation: (P={rot_pitch:.6f}, Y={rot_yaw:.6f}, R={rot_roll:.6f})")

            # Create UE objects with EXACT values
            location = unreal.Vector(loc_x, loc_y, loc_z)
            rotation = make_rotator(rot_pitch, rot_yaw, rot_roll)

            # Apply to CDO
            cdo.set_editor_property("default_mesh_relative_location", location)
            cdo.set_editor_property("default_mesh_relative_rotation", rotation)

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  SAVED: {weapon_name}")

            # Verify what was saved
            verify_loc = cdo.get_editor_property("default_mesh_relative_location")
            verify_rot = cdo.get_editor_property("default_mesh_relative_rotation")
            log(f"  Verified Location: ({verify_loc.x:.6f}, {verify_loc.y:.6f}, {verify_loc.z:.6f})")
            log(f"  Verified Rotation: (P={verify_rot.pitch:.6f}, Y={verify_rot.yaw:.6f}, R={verify_rot.roll:.6f})")

        except Exception as e:
            log(f"  ERROR: {e}")

if __name__ == "__main__":
    apply_transforms()
    log("\n\nEXACT TRANSFORM APPLICATION COMPLETE")
