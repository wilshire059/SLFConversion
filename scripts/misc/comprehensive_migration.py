# comprehensive_migration.py
# Complete migration system that:
# 1. Extracts ALL data from Blueprint structs BEFORE reparenting
# 2. Performs reparenting to C++ classes
# 3. Applies extracted data to C++ properties AFTER reparenting
#
# This ensures no data is lost when restoring from backup.
#
# Usage:
# UnrealEditor-Cmd.exe SLFConversion.uproject -run=pythonscript -script="comprehensive_migration.py"

import unreal
import json
import os
import re

# ═══════════════════════════════════════════════════════════════════════════════
# CONFIGURATION
# ═══════════════════════════════════════════════════════════════════════════════

DATA_CACHE_DIR = "C:/scripts/SLFConversion/migration_cache"

# Ensure cache directory exists
os.makedirs(DATA_CACHE_DIR, exist_ok=True)

# ═══════════════════════════════════════════════════════════════════════════════
# PHASE 1: DATA EXTRACTION (Before Reparenting)
# ═══════════════════════════════════════════════════════════════════════════════

def extract_item_data():
    """Extract IconSmall, IconLargeOptional, WorldNiagaraSystem from PDA_Item assets"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 1A: Extracting Item Data (Icons, Niagara)")
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
            # Get ItemInformation struct and use export_text() on the struct
            item_info = None
            try:
                item_info = asset.get_editor_property('ItemInformation')
            except:
                pass
            if not item_info:
                try:
                    item_info = asset.get_editor_property('item_information')
                except:
                    pass

            if item_info and hasattr(item_info, 'export_text'):
                export_text = item_info.export_text()

                # Extract IconSmall
                icon_small_match = re.search(r'IconSmall[^=]*="([^"]+)"', export_text)
                if icon_small_match:
                    icon_path = icon_small_match.group(1)
                    if icon_path and icon_path != "None":
                        data['IconSmall'] = icon_path

                # Extract IconLargeOptional
                icon_large_match = re.search(r'IconLargeOptional[^=]*="([^"]+)"', export_text)
                if icon_large_match:
                    icon_path = icon_large_match.group(1)
                    if icon_path and icon_path != "None":
                        data['IconLargeOptional'] = icon_path

            # Also try to get WorldNiagaraSystem directly from the asset
            try:
                niagara = asset.get_editor_property('WorldNiagaraSystem')
                if niagara:
                    data['WorldNiagaraSystem'] = niagara.get_path_name()
            except:
                pass

            # Try alternate name
            try:
                niagara = asset.get_editor_property('world_niagara_system')
                if niagara:
                    data['WorldNiagaraSystem'] = niagara.get_path_name()
            except:
                pass

        except Exception as e:
            unreal.log_warning(f"  Error extracting {asset_name}: {e}")

        if data:
            item_data[asset_name] = data
            unreal.log_warning(f"  Extracted from {asset_name}: {list(data.keys())}")

    # Save to cache
    cache_file = os.path.join(DATA_CACHE_DIR, "item_data.json")
    with open(cache_file, 'w') as f:
        json.dump(item_data, f, indent=2)

    unreal.log_warning(f"Extracted data for {len(item_data)} items -> {cache_file}")
    return item_data


def extract_action_data():
    """Extract DodgeMontages from PDA_Action assets"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 1B: Extracting Action Data (Dodge Montages)")
    unreal.log_warning("=" * 60)

    actions_path = "/Game/SoulslikeFramework/Data/Actions"
    assets = unreal.EditorAssetLibrary.list_assets(actions_path, recursive=True, include_folder=False)

    action_data = {}

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            continue

        data = {}

        try:
            # Get DodgeMontages struct
            dodge_montages = None
            try:
                dodge_montages = asset.get_editor_property('DodgeMontages')
            except:
                pass
            if not dodge_montages:
                try:
                    dodge_montages = asset.get_editor_property('dodge_montages')
                except:
                    pass

            if dodge_montages and hasattr(dodge_montages, 'export_text'):
                export_text = dodge_montages.export_text()

                # Extract DodgeMontages struct fields
                dodge_fields = ['Forward', 'ForwardLeft', 'ForwardRight', 'Left', 'Right',
                               'Backward', 'BackwardLeft', 'BackwardRight', 'Backstep']

                for field in dodge_fields:
                    # Match pattern like: Forward="/Game/..."
                    pattern = rf'{field}[^=]*="([^"]+)"'
                    match = re.search(pattern, export_text)
                    if match:
                        value = match.group(1)
                        if value and value != "None":
                            if 'DodgeMontages' not in data:
                                data['DodgeMontages'] = {}
                            data['DodgeMontages'][field] = value

        except Exception as e:
            unreal.log_warning(f"  Error extracting {asset_name}: {e}")

        if data:
            action_data[asset_name] = data
            unreal.log_warning(f"  Extracted from {asset_name}: {list(data.get('DodgeMontages', {}).keys())}")

    # Save to cache
    cache_file = os.path.join(DATA_CACHE_DIR, "action_data.json")
    with open(cache_file, 'w') as f:
        json.dump(action_data, f, indent=2)

    unreal.log_warning(f"Extracted data for {len(action_data)} actions -> {cache_file}")
    return action_data


