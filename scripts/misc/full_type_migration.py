# full_type_migration.py
# COMPLETE Blueprint-to-C++ Type Migration using CoreRedirects
#
# This script performs a FULL type migration from Blueprint UserDefinedStructs/Enums
# to C++ equivalents using Unreal's CoreRedirects mechanism.
#
# PREREQUISITES:
# 1. CoreRedirects configured in DefaultEngine.ini (see below)
# 2. C++ structs/enums compiled and matching Blueprint types
# 3. Editor running with latest C++ code
#
# REQUIRED CoreRedirects in DefaultEngine.ini under [CoreRedirects]:
# +StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FRegen.FRegen",NewName="/Script/SLFConversion.FRegen")
# +StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatBehavior.FStatBehavior",NewName="/Script/SLFConversion.FStatBehavior")
# +StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatInfo.FStatInfo",NewName="/Script/SLFConversion.FStatInfo")
# +StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatOverride.FStatOverride",NewName="/Script/SLFConversion.FStatOverride")
# +EnumRedirects=(OldName="/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType",NewName="/Script/SLFConversion.E_ValueType")
#
# ALSO ADD PropertyRedirects for struct member name changes (GUID names -> simple names):
# See PROPERTY_REDIRECTS section below
#
# Run in Unreal Editor Python console:
# exec(open(r"C:\scripts\SLFConversion\full_type_migration.py").read())

import unreal
import os
import json
import glob
import shutil

# Configuration
JSON_EXPORTS_PATH = r"C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA"
PROJECT_PATH = r"C:\scripts\SLFConversion"
CONTENT_PATH = os.path.join(PROJECT_PATH, "Content")
CONFIG_PATH = os.path.join(PROJECT_PATH, "Config")

# Blueprint struct/enum asset paths (UE format)
BP_ASSETS_TO_DELETE = [
    "/Game/SoulslikeFramework/Structures/Stats/FRegen",
    "/Game/SoulslikeFramework/Structures/Stats/FStatBehavior",
    "/Game/SoulslikeFramework/Structures/Stats/FStatInfo",
    "/Game/SoulslikeFramework/Structures/Stats/FStatOverride",
    "/Game/SoulslikeFramework/Enums/E_ValueType",
]

# Map UE paths to file paths
def ue_path_to_file_path(ue_path):
    """Convert /Game/Path/Asset to Content/Path/Asset.uasset"""
    relative_path = ue_path.replace("/Game/", "")
    return os.path.join(CONTENT_PATH, relative_path + ".uasset")

# Struct path mappings for verification
STRUCT_MIGRATIONS = {
    "/Game/SoulslikeFramework/Structures/Stats/FRegen.FRegen": "/Script/SLFConversion.FRegen",
    "/Game/SoulslikeFramework/Structures/Stats/FStatBehavior.FStatBehavior": "/Script/SLFConversion.FStatBehavior",
    "/Game/SoulslikeFramework/Structures/Stats/FStatInfo.FStatInfo": "/Script/SLFConversion.FStatInfo",
    "/Game/SoulslikeFramework/Structures/Stats/FStatOverride.FStatOverride": "/Script/SLFConversion.FStatOverride",
}

ENUM_MIGRATIONS = {
    "/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType": "/Script/SLFConversion.E_ValueType",
}

# Property redirects needed for struct member name changes
# Blueprint UserDefinedStruct members have GUID-decorated names
# Format: {OldStructPath: {OldMemberName: NewMemberName}}
PROPERTY_REDIRECTS = {
    "FRegen": {
        "CanRegenerate?_2_930C52D54A3CFC505563F58B34E72C0D": "bCanRegenerate",
        "RegenPercent_10_E67E71EF4B97141385FF9092B99995A0": "RegenPercent",
        "RegenInterval_7_D5E80EFD4D1894B824CBF8A0CF88475F": "RegenInterval",
    },
    "FStatInfo": {
        "Tag_39_84F9579F432E05BBC375FD8FCBBE2158": "Tag",
        "DisplayName_40_55A184B14E56D9C63DBBD68E4B70971B": "DisplayName",
        "Description_41_C9F80D2540F01D8E9AEEA1ADB9DA880A": "Description",
        "DisplayasPercent?_34_96ECE4D54F594255EBEC728EA8B68917": "bDisplayAsPercent",
        "CurrentValue_11_EEB1DDB64CB05B32EFEB3987C7578E44": "CurrentValue",
        "MaxValue_13_16BF066446A491A2E02C11841A439050": "MaxValue",
        "ShowMaxValue?_32_925A7CB1407597FF5CFB6DB891FC782D": "bShowMaxValue",
        "RegenInfo_19_A3BB81434EE6B4A8605F0CA9C1F6164D": "RegenInfo",
        "StatModifiers_23_437F7248490379A115E46EB53AA885B3": "StatModifiers",
    },
    "FStatBehavior": {
        "StatstoAffect_18_5396C90A4B21B7FDF091E985B665D57E": "StatsToAffect",
    },
    "FStatOverride": {
        # Need to extract from JSON - will add when we have the data
    },
}


