# test_stat_display_detailed.py
# Detailed test for stat display and equipment functionality

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/stat_display_test.txt"

def log(msg):
    print(msg)
    unreal.log(msg)
    with open(OUTPUT_FILE, "a") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w") as f:
        f.write("")

    log("=== Stat Display & Equipment Detailed Test ===\n")

    # Test 1: Check StatManagerComponent exists and has correct class
    log("[TEST 1] Checking StatManagerComponent class hierarchy")
    stat_mgr_class = unreal.load_class(None, "/Script/SLFConversion.StatManagerComponent")
    if stat_mgr_class:
        log(f"[PASS] UStatManagerComponent class found: {stat_mgr_class.get_name()}")
        parent = stat_mgr_class.get_super_class()
        log(f"  Parent class: {parent.get_name() if parent else 'None'}")
    else:
        log("[FAIL] UStatManagerComponent class not found")

    ac_stat_mgr_class = unreal.load_class(None, "/Script/SLFConversion.AC_StatManager")
    if ac_stat_mgr_class:
        log(f"[INFO] UAC_StatManager class also exists: {ac_stat_mgr_class.get_name()}")
        parent = ac_stat_mgr_class.get_super_class()
        log(f"  Parent class: {parent.get_name() if parent else 'None'}")
    else:
        log("[INFO] UAC_StatManager class not found (OK if using UStatManagerComponent)")

    # Test 2: Check stat Blueprints parent classes
    log("\n[TEST 2] Checking stat Blueprint parent classes")
    stat_paths = [
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity",
    ]

    for path in stat_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            bp_class = asset.generated_class() if hasattr(asset, 'generated_class') else None
            if bp_class:
                parent = bp_class.get_super_class()
                parent_name = parent.get_name() if parent else "None"
                log(f"  {path.split('/')[-1]}: parent = {parent_name}")
            else:
                log(f"  {path.split('/')[-1]}: could not get generated_class")
        else:
            log(f"  [WARN] {path} not found")

    # Test 3: Check SLFStatBase class
    log("\n[TEST 3] Checking USLFStatBase class")
    stat_base_class = unreal.load_class(None, "/Script/SLFConversion.SLFStatBase")
    if stat_base_class:
        log(f"[PASS] USLFStatBase class found: {stat_base_class.get_name()}")
    else:
        log("[WARN] USLFStatBase class not found")

    # Test 4: Check UB_Stat class
    log("\n[TEST 4] Checking UB_Stat class (older hierarchy)")
    b_stat_class = unreal.load_class(None, "/Script/SLFConversion.B_Stat")
    if b_stat_class:
        log(f"[INFO] UB_Stat class exists: {b_stat_class.get_name()}")
        parent = b_stat_class.get_super_class()
        log(f"  Parent class: {parent.get_name() if parent else 'None'}")
    else:
        log("[INFO] UB_Stat class not found (may be Blueprint-only)")

    # Test 5: Check DataTable row structure
    log("\n[TEST 5] Checking Stat DataTable")
    dt_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable"
    dt = unreal.EditorAssetLibrary.load_asset(dt_path)
    if dt:
        row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(dt)
        log(f"[PASS] DataTable found with {len(row_names)} rows")
        for i, name in enumerate(row_names[:5]):
            log(f"  Row {i+1}: {name}")
        if len(row_names) > 5:
            log(f"  ... and {len(row_names) - 5} more rows")
    else:
        log("[FAIL] DataTable not found")

    # Test 6: Check W_StatBlock
    log("\n[TEST 6] Checking W_StatBlock widget")
    widget_path = "/Game/SoulslikeFramework/Widgets/Stats/W_StatBlock"
    widget = unreal.EditorAssetLibrary.load_asset(widget_path)
    if widget:
        log(f"[PASS] W_StatBlock found")
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(widget)
        if errors and "Error" in errors:
            log(f"[WARN] Compile issues: {errors[:300]}")
        else:
            log("[PASS] W_StatBlock compiles")
    else:
        log("[INFO] W_StatBlock not at expected path")

    # Test 7: Check player character component setup
    log("\n[TEST 7] Checking B_Soulslike_Character component setup")
    char_path = "/Game/SoulslikeFramework/Blueprints/_BaseClasses/B_Soulslike_Character"
    char = unreal.EditorAssetLibrary.load_asset(char_path)
    if char:
        log(f"[PASS] B_Soulslike_Character found")
        bp_class = char.generated_class() if hasattr(char, 'generated_class') else None
        if bp_class:
            parent = bp_class.get_super_class()
            log(f"  C++ parent: {parent.get_name() if parent else 'None'}")
    else:
        log("[WARN] B_Soulslike_Character not found")

    log("\n=== TEST SUMMARY ===")
    log("See detailed output above for diagnostics")
    log("If stats don't display, check:")
    log("1. Stat BPs should parent to USLFStatBase (not UB_Stat)")
    log("2. Character should use UStatManagerComponent")
    log("3. Widgets should cast to correct stat type")

if __name__ == "__main__":
    main()
