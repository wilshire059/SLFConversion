#!/usr/bin/env python3
"""Check socket names in bp_only backup"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
]

def check_weapons():
    log("=" * 70)
    log("BP_ONLY WEAPON SOCKET CHECK")
    log("=" * 70)

    for bp_path in WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n=== {bp_name} ===")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            log(f"  ERROR: No CDO")
            continue

        # Check ItemInfo for socket info
        try:
            item_info = cdo.get_editor_property("item_info")
            if item_info:
                log(f"  ItemInfo exists")
                try:
                    eq_details = item_info.get_editor_property("equipment_details")
                    if eq_details:
                        log(f"    EquipmentDetails found")
                        # Try attachment_sockets
                        try:
                            sockets = eq_details.get_editor_property("attachment_sockets")
                            if sockets:
                                left_socket = sockets.get_editor_property("left_hand_socket_name")
                                right_socket = sockets.get_editor_property("right_hand_socket_name")
                                holster_socket = sockets.get_editor_property("holster_socket_name")
                                log(f"    Sockets: L={left_socket}, R={right_socket}, H={holster_socket}")
                            else:
                                log(f"    attachment_sockets is None")
                        except Exception as e:
                            log(f"    attachment_sockets error: {e}")
                    else:
                        log(f"    EquipmentDetails is None")
                except Exception as e:
                    log(f"    EquipmentDetails error: {e}")
            else:
                log(f"  ItemInfo is None")
        except Exception as e:
            log(f"  ItemInfo error: {e}")

        # Also try to export text to see raw data
        try:
            export = unreal.EditorAssetLibrary.export_text(bp_path)
            # Search for socket names
            import re
            socket_matches = re.findall(r'(LeftHandSocketName|RightHandSocketName|HolsterSocketName)[^)]+', export[:20000])
            for match in socket_matches[:5]:
                log(f"    Raw: {match[:100]}")
        except Exception as e:
            log(f"    Export error: {e}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    check_weapons()
