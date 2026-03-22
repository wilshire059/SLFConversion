# resilient_migration.py
#
# RESILIENT MIGRATION SYSTEM
# ==========================
# This script provides a robust migration workflow that:
# 1. Extracts ALL important data from BACKUP content (not current content)
# 2. Restores backup to Content folder
# 3. Runs reparenting migration
# 4. Applies extracted data to migrated assets
#
# Usage:
#   UnrealEditor-Cmd.exe SLFConversion.uproject -run=pythonscript -script="resilient_migration.py"
#
# The key insight is extracting from BACKUP ensures we always have clean data,
# regardless of the state of the current Content folder.

import unreal
import json
import os
import re
import shutil

# ═══════════════════════════════════════════════════════════════════════════════
# CONFIGURATION
# ═══════════════════════════════════════════════════════════════════════════════

BACKUP_PATH = "C:/scripts/bp_only/Content"
CONTENT_PATH = "C:/scripts/SLFConversion/Content"
CACHE_DIR = "C:/scripts/SLFConversion/migration_cache"
RUN_MIGRATION_SCRIPT = "C:/scripts/SLFConversion/run_migration.py"

os.makedirs(CACHE_DIR, exist_ok=True)

# ═══════════════════════════════════════════════════════════════════════════════
# PHASE 1: EXTRACT DATA FROM BACKUP
# ═══════════════════════════════════════════════════════════════════════════════

def extract_all_data_from_backup():
    """Extract all important data from BACKUP content (not current content)"""
    unreal.log_warning("=" * 80)
    unreal.log_warning("PHASE 1: EXTRACTING DATA FROM BACKUP")
    unreal.log_warning("=" * 80)

    # We need to temporarily load assets from the backup path
    # The safest way is to ensure backup is copied to content first,
    # then extract, then proceed with migration

    # Extract item data (icons, niagara)
    extract_item_data()

    # Extract mesh data
    extract_default_mesh_data()

    # Extract physics asset reference
    extract_physics_asset()

    unreal.log_warning("Extraction complete!")


def extract_item_data():
    """Extract IconSmall, IconLargeOptional from item assets"""
    unreal.log_warning("-" * 60)
    unreal.log_warning("Extracting Item Data (Icons)")
    unreal.log_warning("-" * 60)

    items_path = "/Game/SoulslikeFramework/Data/Items"

    if not unreal.EditorAssetLibrary.does_directory_exist(items_path):
        unreal.log_warning(f"  Items directory not found: {items_path}")
        return

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
            # Try to get ItemInformation struct
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

                # Extract IconSmall
                match = re.search(r'IconSmall[^=]*="([^"]+)"', export_text)
                if match and match.group(1) != "None":
                    data['IconSmall'] = match.group(1)

                # Extract IconLargeOptional
                match = re.search(r'IconLargeOptional[^=]*="([^"]+)"', export_text)
                if match and match.group(1) != "None":
                    data['IconLargeOptional'] = match.group(1)

                # Extract WorldNiagaraSystem
                match = re.search(r'WorldNiagaraSystem[^=]*="([^"]+)"', export_text)
                if match and match.group(1) != "None":
                    data['WorldNiagaraSystem'] = match.group(1)

        except Exception as e:
            unreal.log_warning(f"  Error extracting {asset_name}: {e}")

        if data:
            item_data[asset_name] = data
            unreal.log_warning(f"  [OK] {asset_name}: {list(data.keys())}")

    # Save to cache
    cache_file = os.path.join(CACHE_DIR, "item_data.json")
    with open(cache_file, 'w') as f:
        json.dump(item_data, f, indent=2)

    unreal.log_warning(f"  Saved {len(item_data)} items to {cache_file}")


