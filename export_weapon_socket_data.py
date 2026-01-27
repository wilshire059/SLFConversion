# export_weapon_socket_data.py
# Export weapon socket data using export_text

import unreal
import re

WEAPON_DATA_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
]

def log(msg):
    print(f"[SocketExport] {msg}")
    unreal.log_warning(f"[SocketExport] {msg}")

def main():
    log("=" * 70)
    log("EXPORTING WEAPON SOCKET DATA FROM DATA ASSETS")
    log("=" * 70)

    for da_path in WEAPON_DATA_ASSETS:
        da_name = da_path.split('/')[-1]

        da = unreal.load_asset(da_path)
        if not da:
            log(f"{da_name}: NOT FOUND")
            continue

        try:
            # Export the asset text
            text = unreal.EditorAssetLibrary.get_metadata_tag(da, "export")
            if not text:
                text = str(da.export_text())

            log(f"\n{da_name}:")

            # Search for socket-related patterns
            left_socket_match = re.search(r'LeftHandSocketName.*?(?:=|:)\s*["\']?([^"\'(\r\n,}]+)', text, re.IGNORECASE)
            right_socket_match = re.search(r'RightHandSocketName.*?(?:=|:)\s*["\']?([^"\'(\r\n,}]+)', text, re.IGNORECASE)
            attachment_offset_match = re.search(r'AttachmentRotationOffset.*?\(([^)]+)\)', text, re.IGNORECASE)

            if left_socket_match:
                log(f"  LeftSocket: {left_socket_match.group(1).strip()}")
            else:
                log(f"  LeftSocket: NOT FOUND in export")

            if right_socket_match:
                log(f"  RightSocket: {right_socket_match.group(1).strip()}")
            else:
                log(f"  RightSocket: NOT FOUND in export")

            if attachment_offset_match:
                log(f"  AttachmentOffset: {attachment_offset_match.group(1).strip()}")

            # Also search for "hand_r" or "weapon_r" anywhere
            if "weapon_r" in text or "weapon_l" in text:
                log(f"  Contains 'weapon_r' or 'weapon_l' socket reference")
            if "hand_r" in text or "hand_l" in text:
                log(f"  Contains 'hand_r' or 'hand_l' socket reference")

        except Exception as e:
            log(f"{da_name}: ERROR - {e}")

if __name__ == "__main__":
    main()
