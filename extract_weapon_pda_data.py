# extract_weapon_pda_data.py
# Extract ItemInformation.EquipmentDetails from weapon PDAs in bp_only
# Run on bp_only project to see what socket/slot data exists

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

# Weapon PDAs
WEAPON_PDAS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield",
]

def extract_pda_data():
    log("=" * 70)
    log("EXTRACTING WEAPON PDA DATA")
    log("=" * 70)

    results = {}

    for pda_path in WEAPON_PDAS:
        pda_name = pda_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{pda_name}]")
        log(f"{'='*60}")

        pda = unreal.EditorAssetLibrary.load_asset(pda_path)
        if not pda:
            log(f"  Could not load: {pda_path}")
            continue

        pda_data = {
            "path": pda_path,
            "class": pda.get_class().get_name(),
        }

        log(f"  Class: {pda.get_class().get_name()}")

        # Try to get ItemInformation
        try:
            item_info = pda.get_editor_property("item_information")
            log(f"  Has item_information property")

            pda_data["has_item_information"] = True

            # Get EquipmentDetails
            try:
                equip_details = item_info.get_editor_property("equipment_details")
                log(f"  Has equipment_details")

                # Get EquipSlots
                try:
                    equip_slots = equip_details.get_editor_property("equip_slots")
                    log(f"    EquipSlots: {equip_slots}")
                    pda_data["equip_slots"] = str(equip_slots) if equip_slots else "None"
                except Exception as e:
                    log(f"    EquipSlots error: {e}")
                    pda_data["equip_slots"] = f"ERROR: {e}"

                # Get AttachmentSockets
                try:
                    attach_sockets = equip_details.get_editor_property("attachment_sockets")
                    log(f"    AttachmentSockets struct found")

                    # Get socket names
                    try:
                        left_socket = attach_sockets.get_editor_property("left_hand_socket_name")
                        right_socket = attach_sockets.get_editor_property("right_hand_socket_name")
                        log(f"      LeftHandSocketName: {left_socket}")
                        log(f"      RightHandSocketName: {right_socket}")
                        pda_data["left_socket"] = str(left_socket) if left_socket else "None"
                        pda_data["right_socket"] = str(right_socket) if right_socket else "None"
                    except Exception as e:
                        log(f"      Socket names error: {e}")
                        pda_data["socket_error"] = str(e)

                except Exception as e:
                    log(f"    AttachmentSockets error: {e}")
                    pda_data["attachment_sockets_error"] = str(e)

            except Exception as e:
                log(f"  equipment_details error: {e}")
                pda_data["equipment_details_error"] = str(e)

        except Exception as e:
            log(f"  item_information error: {e}")
            pda_data["has_item_information"] = False
            pda_data["item_information_error"] = str(e)

        # Try export_text to see raw data
        try:
            export_text = pda.export_text()
            log(f"\n  EXPORT TEXT (first 3000 chars):")
            log(export_text[:3000])
            pda_data["export_text_preview"] = export_text[:5000]
        except Exception as e:
            log(f"  export_text error: {e}")

        results[pda_name] = pda_data

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/weapon_pda_data.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)
    log(f"\n\nSaved to: {output_path}")

if __name__ == "__main__":
    extract_pda_data()
