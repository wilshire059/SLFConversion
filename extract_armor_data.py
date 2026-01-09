# extract_armor_data.py
# Extracts SkeletalMeshInfo TMap data from armor Data Assets
# Run on bp_only project BEFORE migration to capture original values
#
# Run with:
# UnrealEditor-Cmd.exe "C:/scripts/bp_only/bp_only.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/extract_armor_data.py" -stdout -unattended -nosplash

import unreal
import json
import os
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/armor_mesh_data.json"
DEBUG_FILE = "C:/scripts/SLFConversion/armor_extract_debug.txt"
TEMP_COPY_FILE = "C:/scripts/SLFConversion/migration_cache/temp_asset_copy.copy"

def debug_log(msg):
    """Write debug message to file and console"""
    with open(DEBUG_FILE, 'a') as f:
        f.write(msg + "\n")
    print(msg)

# Armor Data Asset paths to extract
ARMOR_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor02",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHat",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleBracers",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleGreaves",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleTalisman",
]

def get_property_value(obj, prop_name):
    """Try to get a property value from an object using various methods"""
    # Try direct get_editor_property first
    try:
        return obj.get_editor_property(prop_name)
    except:
        pass

    # Try attribute access
    try:
        return getattr(obj, prop_name)
    except:
        pass

    # Try with underscore variation
    try:
        snake_name = ''.join(['_' + c.lower() if c.isupper() else c for c in prop_name]).lstrip('_')
        return obj.get_editor_property(snake_name)
    except:
        pass

    return None

def list_struct_properties(struct_obj, indent="  "):
    """List all accessible properties on a struct object"""
    props = []
    # Try dir() to get all attributes
    for attr in dir(struct_obj):
        if attr.startswith('_'):
            continue
        # Skip methods and callables
        try:
            val = getattr(struct_obj, attr)
            if not callable(val):
                props.append(attr)
        except:
            pass
    return props

