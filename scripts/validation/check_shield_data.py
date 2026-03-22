"""
Check shield item data - EquipSlots and AttachmentSockets
"""
import unreal
import json

SHIELD_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
]

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/shield_data.json"

def main():
    result = {}

    for asset_path in SHIELD_ASSETS:
        asset = unreal.load_asset(asset_path)
        if not asset:
            print(f"ERROR: Could not load {asset_path}")
            continue

        asset_name = asset_path.split("/")[-1]
        print(f"\n=== {asset_name} ===")

        asset_info = {}

        # Try to get ItemInformation struct
        try:
            item_info = asset.get_editor_property("item_information")
            if item_info:
                print(f"  Found ItemInformation")

                # Get EquipmentDetails
                try:
                    equip_details = item_info.get_editor_property("equipment_details")
                    if equip_details:
                        print(f"  Found EquipmentDetails")

                        # Get EquipSlots
                        try:
                            equip_slots = equip_details.get_editor_property("equip_slots")
                            if equip_slots:
                                slots_list = []
                                # GameplayTagContainer iteration
                                for tag in equip_slots:
                                    tag_str = str(tag)
                                    slots_list.append(tag_str)
                                    print(f"    EquipSlot: {tag_str}")
                                asset_info["equip_slots"] = slots_list
                        except Exception as e:
                            print(f"    Error getting equip_slots: {e}")

                        # Get AttachmentSockets
                        try:
                            sockets = equip_details.get_editor_property("attachment_sockets")
                            if sockets:
                                left = str(sockets.get_editor_property("left_hand_socket_name"))
                                right = str(sockets.get_editor_property("right_hand_socket_name"))
                                print(f"    AttachmentSockets - Left: {left}, Right: {right}")
                                asset_info["attachment_sockets"] = {
                                    "left": left,
                                    "right": right
                                }
                        except Exception as e:
                            print(f"    Error getting attachment_sockets: {e}")

                except Exception as e:
                    print(f"  Error getting equipment_details: {e}")

                # Get Category
                try:
                    category = item_info.get_editor_property("category")
                    if category:
                        cat_val = category.get_editor_property("category")
                        print(f"  Category: {cat_val}")
                        asset_info["category"] = str(cat_val)
                except Exception as e:
                    print(f"  Error getting category: {e}")

        except Exception as e:
            print(f"  Error getting item_information: {e}")

        result[asset_name] = asset_info

    # Save to file
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(result, f, indent=2)

    print(f"\nSaved to: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
