"""Apply armor equipment data from cache to fix equipment menu"""
import unreal
import json
import os

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/armor_mesh_data.json"

def log(msg):
    unreal.log_warning(str(msg))

def apply_armor_equipment_data():
    log("=" * 60)
    log("APPLYING ARMOR EQUIPMENT DATA FROM CACHE")
    log("=" * 60)

    if not os.path.exists(CACHE_FILE):
        log(f"[ERROR] Cache file not found: {CACHE_FILE}")
        return

    with open(CACHE_FILE, 'r') as f:
        armor_data = json.load(f)

    for asset_name, data in armor_data.items():
        asset_path = data['path']
        log(f"\nProcessing: {asset_name}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  [ERROR] Failed to load asset")
            continue

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property('item_information')

            # Get EquipmentDetails nested struct
            equip_details = item_info.get_editor_property('equipment_details')

            # Apply EquipSlots (FGameplayTagContainer)
            slot_tags = data.get('equip_slots', [])
            if slot_tags:
                # Get existing container from struct
                equip_slots = equip_details.get_editor_property('equip_slots')

                # Get tag manager
                tag_manager = unreal.GameplayTagsManager.get()

                for slot_tag_str in slot_tags:
                    try:
                        tag = tag_manager.request_gameplay_tag(unreal.Name(slot_tag_str), False)
                        if tag.is_valid():
                            equip_slots.add_tag(tag)
                            log(f"  Added slot tag: {slot_tag_str}")
                        else:
                            log(f"  [WARN] Invalid tag: {slot_tag_str}")
                    except Exception as e:
                        log(f"  [ERROR] Tag {slot_tag_str}: {e}")

                equip_details.set_editor_property('equip_slots', equip_slots)

            # Apply StatChanges (TMap<FGameplayTag, FSLFEquipmentStat>)
            # Skip for now - complex TMap with custom struct value
            stat_changes = data.get('stat_changes', {})
            if stat_changes:
                log(f"  [INFO] Has {len(stat_changes)} stat changes (not applied - manual review needed)")

            # Apply SkeletalMeshInfo - skip for now, focus on equip slots
            mesh_info = data.get('skeletal_mesh_info', {})
            if mesh_info:
                log(f"  [INFO] Has {len(mesh_info)} mesh entries (not applied - manual review needed)")

            # Set the modified structs back
            item_info.set_editor_property('equipment_details', equip_details)
            asset.set_editor_property('item_information', item_info)

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
            log(f"  [OK] Saved")

        except Exception as e:
            log(f"  [ERROR] {e}")
            import traceback
            traceback.print_exc()

    log("\n" + "=" * 60)
    log("DONE")
    log("=" * 60)

if __name__ == "__main__":
    apply_armor_equipment_data()
