"""
check_weapon_animset_links.py
Check if weapon data assets have MovesetWeapons linking to animsets
"""
import unreal

def log(msg):
    unreal.log_warning(f"[WEAPON_CHECK] {msg}")

# Weapon data asset paths
WEAPON_PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
    "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
]

def main():
    log("=" * 70)
    log("CHECKING WEAPON -> ANIMSET LINKS")
    log("=" * 70)

    for path in WEAPON_PATHS:
        log(f"\n--- {path.split('/')[-1]} ---")

        asset = unreal.load_asset(path)
        if not asset:
            log(f"  [ERROR] Could not load asset")
            continue

        # Get ItemInformation struct
        try:
            item_info = asset.get_editor_property('item_information')
            if not item_info:
                log(f"  [WARN] No item_information property")
                continue

            # Get EquipmentDetails struct
            equip_details = item_info.get_editor_property('equipment_details')
            if not equip_details:
                log(f"  [WARN] No equipment_details")
                continue

            # Get MovesetWeapons
            moveset = equip_details.get_editor_property('moveset_weapons')
            if moveset:
                moveset_name = moveset.get_name() if hasattr(moveset, 'get_name') else str(moveset)
                moveset_path = moveset.get_path_name() if hasattr(moveset, 'get_path_name') else ""
                log(f"  [OK] MovesetWeapons: {moveset_name}")
                log(f"       Path: {moveset_path}")
            else:
                log(f"  [PROBLEM] MovesetWeapons is NULL - weapon has no animset!")

        except Exception as e:
            log(f"  [ERROR] {e}")

    log(f"\n=== CHECK COMPLETE ===")

if __name__ == "__main__":
    main()