def extract_via_copy_export(asset_path, asset_name):
    """
    Extract equipment data by exporting asset as COPY format and parsing text.
    COPY format includes all property values in readable text.
    """
    mesh_info = {}
    stat_changes = {}
    equip_slots = []

    try:
        # Export asset to COPY format
        task = unreal.AssetExportTask()
        task.set_editor_property('object', unreal.EditorAssetLibrary.load_asset(asset_path))
        task.set_editor_property('filename', TEMP_COPY_FILE)
        task.set_editor_property('automated', True)
        task.set_editor_property('prompt', False)
        task.set_editor_property('replace_identical', True)

        success = unreal.Exporter.run_asset_export_task(task)
        if not success:
            debug_log(f"  Export failed for {asset_name}")
            return mesh_info, stat_changes, equip_slots

        # Read exported text - try multiple encodings
        export_text = ""
        for encoding in ['utf-8', 'utf-16', 'utf-16-le', 'utf-16-be']:
            try:
                with open(TEMP_COPY_FILE, 'r', encoding=encoding, errors='strict') as f:
                    export_text = f.read()
                    # Check if we got valid text (no excessive spacing)
                    if export_text and not export_text.startswith('B e g i n'):
                        debug_log(f"  Read with encoding: {encoding}")
                        break
            except:
                continue

        # Fallback: if still has spacing issues, remove extra spaces
        if 'B e g i n' in export_text:
            # UTF-16 with null bytes interpreted as spaces - join chars
            export_text = export_text.replace('\x00', '')
            debug_log(f"  Fixed null byte spacing")

        debug_log(f"  Exported {len(export_text)} bytes")

        # Save raw export for armor items for debugging
        if asset_name in ["DA_ExampleArmor", "DA_ExampleArmor02"]:
            debug_file = f"C:/scripts/SLFConversion/migration_cache/{asset_name}_export.txt"
            with open(debug_file, 'w', encoding='utf-8') as f:
                f.write(export_text)
            debug_log(f"  Saved raw export to {debug_file}")

        # Check if SkeletalMeshInfo appears in the text at all
        if 'SkeletalMesh' in export_text:
            debug_log(f"  *** Found 'SkeletalMesh' in export! ***")
        else:
            debug_log(f"  NOTE: No 'SkeletalMesh' found in export text")

        # Parse SkeletalMeshInfo entries
        # Format: SkeletalMeshInfo_XXX=(("CharacterAssetPath", "MeshPath"),("CharacterAssetPath2", "MeshPath2"))
        # Find all entries directly from the export text using a greedy pattern
        # Each entry looks like: ("CharPath", "MeshPath")
        # We look for all such entries after SkeletalMeshInfo_
        if 'SkeletalMeshInfo_' in export_text:
            # Get the substring starting from SkeletalMeshInfo to next property or end
            skel_start = export_text.find('SkeletalMeshInfo_')
            # Find the end - look for next property name pattern or end of struct
            remaining = export_text[skel_start:]
            # The next property starts with a capital letter followed by underscore and numbers
            next_prop_match = re.search(r'\),([A-Z][a-zA-Z]+_\d+)', remaining)
            if next_prop_match:
                skel_end = skel_start + next_prop_match.start() + 1
                skel_content = export_text[skel_start:skel_end]
            else:
                # Take to end of line/export - find closing ))))
                end_match = re.search(r'\)\)\)\)', remaining)
                if end_match:
                    skel_content = remaining[:end_match.end()]
                else:
                    skel_content = remaining[:1000]  # Larger fallback

            debug_log(f"  Found SkeletalMeshInfo section: {len(skel_content)} chars")

            # Parse individual entries: ("CharacterPath", "MeshPath")
            # The character path contains single quotes like:
            # "/Game/.../PDA_BaseCharacterInfo.PDA_BaseCharacterInfo_C'/Game/.../DA_Quinn.DA_Quinn'"
            entry_pattern = r'\("([^"]+)",\s*"([^"]+)"\)'
            entries = re.findall(entry_pattern, skel_content)
            debug_log(f"  Found {len(entries)} mesh entries")

            for char_path, mesh_path in entries:
                # Extract character name from path (e.g., DA_Quinn from full path)
                # Path format: /Game/.../DA_Quinn.DA_Quinn' - get the asset name
                char_name = char_path.rstrip("'").split('.')[-1]
                mesh_info[char_name] = mesh_path
                debug_log(f"    Mesh: {char_name} -> {mesh_path}")

        # Parse stat changes
        # Format: StatChanges_XXX=(((TagName="xxx"), value),((TagName="yyy"), value))
        stat_info_match = re.search(r'StatChanges_[^=]+=\((.*?)\)\)', export_text, re.DOTALL)
        if stat_info_match:
            stat_content = stat_info_match.group(1)
            # Parse: ((TagName="SoulslikeFramework.Stat.X"), value)
            stat_pattern = r'\(\(TagName="([^"]+)"\),\s*([0-9.-]+)\)'
            stat_matches = re.findall(stat_pattern, stat_content)
            for tag_name, value in stat_matches:
                stat_changes[tag_name] = float(value)
                debug_log(f"    Stat: {tag_name} = {value}")

        # Parse equip slots
        slot_pattern = r'EquipSlots.*?TagName="([^"]+)"'
        slot_matches = re.findall(slot_pattern, export_text)
        for slot in slot_matches:
            equip_slots.append(slot)
            debug_log(f"    Slot: {slot}")

    except Exception as e:
        debug_log(f"  Error in export extraction: {e}")

    return mesh_info, stat_changes, equip_slots

