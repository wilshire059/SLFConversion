"""
Bulk Struct Migration Script - FIXED for PrimaryDataAssets
Migrates all Blueprint struct data to C++ convenience properties with pre/post validation.

FIXES:
- PrimaryDataAssets are accessed directly (no generated_class())
- Blueprints use CDO via generated_class()
"""
import unreal
import os
import re
import json
from datetime import datetime

# ============================================================================
# CONFIGURATION
# ============================================================================

BACKUP_PATH = "C:/scripts/bp_only/Content/SoulslikeFramework"
RESULTS_PATH = "C:/scripts/slfconversion/bulk_migration_results.json"
LOG_PATH = "C:/scripts/slfconversion/bulk_migration_log.txt"

# Item data assets to migrate (PrimaryDataAssets - direct access)
ITEM_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Apple",
    "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
    "/Game/SoulslikeFramework/Data/Items/DA_CrimsonEverbloom",
    "/Game/SoulslikeFramework/Data/Items/DA_Cube",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor02",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleBracers",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleGreaves",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHat",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleTalisman",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Lantern",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
    "/Game/SoulslikeFramework/Data/Items/DA_PrisonKey",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
    "/Game/SoulslikeFramework/Data/Items/DA_ThrowingKnife",
]

# Mesh data assets (Blueprint - needs CDO)
MESH_DATA_ASSETS = [
    "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData",
]

# Action data assets (Blueprint - needs CDO)
ACTION_ASSETS = [
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge",
]

# ============================================================================
# LOGGING
# ============================================================================

log_lines = []

def log(msg, level="INFO"):
    timestamp = datetime.now().strftime("%H:%M:%S")
    line = f"[{timestamp}] [{level}] {msg}"
    log_lines.append(line)
    unreal.log(line)

def log_section(title):
    log("=" * 70)
    log(title)
    log("=" * 70)

# ============================================================================
# UTILITY FUNCTIONS
# ============================================================================

eal = unreal.EditorAssetLibrary

def get_asset_object(asset_path):
    """Get the asset object - handles both PrimaryDataAssets and Blueprints"""
    if not eal.does_asset_exist(asset_path):
        return None

    asset = eal.load_asset(asset_path)
    if not asset:
        return None

    class_name = asset.get_class().get_name()

    # PrimaryDataAssets (DA_*, etc.) - access directly
    if 'PrimaryDataAsset' in class_name or class_name.endswith('_C'):
        return asset

    # Check if it's a Blueprint
    if class_name == 'Blueprint':
        # Get the CDO from generated class
        gen_class = asset.generated_class()
        if gen_class:
            return unreal.get_default_object(gen_class)

    # Otherwise return the asset directly
    return asset

def get_property_safe(obj, prop_name):
    """Safely get a property value"""
    try:
        val = obj.get_editor_property(prop_name)
        return val
    except:
        return None

def set_property_safe(obj, prop_name, value):
    """Safely set a property value"""
    try:
        obj.set_editor_property(prop_name, value)
        return True
    except Exception as e:
        log(f"Failed to set {prop_name}: {e}", "ERROR")
        return False

def get_asset_path(soft_ref):
    """Extract path from soft reference"""
    if soft_ref is None:
        return None
    if hasattr(soft_ref, 'get_path_name'):
        path = soft_ref.get_path_name()
        if path and path != "None":
            return path
    if hasattr(soft_ref, 'asset_path_name'):
        return str(soft_ref.asset_path_name)
    return str(soft_ref) if soft_ref else None

def extract_path_from_backup(backup_file, property_pattern):
    """Extract asset path from backup .uasset using text export"""
    try:
        with open(backup_file, 'rb') as f:
            data = f.read()
        text = data.decode('utf-8', errors='ignore')
        match = re.search(rf'{property_pattern}["\s]*=\s*([^\s\n\r"]+\.uasset|/Game/[^\s\n\r"]+)', text)
        if match:
            path = match.group(1)
            path = path.replace('.uasset', '').strip()
            if path.startswith('/Game/'):
                return path
    except Exception as e:
        log(f"Error reading backup {backup_file}: {e}", "WARN")
    return None

# ============================================================================
# PHASE 1: PRE-VALIDATION
# ============================================================================

def pre_validate_items():
    """Pre-validate all item assets"""
    log_section("PHASE 1A: Pre-Validation - Item Assets")

    results = {}

    for asset_path in ITEM_ASSETS:
        asset_name = asset_path.split('/')[-1]
        log(f"Checking {asset_name}...")

        result = {
            "asset_path": asset_path,
            "exists": False,
            "cpp_properties": {},
            "needs_migration": []
        }

        asset = get_asset_object(asset_path)
        if asset:
            result["exists"] = True

            # Check C++ convenience properties
            for prop in ['world_niagara_system', 'world_static_mesh', 'world_skeletal_mesh']:
                val = get_property_safe(asset, prop)
                path = get_asset_path(val) if val else None
                result["cpp_properties"][prop] = path

                if not path or path == "None":
                    result["needs_migration"].append(prop)

        results[asset_name] = result

        if result["needs_migration"]:
            log(f"  {asset_name}: Needs migration for {result['needs_migration']}")
        else:
            log(f"  {asset_name}: All properties set")

    return results