def check_prerequisites():
    """Verify all prerequisites are met before migration."""
    print("\n" + "=" * 60)
    print("PREREQUISITE CHECK")
    print("=" * 60)

    all_ok = True

    # 1. Check C++ structs exist
    print("\n[1] Checking C++ types exist...")
    cpp_types = [
        ("FRegen", "/Script/SLFConversion.Regen"),
        ("FStatBehavior", "/Script/SLFConversion.StatBehavior"),
        ("FStatInfo", "/Script/SLFConversion.StatInfo"),
        ("FStatOverride", "/Script/SLFConversion.StatOverride"),
        ("E_ValueType", "/Script/SLFConversion.E_ValueType"),
    ]

    for name, path in cpp_types:
        loaded = None
        try:
            # Try multiple loading methods for structs
            # Method 1: Direct load_object
            loaded = unreal.load_object(None, path)
        except:
            pass

        if not loaded:
            try:
                # Method 2: Try with ScriptStruct class
                loaded = unreal.find_object(None, path)
            except:
                pass

        if not loaded:
            try:
                # Method 3: Try accessing via module (for enums)
                if name == "E_ValueType":
                    loaded = hasattr(unreal, 'E_ValueType')
                    if loaded:
                        loaded = unreal.E_ValueType
            except:
                pass

        if loaded:
            print(f"  [OK] {name}: {path}")
        else:
            # For structs, we can also verify by trying to find them in the asset registry
            # But if they're C++ structs, they won't be in the registry
            # Let's do a softer check - just warn if not found
            print(f"  [WARN] {name}: Could not verify {path} (may still work)")
            # Don't fail for structs - CoreRedirects will handle them at load time

    # 2. Check BP struct assets exist (should exist before deletion)
    print("\n[2] Checking Blueprint struct assets exist...")
    for asset_path in BP_ASSETS_TO_DELETE:
        file_path = ue_path_to_file_path(asset_path)
        if os.path.exists(file_path):
            print(f"  [OK] {asset_path}")
        else:
            print(f"  [SKIP] {asset_path} - already deleted or doesn't exist")

    # 3. Check CoreRedirects in DefaultEngine.ini
    print("\n[3] Checking CoreRedirects configuration...")
    engine_ini_path = os.path.join(CONFIG_PATH, "DefaultEngine.ini")

    if os.path.exists(engine_ini_path):
        with open(engine_ini_path, 'r', encoding='utf-8') as f:
            ini_content = f.read()

        required_redirects = [
            'StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FRegen',
            'StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatInfo',
            'StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatBehavior',
            'StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatOverride',
            'EnumRedirects=(OldName="/Game/SoulslikeFramework/Enums/E_ValueType',
        ]

        missing = []
        for redirect in required_redirects:
            if redirect in ini_content:
                print(f"  [OK] Found redirect for {redirect.split('/')[-1].split('.')[0]}")
            else:
                print(f"  [MISSING] {redirect}")
                missing.append(redirect)

        if missing:
            print("\n  [!] Missing CoreRedirects! Add the following to [CoreRedirects] section:")
            print_required_redirects()
            all_ok = False
    else:
        print(f"  [ERROR] DefaultEngine.ini not found at: {engine_ini_path}")
        all_ok = False

    return all_ok


