# extract_weapon_iteminfo.py
# Extract ItemInfo.EquipmentDetails from bp_only weapons
# Run on bp_only project!

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
]

def extract_iteminfo():
    log("=" * 70)
    log("EXTRACTING WEAPON ItemInfo.EquipmentDetails FROM BP_ONLY")
    log("=" * 70)

    results = {}

    for bp_path in WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  Could not load")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  No generated class")
            continue

        try:
            cdo = unreal.get_default_object(gen_class)
            weapon_data = {"path": bp_path}

            # Try to get ItemInfo struct
            try:
                item_info = cdo.get_editor_property("item_info")
                log(f"  Found item_info")

                # Equipment details
                try:
                    equip_details = item_info.get_editor_property("equipment_details")
                    log(f"    Found equipment_details")

                    # EquipSlots (GameplayTagContainer)
                    try:
                        equip_slots = equip_details.get_editor_property("equip_slots")
                        slots_str = str(equip_slots)
                        log(f"      EquipSlots: {slots_str}")
                        weapon_data["equip_slots"] = slots_str
                    except Exception as e:
                        log(f"      EquipSlots error: {e}")

                    # AttachmentSockets
                    try:
                        sockets = equip_details.get_editor_property("attachment_sockets")
                        left_socket = sockets.get_editor_property("left_hand_socket_name")
                        right_socket = sockets.get_editor_property("right_hand_socket_name")
                        rot_offset = sockets.get_editor_property("attachment_rotation_offset")

                        log(f"      LeftSocket: {left_socket}")
                        log(f"      RightSocket: {right_socket}")
                        log(f"      RotOffset: P={rot_offset.pitch}, Y={rot_offset.yaw}, R={rot_offset.roll}")

                        weapon_data["left_socket"] = str(left_socket)
                        weapon_data["right_socket"] = str(right_socket)
                        weapon_data["socket_rotation"] = {
                            "pitch": rot_offset.pitch,
                            "yaw": rot_offset.yaw,
                            "roll": rot_offset.roll
                        }
                    except Exception as e:
                        log(f"      AttachmentSockets error: {e}")

                except Exception as e:
                    log(f"    equipment_details error: {e}")

            except Exception as e:
                log(f"  item_info error: {e}")

            if len(weapon_data) > 1:
                results[bp_name] = weapon_data

        except Exception as e:
            log(f"  CDO error: {e}")

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/weapon_iteminfo.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)
    log(f"\n\nSaved to: {output_path}")
    log(f"Found {len(results)} weapons with iteminfo data")

if __name__ == "__main__":
    extract_iteminfo()
