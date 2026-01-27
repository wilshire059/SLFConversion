# check_cdo_transforms.py
# Verify weapon CDO DefaultMeshRelative values

import unreal

def log(msg):
    print(f"[CDO] {msg}")
    unreal.log_warning(f"[CDO] {msg}")

def main():
    log("=" * 70)
    log("VERIFYING WEAPON CDO TRANSFORMS")
    log("=" * 70)

    weapons = [
        ("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana", "Z=-1.644"),
        ("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01", "Z=36.494"),
        ("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02", "Z=50.088"),
        ("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword", "X=2.23"),
    ]

    for weapon_path, expected in weapons:
        weapon_name = weapon_path.split("/")[-1]
        log(f"")
        log(f"--- {weapon_name} (Expected: {expected}) ---")

        bp = unreal.load_asset(weapon_path)
        if not bp:
            log(f"  ERROR: Could not load")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            log(f"  ERROR: No CDO")
            continue

        # Check DefaultMeshRelativeLocation/Rotation
        try:
            default_loc = cdo.get_editor_property('default_mesh_relative_location')
            default_rot = cdo.get_editor_property('default_mesh_relative_rotation')
            log(f"  DefaultMeshRelativeLocation: {default_loc}")
            log(f"  DefaultMeshRelativeRotation: {default_rot}")
        except Exception as e:
            log(f"  ERROR accessing DefaultMesh properties: {e}")

        # Also check WeaponMesh component transform on CDO
        try:
            comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
            for comp in comps:
                if comp:
                    rel_loc = comp.get_editor_property('relative_location')
                    rel_rot = comp.get_editor_property('relative_rotation')
                    log(f"  WeaponMesh component ({comp.get_name()}):")
                    log(f"    Relative Location: {rel_loc}")
                    log(f"    Relative Rotation: {rel_rot}")
        except Exception as e:
            log(f"  ERROR accessing component: {e}")

    log("")
    log("Done verifying CDO transforms")

if __name__ == "__main__":
    main()
