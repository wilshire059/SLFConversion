"""
Full Item Data Migration Script
===============================
Extracts ItemInformation from JSON exports and applies to migrated PDA_Item assets.

This script:
1. Reads all DA_*.json files from Exports/BlueprintDNA/DataAsset
2. Parses the ItemInformation struct string to extract all fields
3. Generates Unreal Python code to apply this data to migrated items

Run with: python migrate_item_data.py
Then run the generated apply_item_data.py in Unreal Editor
"""

import json
import os
import re
from pathlib import Path

# Item data assets to migrate
ITEM_ASSETS = [
    'DA_Apple',
    'DA_BossMace',
    'DA_CrimsonEverbloom',
    'DA_Cube',
    'DA_ExampleArmor',
    'DA_ExampleArmor02',
    'DA_ExampleBracers',
    'DA_ExampleGreaves',
    'DA_ExampleHat',
    'DA_ExampleHelmet',
    'DA_ExampleTalisman',
    'DA_Greatsword',
    'DA_HealthFlask',
    'DA_Katana',
    'DA_Lantern',
    'DA_PoisonSword',
    'DA_PrisonKey',
    'DA_Shield01',
    'DA_Sword01',
    'DA_Sword02',
    'DA_ThrowingKnife',
]

# Category enum mapping (Blueprint enum values to C++ enum)
# NewEnumerator0=None, NewEnumerator1=Tools, etc.
CATEGORY_MAP = {
    'NewEnumerator0': 'None',        # 0
    'NewEnumerator1': 'Tools',       # 1
    'NewEnumerator2': 'Crafting',    # 2
    'NewEnumerator3': 'Bolstering',  # 3
    'NewEnumerator4': 'KeyItems',    # 4
    'NewEnumerator5': 'Abilities',   # 5
    'NewEnumerator6': 'Weapons',     # 6
    'NewEnumerator7': 'Shields',     # 7
    'NewEnumerator8': 'Armor',       # 8
}

# SubCategory enum mapping
SUBCATEGORY_MAP = {
    'NewEnumerator0': 'Flasks',           # 0
    'NewEnumerator1': 'Throwables',       # 1
    'NewEnumerator2': 'Miscellaneous',    # 2 (Throwing knife uses this)
    'NewEnumerator3': 'UpgradeMaterials', # 3
    'NewEnumerator4': 'Consumables',      # 4
    'NewEnumerator5': 'Talismans',        # 5
    'NewEnumerator6': 'Rings',            # 6
    'NewEnumerator7': 'Head',             # 7
    'NewEnumerator8': 'Body',             # 8
    'NewEnumerator9': 'Arms',             # 9
    'NewEnumerator10': 'Legs',            # 10
    'NewEnumerator11': 'OneHanded',       # 11
    'NewEnumerator12': 'TwoHanded',       # 12
    'NewEnumerator13': 'Swords',          # 13
    'NewEnumerator14': 'GreatSwords',     # 14
    'NewEnumerator15': 'Katanas',         # 15
    'NewEnumerator16': 'Axes',            # 16
    'NewEnumerator17': 'GreatAxes',       # 17
    'NewEnumerator18': 'Hammers',         # 18
    'NewEnumerator19': 'GreatHammers',    # 19
    'NewEnumerator20': 'Spears',          # 20
    'NewEnumerator21': 'Halberds',        # 21
    'NewEnumerator22': 'SmallShields',    # 22
    'NewEnumerator23': 'MediumShields',   # 23
    'NewEnumerator24': 'GreatShields',    # 24
    'NewEnumerator25': 'Staves',          # 25
    'NewEnumerator26': 'Catalysts',       # 26
}

# WorldDisplayStyle enum mapping
WORLD_DISPLAY_STYLE_MAP = {
    'NewEnumerator0': 'ShowMesh',        # 0
    'NewEnumerator1': 'ShowNiagaraOnly', # 1
    'NewEnumerator2': 'ShowBoth',        # 2
}


