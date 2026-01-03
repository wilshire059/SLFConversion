# direct_struct_migration.py
# ALTERNATIVE APPROACH: Direct StructType modification
#
# This script DIRECTLY sets each node's StructType property to point to C++ types.
# Unlike the previous approach, this does NOT rely on CoreRedirects.
#
# CRITICAL: This must be run BEFORE deleting Blueprint struct assets!
# The BP structs need to exist so we can match nodes that reference them.
#
# WORKFLOW:
# 1. Run this script to migrate all struct nodes to C++ types
# 2. Verify all Blueprints compile without errors
# 3. THEN delete the Blueprint struct/enum assets
# 4. Reparent AC_StatManager to USoulslikeStatComponent
#
# Run in Unreal Editor Python console:
# exec(open(r"C:\scripts\SLFConversion\direct_struct_migration.py").read())

import unreal
import os
import json
import glob

# Configuration
JSON_EXPORTS_PATH = r"C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA"

# Struct paths mapping (Blueprint path -> C++ path)
# NOTE: C++ struct paths use the struct name without F prefix
STRUCT_MIGRATIONS = {
    "/Game/SoulslikeFramework/Structures/Stats/FRegen.FRegen": "/Script/SLFConversion.Regen",
    "/Game/SoulslikeFramework/Structures/Stats/FStatBehavior.FStatBehavior": "/Script/SLFConversion.StatBehavior",
    "/Game/SoulslikeFramework/Structures/Stats/FStatInfo.FStatInfo": "/Script/SLFConversion.StatInfo",
    "/Game/SoulslikeFramework/Structures/Stats/FStatOverride.FStatOverride": "/Script/SLFConversion.StatOverride",
}

# Enum migrations (for future implementation)
ENUM_MIGRATIONS = {
    "/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType": "/Script/SLFConversion.E_ValueType",
}

# Blueprint struct/enum assets to delete (AFTER migration is complete)
ASSETS_TO_DELETE = [
    "/Game/SoulslikeFramework/Structures/Stats/FRegen",
    "/Game/SoulslikeFramework/Structures/Stats/FStatBehavior",
    "/Game/SoulslikeFramework/Structures/Stats/FStatInfo",
    "/Game/SoulslikeFramework/Structures/Stats/FStatOverride",
    "/Game/SoulslikeFramework/Enums/E_ValueType",
]


def find_affected_blueprints():
    """
    Parse JSON exports to find all Blueprints that reference the structs/enums being migrated.
    Returns a dict: { blueprint_path: [list of struct paths used] }
    """
    affected = {}

    search_terms = list(STRUCT_MIGRATIONS.keys()) + list(ENUM_MIGRATIONS.keys())

    # Also search for simpler forms
    search_terms.extend([
        "FStatInfo", "FRegen", "FStatBehavior", "FStatOverride", "E_ValueType"
    ])

    print("\n[SCAN] Searching JSON exports for struct/enum references...")

    json_files = glob.glob(os.path.join(JSON_EXPORTS_PATH, "**", "*.json"), recursive=True)
    print(f"[SCAN] Found {len(json_files)} JSON files to scan")

    for json_file in json_files:
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                content = f.read()

            # Quick string search first (faster than parsing JSON)
            found_terms = [term for term in search_terms if term in content]

            if found_terms:
                # Parse JSON to get the Blueprint path
                data = json.loads(content)
                bp_path = data.get("Path", "")

                if bp_path:
                    # Convert to UE asset path format
                    asset_path = bp_path.rsplit('.', 1)[0] if '.' in bp_path else bp_path

                    if asset_path not in affected:
                        affected[asset_path] = []
                    affected[asset_path].extend(found_terms)

        except Exception as e:
            pass  # Skip files that can't be parsed

    print(f"[SCAN] Found {len(affected)} affected Blueprints:")
    for bp_path, terms in affected.items():
        unique_terms = list(set(terms))
        print(f"  - {bp_path.split('/')[-1]}: {len(unique_terms)} struct refs")

    return affected