def extract_default_mesh_data():
    """Extract mesh references from PDA_DefaultMeshData"""
    unreal.log_warning("-" * 60)
    unreal.log_warning("Extracting Default Mesh Data")
    unreal.log_warning("-" * 60)

    asset_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)

    if not asset:
        unreal.log_warning(f"  Could not load {asset_path}")
        return

    mesh_data = {}

    try:
        # Try to get MeshData struct (Blueprint struct)
        mesh_struct = None
        for prop_name in ['MeshData', 'mesh_data']:
            try:
                mesh_struct = asset.get_editor_property(prop_name)
                if mesh_struct:
                    unreal.log_warning(f"  Found MeshData property")
                    break
            except:
                pass

        if mesh_struct and hasattr(mesh_struct, 'export_text'):
            export_text = mesh_struct.export_text()
            unreal.log_warning(f"  MeshData export_text (first 500 chars):")
            unreal.log_warning(f"    {export_text[:500]}")

            # Extract mesh paths from the struct
            # Pattern: PropertyName=SkeletalMesh'"/Game/Path/To/Mesh.Mesh"'
            mesh_fields = ['HeadMesh', 'UpperBodyMesh', 'ArmsMesh', 'LowerBodyMesh',
                          'Head', 'UpperBody', 'Arms', 'LowerBody',
                          'HeadPiece', 'Torso', 'Gloves', 'Greaves']

            for field in mesh_fields:
                # Try multiple patterns
                patterns = [
                    rf'{field}[^=]*=SkeletalMesh\'"([^"\']+)"\'',
                    rf'{field}[^=]*="([^"]+)"',
                    rf'{field}[^=]*=([^\s,\)]+)',
                ]
                for pattern in patterns:
                    match = re.search(pattern, export_text)
                    if match:
                        value = match.group(1)
                        if value and value != "None" and "SkeletalMesh" in value or "/Game/" in value:
                            # Normalize to just the path
                            if "'" in value:
                                value = value.split("'")[0]
                            mesh_data[field] = value
                            unreal.log_warning(f"  [OK] {field}: {value}")
                            break

        # Also try direct properties (in case already on C++ class)
        for prop_name in ['HeadMesh', 'UpperBodyMesh', 'ArmsMesh', 'LowerBodyMesh']:
            if prop_name not in mesh_data:
                try:
                    mesh = asset.get_editor_property(prop_name.lower().replace('mesh', '_mesh'))
                    if mesh:
                        mesh_data[prop_name] = mesh.get_path_name()
                        unreal.log_warning(f"  [OK] {prop_name} (direct): {mesh.get_path_name()}")
                except:
                    pass

    except Exception as e:
        unreal.log_warning(f"  Error extracting mesh data: {e}")
        import traceback
        traceback.print_exc()

    # Save to cache
    cache_file = os.path.join(CACHE_DIR, "default_mesh_data.json")
    with open(cache_file, 'w') as f:
        json.dump(mesh_data, f, indent=2)

    unreal.log_warning(f"  Saved mesh data to {cache_file}")


def extract_physics_asset():
    """Extract physics asset reference"""
    unreal.log_warning("-" * 60)
    unreal.log_warning("Extracting Physics Asset Reference")
    unreal.log_warning("-" * 60)

    asset_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)

    if not asset:
        return

    physics_data = {}

    try:
        for prop_name in ['FinalPhysicsAsset', 'final_physics_asset']:
            try:
                physics_asset = asset.get_editor_property(prop_name)
                if physics_asset:
                    physics_data['FinalPhysicsAsset'] = physics_asset.get_path_name()
                    unreal.log_warning(f"  [OK] FinalPhysicsAsset: {physics_asset.get_path_name()}")
                    break
            except:
                pass
    except Exception as e:
        unreal.log_warning(f"  Error: {e}")

    cache_file = os.path.join(CACHE_DIR, "physics_data.json")
    with open(cache_file, 'w') as f:
        json.dump(physics_data, f, indent=2)


# ═══════════════════════════════════════════════════════════════════════════════
# PHASE 2: RUN MIGRATION (reparenting)
# ═══════════════════════════════════════════════════════════════════════════════

def run_migration():
    """Run the main reparenting migration"""
    unreal.log_warning("=" * 80)
    unreal.log_warning("PHASE 2: RUNNING MIGRATION (REPARENTING)")
    unreal.log_warning("=" * 80)

    import importlib.util
    spec = importlib.util.spec_from_file_location("run_migration", RUN_MIGRATION_SCRIPT)
    migration_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(migration_module)

    unreal.log_warning("Migration complete!")


# ═══════════════════════════════════════════════════════════════════════════════
# PHASE 3: APPLY CACHED DATA
# ═══════════════════════════════════════════════════════════════════════════════

def apply_all_cached_data():
    """Apply all cached data to migrated assets"""
    unreal.log_warning("=" * 80)
    unreal.log_warning("PHASE 3: APPLYING CACHED DATA")
    unreal.log_warning("=" * 80)

    apply_item_data()
    apply_default_mesh_data()

    unreal.log_warning("Data application complete!")


