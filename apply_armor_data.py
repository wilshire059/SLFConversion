# apply_armor_data.py
# Applies cached SkeletalMeshInfo data to armor Data Assets after migration
# Restores data to EXISTING nested struct ItemInformation.EquipmentDetails.SkeletalMeshInfo
#
# The SkeletalMeshInfo TMap uses character data assets (DA_Quinn, DA_Manny) as keys
# and skeletal meshes as values to support different armor meshes per character type.
#
# Run AFTER migration with:
# UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_armor_data.py" -stdout -unattended

import unreal
import json
import os

INPUT_FILE = "C:/scripts/SLFConversion/migration_cache/armor_mesh_data.json"
DEBUG_FILE = "C:/scripts/SLFConversion/armor_apply_debug.txt"

# Map from short character names to their data asset paths
CHARACTER_ASSET_PATHS = {
    "DA_Quinn": "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
    "DA_Manny": "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
}

def debug_log(msg):
    """Write debug message to file and console"""
    with open(DEBUG_FILE, 'a') as f:
        f.write(msg + "\n")
    unreal.log_warning(str(msg))

def apply_armor_data():
    """
    Apply cached armor mesh data to Data Assets.
    Restores SkeletalMeshInfo to existing nested struct.
    """
    # Clear debug file
    with open(DEBUG_FILE, 'w') as f:
        f.write("")

    debug_log("")
    debug_log("=" * 70)
    debug_log("APPLYING ARMOR MESH DATA TO DATA ASSETS")
    debug_log("=" * 70)

    # Load cached data
    if not os.path.exists(INPUT_FILE):
        debug_log(f"[ERROR] Cache file not found: {INPUT_FILE}")
        debug_log("Run extract_armor_data.py on bp_only project first!")
        return

    with open(INPUT_FILE, 'r') as f:
        armor_data = json.load(f)

    debug_log(f"Loaded {len(armor_data)} armor items from cache")

    # Pre-load character data assets
    character_assets = {}
    for char_name, char_path in CHARACTER_ASSET_PATHS.items():
        asset = unreal.EditorAssetLibrary.load_asset(char_path)
        if asset:
            character_assets[char_name] = asset
            debug_log(f"Loaded character asset: {char_name}")
        else:
            debug_log(f"[WARN] Could not load character asset: {char_path}")

    success_count = 0
    fail_count = 0

    for asset_name, data in armor_data.items():
        asset_path = data.get("path", "")
        debug_log(f"\n[{asset_name}]")
        debug_log(f"  Path: {asset_path}")

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            debug_log(f"  [SKIP] Could not load asset")
            fail_count += 1
            continue

        # Check if we have mesh data to apply
        skeletal_mesh_info = data.get("skeletal_mesh_info", {})
        stat_changes = data.get("stat_changes", {})
        equip_slots = data.get("equip_slots", [])

        if not skeletal_mesh_info and not stat_changes:
            debug_log(f"  [SKIP] No data to apply (meshes: {len(skeletal_mesh_info)}, stats: {len(stat_changes)})")
            continue

        try:
            # Get the existing ItemInformation struct
            item_info = asset.get_editor_property("item_information")
            if not item_info:
                debug_log(f"  [SKIP] No ItemInformation property")
                fail_count += 1
                continue

            debug_log(f"  Got ItemInformation struct")

            # Get EquipmentDetails from ItemInformation
            equip_details = item_info.get_editor_property("equipment_details")
            debug_log(f"  Got EquipmentDetails: {type(equip_details)}")

            applied_meshes = 0
            applied_stats = 0

            # Apply SkeletalMeshInfo - TMap<TSoftObjectPtr<UPrimaryDataAsset>, TSoftObjectPtr<USkeletalMesh>>
            if skeletal_mesh_info:
                try:
                    # Get current map
                    current_map = equip_details.get_editor_property("skeletal_mesh_info")
                    debug_log(f"  Current SkeletalMeshInfo map type: {type(current_map)}")

                    for char_name, mesh_path in skeletal_mesh_info.items():
                        debug_log(f"  Applying mesh: {char_name} -> {mesh_path}")

                        # Get character data asset
                        char_asset = character_assets.get(char_name)
                        if not char_asset:
                            debug_log(f"    [WARN] Character asset not found: {char_name}")
                            continue

                        # Load the skeletal mesh
                        mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
                        if not mesh:
                            debug_log(f"    [WARN] Could not load mesh: {mesh_path}")
                            continue

                        # Try to add to map
                        try:
                            current_map[char_asset] = mesh
                            applied_meshes += 1
                            debug_log(f"    [OK] Added mesh entry")
                        except Exception as e:
                            debug_log(f"    [ERROR] Could not add to map: {e}")

                    # Set the map back
                    if applied_meshes > 0:
                        equip_details.set_editor_property("skeletal_mesh_info", current_map)
                        debug_log(f"  Set SkeletalMeshInfo with {applied_meshes} entries")

                except Exception as e:
                    debug_log(f"  [ERROR] Failed to apply SkeletalMeshInfo: {e}")

            # Apply StatChanges - TMap<FGameplayTag, FSLFEquipmentStat>
            if stat_changes:
                try:
                    current_stats = equip_details.get_editor_property("stat_changes")
                    debug_log(f"  Current StatChanges map type: {type(current_stats)}")

                    for tag_str, delta_value in stat_changes.items():
                        debug_log(f"  Applying stat: {tag_str} = {delta_value}")

                        try:
                            # Create GameplayTag
                            tag = unreal.GameplayTag.request_gameplay_tag(unreal.Name(tag_str))

                            # Create FSLFEquipmentStat with Delta value
                            # This struct has a Delta property
                            stat_entry = unreal.SLFEquipmentStat()
                            stat_entry.set_editor_property("delta", delta_value)

                            current_stats[tag] = stat_entry
                            applied_stats += 1
                            debug_log(f"    [OK] Added stat entry")
                        except Exception as e:
                            debug_log(f"    [ERROR] Could not add stat: {e}")

                    if applied_stats > 0:
                        equip_details.set_editor_property("stat_changes", current_stats)
                        debug_log(f"  Set StatChanges with {applied_stats} entries")

                except Exception as e:
                    debug_log(f"  [ERROR] Failed to apply StatChanges: {e}")

            # Apply EquipSlots - FGameplayTagContainer
            if equip_slots:
                try:
                    slots_container = unreal.GameplayTagContainer()
                    for slot_str in equip_slots:
                        try:
                            slot_tag = unreal.GameplayTag.request_gameplay_tag(unreal.Name(slot_str))
                            slots_container.add_tag(slot_tag)
                            debug_log(f"  Added EquipSlot: {slot_str}")
                        except Exception as e:
                            debug_log(f"  [WARN] Could not add slot tag: {e}")

                    equip_details.set_editor_property("equip_slots", slots_container)
                    debug_log(f"  Set EquipSlots with {len(equip_slots)} entries")

                except Exception as e:
                    debug_log(f"  [ERROR] Failed to apply EquipSlots: {e}")

            # Set EquipmentDetails back to ItemInformation
            try:
                item_info.set_editor_property("equipment_details", equip_details)
                debug_log(f"  Set EquipmentDetails back")
            except Exception as e:
                debug_log(f"  [WARN] Could not set equipment_details: {e}")

            # Set ItemInformation back to asset
            try:
                asset.set_editor_property("item_information", item_info)
                debug_log(f"  Set ItemInformation back")
            except Exception as e:
                debug_log(f"  [ERROR] Could not set item_information: {e}")
                fail_count += 1
                continue

            # Save the asset
            if applied_meshes > 0 or applied_stats > 0:
                try:
                    unreal.EditorAssetLibrary.save_asset(asset_path)
                    debug_log(f"  [SAVED] Applied {applied_meshes} meshes, {applied_stats} stats")
                    success_count += 1
                except Exception as e:
                    debug_log(f"  [ERROR] Could not save: {e}")
                    fail_count += 1
            else:
                debug_log(f"  [SKIP] No changes applied")

        except Exception as e:
            debug_log(f"  [ERROR] {e}")
            import traceback
            debug_log(f"  {traceback.format_exc()}")
            fail_count += 1

    debug_log("")
    debug_log("=" * 70)
    debug_log(f"COMPLETE: {success_count} succeeded, {fail_count} failed")
    debug_log("=" * 70)


if __name__ == "__main__":
    apply_armor_data()
