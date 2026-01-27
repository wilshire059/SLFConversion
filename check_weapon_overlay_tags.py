# check_weapon_overlay_tags.py
# Check if weapon data assets have overlay tags set

import unreal

def log(msg):
    print(f"[OverlayCheck] {msg}")
    unreal.log_warning(f"[OverlayCheck] {msg}")

ITEM_PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
]

def main():
    log("=" * 70)
    log("CHECKING WEAPON OVERLAY TAGS")
    log("=" * 70)

    for path in ITEM_PATHS:
        item = unreal.load_asset(path)
        if not item:
            log(f"\n{path.split('/')[-1]}: NOT FOUND")
            continue

        item_name = item.get_name()
        log(f"\n--- {item_name} ---")

        # Check if it has ItemInformation.EquipmentDetails.WeaponOverlay
        try:
            item_info = item.get_editor_property('item_information')
            if item_info:
                equip_details = item_info.get_editor_property('equipment_details')
                if equip_details:
                    # Check weapon_overlay
                    overlay = equip_details.get_editor_property('weapon_overlay')
                    log(f"  weapon_overlay: {overlay}")

                    # Check equip_slots
                    slots = equip_details.get_editor_property('equip_slots')
                    log(f"  equip_slots: {slots}")
        except Exception as e:
            log(f"  Error: {e}")

        # Also try getting overlay tag directly
        try:
            tag = unreal.SLFAutomationLibrary.get_weapon_overlay_tag(path)
            log(f"  GetWeaponOverlayTag: {tag}")
        except Exception as e:
            log(f"  GetWeaponOverlayTag error: {e}")

if __name__ == "__main__":
    main()
