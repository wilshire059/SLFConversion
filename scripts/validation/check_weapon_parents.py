# check_weapon_parents.py
# Check actual parent class of weapons

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
]

def check_parents():
    log("=" * 70)
    log("CHECKING WEAPON PARENT CLASSES")
    log("=" * 70)

    for bp_path in WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  Could not load")
            continue

        # Get parent class directly from Blueprint
        try:
            parent = bp.get_editor_property("parent_class")
            if parent:
                log(f"  Blueprint Parent: {parent.get_name()}")
                log(f"  Blueprint Parent Path: {parent.get_path_name()}")
        except Exception as e:
            log(f"  Could not get parent: {e}")

        gen_class = bp.generated_class()
        if gen_class:
            # Get super class
            try:
                super_class = gen_class.get_class().static_class()
                log(f"  Generated class: {gen_class.get_name()}")

                # Walk up hierarchy
                current = gen_class
                depth = 0
                while current and depth < 10:
                    log(f"    Level {depth}: {current.get_name()}")
                    try:
                        current = unreal.SystemLibrary.get_super_class(current)
                    except:
                        break
                    depth += 1
            except Exception as e:
                log(f"  Error walking hierarchy: {e}")

if __name__ == "__main__":
    check_parents()