def verify_cpp_structs():
    """
    Verify that all C++ structs can be loaded.
    Returns True if all structs are available.
    """
    print("\n" + "=" * 60)
    print("VERIFICATION: Check C++ Struct Availability")
    print("=" * 60)

    all_available = True

    for old_path, new_path in STRUCT_MIGRATIONS.items():
        struct_name = new_path.split('.')[-1]
        try:
            # Try to find the struct
            struct = unreal.find_object(None, new_path)
            if struct:
                print(f"[OK] {struct_name} -> {new_path}")
            else:
                print(f"[MISSING] {struct_name} at {new_path}")
                all_available = False
        except Exception as e:
            print(f"[ERROR] {struct_name}: {e}")
            all_available = False

    return all_available


def verify_bp_structs():
    """
    Verify that Blueprint structs still exist (haven't been deleted yet).
    Returns True if all BP structs are still present.
    """
    print("\n" + "=" * 60)
    print("VERIFICATION: Check Blueprint Struct Availability")
    print("=" * 60)

    all_present = True

    for old_path in STRUCT_MIGRATIONS.keys():
        # Extract asset path (remove .StructName suffix)
        asset_path = old_path.rsplit('.', 1)[0]

        if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            print(f"[OK] {asset_path}")
        else:
            print(f"[MISSING] {asset_path} - Already deleted!")
            all_present = False

    if not all_present:
        print("\n[ERROR] Some Blueprint structs are missing!")
        print("Please restore from backup: C:\\scripts\\SLFConversion_Migration\\Backups\\blueprint_only\\Content")

    return all_present


def migrate_struct_nodes_in_blueprint(bp_path):
    """
    Migrate all struct nodes in a single Blueprint using MigrateStructType.
    Returns the total number of nodes migrated.
    """
    print(f"\n[MIGRATE] {bp_path}")

    try:
        automation_lib = unreal.SLFAutomationLibrary
    except AttributeError:
        print("[ERROR] SLFAutomationLibrary not found. Is C++ compiled?")
        return 0

    if not hasattr(automation_lib, 'migrate_struct_type'):
        print("[ERROR] migrate_struct_type not found. Recompile C++ with latest changes.")
        return 0

    try:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            print(f"  -> Could not load asset")
            return 0

        total_migrated = 0

        # Migrate each struct type
        for old_path, new_path in STRUCT_MIGRATIONS.items():
            count = automation_lib.migrate_struct_type(bp, old_path, new_path)
            if count > 0:
                print(f"  -> {old_path.split('/')[-1]}: {count} nodes migrated")
                total_migrated += count

        if total_migrated > 0:
            # Save the Blueprint
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
            print(f"  -> Saved ({total_migrated} total nodes)")
        else:
            print(f"  -> No struct nodes found")

        return total_migrated

    except Exception as e:
        print(f"  -> Error: {e}")
        return 0


def migrate_all_blueprints(affected_blueprints):
    """
    Migrate struct nodes in all affected Blueprints.
    """
    print("\n" + "=" * 60)
    print("PHASE 1: Migrate Struct Nodes (Direct StructType Modification)")
    print("=" * 60)

    total_migrated = 0
    failed_blueprints = []
    successful_blueprints = []

    for bp_path in affected_blueprints.keys():
        try:
            count = migrate_struct_nodes_in_blueprint(bp_path)
            total_migrated += count
            if count > 0:
                successful_blueprints.append(bp_path)
        except Exception as e:
            print(f"  -> Error: {e}")
            failed_blueprints.append(bp_path)

    print(f"\n[PHASE 1 COMPLETE]")
    print(f"  Total nodes migrated: {total_migrated}")
    print(f"  Successful Blueprints: {len(successful_blueprints)}")
    print(f"  Failed Blueprints: {len(failed_blueprints)}")

    if failed_blueprints:
        print("\nFailed Blueprints:")
        for bp in failed_blueprints:
            print(f"  - {bp}")

    return total_migrated, len(failed_blueprints) == 0


