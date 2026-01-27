# verify_weapon_data.py
# Verify that weapon PDAs have correct socket, slot, and transform data
# Run on SLFConversion project

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Weapon PDAs
WEAPON_PDAS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
]

# Weapon Blueprints
WEAPON_BPS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def verify_pdas():
    log("=" * 70)
    log("VERIFYING WEAPON PDA DATA")
    log("=" * 70)

    for pda_path in WEAPON_PDAS:
        pda_name = pda_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{pda_name}]")
        log(f"{'='*60}")

        asset = unreal.EditorAssetLibrary.load_asset(pda_path)
        if not asset:
            log(f"  ERROR: Could not load")
            continue

        try:
            # Get ItemInformation
            item_info = asset.get_editor_property("item_information")

            # Get EquipmentDetails
            equip_details = item_info.get_editor_property("equipment_details")

            # Get EquipSlots
            equip_slots = equip_details.get_editor_property("equip_slots")
            log(f"  EquipSlots: {equip_slots}")

            # Try to get tags
            try:
                tag_array = equip_slots.get_gameplay_tag_array()
                for tag in tag_array:
                    log(f"    - {tag.get_tag_name()}")
            except Exception as e:
                log(f"    Could not enumerate tags: {e}")

            # Get AttachmentSockets
            sockets = equip_details.get_editor_property("attachment_sockets")
            left_socket = sockets.get_editor_property("left_hand_socket_name")
            right_socket = sockets.get_editor_property("right_hand_socket_name")
            log(f"  Sockets: L={left_socket}, R={right_socket}")

        except Exception as e:
            log(f"  ERROR: {e}")

def verify_blueprints():
    log("\n\n")
    log("=" * 70)
    log("VERIFYING WEAPON BLUEPRINT CDO DATA")
    log("=" * 70)

    for bp_path in WEAPON_BPS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{bp_name}]")
        log(f"{'='*60}")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load")
            continue

        try:
            gen_class = bp.generated_class()
            cdo = unreal.get_default_object(gen_class)

            log(f"  Class: {gen_class.get_name()}")

            # Check transform properties
            try:
                mesh_loc = cdo.get_editor_property("default_mesh_relative_location")
                log(f"  DefaultMeshRelativeLocation: {mesh_loc}")
            except:
                log(f"  DefaultMeshRelativeLocation: NOT FOUND")

            try:
                mesh_rot = cdo.get_editor_property("default_mesh_relative_rotation")
                log(f"  DefaultMeshRelativeRotation: {mesh_rot}")
            except:
                log(f"  DefaultMeshRelativeRotation: NOT FOUND")

            try:
                weapon_mesh = cdo.get_editor_property("default_weapon_mesh")
                mesh_name = weapon_mesh.get_name() if weapon_mesh else "NONE"
                log(f"  DefaultWeaponMesh: {mesh_name}")
            except:
                log(f"  DefaultWeaponMesh: NOT FOUND")

        except Exception as e:
            log(f"  ERROR: {e}")

if __name__ == "__main__":
    verify_pdas()
    verify_blueprints()
    log("\n\nVERIFICATION COMPLETE")
