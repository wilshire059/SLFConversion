# apply_weapon_transforms_v4.py
# Apply weapon mesh transforms - fixing rotation axis issue

import unreal
import json

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_transforms_bp_only.json"

def log(msg):
    print(f"[ApplyTransform] {msg}")
    unreal.log_warning(f"[ApplyTransform] {msg}")

def main():
    log("=" * 70)
    log("APPLYING WEAPON TRANSFORMS FROM BP_ONLY (v4 - rotation fix)")
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

        # Get WeaponMesh component
        try:
            mesh_comp = cdo.get_editor_property('weapon_mesh')
            if not mesh_comp:
                log(f"  [ERROR] No weapon_mesh component")
                failed += 1
                continue
        except Exception as e:
            log(f"  [ERROR] Could not get weapon_mesh: {e}")
            failed += 1
            continue

        # Apply location
        loc = transform['location']
        new_loc = unreal.Vector(loc['x'], loc['y'], loc['z'])
        mesh_comp.set_editor_property('relative_location', new_loc)
        log(f"  Location: X={loc['x']:.4f}, Y={loc['y']:.4f}, Z={loc['z']:.4f}")

        # Apply rotation - need to create Rotator correctly
        # bp_only stores: pitch, yaw, roll
        rot = transform['rotation']
        pitch = rot['pitch']
        yaw = rot['yaw']
        roll = rot['roll']

        log(f"  Target Rotation: Pitch={pitch:.2f}, Yaw={yaw:.2f}, Roll={roll:.2f}")

        # Create rotator and set via K2 node-style approach
        # unreal.Rotator takes (roll, pitch, yaw) in Python!
        new_rot = unreal.Rotator(roll, pitch, yaw)
        mesh_comp.set_editor_property('relative_rotation', new_rot)

        # Verify what was set
        check_rot = mesh_comp.get_editor_property('relative_rotation')
        log(f"  Applied Rotation: Pitch={check_rot.pitch:.2f}, Yaw={check_rot.yaw:.2f}, Roll={check_rot.roll:.2f}")

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