def extract_skeletal_mesh_info_direct(asset, asset_name):
    """
    Extract SkeletalMeshInfo directly from the asset's ItemInformation property.
    Uses Python reflection to access nested struct data.
    """
    mesh_info = {}

    # Try to get ItemInformation struct
    item_info = get_property_value(asset, "item_information")
    if item_info is None:
        item_info = get_property_value(asset, "ItemInformation")

    if item_info is None:
        debug_log(f"  Could not get ItemInformation property")
        # List available properties
        try:
            asset_class = asset.get_class()
            debug_log(f"  Asset class: {asset_class.get_name()}")
            # Try to find properties by iterating
            for attr in dir(asset):
                if not attr.startswith('_') and 'item' in attr.lower():
                    debug_log(f"    Potential property: {attr}")
        except Exception as e:
            debug_log(f"  Error listing properties: {e}")
        return mesh_info

    debug_log(f"  Got ItemInformation: {type(item_info)}")

    # DEBUG: List all properties on ItemInformation struct
    props = list_struct_properties(item_info)
    debug_log(f"  ItemInformation properties: {props}")

    # Try to get EquipmentDetails from ItemInformation
    equip_details = get_property_value(item_info, "equipment_details")
    if equip_details is None:
        equip_details = get_property_value(item_info, "EquipmentDetails")

    if equip_details is None:
        debug_log(f"  Could not get EquipmentDetails property")
        # Try all property variations
        for prop_name in props:
            if 'equip' in prop_name.lower():
                debug_log(f"    Trying property: {prop_name}")
                equip_details = get_property_value(item_info, prop_name)
                if equip_details is not None:
                    debug_log(f"    Found via: {prop_name}")
                    break
        if equip_details is None:
            return mesh_info

    debug_log(f"  Got EquipmentDetails: {type(equip_details)}")

    # Try to get SkeletalMeshInfo TMap
    skeletal_mesh_map = get_property_value(equip_details, "skeletal_mesh_info")
    if skeletal_mesh_map is None:
        skeletal_mesh_map = get_property_value(equip_details, "SkeletalMeshInfo")

    if skeletal_mesh_map is None:
        debug_log(f"  Could not get SkeletalMeshInfo property")
        return mesh_info

    debug_log(f"  Got SkeletalMeshInfo: {type(skeletal_mesh_map)}")

    # Extract TMap entries
    try:
        # Python TMap iteration
        if hasattr(skeletal_mesh_map, 'keys'):
            for key in skeletal_mesh_map.keys():
                value = skeletal_mesh_map[key]
                tag_str = str(key)
                if value:
                    mesh_path = str(value.get_path_name()) if hasattr(value, 'get_path_name') else str(value)
                    mesh_info[tag_str] = mesh_path
                    debug_log(f"    Tag: {tag_str} -> {mesh_path}")
        elif hasattr(skeletal_mesh_map, 'items'):
            for key, value in skeletal_mesh_map.items():
                tag_str = str(key)
                if value:
                    mesh_path = str(value.get_path_name()) if hasattr(value, 'get_path_name') else str(value)
                    mesh_info[tag_str] = mesh_path
                    debug_log(f"    Tag: {tag_str} -> {mesh_path}")
        else:
            debug_log(f"  SkeletalMeshInfo is not iterable: {skeletal_mesh_map}")
    except Exception as e:
        debug_log(f"  Error iterating SkeletalMeshInfo: {e}")

    return mesh_info

def extract_stat_changes_direct(asset, asset_name):
    """Extract StatChanges directly from the asset"""
    stat_changes = {}

    item_info = get_property_value(asset, "item_information")
    if item_info is None:
        item_info = get_property_value(asset, "ItemInformation")
    if item_info is None:
        return stat_changes

    equip_details = get_property_value(item_info, "equipment_details")
    if equip_details is None:
        equip_details = get_property_value(item_info, "EquipmentDetails")
    if equip_details is None:
        return stat_changes

    stat_map = get_property_value(equip_details, "stat_changes")
    if stat_map is None:
        stat_map = get_property_value(equip_details, "StatChanges")
    if stat_map is None:
        return stat_changes

    try:
        if hasattr(stat_map, 'keys'):
            for key in stat_map.keys():
                value = stat_map[key]
                tag_str = str(key)
                if hasattr(value, 'delta'):
                    stat_changes[tag_str] = float(value.delta)
                    debug_log(f"    Stat: {tag_str} = {value.delta}")
                elif hasattr(value, 'Delta'):
                    stat_changes[tag_str] = float(value.Delta)
                    debug_log(f"    Stat: {tag_str} = {value.Delta}")
    except Exception as e:
        debug_log(f"  Error extracting stat changes: {e}")

    return stat_changes