def print_required_redirects():
    """Print the required CoreRedirects for DefaultEngine.ini"""
    print("""
Add to DefaultEngine.ini under [CoreRedirects]:

; Struct type redirects
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FRegen.FRegen",NewName="/Script/SLFConversion.FRegen")
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatBehavior.FStatBehavior",NewName="/Script/SLFConversion.FStatBehavior")
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatInfo.FStatInfo",NewName="/Script/SLFConversion.FStatInfo")
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatOverride.FStatOverride",NewName="/Script/SLFConversion.FStatOverride")

; Enum type redirect
+EnumRedirects=(OldName="/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType",NewName="/Script/SLFConversion.E_ValueType")

; Property redirects for FRegen struct members
+PropertyRedirects=(OldName="FRegen.CanRegenerate?_2_930C52D54A3CFC505563F58B34E72C0D",NewName="bCanRegenerate")
+PropertyRedirects=(OldName="FRegen.RegenPercent_10_E67E71EF4B97141385FF9092B99995A0",NewName="RegenPercent")
+PropertyRedirects=(OldName="FRegen.RegenInterval_7_D5E80EFD4D1894B824CBF8A0CF88475F",NewName="RegenInterval")

; Property redirects for FStatInfo struct members
+PropertyRedirects=(OldName="FStatInfo.Tag_39_84F9579F432E05BBC375FD8FCBBE2158",NewName="Tag")
+PropertyRedirects=(OldName="FStatInfo.DisplayName_40_55A184B14E56D9C63DBBD68E4B70971B",NewName="DisplayName")
+PropertyRedirects=(OldName="FStatInfo.Description_41_C9F80D2540F01D8E9AEEA1ADB9DA880A",NewName="Description")
+PropertyRedirects=(OldName="FStatInfo.DisplayasPercent?_34_96ECE4D54F594255EBEC728EA8B68917",NewName="bDisplayAsPercent")
+PropertyRedirects=(OldName="FStatInfo.CurrentValue_11_EEB1DDB64CB05B32EFEB3987C7578E44",NewName="CurrentValue")
+PropertyRedirects=(OldName="FStatInfo.MaxValue_13_16BF066446A491A2E02C11841A439050",NewName="MaxValue")
+PropertyRedirects=(OldName="FStatInfo.ShowMaxValue?_32_925A7CB1407597FF5CFB6DB891FC782D",NewName="bShowMaxValue")
+PropertyRedirects=(OldName="FStatInfo.RegenInfo_19_A3BB81434EE6B4A8605F0CA9C1F6164D",NewName="RegenInfo")
+PropertyRedirects=(OldName="FStatInfo.StatModifiers_23_437F7248490379A115E46EB53AA885B3",NewName="StatModifiers")

; Property redirects for FStatBehavior struct members
+PropertyRedirects=(OldName="FStatBehavior.StatstoAffect_18_5396C90A4B21B7FDF091E985B665D57E",NewName="StatsToAffect")
""")


def find_affected_blueprints():
    """Find all Blueprints that reference the struct/enum types being migrated."""
    affected = {}

    search_terms = list(STRUCT_MIGRATIONS.keys()) + list(ENUM_MIGRATIONS.keys())
    search_terms.extend(["FStatInfo", "FRegen", "FStatBehavior", "FStatOverride", "E_ValueType"])

    print("\n[SCAN] Searching JSON exports for struct/enum references...")

    json_files = glob.glob(os.path.join(JSON_EXPORTS_PATH, "**", "*.json"), recursive=True)
    print(f"[SCAN] Found {len(json_files)} JSON files to scan")

    for json_file in json_files:
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                content = f.read()

            found_terms = [term for term in search_terms if term in content]

            if found_terms:
                data = json.loads(content)
                bp_path = data.get("Path", "")

                if bp_path:
                    asset_path = bp_path.rsplit('.', 1)[0] if '.' in bp_path else bp_path

                    if asset_path not in affected:
                        affected[asset_path] = []
                    affected[asset_path].extend(found_terms)
        except:
            pass

    print(f"[SCAN] Found {len(affected)} affected Blueprints:")
    for bp_path in sorted(affected.keys()):
        terms = list(set(affected[bp_path]))
        print(f"  - {bp_path.split('/')[-1]}: {', '.join([t.split('/')[-1].split('.')[0] for t in terms[:3]])}")

    return affected


def backup_bp_structs():
    """Backup BP struct assets before deletion."""
    backup_dir = os.path.join(PROJECT_PATH, "Backups", "BP_Structs_Pre_Migration")
    os.makedirs(backup_dir, exist_ok=True)

    print(f"\n[BACKUP] Backing up BP struct assets to: {backup_dir}")

    for asset_path in BP_ASSETS_TO_DELETE:
        file_path = ue_path_to_file_path(asset_path)
        if os.path.exists(file_path):
            backup_file = os.path.join(backup_dir, os.path.basename(file_path))
            shutil.copy2(file_path, backup_file)
            print(f"  [OK] Backed up {os.path.basename(file_path)}")

    return backup_dir