def extract_nsloctext(text, field_prefix):
    """Extract text from NSLOCTEXT format"""
    # Pattern: DisplayName_5_GUID=NSLOCTEXT("namespace", "key", "Actual Text")
    # The JSON has escaped quotes that become regular quotes when parsed
    # Format: =NSLOCTEXT("[namespace]", "key", "text")
    pattern = rf'{field_prefix}_\d+_[A-F0-9]+=NSLOCTEXT\(\s*"[^"]*"\s*,\s*"[^"]*"\s*,\s*"([^"]*)"'
    match = re.search(pattern, text)
    if match:
        # Unescape special characters
        result = match.group(1)
        result = result.replace('\\r\\n', '\n')
        result = result.replace("\\'", "'")
        result = result.replace('\\"', '"')
        return result

    # Try empty string format: =""
    pattern2 = rf'{field_prefix}_\d+_[A-F0-9]+=""'
    if re.search(pattern2, text):
        return ""

    return ""


def extract_texture_path(text, field_prefix):
    """Extract texture path from the serialized string"""
    # Pattern: IconSmall_15_GUID="/Game/...T_Name.T_Name"
    pattern = rf'{field_prefix}_\d+_[A-F0-9]+=("?/Game/[^"]+"|None)'
    match = re.search(pattern, text)
    if match:
        path = match.group(1)
        if path == 'None' or path == '""':
            return None
        # Clean up path - remove quotes and .T_Name suffix
        path = path.strip('"')
        if '.' in path:
            path = path.split('.')[0]  # Get just /Game/.../T_Name
        return path
    return None


def extract_int(text, field_prefix):
    """Extract integer value"""
    pattern = rf'{field_prefix}_\d+_[A-F0-9]+=(\d+)'
    match = re.search(pattern, text)
    if match:
        return int(match.group(1))
    return None


def extract_bool(text, field_prefix):
    """Extract boolean value"""
    pattern = rf'{field_prefix}_\d+_[A-F0-9]+=(True|False)'
    match = re.search(pattern, text, re.IGNORECASE)
    if match:
        return match.group(1).lower() == 'true'
    return False


def extract_enum(text, field_prefix):
    """Extract enum value"""
    pattern = rf'{field_prefix}_\d+_[A-F0-9]+=(NewEnumerator\d+)'
    match = re.search(pattern, text)
    if match:
        return match.group(1)
    return None


def extract_gameplay_tag(text, field_prefix):
    """Extract GameplayTag TagName"""
    pattern = rf'{field_prefix}_\d+_[A-F0-9]+=\(TagName="([^"]*)"\)'
    match = re.search(pattern, text)
    if match:
        return match.group(1)
    return ""


def extract_soft_class_path(text, field_prefix):
    """Extract soft class path"""
    pattern = rf'{field_prefix}_\d+_[A-F0-9]+=("?/Game/[^"]+"|None)'
    match = re.search(pattern, text)
    if match:
        path = match.group(1)
        if path == 'None' or path == '""':
            return None
        return path.strip('"')
    return None


def extract_niagara_path(text):
    """Extract WorldNiagaraSystem path from WorldPickupInfo"""
    pattern = r'WorldNiagaraSystem_\d+_[A-F0-9]+="(/Game/[^"]+)"'
    match = re.search(pattern, text)
    if match:
        path = match.group(1)
        if '.' in path:
            path = path.split('.')[0]
        return path
    return None


