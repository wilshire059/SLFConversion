# test_weapon_components.py
# Quick test to verify weapon Blueprints have DefaultWeaponMesh set

import unreal

def log(msg):
    unreal.log_warning(str(msg))

def test_weapon_components():
    log("")
    log("=" * 70)
    log("TESTING WEAPON DefaultWeaponMesh PROPERTY")
    log("=" * 70)

    weapon_paths = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    ]

    for bp_path in weapon_paths:
        bp_name = bp_path.split("/")[-1]
        log(f"[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  [SKIP] Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  [SKIP] No generated class")
            continue

        log(f"  Generated class: {gen_class.get_name()}")

        try:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log(f"  CDO: {cdo.get_name()}")

                # Check DefaultWeaponMesh property
                try:
                    default_mesh = cdo.get_editor_property("default_weapon_mesh")
                    if default_mesh:
                        log(f"  DefaultWeaponMesh: {default_mesh.get_path_name()}")
                    else:
                        log(f"  DefaultWeaponMesh: <NONE>")
                except Exception as e:
                    log(f"  DefaultWeaponMesh error: {e}")

                # Also check components
                all_comps = cdo.get_components_by_class(unreal.ActorComponent)
                log(f"  Components ({len(all_comps)}):")
                for comp in all_comps:
                    comp_name = comp.get_name()
                    comp_class = comp.get_class().get_name()
                    log(f"    - {comp_name}: {comp_class}")
            else:
                log(f"  [SKIP] Could not get CDO")
        except Exception as e:
            log(f"  [ERROR] {e}")

    log("")
    log("=" * 70)
    log("TEST COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    test_weapon_components()