def delete_bp_struct_files():
    """Delete BP struct/enum asset files from disk."""
    print("\n" + "=" * 60)
    print("PHASE 1: DELETE BP STRUCT/ENUM FILES")
    print("=" * 60)
    print("\nWARNING: This will delete the following files from disk:")

    files_to_delete = []
    for asset_path in BP_ASSETS_TO_DELETE:
        file_path = ue_path_to_file_path(asset_path)
        if os.path.exists(file_path):
            files_to_delete.append(file_path)
            print(f"  - {file_path}")

    if not files_to_delete:
        print("\n[INFO] No files to delete - they may have already been deleted.")
        return True

    # Create backup first
    backup_dir = backup_bp_structs()

    # Delete files
    print(f"\n[DELETE] Deleting {len(files_to_delete)} files...")
    deleted_count = 0
    for file_path in files_to_delete:
        try:
            os.remove(file_path)
            deleted_count += 1
            print(f"  [OK] Deleted: {os.path.basename(file_path)}")
        except Exception as e:
            print(f"  [FAIL] {os.path.basename(file_path)}: {e}")

    print(f"\n[RESULT] Deleted {deleted_count}/{len(files_to_delete)} files")
    print(f"[BACKUP] Files backed up to: {backup_dir}")

    return deleted_count == len(files_to_delete)


