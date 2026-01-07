"""
Bulk Struct Migration Script v3 - Uses pre-extracted JSON data

The struct data was previously extracted when items were still in Blueprint form.
Now we use that pre-extracted data to set the C++ convenience properties.
"""
import unreal
import json
from datetime import datetime

# ============================================================================
# CONFIGURATION
# ============================================================================

# Pre-extracted data files
NIAGARA_DATA_FILE = "C:/scripts/SLFConversion/niagara_paths.json"

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

def load_pre_extracted_data():
    """Load pre-extracted niagara paths from JSON"""
    try:
        with open(NIAGARA_DATA_FILE, 'r') as f:
            data = json.load(f)
        log(f"Loaded {len(data)} pre-extracted niagara paths")
        return data
    except Exception as e:
        log(f"Failed to load {NIAGARA_DATA_FILE}: {e}", "ERROR")
        return {}

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

        asset = eal.load_asset(asset_path)
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

        # For Blueprints, get CDO
        bp = eal.load_asset(asset_path)
        if bp and bp.get_class().get_name() == 'Blueprint':
            gen_class = bp.generated_class()
            if gen_class:
                asset = unreal.get_default_object(gen_class)
            else:
                asset = None
        else:
            asset = bp

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

def migrate_items(niagara_data):
    """Migrate item struct data to C++ properties using pre-extracted data"""
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

        asset = eal.load_asset(asset_path)
        if not asset:
            result["errors"].append("Could not load asset")
            results[asset_name] = result
            continue

        # Migrate WorldNiagaraSystem using pre-extracted data
        current_niagara = get_property_safe(asset, 'world_niagara_system')
        if not current_niagara or get_asset_path(current_niagara) in [None, "None", ""]:
            if asset_name in niagara_data:
                niagara_full_path = niagara_data[asset_name]
                # Extract just the asset path (remove .ClassName suffix)
                niagara_path = niagara_full_path.split('.')[0]

                if eal.does_asset_exist(niagara_path):
                    niagara = eal.load_asset(niagara_path)
                    if niagara and set_property_safe(asset, 'world_niagara_system', niagara):
                        result["migrated"].append(f"world_niagara_system={niagara_path}")
                        log(f"  Migrated world_niagara_system: {niagara_path}")
                    else:
                        result["errors"].append(f"Failed to set world_niagara_system")
                else:
                    result["errors"].append(f"world_niagara_system: asset not found {niagara_path}")
            else:
                result["skipped"].append("world_niagara_system: no pre-extracted data")
        else:
            result["skipped"].append("world_niagara_system: already set")

        # world_static_mesh and world_skeletal_mesh - skip for now (no pre-extracted data)
        result["skipped"].append("world_static_mesh: no source data (items don't use static meshes)")
        result["skipped"].append("world_skeletal_mesh: no source data (items don't use skeletal meshes)")

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

        # For Blueprints, get CDO
        bp = eal.load_asset(asset_path)
        if bp and bp.get_class().get_name() == 'Blueprint':
            gen_class = bp.generated_class()
            if gen_class:
                asset = unreal.get_default_object(gen_class)
            else:
                asset = None
        else:
            asset = bp

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
    niagara_count = 0
    total_niagara = 0

    for asset_path in ITEM_ASSETS:
        asset_name = asset_path.split('/')[-1]

        result = {
            "asset_path": asset_path,
            "properties": {},
            "niagara_set": False
        }

        asset = eal.load_asset(asset_path)
        if asset:
            total_niagara += 1

            # Check world_niagara_system
            val = get_property_safe(asset, 'world_niagara_system')
            path = get_asset_path(val) if val else None
            result["properties"]["world_niagara_system"] = path

            if path and path != "None":
                niagara_count += 1
                result["niagara_set"] = True

        results[asset_name] = result

        status = "OK" if result["niagara_set"] else "MISSING"
        log(f"  {asset_name}: {status}")
        if result["niagara_set"]:
            log(f"    world_niagara_system: {result['properties']['world_niagara_system']}")

    log(f"Items with niagara: {niagara_count}/{total_niagara}")
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

        # For Blueprints, get CDO
        bp = eal.load_asset(asset_path)
        if bp and bp.get_class().get_name() == 'Blueprint':
            gen_class = bp.generated_class()
            if gen_class:
                asset = unreal.get_default_object(gen_class)
            else:
                asset = None
        else:
            asset = bp

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
    log_section("BULK STRUCT MIGRATION v3")
    log(f"Started at {datetime.now().isoformat()}")

    # Load pre-extracted data
    niagara_data = load_pre_extracted_data()

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

    all_results["migration"]["items"] = migrate_items(niagara_data)
    all_results["migration"]["mesh_data"] = migrate_mesh_data()

    # Phase 3: Post-validation
    log("\n" + "=" * 70)
    log("PHASE 3: POST-VALIDATION")
    log("=" * 70)

    all_results["post_validation"]["items"] = post_validate_items()
    all_results["post_validation"]["mesh_data"] = post_validate_mesh_data()

    # Summary
    log_section("PHASE 4: SUMMARY")

    item_with_niagara = sum(1 for r in all_results["post_validation"]["items"].values() if r.get("niagara_set"))
    mesh_complete = sum(1 for r in all_results["post_validation"]["mesh_data"].values() if r.get("all_set"))

    log(f"Items with niagara set: {item_with_niagara}/{len(ITEM_ASSETS)}")
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
