"""
debug_weapon_chain.py
Debug the complete weapon -> animset -> montage chain
"""
import unreal

def log(msg):
    unreal.log_warning(f"[DEBUG_CHAIN] {msg}")

def main():
    log("=" * 70)
    log("DEBUGGING WEAPON -> ANIMSET CHAIN")
    log("=" * 70)

    weapon_path = "/Game/SoulslikeFramework/Data/Items/DA_Sword01"
    animset_path = "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword"

    # 1. Check weapon directly
    log("\n=== STEP 1: Load Weapon ===")
    weapon = unreal.load_asset(weapon_path)
    if not weapon:
        log(f"[ERROR] Could not load weapon")
        return

    log(f"Weapon: {weapon.get_name()}")
    log(f"Weapon class: {weapon.get_class().get_name()}")

    # 2. Check ItemInformation.EquipmentDetails.MovesetWeapons
    log("\n=== STEP 2: Check MovesetWeapons on Weapon ===")
    try:
        item_info = weapon.get_editor_property('item_information')
        log(f"ItemInformation: {type(item_info)}")

        equip_details = item_info.get_editor_property('equipment_details')
        log(f"EquipmentDetails: {type(equip_details)}")

        moveset = equip_details.get_editor_property('moveset_weapons')
        if moveset:
            log(f"MovesetWeapons: {moveset.get_name()}")
            log(f"MovesetWeapons class: {moveset.get_class().get_name()}")
            log(f"MovesetWeapons path: {moveset.get_path_name()}")
        else:
            log(f"[ERROR] MovesetWeapons is NULL!")
    except Exception as e:
        log(f"[ERROR] {e}")

    # 3. Load animset directly and check montages
    log("\n=== STEP 3: Load Animset Directly ===")
    animset = unreal.load_asset(animset_path)
    if animset:
        log(f"Animset: {animset.get_name()}")
        log(f"Animset class: {animset.get_class().get_name()}")

        # Check if we can access montage
        try:
            montage_ref = animset.get_editor_property('one_h_light_combo_montage_r')
            if montage_ref:
                log(f"1h Light Montage R: {montage_ref}")
            else:
                log(f"[WARN] 1h Light Montage R is NULL")
        except Exception as e:
            log(f"[ERROR] Could not get montage: {e}")
    else:
        log(f"[ERROR] Could not load animset")

    # 4. Check C++ class hierarchy
    log("\n=== STEP 4: Check C++ Class ===")
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_WeaponAnimset")
    if cpp_class:
        log(f"C++ class: {cpp_class.get_name()}")
        log(f"C++ class path: {cpp_class.get_path_name()}")

        if animset:
            animset_class = animset.get_class()
            log(f"Animset class: {animset_class.get_name()}")

            # Check if animset's class inherits from C++
            try:
                is_child = animset_class.is_child_of(cpp_class)
                log(f"Is animset a subclass of C++ class? {is_child}")
            except Exception as e:
                log(f"is_child_of check failed: {e}")

    log("\n=== DEBUG COMPLETE ===")

if __name__ == "__main__":
    main()