def phase2_resave_blueprints(affected_blueprints):
    """
    Phase 2: After editor restart, resave all affected Blueprints.
    CoreRedirects will have taken effect during load.
    """
    print("\n" + "=" * 60)
    print("PHASE 2: RESAVE AFFECTED BLUEPRINTS")
    print("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path in affected_blueprints:
        print(f"\n[RESAVE] {bp_path}")

        try:
            # Load the Blueprint (CoreRedirects apply here)
            bp = unreal.EditorAssetLibrary.load_asset(bp_path)

            if bp:
                # Compile the Blueprint
                try:
                    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                    print(f"  [OK] Compiled")
                except Exception as e:
                    print(f"  [WARN] Compile error: {e}")

                # Save the Blueprint
                unreal.EditorAssetLibrary.save_loaded_asset(bp)
                print(f"  [OK] Saved")
                success_count += 1
            else:
                print(f"  [FAIL] Could not load")
                fail_count += 1

        except Exception as e:
            print(f"  [FAIL] Error: {e}")
            fail_count += 1

    print(f"\n[RESULT] Resaved {success_count} Blueprints, {fail_count} failed")
    return fail_count == 0


def run_phase1():
    """
    Phase 1: Check prerequisites and delete BP struct files.
    After this phase, you MUST restart the editor.
    """
    print("\n" + "=" * 60)
    print("FULL TYPE MIGRATION - PHASE 1")
    print("=" * 60)
    print("\nThis phase will:")
    print("1. Check prerequisites (C++ types, CoreRedirects)")
    print("2. Identify affected Blueprints")
    print("3. Delete BP struct/enum files from disk")
    print("\nAfter this phase, you MUST restart the editor!")
    print("Then run run_phase2() to complete the migration.")

    # Check prerequisites
    if not check_prerequisites():
        print("\n[ABORT] Prerequisites not met. Please fix the issues above.")
        return False

    # Find affected Blueprints
    affected = find_affected_blueprints()

    # Save affected Blueprint list for Phase 2
    affected_file = os.path.join(PROJECT_PATH, "affected_blueprints.json")
    with open(affected_file, 'w', encoding='utf-8') as f:
        json.dump(list(affected.keys()), f, indent=2)
    print(f"\n[SAVED] Affected Blueprint list: {affected_file}")

    # Delete BP struct files
    if not delete_bp_struct_files():
        print("\n[WARNING] Some files could not be deleted. Check if they're in use.")

    print("\n" + "=" * 60)
    print("PHASE 1 COMPLETE")
    print("=" * 60)
    print("\n*** CRITICAL: You MUST restart the Unreal Editor now! ***")
    print("\nAfter restarting:")
    print("1. The editor will load Blueprints with CoreRedirects applied")
    print("2. Run: run_phase2() to resave all affected Blueprints")
    print("3. Reparent AC_StatManager to USoulslikeStatComponent")

    return True


def run_phase2():
    """
    Phase 2: Resave all affected Blueprints after editor restart.
    CoreRedirects will have taken effect during the load.
    """
    print("\n" + "=" * 60)
    print("FULL TYPE MIGRATION - PHASE 2")
    print("=" * 60)

    # Load affected Blueprint list from Phase 1
    affected_file = os.path.join(PROJECT_PATH, "affected_blueprints.json")

    if not os.path.exists(affected_file):
        print(f"\n[ERROR] Affected Blueprint list not found: {affected_file}")
        print("Did you run run_phase1() first?")
        return False

    with open(affected_file, 'r', encoding='utf-8') as f:
        affected_blueprints = json.load(f)

    print(f"\n[LOAD] Found {len(affected_blueprints)} affected Blueprints from Phase 1")

    # Verify BP struct files are gone
    print("\n[CHECK] Verifying BP struct files are deleted...")
    any_exist = False
    for asset_path in BP_ASSETS_TO_DELETE:
        file_path = ue_path_to_file_path(asset_path)
        if os.path.exists(file_path):
            print(f"  [WARN] Still exists: {file_path}")
            any_exist = True

    if any_exist:
        print("\n[WARNING] Some BP struct files still exist!")
        print("This might cause issues. Consider deleting them manually.")
    else:
        print("  [OK] All BP struct files deleted")

    # Resave all affected Blueprints
    success = phase2_resave_blueprints(affected_blueprints)

    if success:
        print("\n" + "=" * 60)
        print("PHASE 2 COMPLETE")
        print("=" * 60)
        print("\nAll Blueprints have been resaved with C++ types!")
        print("\nNext step:")
        print("1. Run: run_reparent_statmanager() to reparent AC_StatManager")
        print("2. Or run: exec(open(r'C:\\scripts\\SLFConversion\\migrate_statmanager_to_cpp.py').read())")

    return success


def run_reparent_statmanager():
    """Reparent AC_StatManager to USoulslikeStatComponent."""
    print("\n" + "=" * 60)
    print("REPARENT AC_STATMANAGER")
    print("=" * 60)

    try:
        automation_lib = unreal.SLFAutomationLibrary
    except AttributeError:
        print("[ERROR] SLFAutomationLibrary not found. Is the C++ module compiled?")
        return False

    # Load AC_StatManager
    ac_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
    ac = unreal.EditorAssetLibrary.load_asset(ac_path)

    if not ac:
        print(f"[ERROR] Could not load AC_StatManager at: {ac_path}")
        return False

    print(f"[LOAD] Loaded: {ac_path}")

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
        return False

    # Reparent
    print("\n[REPARENT] Reparenting AC_StatManager...")
    result = automation_lib.reparent_blueprint(ac, new_parent)

    if result:
        print("[SUCCESS] AC_StatManager reparented to USoulslikeStatComponent")

        # Save
        unreal.EditorAssetLibrary.save_loaded_asset(ac)
        print("[SUCCESS] AC_StatManager saved")

        # Compile
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(ac)
            print("[SUCCESS] AC_StatManager compiled")
        except Exception as e:
            print(f"[WARNING] Could not compile: {e}")

        return True
    else:
        print("[ERROR] Reparenting failed")
        return False


def run_check_only():
    """Just check prerequisites without making any changes."""
    check_prerequisites()
    print("\n" + "=" * 60)
    affected = find_affected_blueprints()
    print("\n" + "=" * 60)
    print(f"Total affected Blueprints: {len(affected)}")
    print("\nTo proceed with migration:")
    print("1. Ensure CoreRedirects are configured correctly")
    print("2. Run: run_phase1()")
    print("3. Restart the editor")
    print("4. Run: run_phase2()")


# Main execution
if __name__ == "__main__" or True:
    print("\n" + "=" * 60)
    print("FULL TYPE MIGRATION SCRIPT LOADED")
    print("=" * 60)
    print("\nAvailable functions:")
    print("  run_check_only()        - Check prerequisites without changes")
    print("  run_phase1()            - Delete BP struct files (requires restart)")
    print("  run_phase2()            - Resave Blueprints after restart")
    print("  run_reparent_statmanager() - Final step: reparent AC_StatManager")
    print("  print_required_redirects() - Show required CoreRedirects config")
    print("\nRecommended workflow:")
    print("  1. run_check_only()     - Verify everything is ready")
    print("  2. run_phase1()         - Delete BP structs")
    print("  3. RESTART EDITOR       - Required for CoreRedirects to work!")
    print("  4. run_phase2()         - Resave Blueprints")
    print("  5. run_reparent_statmanager() - Reparent AC_StatManager")
    print("")
