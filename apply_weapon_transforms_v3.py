# apply_weapon_transforms_v3.py
# Apply weapon mesh transforms from bp_only cache to current project

import unreal
import json

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_transforms_bp_only.json"

def log(msg):
    print(f"[ApplyTransform] {msg}")
    unreal.log_warning(f"[ApplyTransform] {msg}")

def main():
    log("=" * 70)
    log("APPLYING WEAPON TRANSFORMS FROM BP_ONLY")
    log("=" * 70)

    # Load bp_only reference
    with open(CACHE_FILE, 'r') as f:
        bp_only_data = json.load(f)

    succeeded = 0
    failed = 0

    for bp_name, data in bp_only_data.items():
        transform = data.get('static_mesh_transform')
        if not transform:
            log(f"  [SKIP] {bp_name}: No transform data")
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
            log(f"  [ERROR] No generated class")
            failed += 1
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            log(f"  [ERROR] No CDO")
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

        # Apply transform
        loc = transform['location']
        rot = transform['rotation']

        new_loc = unreal.Vector(loc['x'], loc['y'], loc['z'])
        new_rot = unreal.Rotator(rot['pitch'], rot['yaw'], rot['roll'])

        log(f"  Setting Location: X={loc['x']:.4f}, Y={loc['y']:.4f}, Z={loc['z']:.4f}")
        log(f"  Setting Rotation: P={rot['pitch']:.2f}, Y={rot['yaw']:.2f}, R={rot['roll']:.2f}")

        try:
            mesh_comp.set_editor_property('relative_location', new_loc)
            mesh_comp.set_editor_property('relative_rotation', new_rot)
            log(f"  [OK] Transform applied")
        except Exception as e:
            log(f"  [ERROR] Could not set transform: {e}")
            failed += 1
            continue

        # Save the Blueprint
        try:
            # Mark dirty and save
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  [SAVED] {bp_path}")
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