def extract_default_mesh_data():
    """Extract mesh references from PDA_DefaultMeshData"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 1C: Extracting Default Mesh Data")
    unreal.log_warning("=" * 60)

    asset_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        unreal.log_warning(f"  Could not load {asset_path}")
        return {}

    mesh_data = {}

    try:
        # Try to get DefaultMeshes struct
        default_meshes = None
        try:
            default_meshes = asset.get_editor_property('DefaultMeshes')
        except:
            pass
        if not default_meshes:
            try:
                default_meshes = asset.get_editor_property('default_meshes')
            except:
                pass

        if default_meshes and hasattr(default_meshes, 'export_text'):
            export_text = default_meshes.export_text()

            mesh_fields = ['HeadMesh', 'UpperBodyMesh', 'ArmsMesh', 'LowerBodyMesh']

            for field in mesh_fields:
                pattern = rf'{field}[^=]*="([^"]+)"'
                match = re.search(pattern, export_text)
                if match:
                    value = match.group(1)
                    if value and value != "None":
                        mesh_data[field] = value
                        unreal.log_warning(f"  {field}: {value}")

        # Also try direct properties
        if not mesh_data:
            for bp_name, cpp_name in [('HeadMesh', 'head_mesh'), ('UpperBodyMesh', 'upper_body_mesh'),
                                       ('ArmsMesh', 'arms_mesh'), ('LowerBodyMesh', 'lower_body_mesh')]:
                try:
                    mesh = asset.get_editor_property(bp_name)
                    if mesh:
                        mesh_data[bp_name] = mesh.get_path_name()
                        unreal.log_warning(f"  {bp_name}: {mesh.get_path_name()}")
                except:
                    try:
                        mesh = asset.get_editor_property(cpp_name)
                        if mesh:
                            mesh_data[bp_name] = mesh.get_path_name()
                            unreal.log_warning(f"  {bp_name}: {mesh.get_path_name()}")
                    except:
                        pass

    except Exception as e:
        unreal.log_warning(f"  Error extracting mesh data: {e}")

    # Save to cache
    cache_file = os.path.join(DATA_CACHE_DIR, "default_mesh_data.json")
    with open(cache_file, 'w') as f:
        json.dump(mesh_data, f, indent=2)

    unreal.log_warning(f"Extracted {len(mesh_data)} mesh references -> {cache_file}")
    return mesh_data


def extract_category_data():
    """Extract category icon textures from InventoryManager"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 1D: Extracting Category Icon Data")
    unreal.log_warning("=" * 60)

    # Categories are typically defined in AC_InventoryManager or a DataTable
    # For now, search for category textures in the project

    category_data = {}

    # Common category icon paths based on typical SLF setup
    textures_path = "/Game/SoulslikeFramework/Textures"
    if unreal.EditorAssetLibrary.does_directory_exist(textures_path):
        assets = unreal.EditorAssetLibrary.list_assets(textures_path, recursive=True, include_folder=False)
        for asset_path in assets:
            asset_name = asset_path.split("/")[-1].split(".")[0]
            if "Category" in asset_name or "Icon" in asset_name:
                category_data[asset_name] = asset_path.split(".")[0]
                unreal.log_warning(f"  Found: {asset_name}")

    # Save to cache
    cache_file = os.path.join(DATA_CACHE_DIR, "category_data.json")
    with open(cache_file, 'w') as f:
        json.dump(category_data, f, indent=2)

    unreal.log_warning(f"Found {len(category_data)} category-related textures")
    return category_data


# ═══════════════════════════════════════════════════════════════════════════════
# PHASE 2: REPARENTING (from run_migration.py)
# ═══════════════════════════════════════════════════════════════════════════════