def pre_validate_mesh_data():
    """Pre-validate mesh data assets"""
    log_section("PHASE 1B: Pre-Validation - Mesh Data Assets")

    results = {}

    for asset_path in MESH_DATA_ASSETS:
        asset_name = asset_path.split('/')[-1]
        log(f"Checking {asset_name}...")

        result = {
            "asset_path": asset_path,
            "exists": False,
            "cpp_properties": {},
            "needs_migration": []
        }

        asset = get_asset_object(asset_path)
        if asset:
            result["exists"] = True

            for prop in ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']:
                val = get_property_safe(asset, prop)
                path = get_asset_path(val) if val else None
                result["cpp_properties"][prop] = path

                if not path or path == "None":
                    result["needs_migration"].append(prop)

        results[asset_name] = result

        if result["needs_migration"]:
            log(f"  {asset_name}: Needs migration for {result['needs_migration']}")
        else:
            log(f"  {asset_name}: All properties already set")

    return results

# ============================================================================
# PHASE 2: MIGRATION
# ============================================================================

def migrate_items():
    """Migrate item struct data to C++ properties"""
    log_section("PHASE 2A: Migration - Item Assets")

    results = {}

    for asset_path in ITEM_ASSETS:
        asset_name = asset_path.split('/')[-1]
        log(f"Migrating {asset_name}...")

        result = {
            "migrated": [],
            "skipped": [],
            "errors": []
        }

        asset = get_asset_object(asset_path)
        if not asset:
            result["errors"].append("Could not load asset")
            results[asset_name] = result
            continue

        # Check WorldNiagaraSystem
        current_niagara = get_property_safe(asset, 'world_niagara_system')
        if not current_niagara or get_asset_path(current_niagara) in [None, "None", ""]:
            backup_file = f"{BACKUP_PATH}/Data/Items/{asset_name}.uasset"
            if os.path.exists(backup_file):
                path = extract_path_from_backup(backup_file, "WorldNiagaraSystem")
                if path:
                    if eal.does_asset_exist(path):
                        mesh = eal.load_asset(path)
                        if mesh and set_property_safe(asset, 'world_niagara_system', mesh):
                            result["migrated"].append(f"world_niagara_system={path}")
                            log(f"  Migrated world_niagara_system: {path}")
                        else:
                            result["errors"].append(f"Failed to set world_niagara_system")
                    else:
                        result["skipped"].append(f"world_niagara_system: asset not found {path}")
                else:
                    result["skipped"].append("world_niagara_system: no data in backup")
            else:
                result["skipped"].append("world_niagara_system: no backup file")
        else:
            result["skipped"].append("world_niagara_system: already set")

        # Check WorldStaticMesh and WorldSkeletalMesh
        for prop_name, backup_prop in [('world_static_mesh', 'WorldStaticMesh'),
                                        ('world_skeletal_mesh', 'WorldSkeletalMesh')]:
            current = get_property_safe(asset, prop_name)
            if not current or get_asset_path(current) in [None, "None", ""]:
                backup_file = f"{BACKUP_PATH}/Data/Items/{asset_name}.uasset"
                if os.path.exists(backup_file):
                    path = extract_path_from_backup(backup_file, backup_prop)
                    if path:
                        if eal.does_asset_exist(path):
                            mesh = eal.load_asset(path)
                            if mesh and set_property_safe(asset, prop_name, mesh):
                                result["migrated"].append(f"{prop_name}={path}")
                                log(f"  Migrated {prop_name}: {path}")
                            else:
                                result["errors"].append(f"Failed to set {prop_name}")
                        else:
                            result["skipped"].append(f"{prop_name}: asset not found {path}")
                    else:
                        result["skipped"].append(f"{prop_name}: no data in backup")
            else:
                result["skipped"].append(f"{prop_name}: already set")

        # Save asset
        try:
            eal.save_asset(asset_path, only_if_is_dirty=False)
            log(f"  Saved {asset_name}")
        except Exception as e:
            result["errors"].append(f"Save failed: {e}")

        results[asset_name] = result

    return results

def migrate_mesh_data():
    """Migrate mesh data struct to C++ properties"""
    log_section("PHASE 2B: Migration - Mesh Data Assets")

    results = {}

    MESH_PATHS = {
        'head_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_Head',
        'upper_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_Quinn_UpperBody',
        'arms_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_Quinn_Arms',
        'lower_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_LowerBody',
    }

    for asset_path in MESH_DATA_ASSETS:
        asset_name = asset_path.split('/')[-1]
        log(f"Migrating {asset_name}...")

        result = {
            "migrated": [],
            "skipped": [],
            "errors": []
        }

        asset = get_asset_object(asset_path)
        if not asset:
            result["errors"].append("Could not load asset")
            results[asset_name] = result
            continue

        for prop_name, mesh_path in MESH_PATHS.items():
            current = get_property_safe(asset, prop_name)
            if not current or get_asset_path(current) in [None, "None", ""]:
                if eal.does_asset_exist(mesh_path):
                    mesh = eal.load_asset(mesh_path)
                    if mesh and set_property_safe(asset, prop_name, mesh):
                        result["migrated"].append(f"{prop_name}={mesh_path}")
                        log(f"  Migrated {prop_name}: {mesh_path}")
                    else:
                        result["errors"].append(f"Failed to set {prop_name}")
                else:
                    result["errors"].append(f"{prop_name}: mesh not found {mesh_path}")
            else:
                result["skipped"].append(f"{prop_name}: already set")

        try:
            eal.save_asset(asset_path, only_if_is_dirty=False)
            log(f"  Saved {asset_name}")
        except Exception as e:
            result["errors"].append(f"Save failed: {e}")

        results[asset_name] = result

    return results