def delete_blueprint_assets():
    """
    Delete Blueprint UserDefinedStruct/Enum assets.
    ONLY call this AFTER successful migration!
    """
    print("\n" + "=" * 60)
    print("PHASE 2: Delete Blueprint Struct/Enum Assets")
    print("=" * 60)
    print("\n[WARNING] This will permanently delete Blueprint structs!")
    print("[WARNING] Only proceed if Phase 1 was successful!")

    deleted_count = 0

    for asset_path in ASSETS_TO_DELETE:
        if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            print(f"[DELETE] {asset_path}")
            try:
                result = unreal.EditorAssetLibrary.delete_asset(asset_path)
                if result:
                    deleted_count += 1
                    print(f"  -> Deleted successfully")
                else:
                    print(f"  -> Delete returned False (may be in use)")
            except Exception as e:
                print(f"  -> Error: {e}")
        else:
            print(f"[SKIP] {asset_path} - does not exist")

    print(f"\n[PHASE 2 COMPLETE] Deleted {deleted_count}/{len(ASSETS_TO_DELETE)} assets")
    return deleted_count > 0


def reparent_ac_statmanager():
    """
    Reparent AC_StatManager to USoulslikeStatComponent.
    """
    print("\n" + "=" * 60)
    print("PHASE 3: Reparent AC_StatManager to USoulslikeStatComponent")
    print("=" * 60)

    try:
        automation_lib = unreal.SLFAutomationLibrary
    except AttributeError:
        print("[ERROR] SLFAutomationLibrary not found.")
        return False

    # Load AC_StatManager
    ac_statmanager_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
    ac_statmanager = unreal.EditorAssetLibrary.load_asset(ac_statmanager_path)

    if not ac_statmanager:
        print(f"[ERROR] Could not load AC_StatManager at: {ac_statmanager_path}")
        return False

    print(f"[INFO] Loaded: {ac_statmanager_path}")

    # Find USoulslikeStatComponent
    new_parent = None

    try:
        new_parent = unreal.SoulslikeStatComponent.static_class()
        print("[INFO] Found USoulslikeStatComponent via direct reference")
    except:
        pass

    if not new_parent:
        try:
            new_parent = unreal.load_class(None, "/Script/SLFConversion.SoulslikeStatComponent")
            print("[INFO] Found USoulslikeStatComponent via load_class")
        except:
            pass

    if not new_parent:
        print("[ERROR] Could not find USoulslikeStatComponent class")
        print("       Make sure C++ is compiled!")
        return False

    # Reparent
    print("\n[REPARENT] Reparenting AC_StatManager...")

    try:
        result = automation_lib.reparent_blueprint(ac_statmanager, new_parent)

        if result:
            print("[SUCCESS] AC_StatManager reparented to USoulslikeStatComponent")

            # Save
            unreal.EditorAssetLibrary.save_loaded_asset(ac_statmanager)
            print("[SUCCESS] AC_StatManager saved")

            # Compile
            try:
                unreal.BlueprintEditorLibrary.compile_blueprint(ac_statmanager)
                print("[SUCCESS] AC_StatManager compiled")
            except Exception as e:
                print(f"[WARNING] Could not compile: {e}")

            return True
        else:
            print("[ERROR] Reparenting failed")
            return False

    except Exception as e:
        print(f"[ERROR] Exception during reparenting: {e}")
        return False


def run_scan_only():
    """
    Just scan and report affected Blueprints without making changes.
    """
    print("\n" + "=" * 60)
    print("SCAN ONLY - No changes will be made")
    print("=" * 60)

    # Verify prerequisites
    verify_bp_structs()
    verify_cpp_structs()

    affected = find_affected_blueprints()

    print(f"\n[RESULT] Found {len(affected)} affected Blueprints")
    print("\nTo run migration, call: run_migrate_structs()")


def run_migrate_structs():
    """
    Run struct node migration on all affected Blueprints.
    This is Phase 1 of the migration.
    """
    print("\n" + "=" * 60)
    print("STRUCT NODE MIGRATION (Phase 1)")
    print("=" * 60)
    print("\nThis will DIRECTLY modify struct node types in Blueprints.")
    print("Blueprint structs will NOT be deleted yet.\n")

    # Verify prerequisites
    bp_ok = verify_bp_structs()
    cpp_ok = verify_cpp_structs()

    if not bp_ok:
        print("\n[ABORT] Blueprint structs are missing. Restore from backup first.")
        return

    if not cpp_ok:
        print("\n[ABORT] C++ structs are not available. Compile C++ first.")
        return

    # Find affected Blueprints
    affected = find_affected_blueprints()

    if not affected:
        print("\n[WARNING] No affected Blueprints found. Nothing to migrate.")
        return

    # Migrate all Blueprints
    total, success = migrate_all_blueprints(affected)

    if success:
        print("\n" + "=" * 60)
        print("PHASE 1 COMPLETE - NEXT STEPS")
        print("=" * 60)
        print("\n1. Open affected Blueprints and verify they compile")
        print("2. If all Blueprints are OK, run: run_delete_bp_structs()")
        print("3. Then run: run_reparent_statmanager()")
    else:
        print("\n[WARNING] Some Blueprints failed to migrate.")
        print("Check errors above before proceeding.")


