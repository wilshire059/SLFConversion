"""
Bulk Struct Migration Script v2 - Uses export_text() for proper struct data extraction

FIXES from v1:
- Uses export_text() to extract struct data from loaded assets (not binary file parsing)
- Proper extraction of nested struct properties
"""
import unreal
import os
import re
import json
from datetime import datetime

# ============================================================================
# CONFIGURATION
# ============================================================================

RESULTS_PATH = "C:/scripts/slfconversion/bulk_migration_results.json"
LOG_PATH = "C:/scripts/slfconversion/bulk_migration_log.txt"

# Item data assets to migrate
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

# Mesh data assets
MESH_DATA_ASSETS = [
    "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData",
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

    # For Blueprints, get CDO from generated class
    if class_name == 'Blueprint':
        gen_class = asset.generated_class()
        if gen_class:
            return unreal.get_default_object(gen_class)

    # For PrimaryDataAssets and everything else, return directly
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

def extract_struct_data_via_export(asset):
    """Extract struct data from asset using export_text()"""
    data = {}
    try:
        export = unreal.EditorAssetLibrary.export_text(asset)
        if not export:
            return data

        # Extract WorldNiagaraSystem from ItemInformation struct
        niagara_match = re.search(r'WorldNiagaraSystem\s*=\s*([^\s,\)\}]+)', export)
        if niagara_match:
            path = niagara_match.group(1).strip()
            # Clean up path formats
            if "'" in path:
                # Format: NiagaraSystem'/Game/Path/To/Asset.Asset'
                path_match = re.search(r"'([^']+)'", path)
                if path_match:
                    path = path_match.group(1).split('.')[0]
            elif path.startswith('/Game/'):
                path = path.split('.')[0]
            if path and path != 'None' and path.startswith('/Game/'):
                data['WorldNiagaraSystem'] = path

        # Extract WorldStaticMesh
        static_match = re.search(r'WorldStaticMesh\s*=\s*([^\s,\)\}]+)', export)
        if static_match:
            path = static_match.group(1).strip()
            if "'" in path:
                path_match = re.search(r"'([^']+)'", path)
                if path_match:
                    path = path_match.group(1).split('.')[0]
            elif path.startswith('/Game/'):
                path = path.split('.')[0]
            if path and path != 'None' and path.startswith('/Game/'):
                data['WorldStaticMesh'] = path

        # Extract WorldSkeletalMesh
        skeletal_match = re.search(r'WorldSkeletalMesh\s*=\s*([^\s,\)\}]+)', export)
        if skeletal_match:
            path = skeletal_match.group(1).strip()
            if "'" in path:
                path_match = re.search(r"'([^']+)'", path)
                if path_match:
                    path = path_match.group(1).split('.')[0]
            elif path.startswith('/Game/'):
                path = path.split('.')[0]
            if path and path != 'None' and path.startswith('/Game/'):
                data['WorldSkeletalMesh'] = path

    except Exception as e:
        log(f"Error extracting struct data: {e}", "WARN")

    return data

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
            "struct_data": {},
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

            # Extract struct data via export_text
            raw_asset = eal.load_asset(asset_path)
            if raw_asset:
                struct_data = extract_struct_data_via_export(raw_asset)
                result["struct_data"] = struct_data
                log(f"  Struct data: {struct_data}")

        results[asset_name] = result

        if result["needs_migration"]:
            log(f"  {asset_name}: Needs migration for {result['needs_migration']}")
        else:
            log(f"  {asset_name}: All properties already set")

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
        raw_asset = eal.load_asset(asset_path)

        if not asset or not raw_asset:
            result["errors"].append("Could not load asset")
            results[asset_name] = result
            continue

        # Extract struct data from export
        struct_data = extract_struct_data_via_export(raw_asset)
        log(f"  Extracted struct data: {struct_data}")

        # Migrate WorldNiagaraSystem
        current_niagara = get_property_safe(asset, 'world_niagara_system')
        if not current_niagara or get_asset_path(current_niagara) in [None, "None", ""]:
            if 'WorldNiagaraSystem' in struct_data:
                path = struct_data['WorldNiagaraSystem']
                if eal.does_asset_exist(path):
                    niagara = eal.load_asset(path)
                    if niagara and set_property_safe(asset, 'world_niagara_system', niagara):
                        result["migrated"].append(f"world_niagara_system={path}")
                        log(f"  Migrated world_niagara_system: {path}")
                    else:
                        result["errors"].append(f"Failed to set world_niagara_system")
                else:
                    result["skipped"].append(f"world_niagara_system: asset not found {path}")
            else:
                result["skipped"].append("world_niagara_system: no struct data")
        else:
            result["skipped"].append("world_niagara_system: already set")

        # Migrate WorldStaticMesh
        current_static = get_property_safe(asset, 'world_static_mesh')
        if not current_static or get_asset_path(current_static) in [None, "None", ""]:
            if 'WorldStaticMesh' in struct_data:
                path = struct_data['WorldStaticMesh']
                if eal.does_asset_exist(path):
                    mesh = eal.load_asset(path)
                    if mesh and set_property_safe(asset, 'world_static_mesh', mesh):
                        result["migrated"].append(f"world_static_mesh={path}")
                        log(f"  Migrated world_static_mesh: {path}")
                    else:
                        result["errors"].append(f"Failed to set world_static_mesh")
                else:
                    result["skipped"].append(f"world_static_mesh: asset not found {path}")
            else:
                result["skipped"].append("world_static_mesh: no struct data")
        else:
            result["skipped"].append("world_static_mesh: already set")

        # Migrate WorldSkeletalMesh
        current_skeletal = get_property_safe(asset, 'world_skeletal_mesh')
        if not current_skeletal or get_asset_path(current_skeletal) in [None, "None", ""]:
            if 'WorldSkeletalMesh' in struct_data:
                path = struct_data['WorldSkeletalMesh']
                if eal.does_asset_exist(path):
                    mesh = eal.load_asset(path)
                    if mesh and set_property_safe(asset, 'world_skeletal_mesh', mesh):
                        result["migrated"].append(f"world_skeletal_mesh={path}")
                        log(f"  Migrated world_skeletal_mesh: {path}")
                    else:
                        result["errors"].append(f"Failed to set world_skeletal_mesh")
                else:
                    result["skipped"].append(f"world_skeletal_mesh: asset not found {path}")
            else:
                result["skipped"].append("world_skeletal_mesh: no struct data")
        else:
            result["skipped"].append("world_skeletal_mesh: already set")

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

    # Known mesh paths (already successfully migrated)
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

        status = "OK" if result["all_set"] else "PARTIAL"
        log(f"  {asset_name}: {status}")

        if result["all_set"]:
            for prop, path in result["properties"].items():
                log(f"    {prop}: {path}")

    return results

# ============================================================================
# MAIN EXECUTION
# ============================================================================

def main():
    log_section("BULK STRUCT MIGRATION v2")
    log(f"Started at {datetime.now().isoformat()}")

    all_results = {
        "pre_validation": {},
        "migration": {},
        "post_validation": {}
    }

    # Phase 1: Pre-validation
    log("\n" + "=" * 70)
    log("PHASE 1: PRE-VALIDATION")
    log("=" * 70)

    all_results["pre_validation"]["items"] = pre_validate_items()
    all_results["pre_validation"]["mesh_data"] = pre_validate_mesh_data()

    # Phase 2: Migration
    log("\n" + "=" * 70)
    log("PHASE 2: MIGRATION")
    log("=" * 70)

    all_results["migration"]["items"] = migrate_items()
    all_results["migration"]["mesh_data"] = migrate_mesh_data()

    # Phase 3: Post-validation
    log("\n" + "=" * 70)
    log("PHASE 3: POST-VALIDATION")
    log("=" * 70)

    all_results["post_validation"]["items"] = post_validate_items()
    all_results["post_validation"]["mesh_data"] = post_validate_mesh_data()

    # Summary
    log_section("PHASE 4: SUMMARY")

    item_complete = sum(1 for r in all_results["post_validation"]["items"].values() if r.get("all_set"))
    mesh_complete = sum(1 for r in all_results["post_validation"]["mesh_data"].values() if r.get("all_set"))

    log(f"Items complete: {item_complete}/{len(ITEM_ASSETS)}")
    log(f"Mesh data complete: {mesh_complete}/{len(MESH_DATA_ASSETS)}")

    # Save results
    with open(RESULTS_PATH, 'w') as f:
        json.dump(all_results, f, indent=2)
    log(f"Results saved to {RESULTS_PATH}")

    # Save log
    with open(LOG_PATH, 'w') as f:
        f.write('\n'.join(log_lines))

if __name__ == "__main__":
    main()
