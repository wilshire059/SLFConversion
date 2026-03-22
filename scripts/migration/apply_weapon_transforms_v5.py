# apply_weapon_transforms_v5.py
# Apply weapon mesh transforms to DefaultMeshRelativeLocation/Rotation properties
# These properties are then applied to WeaponMesh component in BeginPlay

import unreal
import json

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_transforms_bp_only.json"

def log(msg):
    print(f"[ApplyTransform] {msg}")
    unreal.log_warning(f"[ApplyTransform] {msg}")

def main():
    log("=" * 70)
    log("APPLYING WEAPON TRANSFORMS TO DefaultMesh* PROPERTIES (v5)")
    log("=" * 70)

    # Load bp_only reference
    with open(CACHE_FILE, 'r') as f:
        bp_only_data = json.load(f)

    succeeded = 0
    failed = 0

    for bp_name, data in bp_only_data.items():
        transform = data.get('static_mesh_transform')
        if not transform:
            continue

        bp_path = data['path']
        log(f"\n--- {bp_name} ---")

        # Load the Blueprint
        bp = unreal.load_asset(bp_path)
        if not bp:
            log(f"  [ERROR] Could not load: {bp_path}")
            failed += 1
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            failed += 1
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            failed += 1
            continue

        # Set DefaultMeshRelativeLocation
        loc = transform['location']
        new_loc = unreal.Vector(loc['x'], loc['y'], loc['z'])
        try:
            cdo.set_editor_property('default_mesh_relative_location', new_loc)
            log(f"  DefaultMeshRelativeLocation: X={loc['x']:.4f}, Y={loc['y']:.4f}, Z={loc['z']:.4f}")
        except Exception as e:
            log(f"  [ERROR] Could not set default_mesh_relative_location: {e}")
            failed += 1
            continue

        # Set DefaultMeshRelativeRotation
        rot = transform['rotation']
        pitch = rot['pitch']
        yaw = rot['yaw']
        roll = rot['roll']

        log(f"  Target Rotation: Pitch={pitch:.2f}, Yaw={yaw:.2f}, Roll={roll:.2f}")

        # unreal.Rotator takes (roll, pitch, yaw) in Python!
        new_rot = unreal.Rotator(roll, pitch, yaw)
        try:
            cdo.set_editor_property('default_mesh_relative_rotation', new_rot)
        except Exception as e:
            log(f"  [ERROR] Could not set default_mesh_relative_rotation: {e}")
            failed += 1
            continue

        # Verify what was set
        check_loc = cdo.get_editor_property('default_mesh_relative_location')
        check_rot = cdo.get_editor_property('default_mesh_relative_rotation')
        log(f"  Verified Location: X={check_loc.x:.4f}, Y={check_loc.y:.4f}, Z={check_loc.z:.4f}")
        log(f"  Verified Rotation: Pitch={check_rot.pitch:.2f}, Yaw={check_rot.yaw:.2f}, Roll={check_rot.roll:.2f}")

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  [SAVED]")
            succeeded += 1
        except Exception as e:
            log(f"  [ERROR] Save failed: {e}")
            failed += 1

    log("")
    log("=" * 70)
    log(f"DONE: {succeeded} succeeded, {failed} failed")
    log("=" * 70)

if __name__ == "__main__":
    main()
