# compare_weapon_transforms_live.py
# Compare current weapon transforms with bp_only cached values

import unreal
import json

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_transforms_bp_only.json"

WEAPON_BPS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace",
]

def log(msg):
    print(f"[Compare] {msg}")
    unreal.log_warning(f"[Compare] {msg}")

def get_mesh_transform(bp_path):
    """Get the WeaponMesh component's relative transform from Blueprint CDO"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        return None

    gen_class = bp.generated_class()
    if not gen_class:
        return None

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        return None

    # Try to get WeaponMesh component
    try:
        mesh_comp = cdo.get_editor_property('weapon_mesh')
        if mesh_comp:
            rel_loc = mesh_comp.get_editor_property('relative_location')
            rel_rot = mesh_comp.get_editor_property('relative_rotation')
            return {
                'location': {'x': rel_loc.x, 'y': rel_loc.y, 'z': rel_loc.z},
                'rotation': {'pitch': rel_rot.pitch, 'yaw': rel_rot.yaw, 'roll': rel_rot.roll}
            }
    except:
        pass

    return None

def main():
    log("=" * 70)
    log("COMPARING WEAPON TRANSFORMS: CURRENT vs BP_ONLY")
    log("=" * 70)

    # Load bp_only reference
    with open(CACHE_FILE, 'r') as f:
        bp_only_data = json.load(f)

    for bp_path in WEAPON_BPS:
        bp_name = bp_path.split('/')[-1]
        log(f"\n--- {bp_name} ---")

        # Get current transform
        current = get_mesh_transform(bp_path)

        # Get bp_only transform
        bp_only = bp_only_data.get(bp_name, {}).get('static_mesh_transform')

        if not current:
            log(f"  CURRENT: Could not get transform")
        else:
            log(f"  CURRENT Location: X={current['location']['x']:.4f}, Y={current['location']['y']:.4f}, Z={current['location']['z']:.4f}")
            log(f"  CURRENT Rotation: P={current['rotation']['pitch']:.2f}, Y={current['rotation']['yaw']:.2f}, R={current['rotation']['roll']:.2f}")

        if not bp_only:
            log(f"  BP_ONLY: No transform in cache")
        else:
            loc = bp_only['location']
            rot = bp_only['rotation']
            log(f"  BP_ONLY Location: X={loc['x']:.4f}, Y={loc['y']:.4f}, Z={loc['z']:.4f}")
            log(f"  BP_ONLY Rotation: P={rot['pitch']:.2f}, Y={rot['yaw']:.2f}, R={rot['roll']:.2f}")

        # Compare
        if current and bp_only:
            loc_diff = (
                abs(current['location']['x'] - bp_only['location']['x']) +
                abs(current['location']['y'] - bp_only['location']['y']) +
                abs(current['location']['z'] - bp_only['location']['z'])
            )
            rot_diff = (
                abs(current['rotation']['pitch'] - bp_only['rotation']['pitch']) +
                abs(current['rotation']['yaw'] - bp_only['rotation']['yaw']) +
                abs(current['rotation']['roll'] - bp_only['rotation']['roll'])
            )

            if loc_diff < 0.01 and rot_diff < 0.1:
                log(f"  STATUS: MATCH")
            else:
                log(f"  STATUS: MISMATCH - loc_diff={loc_diff:.4f}, rot_diff={rot_diff:.2f}")

if __name__ == "__main__":
    main()
