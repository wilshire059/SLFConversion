#!/usr/bin/env python3
"""Check weapon Blueprint CDO values for mesh transforms"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def check_weapons():
    log("=" * 70)
    log("WEAPON CDO PROPERTY CHECK")
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

        log(f"  CDO class: {cdo.get_class().get_name()}")

        # Check all the mesh-related properties
        props_to_check = [
            "default_weapon_mesh",
            "default_mesh_relative_location",
            "default_mesh_relative_rotation",
            "default_attachment_rotation_offset",
        ]

        for prop_name in props_to_check:
            try:
                value = cdo.get_editor_property(prop_name)
                log(f"  {prop_name}: {value}")
            except Exception as e:
                log(f"  {prop_name}: ERROR - {e}")

        # Also check ItemInfo for socket info
        try:
            item_info = cdo.get_editor_property("item_info")
            if item_info:
                log(f"  ItemInfo exists")
                try:
                    eq_details = item_info.get_editor_property("equipment_details")
                    if eq_details:
                        sockets = eq_details.get_editor_property("attachment_sockets")
                        if sockets:
                            left_socket = sockets.get_editor_property("left_hand_socket_name")
                            right_socket = sockets.get_editor_property("right_hand_socket_name")
                            log(f"  Sockets: L={left_socket}, R={right_socket}")
                except Exception as e:
                    log(f"  Socket info error: {e}")
        except Exception as e:
            log(f"  ItemInfo error: {e}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    check_weapons()
