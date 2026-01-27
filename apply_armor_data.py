# apply_armor_data.py
# Applies cached armor data (SkeletalMeshInfo, StatChanges, EquipSlots) to Data Assets
# Uses C++ ApplyArmorStatChanges and ApplySkeletalMeshInfoToItem for reliable GameplayTag handling
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
    Apply cached armor data to Data Assets using C++ automation functions.
    """
    # Clear debug file
    with open(DEBUG_FILE, 'w') as f:
        f.write("")

    debug_log("")
    debug_log("=" * 70)
    debug_log("APPLYING ARMOR DATA TO DATA ASSETS (via C++ Automation)")
    debug_log("=" * 70)

    # Load cached data
    if not os.path.exists(INPUT_FILE):
        debug_log(f"[ERROR] Cache file not found: {INPUT_FILE}")
        debug_log("Run extract_armor_data.py on bp_only project first!")
        return

    with open(INPUT_FILE, 'r') as f:
        armor_data = json.load(f)

    debug_log(f"Loaded {len(armor_data)} armor items from cache")

    success_count = 0
    fail_count = 0

    for asset_name, data in armor_data.items():
        asset_path = data.get("path", "")
        debug_log(f"\n[{asset_name}]")
        debug_log(f"  Path: {asset_path}")

        # Get data to apply
        skeletal_mesh_info = data.get("skeletal_mesh_info", {})
        stat_changes = data.get("stat_changes", {})
        equip_slots = data.get("equip_slots", [])

        if not skeletal_mesh_info and not stat_changes:
            debug_log(f"  [SKIP] No data to apply")
            continue

        applied_any = False

        # Apply SkeletalMeshInfo using C++ function
        if skeletal_mesh_info:
            try:
                char_names = []
                mesh_paths = []
                for char_name, mesh_path in skeletal_mesh_info.items():
                    char_names.append(char_name)
                    mesh_paths.append(mesh_path)

                result = unreal.SLFAutomationLibrary.apply_skeletal_mesh_info_to_item(
                    asset_path, char_names, mesh_paths
                )

                if result:
                    debug_log(f"  [OK] Applied {len(skeletal_mesh_info)} skeletal meshes")
                    applied_any = True
                else:
                    debug_log(f"  [FAILED] Could not apply skeletal mesh info")

            except Exception as e:
                debug_log(f"  [ERROR] SkeletalMeshInfo: {e}")

        # Apply StatChanges and EquipSlots using new C++ function
        if stat_changes or equip_slots:
            try:
                # Convert stat_changes dict to array of "Tag:Value" strings
                stat_array = []
                for tag_str, value in stat_changes.items():
                    stat_array.append(f"{tag_str}:{value}")

                result = unreal.SLFAutomationLibrary.apply_armor_stat_changes(
                    asset_path, stat_array, equip_slots
                )

                if result:
                    debug_log(f"  [OK] Applied {len(stat_changes)} stats, {len(equip_slots)} equip slots")
                    applied_any = True
                else:
                    debug_log(f"  [FAILED] Could not apply stat changes")

            except Exception as e:
                debug_log(f"  [ERROR] StatChanges: {e}")

        if applied_any:
            success_count += 1
        else:
            fail_count += 1

    debug_log("")
    debug_log("=" * 70)
    debug_log(f"COMPLETE: {success_count} succeeded, {fail_count} failed")
    debug_log("=" * 70)


if __name__ == "__main__":
    apply_armor_data()
