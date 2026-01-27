"""
test_attack_chain.py
Test the complete attack animation chain:
1. Load weapon DA
2. Get its MovesetWeapons (animset)
3. Get montage from animset
4. Verify montage is valid
"""
import unreal

def log(msg):
    unreal.log_warning(f"[ATTACK_TEST] {msg}")

def main():
    log("=" * 70)
    log("TESTING COMPLETE ATTACK ANIMATION CHAIN")
    log("=" * 70)

    # Test with DA_Sword01
    weapon_path = "/Game/SoulslikeFramework/Data/Items/DA_Sword01"
    log(f"\nLoading weapon: {weapon_path}")

    weapon = unreal.load_asset(weapon_path)
    if not weapon:
        log(f"  [ERROR] Could not load weapon")
        return

    log(f"  [OK] Weapon loaded: {weapon.get_name()}")

    # Get ItemInformation.EquipmentDetails.MovesetWeapons
    try:
        item_info = weapon.get_editor_property('item_information')
        equip_details = item_info.get_editor_property('equipment_details')
        animset = equip_details.get_editor_property('moveset_weapons')

        if not animset:
            log(f"  [ERROR] MovesetWeapons is NULL!")
            return

        log(f"  [OK] MovesetWeapons: {animset.get_name()}")
        animset_path = animset.get_path_name()
        log(f"       Path: {animset_path}")

        # Get montage from animset
        log(f"\nLoading 1h light combo montage from animset...")

        # The animset should have one_h_light_combo_montage_r
        montage_ref = animset.get_editor_property('one_h_light_combo_montage_r')
        if montage_ref:
            log(f"  [OK] Montage reference found")

            # Load the actual montage
            if hasattr(montage_ref, 'get_path_name'):
                montage_path = montage_ref.get_path_name()
                if montage_path and montage_path != "None":
                    log(f"       Montage path: {montage_path}")

                    montage = unreal.load_asset(montage_path.split('.')[0])
                    if montage:
                        log(f"  [OK] Montage loaded: {montage.get_name()}")
                        log(f"       Duration: {montage.get_play_length()} seconds")
                    else:
                        log(f"  [WARN] Could not load montage")
                else:
                    log(f"  [WARN] Montage path is empty")
            else:
                # Direct reference
                log(f"  [OK] Direct montage reference: {montage_ref.get_name() if hasattr(montage_ref, 'get_name') else montage_ref}")
        else:
            log(f"  [ERROR] one_h_light_combo_montage_r is NULL!")

        # Also check 2h montage
        log(f"\nLoading 2h light combo montage from animset...")
        montage_ref_2h = animset.get_editor_property('two_h_light_combo_montage')
        if montage_ref_2h:
            log(f"  [OK] 2h Montage reference found")
            if hasattr(montage_ref_2h, 'get_path_name'):
                log(f"       Path: {montage_ref_2h.get_path_name()}")
            else:
                log(f"       Direct: {montage_ref_2h}")
        else:
            log(f"  [WARN] two_h_light_combo_montage is NULL")

    except Exception as e:
        log(f"  [ERROR] {e}")
        import traceback
        traceback.print_exc()

    log(f"\n=== TEST COMPLETE ===")
    log(f"If all checks show [OK], attack animations should work!")
    log(f"To test in-game:")
    log(f"  1. Open editor, PIE")
    log(f"  2. Open inventory, equip DA_Sword01")
    log(f"  3. Press light attack (usually left mouse)")
    log(f"  4. Character should play AM_SLF_1h_Light_R_Greystone")

if __name__ == "__main__":
    main()