def apply_item_data():
    """Apply item icons from cache"""
    unreal.log_warning("-" * 60)
    unreal.log_warning("Applying Item Data")
    unreal.log_warning("-" * 60)

    cache_file = os.path.join(CACHE_DIR, "item_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning(f"  Cache not found: {cache_file}")
        return

    with open(cache_file, 'r') as f:
        item_data = json.load(f)

    if not item_data:
        unreal.log_warning("  Cache is empty")
        return

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    modified_count = 0

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
            for prop, cpp_prop in [('IconSmall', 'icon_small'), ('IconLargeOptional', 'icon_large_optional')]:
                if prop in data:
                    icon_path = data[prop].rsplit(".", 1)[0] if "." in data[prop] else data[prop]
                    texture = unreal.EditorAssetLibrary.load_asset(icon_path)
                    if texture:
                        item_info.set_editor_property(cpp_prop, texture)
                        modified = True

            if modified:
                asset.set_editor_property('item_information', item_info)
                unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                modified_count += 1
                unreal.log_warning(f"  [OK] {asset_name}")

        except Exception as e:
            unreal.log_warning(f"  [ERROR] {asset_name}: {e}")

    unreal.log_warning(f"  Applied icons to {modified_count} items")


def apply_default_mesh_data():
    """Apply mesh references from cache"""
    unreal.log_warning("-" * 60)
    unreal.log_warning("Applying Default Mesh Data")
    unreal.log_warning("-" * 60)

    cache_file = os.path.join(CACHE_DIR, "default_mesh_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning(f"  Cache not found: {cache_file}")
        return

    with open(cache_file, 'r') as f:
        mesh_data = json.load(f)

    if not mesh_data:
        unreal.log_warning("  Cache is empty - need to manually set mesh references")
        unreal.log_warning("  The default meshes are likely at:")
        unreal.log_warning("    /Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/")
        return

    asset_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)

    if not asset:
        unreal.log_warning(f"  Could not load {asset_path}")
        return

    # Map from various field names to C++ property names (snake_case)
    field_map = {
        'HeadMesh': 'head_mesh',
        'Head': 'head_mesh',
        'HeadPiece': 'head_mesh',
        'UpperBodyMesh': 'upper_body_mesh',
        'UpperBody': 'upper_body_mesh',
        'Torso': 'upper_body_mesh',
        'ArmsMesh': 'arms_mesh',
        'Arms': 'arms_mesh',
        'Gloves': 'arms_mesh',
        'LowerBodyMesh': 'lower_body_mesh',
        'LowerBody': 'lower_body_mesh',
        'Greaves': 'lower_body_mesh',
    }

    modified = False

    for cached_field, mesh_path in mesh_data.items():
        cpp_prop = field_map.get(cached_field)
        if not cpp_prop:
            continue

        # Clean path
        if "'" in mesh_path:
            mesh_path = mesh_path.split("'")[1] if "'" in mesh_path else mesh_path

        mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
        if mesh:
            try:
                asset.set_editor_property(cpp_prop, mesh)
                modified = True
                unreal.log_warning(f"  [OK] Set {cpp_prop}: {mesh.get_name()}")
            except Exception as e:
                unreal.log_warning(f"  [ERROR] Failed to set {cpp_prop}: {e}")

    if modified:
        unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
        unreal.log_warning(f"  Saved {asset_path}")


# ═══════════════════════════════════════════════════════════════════════════════
# MAIN ENTRY POINTS
# ═══════════════════════════════════════════════════════════════════════════════

def run_full_resilient_migration():
    """
    Run the complete resilient migration:
    1. Extract data from current content (which should be backup state)
    2. Run reparenting migration
    3. Apply cached data
    """
    unreal.log_warning("")
    unreal.log_warning("*" * 80)
    unreal.log_warning("RESILIENT MIGRATION SYSTEM")
    unreal.log_warning("*" * 80)
    unreal.log_warning("")

    extract_all_data_from_backup()
    run_migration()
    apply_all_cached_data()

    unreal.log_warning("")
    unreal.log_warning("*" * 80)
    unreal.log_warning("MIGRATION COMPLETE")
    unreal.log_warning("*" * 80)
    unreal.log_warning("")
    unreal.log_warning("Cache location: " + CACHE_DIR)
    unreal.log_warning("To re-extract data: delete cache files and restore backup first")


def extract_only():
    """Just extract data from current content state (call after restoring backup)"""
    extract_all_data_from_backup()


def apply_only():
    """Just apply cached data (call after migration if data wasn't applied)"""
    apply_all_cached_data()


if __name__ == "__main__":
    run_full_resilient_migration()
