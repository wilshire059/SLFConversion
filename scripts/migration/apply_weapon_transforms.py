# apply_weapon_transforms.py
# Apply mesh transform offsets to weapon Blueprint CDOs
# These transforms come from bp_only Blueprint SCS component settings
# Run on SLFConversion project

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Helper to create Rotator with explicit property assignment
def make_rotator(pitch, yaw, roll):
    r = unreal.Rotator()
    r.pitch = pitch
    r.yaw = yaw
    r.roll = roll
    return r

# Weapon Blueprint paths and their transforms from bp_only
# Data extracted from bp_only_weapon_transforms.json
WEAPON_TRANSFORMS = {
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01": {
        "location": unreal.Vector(0.0, 0.0, 36.49),
        "pitch": 0.0, "yaw": 0.0, "roll": 180.0,  # from bp_only
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword": {
        "location": unreal.Vector(2.23, 0.0, 0.0),
        "pitch": -0.9, "yaw": -65.85, "roll": -4.45,
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana": {
        "location": unreal.Vector(0.0, 0.0, -1.64),
        "pitch": 0.0, "yaw": -103.24, "roll": 0.0,
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield": {
        "location": unreal.Vector(0.0, 0.0, 0.227),
        "pitch": 0.0, "yaw": 0.0, "roll": 0.0,
    },
}

def apply_transforms():
    log("=" * 70)
    log("APPLYING WEAPON MESH TRANSFORMS")
    log("=" * 70)

    for bp_path, transform_data in WEAPON_TRANSFORMS.items():
        bp_name = bp_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{bp_name}]")
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
            location = transform_data["location"]
            rotation = make_rotator(transform_data["pitch"], transform_data["yaw"], transform_data["roll"])

            # Set DefaultMeshRelativeLocation
            cdo.set_editor_property("default_mesh_relative_location", location)
            log(f"  Location: ({location.x:.2f}, {location.y:.2f}, {location.z:.2f})")

            # Set DefaultMeshRelativeRotation
            cdo.set_editor_property("default_mesh_relative_rotation", rotation)
            log(f"  Rotation: (P={rotation.pitch:.2f}, Y={rotation.yaw:.2f}, R={rotation.roll:.2f})")

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  SAVED: {bp_name}")

        except Exception as e:
            log(f"  ERROR: {e}")

if __name__ == "__main__":
    apply_transforms()
    log("\n\nTRANSFORM APPLICATION COMPLETE")