# Import the existing migration maps
PRIORITY_ANIM_NOTIFY_STATES = {
    "ANS_InputBuffer": "/Script/SLFConversion.ANS_InputBuffer",
    "ANS_RegisterAttackSequence": "/Script/SLFConversion.ANS_RegisterAttackSequence",
    "ANS_InvincibilityFrame": "/Script/SLFConversion.ANS_InvincibilityFrame",
    "ANS_HyperArmor": "/Script/SLFConversion.ANS_HyperArmor",
}

ANIM_BP_MAP = {
    "ABP_SoulslikeNPC": "/Script/SLFConversion.SLFNPCAnimInstance",
    "ABP_SoulslikeEnemy": "/Script/SLFConversion.SLFEnemyAnimInstance",
    "ABP_SoulslikeBossNew": "/Script/SLFConversion.SLFBossAnimInstance",
}

NPC_CHARACTER_MAP = {
    "B_Soulslike_NPC": "/Script/SLFConversion.SLFNPCCharacter",
    "ShowcaseGuide": "/Script/SLFConversion.SLFNPCCharacter",
    "ShowcaseVendor": "/Script/SLFConversion.SLFNPCCharacter",
}

DATA_ASSET_MAP = {
    "PDA_Dialog": "/Script/SLFConversion.PDA_Dialog",
    "PDA_DefaultMeshData": "/Script/SLFConversion.PDA_DefaultMeshData",
    "PDA_Action": "/Script/SLFConversion.PDA_Action",
    "PDA_Item": "/Script/SLFConversion.PDA_Item",
}

# Main migration map (abbreviated - full version in run_migration.py)
MIGRATION_MAP = {
    # Components
    "AC_ActionManager": "/Script/SLFConversion.AC_ActionManager",
    "AC_CombatManager": "/Script/SLFConversion.AC_CombatManager",
    "AC_EquipmentManager": "/Script/SLFConversion.AC_EquipmentManager",
    "AC_InputBuffer": "/Script/SLFConversion.InputBufferComponent",
    "AC_InteractionManager": "/Script/SLFConversion.AC_InteractionManager",
    "AC_InventoryManager": "/Script/SLFConversion.AC_InventoryManager",
    "AC_ProgressManager": "/Script/SLFConversion.AC_ProgressManager",
    "AC_SaveLoadManager": "/Script/SLFConversion.SaveLoadManagerComponent",
    "AC_StatManager": "/Script/SLFConversion.AC_StatManager",
    "AC_AI_Boss": "/Script/SLFConversion.AC_AI_Boss",
    "AC_AI_CombatManager": "/Script/SLFConversion.AC_AI_CombatManager",

    # Characters
    "B_Soulslike_Character": "/Script/SLFConversion.SLFSoulslikeCharacter",
    "B_Soulslike_Enemy": "/Script/SLFConversion.SLFEnemyCharacter",

    # World Actors
    "B_Interactable": "/Script/SLFConversion.B_Interactable",
    "B_PickupItem": "/Script/SLFConversion.SLFPickupItemBase",
    "B_RestingPoint": "/Script/SLFConversion.SLFRestingPointBase",

    # Game Framework
    "GI_Soulslike": "/Script/SLFConversion.SLFGameInstance",
    "GM_Soulslike": "/Script/SLFConversion.SLFGameMode",
    "PC_SoulslikeFramework": "/Script/SLFConversion.PC_SoulslikeFramework",
    "PS_Soulslike": "/Script/SLFConversion.SLFPlayerState",

    # Controllers
    "AIC_SoulslikeFramework": "/Script/SLFConversion.AIC_SoulslikeFramework",
}


