# automated_struct_migration.py
# Full automated Blueprint struct-to-C++ migration using JSON exports
#
# This script implements the breakthrough solution:
# 1. Parse JSON exports to find all affected Blueprints
# 2. Delete Blueprint UserDefinedStructs (CoreRedirects handle loading)
# 3. Refresh struct nodes in all affected Blueprints using RefreshStructNodes()
# 4. Reparent AC_StatManager to USoulslikeStatComponent
#
# PREREQUISITES:
# 1. CoreRedirects + PropertyRedirects configured in DefaultEngine.ini
# 2. Editor restarted after adding redirects
# 3. C++ module compiled with RefreshStructNodes() function
#
# Run in Unreal Editor Python console:
# exec(open(r"C:\scripts\SLFConversion\automated_struct_migration.py").read())

import unreal
import os
import json
import glob

# Configuration
JSON_EXPORTS_PATH = r"C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA"

# Struct paths to migrate (Old Blueprint path -> New C++ path)
STRUCT_MIGRATIONS = {
    "/Game/SoulslikeFramework/Structures/Stats/FRegen.FRegen": "/Script/SLFConversion.Regen",
    "/Game/SoulslikeFramework/Structures/Stats/FStatBehavior.FStatBehavior": "/Script/SLFConversion.StatBehavior",
    "/Game/SoulslikeFramework/Structures/Stats/FStatInfo.FStatInfo": "/Script/SLFConversion.StatInfo",
    "/Game/SoulslikeFramework/Structures/Stats/FStatOverride.FStatOverride": "/Script/SLFConversion.StatOverride",
}

# Enum paths to migrate
ENUM_MIGRATIONS = {
    "/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType": "/Script/SLFConversion.E_ValueType",
}

# Blueprint struct/enum assets to delete
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
                    # "/Game/SoulslikeFramework/Data/Stats/B_Stat.B_Stat" -> "/Game/SoulslikeFramework/Data/Stats/B_Stat"
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


def delete_blueprint_assets():
    """
    Delete Blueprint UserDefinedStruct/Enum assets.
    CoreRedirects will handle loading the C++ types instead.
    """
    print("\n" + "=" * 60)
    print("PHASE 1: Delete Blueprint Struct/Enum Assets")
    print("=" * 60)

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

    print(f"\n[PHASE 1 COMPLETE] Deleted {deleted_count}/{len(ASSETS_TO_DELETE)} assets")
    return deleted_count > 0


def refresh_struct_nodes_in_blueprints(affected_blueprints):
    """
    Call RefreshStructNodes() on all affected Blueprints.
    This reconstructs Make/Break struct nodes to use the new C++ types.
    """
    print("\n" + "=" * 60)
    print("PHASE 2: Refresh Struct Nodes in Affected Blueprints")
    print("=" * 60)

    try:
        automation_lib = unreal.SLFAutomationLibrary
    except AttributeError:
        print("[ERROR] SLFAutomationLibrary not found. Is the C++ module compiled?")
        return False

    # Check if RefreshAllStructNodes exists
    if not hasattr(automation_lib, 'refresh_all_struct_nodes'):
        print("[ERROR] RefreshAllStructNodes not found. Recompile C++ with latest changes.")
        return False

    total_fixed = 0
    failed_blueprints = []

    for bp_path in affected_blueprints.keys():
        print(f"\n[REFRESH] {bp_path}")

        try:
            bp = unreal.EditorAssetLibrary.load_asset(bp_path)
            if not bp:
                print(f"  -> Could not load asset")
                failed_blueprints.append(bp_path)
                continue

            # Use RefreshAllStructNodes which reconstructs ALL struct nodes
            # This is more reliable than targeted refresh since CoreRedirects are in place
            fixed_count = automation_lib.refresh_all_struct_nodes(bp)

            if fixed_count > 0:
                print(f"  -> Fixed {fixed_count} struct nodes")
                total_fixed += fixed_count

                # Save the Blueprint
                unreal.EditorAssetLibrary.save_loaded_asset(bp)
                print(f"  -> Saved")
            else:
                print(f"  -> No struct nodes found (may not need fixing)")

        except Exception as e:
            print(f"  -> Error: {e}")
            failed_blueprints.append(bp_path)

    print(f"\n[PHASE 2 COMPLETE] Fixed {total_fixed} nodes across {len(affected_blueprints)} Blueprints")

    if failed_blueprints:
        print(f"[WARNING] Failed Blueprints ({len(failed_blueprints)}):")
        for bp in failed_blueprints:
            print(f"  - {bp}")

    return len(failed_blueprints) == 0


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


