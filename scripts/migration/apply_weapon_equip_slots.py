"""
Apply EquipSlots tags to weapon data assets.
Weapons should have Right Hand Weapon tags, Shields should have Left Hand Weapon tags.
"""
import unreal

def apply_weapon_equip_slots():
    """Apply EquipSlots tags to weapons."""

    # Define weapon -> slot tag mappings
    weapon_slots = {
        # Weapons get Right Hand tags
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01": "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
        "/Game/SoulslikeFramework/Data/Items/DA_Sword02": "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
        "/Game/SoulslikeFramework/Data/Items/DA_Greatsword": "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
        "/Game/SoulslikeFramework/Data/Items/DA_Katana": "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
        "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword": "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
        "/Game/SoulslikeFramework/Data/Items/DA_BossMace": "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
        # Shields get Left Hand tags
        "/Game/SoulslikeFramework/Data/Items/DA_Shield01": "SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 1",
    }

    success_count = 0
    fail_count = 0

    for path, slot_tag_str in weapon_slots.items():
        try:
            asset = unreal.EditorAssetLibrary.load_asset(path)
            if not asset:
                unreal.log_warning(f"Asset not found: {path}")
                fail_count += 1
                continue

            # Get ItemInformation struct
            item_info = asset.get_editor_property("item_information")
            
            # Get equipment details
            equipment_details = item_info.get_editor_property("equipment_details")
            
            # Get current equip slots container
            equip_slots = equipment_details.get_editor_property("equip_slots")
            
            # Create a new gameplay tag container with the tag
            new_container = unreal.GameplayTagContainer()
            new_container.add_tag_fast(unreal.Name(slot_tag_str))
            
            # Set it back (structs are copied, so we need to set back)
            equipment_details.set_editor_property("equip_slots", new_container)
            item_info.set_editor_property("equipment_details", equipment_details)
            asset.set_editor_property("item_information", item_info)
            
            # Save the asset
            unreal.EditorAssetLibrary.save_asset(path)
            
            unreal.log(f"OK: Applied {slot_tag_str} to {asset.get_name()}")
            success_count += 1
            
        except Exception as e:
            unreal.log_warning(f"FAIL: {path} - {e}")
            fail_count += 1

    unreal.log(f"=== Done: {success_count} succeeded, {fail_count} failed ===")

if __name__ == "__main__":
    apply_weapon_equip_slots()