# ============================================================================
# PHASE 3: POST-VALIDATION
# ============================================================================

def post_validate_items():
    """Post-validate all item assets"""
    log_section("PHASE 3A: Post-Validation - Item Assets")

    results = {}
    success_count = 0
    total_props = 0

    for asset_path in ITEM_ASSETS:
        asset_name = asset_path.split('/')[-1]

        result = {
            "asset_path": asset_path,
            "properties": {},
            "all_set": True
        }

        asset = get_asset_object(asset_path)
        if asset:
            for prop in ['world_niagara_system', 'world_static_mesh', 'world_skeletal_mesh']:
                total_props += 1
                val = get_property_safe(asset, prop)
                path = get_asset_path(val) if val else None
                result["properties"][prop] = path

                if path and path != "None":
                    success_count += 1
                else:
                    result["all_set"] = False

        results[asset_name] = result
        status = "OK" if result["all_set"] else "PARTIAL"
        log(f"  {asset_name}: {status}")

    log(f"Items: {success_count}/{total_props} properties set")
    return results

def post_validate_mesh_data():
    """Post-validate mesh data assets"""
    log_section("PHASE 3B: Post-Validation - Mesh Data Assets")

    results = {}

    for asset_path in MESH_DATA_ASSETS:
        asset_name = asset_path.split('/')[-1]

        result = {
            "asset_path": asset_path,
            "properties": {},
            "all_set": True
        }

        asset = get_asset_object(asset_path)
        if asset:
            for prop in ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']:
                val = get_property_safe(asset, prop)
                path = get_asset_path(val) if val else None
                result["properties"][prop] = path

                if not path or path == "None":
                    result["all_set"] = False

        results[asset_name] = result
        status = "OK" if result["all_set"] else "INCOMPLETE"
        log(f"  {asset_name}: {status}")

        for prop, path in result["properties"].items():
            log(f"    {prop}: {path or 'NONE'}")

    return results

# ============================================================================
# MAIN EXECUTION
# ============================================================================

def main():
    log_section("BULK STRUCT MIGRATION")
    log(f"Started at {datetime.now().isoformat()}")

    all_results = {
        "timestamp": datetime.now().isoformat(),
        "phases": {}
    }

    # Phase 1: Pre-validation
    log("\n" + "=" * 70)
    log("PHASE 1: PRE-VALIDATION")
    log("=" * 70)

    all_results["phases"]["pre_validation"] = {
        "items": pre_validate_items(),
        "mesh_data": pre_validate_mesh_data()
    }

    # Phase 2: Migration
    log("\n" + "=" * 70)
    log("PHASE 2: MIGRATION")
    log("=" * 70)

    all_results["phases"]["migration"] = {
        "items": migrate_items(),
        "mesh_data": migrate_mesh_data()
    }

    # Phase 3: Post-validation
    log("\n" + "=" * 70)
    log("PHASE 3: POST-VALIDATION")
    log("=" * 70)

    all_results["phases"]["post_validation"] = {
        "items": post_validate_items(),
        "mesh_data": post_validate_mesh_data()
    }

    # Phase 4: Summary
    log_section("PHASE 4: SUMMARY")

    items_ok = sum(1 for r in all_results["phases"]["post_validation"]["items"].values() if r.get("all_set"))
    mesh_ok = sum(1 for r in all_results["phases"]["post_validation"]["mesh_data"].values() if r.get("all_set"))

    log(f"Items complete: {items_ok}/{len(ITEM_ASSETS)}")
    log(f"Mesh data complete: {mesh_ok}/{len(MESH_DATA_ASSETS)}")

    all_results["summary"] = {
        "items_total": len(ITEM_ASSETS),
        "items_complete": items_ok,
        "mesh_data_total": len(MESH_DATA_ASSETS),
        "mesh_data_complete": mesh_ok
    }

    # Save results
    try:
        with open(RESULTS_PATH, 'w') as f:
            json.dump(all_results, f, indent=2, default=str)
        log(f"Results saved to {RESULTS_PATH}")
    except Exception as e:
        log(f"Failed to save results: {e}", "ERROR")

    # Save log
    try:
        with open(LOG_PATH, 'w') as f:
            f.write('\n'.join(log_lines))
        log(f"Log saved to {LOG_PATH}")
    except Exception as e:
        log(f"Failed to save log: {e}", "ERROR")

    log_section("MIGRATION COMPLETE")

# Run
main()
