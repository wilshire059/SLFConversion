# check_weapon_sockets.py
# Check what socket names are stored in weapon data assets

import unreal
import json

WEAPON_DATA_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
]

def log(msg):
    print(f"[SocketCheck] {msg}")
    unreal.log_warning(f"[SocketCheck] {msg}")

def main():
    log("=" * 70)
    log("CHECKING WEAPON DATA ASSET SOCKET NAMES")
    log("=" * 70)

    results = {}

    for da_path in WEAPON_DATA_ASSETS:
        da_name = da_path.split('/')[-1]

        da = unreal.load_asset(da_path)
        if not da:
            log(f"{da_name}: NOT FOUND")
            continue

        result = {"path": da_path, "name": da_name}

        try:
            # Get ItemInformation.EquipmentDetails.AttachmentSockets
            item_info = da.get_editor_property('item_information')
            equip_details = item_info.get_editor_property('equipment_details')
            sockets = equip_details.get_editor_property('attachment_sockets')

            left_socket = sockets.get_editor_property('left_hand_socket_name')
            right_socket = sockets.get_editor_property('right_hand_socket_name')
            rotation_offset = sockets.get_editor_property('attachment_rotation_offset')

            result["left_socket"] = str(left_socket) if left_socket else "None"
            result["right_socket"] = str(right_socket) if right_socket else "None"
            result["rotation_offset"] = {
                "pitch": rotation_offset.pitch,
                "yaw": rotation_offset.yaw,
                "roll": rotation_offset.roll
            } if rotation_offset else None

            log(f"{da_name}:")
            log(f"  LeftSocket: {result['left_socket']}")
            log(f"  RightSocket: {result['right_socket']}")
            if rotation_offset:
                log(f"  RotationOffset: P={rotation_offset.pitch:.2f}, Y={rotation_offset.yaw:.2f}, R={rotation_offset.roll:.2f}")

        except Exception as e:
            log(f"{da_name}: ERROR - {e}")
            result["error"] = str(e)

        results[da_name] = result

    # Save to file
    output_path = "C:/scripts/slfconversion/migration_cache/weapon_socket_data.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to {output_path}")

if __name__ == "__main__":
    main()
