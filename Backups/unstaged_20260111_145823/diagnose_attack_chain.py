"""
diagnose_attack_chain.py
Diagnose attack animation chain - check weapon items, animsets, and montages
"""
import unreal

def log(msg):
    unreal.log_warning(f"[ATTACK_DIAG] {msg}")

def main():
    log("=" * 70)
    log("ATTACK ANIMATION CHAIN DIAGNOSIS")
    log("=" * 70)

    # Weapon items to check
    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/B_Item_Weapon_SwordExample01",
        "/Game/SoulslikeFramework/Data/Items/B_Item_Weapon_Greatsword",
        "/Game/SoulslikeFramework/Data/Items/B_Item_Weapon_Katana",
        "/Game/SoulslikeFramework/Data/Items/B_Item_Weapon_Shield",
    ]

    # Animset data assets
    animset_paths = [
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Greatsword",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Katana",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Shield",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Unarmed",
    ]

    log("")
    log("=== CHECKING WEAPON ITEMS ===")

    for path in weapon_paths:
        log(f"\n--- {path.split('/')[-1]} ---")
        asset = unreal.load_asset(path)
        if not asset:
            log(f"  [ERROR] Could not load asset")
            continue

        # Get CDO if it's a Blueprint
        if hasattr(asset, 'generated_class') and asset.generated_class():
            cdo = unreal.get_default_object(asset.generated_class())
            if cdo:
                check_weapon_item(cdo)
            else:
                log(f"  [ERROR] No CDO found")
        else:
            # Direct data asset
            check_weapon_item(asset)

    log("")
    log("=== CHECKING ANIMSET DATA ASSETS ===")

    for path in animset_paths:
        log(f"\n--- {path.split('/')[-1]} ---")
        asset = unreal.load_asset(path)
        if not asset:
            log(f"  [ERROR] Could not load asset")
            continue

        check_animset(asset)

    log("")
    log("=== DIAGNOSIS COMPLETE ===")

def check_weapon_item(item):
    """Check a weapon item for MovesetWeapons"""
    # Try different property names
    props_to_check = [
        'item_information',
        'ItemInformation',
    ]

    item_info = None
    for prop_name in props_to_check:
        try:
            item_info = item.get_editor_property(prop_name)
            if item_info:
                log(f"  Found ItemInformation via '{prop_name}'")
                break
        except:
            pass

    if not item_info:
        # Try reflection
        log(f"  Checking class: {item.get_class().get_name()}")

        # List all properties
        props = []
        for p in dir(item):
            if not p.startswith('_'):
                try:
                    val = getattr(item, p)
                    if not callable(val):
                        props.append(p)
                except:
                    pass
        log(f"  Available properties: {props[:20]}...")
        return

    # Check EquipmentDetails
    equip_details = None
    for prop_name in ['equipment_details', 'EquipmentDetails']:
        try:
            equip_details = item_info.get_editor_property(prop_name) if hasattr(item_info, 'get_editor_property') else getattr(item_info, prop_name, None)
            if equip_details:
                log(f"  Found EquipmentDetails via '{prop_name}'")
                break
        except:
            pass

    if not equip_details:
        log(f"  [WARN] No EquipmentDetails found in ItemInformation")
        return

    # Check MovesetWeapons
    moveset = None
    for prop_name in ['moveset_weapons', 'MovesetWeapons']:
        try:
            moveset = equip_details.get_editor_property(prop_name) if hasattr(equip_details, 'get_editor_property') else getattr(equip_details, prop_name, None)
            if moveset:
                log(f"  MovesetWeapons: {moveset.get_name() if moveset else 'None'}")
                break
        except Exception as e:
            pass

    if not moveset:
        log(f"  [PROBLEM] MovesetWeapons is NOT SET - weapon has no animset!")
    else:
        log(f"  [OK] MovesetWeapons is set to: {moveset}")

def check_animset(animset):
    """Check an animset for montage references"""
    montage_props = [
        ('one_h_light_combo_montage_r', '1h_LightComboMontage_R'),
        ('one_h_light_combo_montage_l', '1h_LightComboMontage_L'),
        ('two_h_light_combo_montage', '2h_LightComboMontage'),
        ('one_h_heavy_combo_montage_r', '1h_HeavyComboMontage_R'),
        ('one_h_heavy_combo_montage_l', '1h_HeavyComboMontage_L'),
        ('two_h_heavy_combo_montage', '2h_HeavyComboMontage'),
        ('jump_attack_montage', 'JumpAttackMontage'),
        ('sprint_attack_montage', 'SprintAttackMontage'),
    ]

    found_montages = 0
    missing_montages = []

    for snake_name, display_name in montage_props:
        try:
            montage_ref = animset.get_editor_property(snake_name)
            if montage_ref:
                # It's a TSoftObjectPtr, check if it's valid
                if hasattr(montage_ref, 'get_asset_name'):
                    asset_name = montage_ref.get_asset_name()
                    if asset_name:
                        log(f"  {display_name}: {asset_name}")
                        found_montages += 1
                    else:
                        missing_montages.append(display_name)
                else:
                    # Direct reference
                    log(f"  {display_name}: {montage_ref.get_name() if montage_ref else 'None'}")
                    if montage_ref:
                        found_montages += 1
                    else:
                        missing_montages.append(display_name)
            else:
                missing_montages.append(display_name)
        except Exception as e:
            missing_montages.append(f"{display_name} (error: {e})")

    log(f"  Summary: {found_montages} montages set, {len(missing_montages)} missing")
    if missing_montages:
        log(f"  Missing: {', '.join(missing_montages[:5])}...")

if __name__ == "__main__":
    main()