def extract_equip_slots_direct(asset, asset_name):
    """Extract EquipSlots directly from the asset"""
    equip_slots = []

    item_info = get_property_value(asset, "item_information")
    if item_info is None:
        item_info = get_property_value(asset, "ItemInformation")
    if item_info is None:
        return equip_slots

    equip_details = get_property_value(item_info, "equipment_details")
    if equip_details is None:
        equip_details = get_property_value(item_info, "EquipmentDetails")
    if equip_details is None:
        return equip_slots

    slots = get_property_value(equip_details, "equip_slots")
    if slots is None:
        slots = get_property_value(equip_details, "EquipSlots")
    if slots is None:
        return equip_slots

    try:
        # GameplayTagContainer iteration
        if hasattr(slots, '__iter__'):
            for tag in slots:
                equip_slots.append(str(tag))
                debug_log(f"    EquipSlot: {tag}")
    except Exception as e:
        debug_log(f"  Error extracting equip slots: {e}")

    return equip_slots

def extract_armor_data():
    """
    Extract armor mesh and equipment data from Data Assets.
    Uses direct property access for Blueprint data assets.
    """
    # Clear debug file
    with open(DEBUG_FILE, 'w') as f:
        f.write("")

    # Ensure output directory exists
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)

    debug_log("")
    debug_log("=" * 70)
    debug_log("EXTRACTING ARMOR MESH DATA FROM DATA ASSETS (Direct Property Access)")
    debug_log("=" * 70)

    armor_data = {}

    for asset_path in ARMOR_ASSETS:
        asset_name = asset_path.split("/")[-1]
        debug_log(f"\n[{asset_name}]")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            debug_log(f"  [SKIP] Could not load")
            continue

        debug_log(f"  Loaded: {type(asset)}")

        # Extract data using direct property access
        skeletal_mesh_info = extract_skeletal_mesh_info_direct(asset, asset_name)
        stat_changes = extract_stat_changes_direct(asset, asset_name)
        equip_slots = extract_equip_slots_direct(asset, asset_name)

        armor_info = {
            "path": asset_path,
            "skeletal_mesh_info": skeletal_mesh_info,
            "stat_changes": stat_changes,
            "equip_slots": equip_slots
        }

        # If direct access failed, try export-based extraction
        if not skeletal_mesh_info and not stat_changes and not equip_slots:
            debug_log(f"  Direct access failed, trying COPY export...")
            skeletal_mesh_info, stat_changes, equip_slots = extract_via_copy_export(asset_path, asset_name)

        armor_info = {
            "path": asset_path,
            "skeletal_mesh_info": skeletal_mesh_info,
            "stat_changes": stat_changes,
            "equip_slots": equip_slots
        }

        # Always store the asset info even if empty (for debugging)
        armor_data[asset_name] = armor_info

        if skeletal_mesh_info or stat_changes or equip_slots:
            debug_log(f"  [OK] Meshes: {len(skeletal_mesh_info)}, Stats: {len(stat_changes)}, Slots: {len(equip_slots)}")
        else:
            debug_log(f"  [EMPTY] No equipment data found via direct access or export")

    # Save to JSON
    debug_log("")
    debug_log("=" * 70)
    debug_log(f"SAVING TO {OUTPUT_FILE}")
    debug_log("=" * 70)

    with open(OUTPUT_FILE, 'w') as f:
        json.dump(armor_data, f, indent=2)

    debug_log(f"\nExtracted data for {len(armor_data)} armor items:")
    for name, data in armor_data.items():
        mesh_count = len(data.get("skeletal_mesh_info", {}))
        stat_count = len(data.get("stat_changes", {}))
        debug_log(f"  - {name}: {mesh_count} meshes, {stat_count} stats")

    return armor_data


if __name__ == "__main__":
    extract_armor_data()
