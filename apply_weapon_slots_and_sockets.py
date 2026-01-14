# apply_weapon_slots_and_sockets.py
# Apply correct EquipSlots and AttachmentSockets to weapon PDAs
# Run on SLFConversion project AFTER reparenting

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Weapon configuration
# NOTE: Socket names must be "soulslike_weapon_r" / "soulslike_weapon_l" (NOT "hand_r"/"hand_l" which are wrist sockets!)
WEAPON_CONFIG = {
    # Swords - right hand weapons
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01": {
        "equip_slots": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"],
        "left_socket": "soulslike_weapon_l",
        "right_socket": "soulslike_weapon_r",
    },
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02": {
        "equip_slots": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"],
        "left_socket": "soulslike_weapon_l",
        "right_socket": "soulslike_weapon_r",
    },
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword": {
        "equip_slots": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"],
        "left_socket": "soulslike_weapon_l",
        "right_socket": "soulslike_weapon_r",
    },
    "/Game/SoulslikeFramework/Data/Items/DA_Katana": {
        "equip_slots": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"],
        "left_socket": "soulslike_weapon_l",
        "right_socket": "soulslike_weapon_r",
    },
    "/Game/SoulslikeFramework/Data/Items/DA_BossMace": {
        "equip_slots": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"],
        "left_socket": "soulslike_weapon_l",
        "right_socket": "soulslike_weapon_r",
    },
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword": {
        "equip_slots": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"],
        "left_socket": "soulslike_weapon_l",
        "right_socket": "soulslike_weapon_r",
    },
    # Shield - left hand weapon
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01": {
        "equip_slots": ["SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 1"],
        "left_socket": "soulslike_weapon_l",
        "right_socket": "soulslike_weapon_r",
    },
}

def apply_weapon_data():
    log("=" * 70)
    log("APPLYING WEAPON SLOTS AND SOCKETS")
    log("=" * 70)

    success_count = 0

    for pda_path, config in WEAPON_CONFIG.items():
        pda_name = pda_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{pda_name}]")
        log(f"{'='*60}")

        # Load asset
        asset = unreal.EditorAssetLibrary.load_asset(pda_path)
        if not asset:
            log(f"  ERROR: Could not load {pda_path}")
            continue

        log(f"  Class: {asset.get_class().get_name()}")

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property("item_information")
            log(f"  Got item_information")

            # Get EquipmentDetails struct
            equip_details = item_info.get_editor_property("equipment_details")
            log(f"  Got equipment_details")

            # Set EquipSlots
            equip_slots = equip_details.get_editor_property("equip_slots")
            log(f"  Current EquipSlots: {equip_slots}")

            # Create new tag container with our tags
            new_tags = unreal.GameplayTagContainer()
            for tag_str in config.get("equip_slots", []):
                # Try to request the tag - it needs to be registered in GameplayTags
                tag = unreal.GameplayTag()
                # Use the native function to request tag
                try:
                    # Try loading the tag
                    new_tags.add_tag(unreal.GameplayTag.request_gameplay_tag(tag_str))
                    log(f"  Added tag: {tag_str}")
                except Exception as e:
                    log(f"  Could not add tag '{tag_str}': {e}")

            equip_details.set_editor_property("equip_slots", new_tags)
            log(f"  Set EquipSlots")

            # Set AttachmentSockets
            sockets = equip_details.get_editor_property("attachment_sockets")
            log(f"  Got attachment_sockets")

            # Set socket names
            left_socket = config.get("left_socket", "hand_l")
            right_socket = config.get("right_socket", "hand_r")

            sockets.set_editor_property("left_hand_socket_name", left_socket)
            sockets.set_editor_property("right_hand_socket_name", right_socket)
            log(f"  Set sockets: L={left_socket}, R={right_socket}")

            # Write back the structs
            equip_details.set_editor_property("attachment_sockets", sockets)
            item_info.set_editor_property("equipment_details", equip_details)
            asset.set_editor_property("item_information", item_info)

            # Save the asset
            if unreal.EditorAssetLibrary.save_asset(pda_path):
                log(f"  SAVED successfully")
                success_count += 1
            else:
                log(f"  ERROR: Failed to save")

        except Exception as e:
            log(f"  ERROR: {e}")
            import traceback
            log(traceback.format_exc())

    log(f"\n\n{'='*70}")
    log(f"COMPLETE: {success_count}/{len(WEAPON_CONFIG)} weapons updated")
    log(f"{'='*70}")

if __name__ == "__main__":
    apply_weapon_data()