def parse_item_information(value_string):
    """Parse the ItemInformation struct string and extract all fields"""
    data = {
        'item_tag': '',
        'display_name': '',
        'short_description': '',
        'long_description': '',
        'discovery_description': '',
        'on_use_description': '',
        'icon_small': None,
        'icon_large_optional': None,
        'max_amount': 1,
        'category': 'None',
        'sub_category': 'Miscellaneous',
        'is_usable': False,
        'is_rechargeable': False,
        'action_to_trigger': '',
        'item_class': None,
        'world_display_style': 'ShowMesh',
        'world_niagara_system': None,
    }

    # Extract ItemTag
    item_tag_match = re.search(r'ItemTag_\d+_[A-F0-9]+=\(TagName="([^"]*)"\)', value_string)
    if item_tag_match:
        data['item_tag'] = item_tag_match.group(1)

    # Extract text fields
    data['display_name'] = extract_nsloctext(value_string, 'DisplayName')
    data['short_description'] = extract_nsloctext(value_string, 'ShortDescription')
    data['long_description'] = extract_nsloctext(value_string, 'LongDescription')
    data['discovery_description'] = extract_nsloctext(value_string, 'DiscoveryDescription')
    data['on_use_description'] = extract_nsloctext(value_string, 'OnUseDescription')

    # Extract texture paths
    data['icon_small'] = extract_texture_path(value_string, 'IconSmall')
    data['icon_large_optional'] = extract_texture_path(value_string, 'IconLargeOptional')

    # Extract integers
    max_amount = extract_int(value_string, 'MaxAmount')
    if max_amount is not None:
        data['max_amount'] = max_amount

    # Extract booleans
    data['is_usable'] = extract_bool(value_string, r'Usable\?')
    data['is_rechargeable'] = extract_bool(value_string, r'Rechargeable\?')

    # Extract category enum
    category_enum = extract_enum(value_string, 'Category')
    if category_enum and category_enum in CATEGORY_MAP:
        data['category'] = CATEGORY_MAP[category_enum]

    # Extract subcategory enum
    subcategory_enum = extract_enum(value_string, 'SubCategory')
    if subcategory_enum and subcategory_enum in SUBCATEGORY_MAP:
        data['sub_category'] = SUBCATEGORY_MAP[subcategory_enum]

    # Extract world display style
    world_style_enum = extract_enum(value_string, 'WorldDisplayStyle')
    if world_style_enum and world_style_enum in WORLD_DISPLAY_STYLE_MAP:
        data['world_display_style'] = WORLD_DISPLAY_STYLE_MAP[world_style_enum]

    # Extract gameplay tags
    data['action_to_trigger'] = extract_gameplay_tag(value_string, 'ActiontoTrigger')

    # Extract soft class path
    data['item_class'] = extract_soft_class_path(value_string, 'ItemClass')

    # Extract Niagara system path
    data['world_niagara_system'] = extract_niagara_path(value_string)

    return data


def load_json_exports():
    """Load all item JSON exports"""
    exports_dir = Path('C:/scripts/slfconversion/Exports/BlueprintDNA/DataAsset')
    items_data = {}

    for item_name in ITEM_ASSETS:
        json_path = exports_dir / f'{item_name}.json'
        if json_path.exists():
            with open(json_path, 'r', encoding='utf-8') as f:
                data = json.load(f)

            if 'Properties' in data and 'ItemInformation' in data['Properties']:
                value = data['Properties']['ItemInformation'].get('Value', '')
                items_data[item_name] = parse_item_information(value)
                print(f"Loaded: {item_name}")
                print(f"  DisplayName: {items_data[item_name]['display_name']}")
                print(f"  Category: {items_data[item_name]['category']}")
                print(f"  IconSmall: {items_data[item_name]['icon_small']}")
            else:
                print(f"WARNING: No ItemInformation in {item_name}")
        else:
            print(f"WARNING: JSON not found: {json_path}")

    return items_data


