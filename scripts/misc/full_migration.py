# full_migration.py - Complete migration workflow
# 1. Extract data from backup content
# 2. Run reparenting migration
# 3. Apply all data (icons, niagara, dodge montages)

import unreal
import json
import os
import re

CACHE_DIR = "C:/scripts/SLFConversion/migration_cache"
os.makedirs(CACHE_DIR, exist_ok=True)

# ===============================================================================
# PHASE 1: EXTRACT DATA
# ===============================================================================

def extract_item_data():
    """Extract icons and niagara from item assets"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 1A: Extracting Item Data")
    unreal.log_warning("=" * 60)

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)
    item_data = {}

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            continue

        data = {}
        try:
            item_info = None
            for prop_name in ['ItemInformation', 'item_information']:
                try:
                    item_info = asset.get_editor_property(prop_name)
                    if item_info:
                        break
                except:
                    pass

            if item_info and hasattr(item_info, 'export_text'):
                export_text = item_info.export_text()

                for field in ['IconSmall', 'IconLargeOptional', 'WorldNiagaraSystem']:
                    pattern = field + '[^=]*="([^"]+)"'
                    match = re.search(pattern, export_text)
                    if match and match.group(1) != "None":
                        data[field] = match.group(1)

        except Exception as e:
            pass

        if data:
            item_data[asset_name] = data
            unreal.log_warning("  [OK] " + asset_name)

    cache_file = os.path.join(CACHE_DIR, "item_data.json")
    with open(cache_file, 'w') as f:
        json.dump(item_data, f, indent=2)
    unreal.log_warning("  Extracted " + str(len(item_data)) + " items")

# ===============================================================================
# PHASE 2: RUN MIGRATION
# ===============================================================================

def run_migration():
    """Run the reparenting migration"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 2: Running Migration")
    unreal.log_warning("=" * 60)

    import importlib.util
    spec = importlib.util.spec_from_file_location("run_migration", "C:/scripts/SLFConversion/run_migration.py")
    migration_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(migration_module)

# ===============================================================================
# PHASE 3: APPLY DATA
# ===============================================================================

def apply_item_data():
    """Apply icons and niagara to items"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 3A: Applying Item Data (Icons + Niagara)")
    unreal.log_warning("=" * 60)

    cache_file = os.path.join(CACHE_DIR, "item_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning("  Cache not found!")
        return

    with open(cache_file, 'r') as f:
        item_data = json.load(f)

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    count = 0
    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if asset_name not in item_data:
            continue

        data = item_data[asset_name]
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            continue

        try:
            item_info = asset.get_editor_property('item_information')
            if item_info is None:
                continue

            modified = False

            # Apply icons
            if 'IconSmall' in data:
                icon_path = data['IconSmall'].rsplit(".", 1)[0]
                texture = unreal.EditorAssetLibrary.load_asset(icon_path)
                if texture:
                    item_info.set_editor_property('icon_small', texture)
                    modified = True

            if 'IconLargeOptional' in data:
                icon_path = data['IconLargeOptional'].rsplit(".", 1)[0]
                texture = unreal.EditorAssetLibrary.load_asset(icon_path)
                if texture:
                    item_info.set_editor_property('icon_large_optional', texture)
                    modified = True

            # Apply niagara
            if 'WorldNiagaraSystem' in data:
                niagara_path = data['WorldNiagaraSystem'].rsplit(".", 1)[0]
                niagara = unreal.EditorAssetLibrary.load_asset(niagara_path)
                if niagara:
                    asset.set_editor_property('world_niagara_system', niagara)
                    modified = True

            if modified:
                asset.set_editor_property('item_information', item_info)
                unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                count += 1
                unreal.log_warning("  [OK] " + asset_name)

        except Exception as e:
            unreal.log_warning("  [ERROR] " + asset_name + ": " + str(e))

    unreal.log_warning("  Applied to " + str(count) + " items")


def apply_dodge_montages():
    """Apply dodge montages to B_Action_Dodge"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 3B: Applying Dodge Montages")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_warning("  Could not load B_Action_Dodge")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("  Could not get generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("  Could not get CDO")
        return

    montage_base = "/Game/SoulslikeFramework/Demo/_Animations/Dodges"
    montage_map = {
        'forward': montage_base + "/AM_SLF_Dodge_F",
        'forward_left': montage_base + "/AM_SLF_Dodge_FL",
        'forward_right': montage_base + "/AM_SLF_Dodge_FR",
        'left': montage_base + "/AM_SLF_Dodge_L",
        'right': montage_base + "/AM_SLF_Dodge_R",
        'backward': montage_base + "/AM_SLF_Dodge_B",
        'backward_left': montage_base + "/AM_SLF_Dodge_BL",
        'backward_right': montage_base + "/AM_SLF_Dodge_BR",
    }

    try:
        dodge_montages = cdo.get_editor_property('dodge_montages')
        if dodge_montages:
            for field, path in montage_map.items():
                montage = unreal.EditorAssetLibrary.load_asset(path)
                if montage:
                    dodge_montages.set_editor_property(field, montage)
                    unreal.log_warning("  [OK] " + field)

            cdo.set_editor_property('dodge_montages', dodge_montages)
            bp.modify()
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log_warning("  Saved B_Action_Dodge")

    except Exception as e:
        unreal.log_warning("  Error: " + str(e))


# ===============================================================================
# MAIN
# ===============================================================================

def main():
    unreal.log_warning("")
    unreal.log_warning("*" * 80)
    unreal.log_warning("FULL MIGRATION - START")
    unreal.log_warning("*" * 80)
    unreal.log_warning("")

    # Phase 1: Extract
    extract_item_data()

    # Phase 2: Migrate
    run_migration()

    # Phase 3: Apply
    apply_item_data()
    apply_dodge_montages()

    unreal.log_warning("")
    unreal.log_warning("*" * 80)
    unreal.log_warning("FULL MIGRATION - COMPLETE")
    unreal.log_warning("*" * 80)

if __name__ == "__main__":
    main()
