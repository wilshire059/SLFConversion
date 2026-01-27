"""
apply_weapon_animset_links.py
Link weapon data assets to their corresponding animset data assets
"""
import unreal

def log(msg):
    unreal.log_warning(f"[LINK_WEAPONS] {msg}")

# Weapon -> Animset mapping
# Swords use LightSword animset, Greatsword uses Greatsword animset, etc.
WEAPON_ANIMSET_MAP = {
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Shield",
    "/Game/SoulslikeFramework/Data/Items/DA_BossMace": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Greatsword",  # Mace uses greatsword anims
}

def main():
    log("=" * 70)
    log("LINKING WEAPONS TO ANIMSETS")
    log("=" * 70)

    success_count = 0
    fail_count = 0

    for weapon_path, animset_path in WEAPON_ANIMSET_MAP.items():
        weapon_name = weapon_path.split('/')[-1]
        animset_name = animset_path.split('/')[-1]
        log(f"\n--- {weapon_name} -> {animset_name} ---")

        # Load weapon
        weapon = unreal.load_asset(weapon_path)
        if not weapon:
            log(f"  [ERROR] Could not load weapon")
            fail_count += 1
            continue

        # Load animset
        animset = unreal.load_asset(animset_path)
        if not animset:
            log(f"  [ERROR] Could not load animset")
            fail_count += 1
            continue

        try:
            # Get ItemInformation struct
            item_info = weapon.get_editor_property('item_information')

            # Get EquipmentDetails struct
            equip_details = item_info.get_editor_property('equipment_details')

            # Set MovesetWeapons
            equip_details.set_editor_property('moveset_weapons', animset)

            # Need to re-assign the nested structs to trigger dirty flag
            item_info.set_editor_property('equipment_details', equip_details)
            weapon.set_editor_property('item_information', item_info)

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(weapon_path, only_if_is_dirty=False)

            log(f"  [OK] Linked and saved")
            success_count += 1

        except Exception as e:
            log(f"  [ERROR] {e}")
            fail_count += 1

    log(f"\n=== COMPLETE: {success_count} weapons linked, {fail_count} failed ===")

if __name__ == "__main__":
    main()