def generate_unreal_script(items_data):
    """Generate Unreal Python script to apply extracted data"""

    script = '''"""
Apply Item Data Migration Script
================================
Auto-generated script to apply extracted item data to migrated PDA_Item assets.

Run in Unreal Editor: File > Execute Python Script
"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Extracted item data from JSON exports
ITEMS_DATA = '''

    # Convert JSON to Python syntax (true->True, false->False, null->None)
    json_str = json.dumps(items_data, indent=2, ensure_ascii=False)
    python_str = json_str.replace(': true', ': True').replace(': false', ': False').replace(': null', ': None')
    script += python_str

    script += '''

def apply_item_data():
    """Apply extracted data to all item assets"""
    log("=" * 80)
    log("APPLYING ITEM DATA MIGRATION")
    log("=" * 80)

    success_count = 0
    error_count = 0

    for item_name, data in ITEMS_DATA.items():
        item_path = f'/Game/SoulslikeFramework/Data/Items/{item_name}'
        log(f"\\nProcessing: {item_name}")

        try:
            item = unreal.load_asset(item_path)
            if not item:
                log(f"  ERROR: Could not load asset")
                error_count += 1
                continue

            # Get the ItemInformation struct
            item_info = item.get_editor_property('item_information')
            if item_info is None:
                log(f"  ERROR: No item_information property")
                error_count += 1
                continue

            # Apply text fields
            if data.get('display_name'):
                item_info.set_editor_property('display_name', data['display_name'])
                log(f"  Set DisplayName: {data['display_name']}")

            if data.get('short_description'):
                item_info.set_editor_property('short_description', data['short_description'])
                log(f"  Set ShortDescription: {data['short_description'][:50]}...")

            if data.get('long_description'):
                item_info.set_editor_property('long_description', data['long_description'])
                log(f"  Set LongDescription: (set)")

            if data.get('discovery_description'):
                item_info.set_editor_property('discovery_description', data['discovery_description'])

            if data.get('on_use_description'):
                item_info.set_editor_property('on_use_description', data['on_use_description'])

            # Apply boolean fields - try multiple property names
            try:
                item_info.set_editor_property('b_usable', data.get('is_usable', False))
            except:
                try:
                    item_info.set_editor_property('usable', data.get('is_usable', False))
                except Exception as e:
                    log(f"  Warning: Could not set usable property: {e}")
            try:
                item_info.set_editor_property('b_rechargeable', data.get('is_rechargeable', False))
            except:
                try:
                    item_info.set_editor_property('rechargeable', data.get('is_rechargeable', False))
                except Exception as e:
                    log(f"  Warning: Could not set rechargeable property: {e}")

            # Apply max amount
            item_info.set_editor_property('max_amount', data.get('max_amount', 1))
            log(f"  Set MaxAmount: {data.get('max_amount', 1)}")

            # Apply icon textures
            if data.get('icon_small'):
                texture = unreal.load_asset(data['icon_small'])
                if texture:
                    item_info.set_editor_property('icon_small', texture)
                    log(f"  Set IconSmall: {data['icon_small']}")
                else:
                    log(f"  WARNING: Could not load texture: {data['icon_small']}")

            if data.get('icon_large_optional'):
                texture = unreal.load_asset(data['icon_large_optional'])
                if texture:
                    item_info.set_editor_property('icon_large_optional', texture)

            # Apply category
            category_str = data.get('category', 'None')
            try:
                category_enum = getattr(unreal.ESLFItemCategory, category_str.upper(), None)
                if category_enum is None:
                    # Try exact match
                    category_enum = getattr(unreal.ESLFItemCategory, category_str, None)
                if category_enum:
                    category_data = item_info.get_editor_property('category')
                    if category_data:
                        category_data.set_editor_property('category', category_enum)
                        log(f"  Set Category: {category_str}")
            except Exception as e:
                log(f"  Category error: {e}")

            # Apply subcategory
            subcategory_str = data.get('sub_category', 'Miscellaneous')
            try:
                subcategory_enum = getattr(unreal.ESLFItemSubCategory, subcategory_str.upper(), None)
                if subcategory_enum is None:
                    subcategory_enum = getattr(unreal.ESLFItemSubCategory, subcategory_str, None)
                if subcategory_enum:
                    category_data = item_info.get_editor_property('category')
                    if category_data:
                        category_data.set_editor_property('sub_category', subcategory_enum)
                        log(f"  Set SubCategory: {subcategory_str}")
            except Exception as e:
                log(f"  SubCategory error: {e}")

            # Apply ItemTag
            if data.get('item_tag'):
                try:
                    tag = unreal.GameplayTag()
                    # Note: GameplayTag construction may need different approach
                    item_info.set_editor_property('item_tag', tag)
                except Exception as e:
                    log(f"  ItemTag error: {e}")

            # Apply ActionToTrigger
            if data.get('action_to_trigger'):
                try:
                    tag = unreal.GameplayTag()
                    item_info.set_editor_property('action_to_trigger', tag)
                except Exception as e:
                    log(f"  ActionToTrigger error: {e}")

            # Apply WorldPickupInfo
            try:
                world_info = item_info.get_editor_property('world_pickup_info')
                if world_info:
                    # Set world display style
                    style_str = data.get('world_display_style', 'ShowMesh')
                    style_enum = getattr(unreal.ESLFWorldMeshStyle, style_str.upper(), None)
                    if style_enum is None:
                        style_enum = getattr(unreal.ESLFWorldMeshStyle, style_str, None)
                    if style_enum:
                        world_info.set_editor_property('world_display_style', style_enum)

                    # Set Niagara system
                    if data.get('world_niagara_system'):
                        ns = unreal.load_asset(data['world_niagara_system'])
                        if ns:
                            world_info.set_editor_property('world_niagara_system', ns)
                            log(f"  Set WorldNiagaraSystem: {data['world_niagara_system']}")
            except Exception as e:
                log(f"  WorldPickupInfo error: {e}")

            # Apply ItemClass
            if data.get('item_class'):
                try:
                    item_class = unreal.load_class(None, data['item_class'])
                    if item_class:
                        item_info.set_editor_property('item_class', item_class)
                        log(f"  Set ItemClass: {data['item_class']}")
                except Exception as e:
                    log(f"  ItemClass error: {e}")

            # Write the modified struct back to the asset
            item.set_editor_property('item_information', item_info)

            # Mark dirty and save
            unreal.EditorAssetLibrary.save_loaded_asset(item)
            log(f"  SAVED: {item_name}")
            success_count += 1

        except Exception as e:
            log(f"  ERROR: {e}")
            error_count += 1

    log("\\n" + "=" * 80)
    log(f"MIGRATION COMPLETE: {success_count} succeeded, {error_count} failed")
    log("=" * 80)

if __name__ == "__main__":
    apply_item_data()
'''

    return script


def main():
    print("=" * 80)
    print("ITEM DATA MIGRATION - EXTRACTION PHASE")
    print("=" * 80)

    # Load and parse all JSON exports
    items_data = load_json_exports()

    print(f"\n\nExtracted data for {len(items_data)} items")

    # Save extracted data to JSON for reference
    output_json = Path('C:/scripts/slfconversion/extracted_item_data.json')
    with open(output_json, 'w', encoding='utf-8') as f:
        json.dump(items_data, f, indent=2, ensure_ascii=False)
    print(f"\nSaved extracted data to: {output_json}")

    # Generate Unreal Python script
    unreal_script = generate_unreal_script(items_data)

    output_script = Path('C:/scripts/slfconversion/apply_item_data.py')
    with open(output_script, 'w', encoding='utf-8') as f:
        f.write(unreal_script)
    print(f"Generated Unreal script: {output_script}")

    print("\n" + "=" * 80)
    print("NEXT STEPS:")
    print("=" * 80)
    print("1. Review extracted_item_data.json for accuracy")
    print("2. Run apply_item_data.py in Unreal Editor:")
    print('   File > Execute Python Script > apply_item_data.py')
    print("   OR run headless:")
    print('   UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_item_data.py"')


if __name__ == "__main__":
    main()