def compile_all_blueprints():
    """
    Compile all Blueprints that might be affected.
    """
    print("\n" + "=" * 60)
    print("PHASE 4: Compile All Affected Blueprints")
    print("=" * 60)

    # Key Blueprints that depend on the stats system
    key_blueprints = [
        "/Game/SoulslikeFramework/Data/Stats/B_Stat",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
        "/Game/SoulslikeFramework/Blueprints/Characters/B_Character",
    ]

    for bp_path in key_blueprints:
        if unreal.EditorAssetLibrary.does_asset_exist(bp_path):
            print(f"[COMPILE] {bp_path}")
            try:
                bp = unreal.EditorAssetLibrary.load_asset(bp_path)
                if bp:
                    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                    print(f"  -> Compiled successfully")
            except Exception as e:
                print(f"  -> Error: {e}")
        else:
            print(f"[SKIP] {bp_path} - does not exist")


def run_full_migration():
    """
    Run the complete automated migration.
    """
    print("\n" + "=" * 60)
    print("AUTOMATED BLUEPRINT STRUCT-TO-C++ MIGRATION")
    print("=" * 60)
    print("\nThis script will:")
    print("1. Scan JSON exports to find affected Blueprints")
    print("2. Delete Blueprint struct/enum assets")
    print("3. Refresh struct nodes in all affected Blueprints")
    print("4. Reparent AC_StatManager to USoulslikeStatComponent")
    print("5. Compile key Blueprints")
    print("\nPREREQUISITES:")
    print("- CoreRedirects configured in DefaultEngine.ini")
    print("- Editor restarted after adding redirects")
    print("- C++ module compiled with RefreshStructNodes()")
    print("\n")

    # Step 1: Find affected Blueprints
    affected_blueprints = find_affected_blueprints()

    if not affected_blueprints:
        print("\n[WARNING] No affected Blueprints found. Proceeding anyway...")

    # Step 2: Delete Blueprint assets
    # NOTE: Commenting out for now - user should confirm before deletion
    # delete_blueprint_assets()
    print("\n[SKIP] Asset deletion skipped - run delete_blueprint_assets() manually if needed")

    # Step 3: Refresh struct nodes
    refresh_success = refresh_struct_nodes_in_blueprints(affected_blueprints)

    # Step 4: Reparent AC_StatManager
    reparent_success = reparent_ac_statmanager()

    # Step 5: Compile all
    compile_all_blueprints()

    # Summary
    print("\n" + "=" * 60)
    print("MIGRATION SUMMARY")
    print("=" * 60)
    print(f"Affected Blueprints: {len(affected_blueprints)}")
    print(f"Struct Node Refresh: {'SUCCESS' if refresh_success else 'PARTIAL'}")
    print(f"AC_StatManager Reparent: {'SUCCESS' if reparent_success else 'FAILED'}")
    print("\nNext steps:")
    print("1. Check Output Log for any errors")
    print("2. Open AC_StatManager Blueprint - verify it inherits from USoulslikeStatComponent")
    print("3. Open B_Character Blueprint - verify no compilation errors")
    print("4. Test in Play mode")

    if not reparent_success:
        print("\n[WARNING] AC_StatManager reparent failed!")
        print("Check that USoulslikeStatComponent class exists in C++")


def run_scan_only():
    """
    Just scan and report affected Blueprints without making changes.
    """
    print("\n" + "=" * 60)
    print("SCAN ONLY - No changes will be made")
    print("=" * 60)

    affected = find_affected_blueprints()

    print(f"\n[RESULT] Found {len(affected)} affected Blueprints")
    print("\nTo run the full migration, call: run_full_migration()")


def run_refresh_only():
    """
    Only refresh struct nodes without deleting assets or reparenting.
    Useful for testing the RefreshStructNodes() function.
    """
    print("\n" + "=" * 60)
    print("REFRESH ONLY - Struct nodes will be reconstructed")
    print("=" * 60)

    affected = find_affected_blueprints()
    refresh_struct_nodes_in_blueprints(affected)


# Main execution
if __name__ == "__main__" or True:
    print("\n" + "=" * 60)
    print("AUTOMATED STRUCT MIGRATION SCRIPT LOADED")
    print("=" * 60)
    print("\nAvailable functions:")
    print("  run_scan_only()      - Scan and report affected Blueprints")
    print("  run_refresh_only()   - Refresh struct nodes without other changes")
    print("  run_full_migration() - Run complete migration")
    print("  delete_blueprint_assets() - Delete Blueprint struct/enum assets")
    print("\nRecommended order:")
    print("  1. run_scan_only()        - See what will be affected")
    print("  2. run_refresh_only()     - Test struct node refresh")
    print("  3. run_full_migration()   - Complete migration")
    print("")