def run_reparenting():
    """Execute the reparenting phase"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 2: REPARENTING BLUEPRINTS")
    unreal.log_warning("=" * 60)

    # Import and run the main migration script
    import importlib.util
    spec = importlib.util.spec_from_file_location("run_migration", "C:/scripts/SLFConversion/run_migration.py")
    migration_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(migration_module)

    unreal.log_warning("Reparenting complete")


# ═══════════════════════════════════════════════════════════════════════════════
# PHASE 3: DATA APPLICATION (After Reparenting)
# ═══════════════════════════════════════════════════════════════════════════════

def apply_item_data():
    """Apply IconSmall, IconLargeOptional, WorldNiagaraSystem to reparented PDA_Item assets"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 3A: Applying Item Data")
    unreal.log_warning("=" * 60)

    cache_file = os.path.join(DATA_CACHE_DIR, "item_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning(f"  Cache file not found: {cache_file}")
        return

    with open(cache_file, 'r') as f:
        item_data = json.load(f)

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
            unreal.log_warning(f"  [SKIP] {asset_name} - could not load")
            continue

        try:
            # Try both naming conventions for the struct property
            item_info = None
            try:
                item_info = asset.get_editor_property('item_information')
            except:
                pass
            if item_info is None:
                try:
                    item_info = asset.get_editor_property('ItemInformation')
                except:
                    pass

            if item_info is None:
                unreal.log_warning(f"  [SKIP] {asset_name} - no item_information property (class: {asset.get_class().get_name()})")
                continue

            modified = False

            # Apply IconSmall - try both naming conventions
            if 'IconSmall' in data:
                icon_path = data['IconSmall']
                # Clean path - remove .ClassName suffix if present
                if "." in icon_path:
                    parts = icon_path.rsplit(".", 1)
                    if "/" in parts[0] and parts[0].endswith("/" + parts[1].split(".")[0] if "." in parts[1] else parts[1]):
                        icon_path = parts[0]
                    else:
                        icon_path = parts[0]  # Take everything before last dot

                texture = unreal.load_asset(icon_path)
                if texture:
                    try:
                        item_info.set_editor_property('icon_small', texture)
                        modified = True
                    except:
                        try:
                            item_info.set_editor_property('IconSmall', texture)
                            modified = True
                        except Exception as e:
                            unreal.log_warning(f"    Failed to set IconSmall: {e}")

            # Apply IconLargeOptional
            if 'IconLargeOptional' in data:
                icon_path = data['IconLargeOptional']
                if "." in icon_path:
                    parts = icon_path.rsplit(".", 1)
                    icon_path = parts[0]

                texture = unreal.load_asset(icon_path)
                if texture:
                    try:
                        item_info.set_editor_property('icon_large_optional', texture)
                        modified = True
                    except:
                        try:
                            item_info.set_editor_property('IconLargeOptional', texture)
                            modified = True
                        except Exception as e:
                            unreal.log_warning(f"    Failed to set IconLargeOptional: {e}")

            # Apply WorldNiagaraSystem to asset directly
            if 'WorldNiagaraSystem' in data:
                niagara_path = data['WorldNiagaraSystem']
                try:
                    niagara_system = unreal.load_asset(niagara_path)
                    if niagara_system:
                        try:
                            asset.set_editor_property('world_niagara_system', niagara_system)
                            modified = True
                        except:
                            try:
                                asset.set_editor_property('WorldNiagaraSystem', niagara_system)
                                modified = True
                            except:
                                pass
                except:
                    pass

            if modified:
                try:
                    asset.set_editor_property('item_information', item_info)
                except:
                    try:
                        asset.set_editor_property('ItemInformation', item_info)
                    except:
                        pass
                unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                modified_count += 1
                unreal.log_warning(f"  [OK] {asset_name}")

        except Exception as e:
            unreal.log_warning(f"  [ERROR] {asset_name}: {e}")
            import traceback
            traceback.print_exc()

    unreal.log_warning(f"Applied data to {modified_count} items")


def apply_action_data():
    """Apply DodgeMontages to reparented PDA_Action assets"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 3B: Applying Action Data (Dodge Montages)")
    unreal.log_warning("=" * 60)

    cache_file = os.path.join(DATA_CACHE_DIR, "action_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning(f"  Cache file not found: {cache_file}")
        return

    with open(cache_file, 'r') as f:
        action_data = json.load(f)

    actions_path = "/Game/SoulslikeFramework/Data/Actions"
    assets = unreal.EditorAssetLibrary.list_assets(actions_path, recursive=True, include_folder=False)

    modified_count = 0

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if asset_name not in action_data:
            continue

        data = action_data[asset_name]
        if 'DodgeMontages' not in data:
            continue

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            continue

        try:
            # Get the C++ DodgeMontages struct
            dodge_montages = asset.get_editor_property('dodge_montages')
            if dodge_montages is None:
                unreal.log_warning(f"  [SKIP] {asset_name} - no dodge_montages property")
                continue

            modified = False

            for field, montage_path in data['DodgeMontages'].items():
                if "'" in montage_path:
                    montage_path = montage_path.split("'")[0]
                montage = unreal.load_asset(montage_path)
                if montage:
                    # Convert field name to snake_case for C++ property
                    prop_name = field.lower()
                    if field == 'ForwardLeft':
                        prop_name = 'forward_left'
                    elif field == 'ForwardRight':
                        prop_name = 'forward_right'
                    elif field == 'BackwardLeft':
                        prop_name = 'backward_left'
                    elif field == 'BackwardRight':
                        prop_name = 'backward_right'

                    try:
                        dodge_montages.set_editor_property(prop_name, montage)
                        modified = True
                    except:
                        pass

            if modified:
                asset.set_editor_property('dodge_montages', dodge_montages)
                unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                modified_count += 1
                unreal.log_warning(f"  [OK] {asset_name}")

        except Exception as e:
            unreal.log_warning(f"  [ERROR] {asset_name}: {e}")

    unreal.log_warning(f"Applied dodge montages to {modified_count} actions")


def apply_default_mesh_data():
    """Apply mesh references to reparented PDA_DefaultMeshData"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("PHASE 3C: Applying Default Mesh Data")
    unreal.log_warning("=" * 60)

    cache_file = os.path.join(DATA_CACHE_DIR, "default_mesh_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning(f"  Cache file not found: {cache_file}")
        return

    with open(cache_file, 'r') as f:
        mesh_data = json.load(f)

    asset_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        unreal.log_warning(f"  Could not load {asset_path}")
        return

    modified = False

    field_map = {
        'HeadMesh': 'head_mesh',
        'UpperBodyMesh': 'upper_body_mesh',
        'ArmsMesh': 'arms_mesh',
        'LowerBodyMesh': 'lower_body_mesh',
    }

    for bp_field, cpp_field in field_map.items():
        if bp_field in mesh_data:
            mesh_path = mesh_data[bp_field]
            if "'" in mesh_path:
                mesh_path = mesh_path.split("'")[0]
            mesh = unreal.load_asset(mesh_path)
            if mesh:
                try:
                    asset.set_editor_property(cpp_field, mesh)
                    modified = True
                    unreal.log_warning(f"  Set {cpp_field}: {mesh.get_name()}")
                except Exception as e:
                    unreal.log_warning(f"  Failed to set {cpp_field}: {e}")

    if modified:
        unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
        unreal.log_warning(f"  Saved {asset_path}")


# ═══════════════════════════════════════════════════════════════════════════════
# MAIN ENTRY POINT
# ═══════════════════════════════════════════════════════════════════════════════

def cache_has_data(filename):
    """Check if a cache file exists and has data"""
    cache_file = os.path.join(DATA_CACHE_DIR, filename)
    if not os.path.exists(cache_file):
        return False
    try:
        with open(cache_file, 'r') as f:
            data = json.load(f)
            return len(data) > 0
    except:
        return False


def run_comprehensive_migration():
    """Run the complete migration process"""
    unreal.log_warning("")
    unreal.log_warning("=" * 80)
    unreal.log_warning("COMPREHENSIVE MIGRATION - START")
    unreal.log_warning("=" * 80)
    unreal.log_warning("")

    # PHASE 1: Extract all data BEFORE reparenting
    # ONLY extract if cache doesn't exist or is empty
    unreal.log_warning(">>> PHASE 1: DATA EXTRACTION <<<")

    if cache_has_data("item_data.json"):
        unreal.log_warning("  item_data.json already has data - SKIPPING extraction")
    else:
        extract_item_data()

    if cache_has_data("action_data.json"):
        unreal.log_warning("  action_data.json already has data - SKIPPING extraction")
    else:
        extract_action_data()

    if cache_has_data("default_mesh_data.json"):
        unreal.log_warning("  default_mesh_data.json already has data - SKIPPING extraction")
    else:
        extract_default_mesh_data()

    extract_category_data()  # This one is fast and doesn't overwrite important data

    # PHASE 2: Run reparenting
    unreal.log_warning("")
    unreal.log_warning(">>> PHASE 2: REPARENTING <<<")
    run_reparenting()

    # PHASE 3: Apply extracted data
    unreal.log_warning("")
    unreal.log_warning(">>> PHASE 3: DATA APPLICATION <<<")
    apply_item_data()
    apply_action_data()
    apply_default_mesh_data()

    unreal.log_warning("")
    unreal.log_warning("=" * 80)
    unreal.log_warning("COMPREHENSIVE MIGRATION - COMPLETE")
    unreal.log_warning("=" * 80)
    unreal.log_warning("")
    unreal.log_warning("Data cache saved to: " + DATA_CACHE_DIR)
    unreal.log_warning("To force re-extraction, delete files in: " + DATA_CACHE_DIR)


if __name__ == "__main__":
    run_comprehensive_migration()
