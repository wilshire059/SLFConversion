# check_weapon_inheritance.py
# Verify weapon class inheritance chain

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
]

def check_inheritance():
    log("=" * 70)
    log("CHECKING WEAPON CLASS INHERITANCE")
    log("=" * 70)

    # Load the C++ class
    slf_weapon_class = unreal.load_class(None, "/Script/SLFConversion.SLFWeaponBase")
    if not slf_weapon_class:
        log("ERROR: Could not load SLFWeaponBase class!")
        return
    log(f"SLFWeaponBase class: {slf_weapon_class.get_name()}")

    for bp_path in WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        # Check if it's a child of SLFWeaponBase using UClass cast
        try:
            is_child = unreal.MathLibrary.class_is_child_of(gen_class, slf_weapon_class)
            log(f"  is_child_of(SLFWeaponBase): {is_child}")
        except Exception as e:
            log(f"  is_child_of check failed: {e}")

        # Get parent class
        try:
            parent_class = unreal.SystemLibrary.get_class_parent_class(gen_class)
            if parent_class:
                log(f"  Parent class: {parent_class.get_name()}")

                # Check parent's parent
                grandparent = unreal.SystemLibrary.get_class_parent_class(parent_class)
                if grandparent:
                    log(f"  Grandparent: {grandparent.get_name()}")
        except Exception as e:
            log(f"  Error getting parent: {e}")

if __name__ == "__main__":
    check_inheritance()