def run_delete_bp_structs():
    """
    Delete Blueprint struct/enum assets after migration.
    Only run this after verifying all Blueprints compile!
    """
    print("\n" + "=" * 60)
    print("DELETE BLUEPRINT STRUCTS (Phase 2)")
    print("=" * 60)

    delete_blueprint_assets()


def run_reparent_statmanager():
    """
    Reparent AC_StatManager to USoulslikeStatComponent.
    Run this after struct migration is complete.
    """
    print("\n" + "=" * 60)
    print("REPARENT AC_STATMANAGER (Phase 3)")
    print("=" * 60)

    reparent_ac_statmanager()


def run_full_migration():
    """
    Run the complete migration in order:
    1. Migrate struct nodes
    2. Delete BP structs
    3. Reparent AC_StatManager
    """
    print("\n" + "=" * 60)
    print("FULL MIGRATION - ALTERNATIVE APPROACH")
    print("=" * 60)
    print("\nThis script uses DIRECT struct type modification.")
    print("It does NOT rely on CoreRedirects.\n")

    # Verify prerequisites
    bp_ok = verify_bp_structs()
    cpp_ok = verify_cpp_structs()

    if not bp_ok:
        print("\n[ABORT] Blueprint structs are missing. Restore from backup first.")
        return

    if not cpp_ok:
        print("\n[ABORT] C++ structs are not available. Compile C++ first.")
        return

    # Find affected Blueprints
    affected = find_affected_blueprints()

    # Phase 1: Migrate struct nodes
    total, success = migrate_all_blueprints(affected)

    if not success:
        print("\n[ABORT] Phase 1 had failures. Check errors before proceeding.")
        return

    # Phase 2: Delete BP structs
    delete_blueprint_assets()

    # Phase 3: Reparent AC_StatManager
    reparent_success = reparent_ac_statmanager()

    # Summary
    print("\n" + "=" * 60)
    print("MIGRATION SUMMARY")
    print("=" * 60)
    print(f"Affected Blueprints: {len(affected)}")
    print(f"Struct Nodes Migrated: {total}")
    print(f"AC_StatManager Reparent: {'SUCCESS' if reparent_success else 'FAILED'}")
    print("\nNext steps:")
    print("1. Open AC_StatManager Blueprint - verify it inherits from USoulslikeStatComponent")
    print("2. Compile all Blueprints")
    print("3. Test in Play mode")


# Main execution
if __name__ == "__main__" or True:
    print("\n" + "=" * 60)
    print("DIRECT STRUCT MIGRATION SCRIPT LOADED")
    print("=" * 60)
    print("\nThis is the ALTERNATIVE APPROACH that directly sets StructType.")
    print("It does NOT rely on CoreRedirects.\n")
    print("Available functions:")
    print("  run_scan_only()          - Verify prerequisites and scan Blueprints")
    print("  run_migrate_structs()    - Phase 1: Migrate struct nodes")
    print("  run_delete_bp_structs()  - Phase 2: Delete BP struct assets")
    print("  run_reparent_statmanager() - Phase 3: Reparent AC_StatManager")
    print("  run_full_migration()     - Run all phases")
    print("\nRecommended order:")
    print("  1. run_scan_only()       - Check prerequisites")
    print("  2. run_migrate_structs() - Migrate struct nodes")
    print("  3. Verify Blueprints compile in Editor")
    print("  4. run_delete_bp_structs() - Delete BP structs")
    print("  5. run_reparent_statmanager() - Reparent component")
    print("")
