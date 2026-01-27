# verify_stat_init.py
# Verify stat initialization by checking DataTable contents

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/stat_init_verify.txt"

def log(msg):
    print(msg)
    unreal.log(msg)
    with open(OUTPUT_FILE, "a") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w") as f:
        f.write("")

    log("=== Stat Initialization Verification ===\n")

    # Check DataTable
    log("[TEST 1] Checking Stat DataTable")
    dt_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable"
    dt = unreal.EditorAssetLibrary.load_asset(dt_path)
    if dt:
        row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(dt)
        log(f"[PASS] DataTable has {len(row_names)} rows")
        for name in row_names[:10]:
            log(f"  Row: {name}")
    else:
        log("[FAIL] DataTable not found")

    # Check stat Blueprint assets
    log("\n[TEST 2] Checking stat Blueprint classes")
    stat_paths = [
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity",
    ]

    for path in stat_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            log(f"[PASS] {path.split('/')[-1]} loaded")
            # Check if generated class exists
            gen_class = unreal.load_class(None, path + "." + path.split('/')[-1] + "_C")
            if gen_class:
                log(f"  Generated class found")
            else:
                log(f"  [WARN] Generated class not found")
        else:
            log(f"[FAIL] {path} not found")

    # Check player character
    log("\n[TEST 3] Checking player character")
    char_path = "/Game/SoulslikeFramework/Blueprints/_BaseClasses/B_Soulslike_Character"
    char = unreal.EditorAssetLibrary.load_asset(char_path)
    if char:
        log(f"[PASS] B_Soulslike_Character loaded")
        gen_class = unreal.load_class(None, char_path + "." + char_path.split('/')[-1] + "_C")
        if gen_class:
            log(f"  Generated class found")
        else:
            log(f"  [WARN] Generated class not found")
    else:
        log("[FAIL] B_Soulslike_Character not found")

    # Check W_StatBlock widget
    log("\n[TEST 4] Checking W_StatBlock widget")
    widget_path = "/Game/SoulslikeFramework/Widgets/Stats/W_StatBlock"
    if unreal.EditorAssetLibrary.does_asset_exist(widget_path):
        log(f"[PASS] W_StatBlock exists")
    else:
        log("[INFO] W_StatBlock may be at different path")
        # Try alternate paths
        for alt_path in [
            "/Game/SoulslikeFramework/Widgets/W_StatBlock",
            "/Game/SoulslikeFramework/Widgets/Character/W_StatBlock",
        ]:
            if unreal.EditorAssetLibrary.does_asset_exist(alt_path):
                log(f"  Found at: {alt_path}")
                break

    # Check W_Inventory widget
    log("\n[TEST 5] Checking W_Inventory widget")
    inventory_path = "/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory"
    inventory = unreal.EditorAssetLibrary.load_asset(inventory_path)
    if inventory:
        log(f"[PASS] W_Inventory loaded")
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(inventory)
        if errors and "Error" in errors and "0 Errors" not in errors:
            log(f"[WARN] Compile issues: {errors[:200]}")
        else:
            log("[PASS] W_Inventory compiles")
    else:
        log("[FAIL] W_Inventory not found")

    log("\n=== SUMMARY ===")
    log("All checks complete. If stats don't display in PIE:")
    log("1. Check C++ build has no errors")
    log("2. Verify OnStatsInitialized is being broadcast")
    log("3. Check W_StatBlock finds StatManagerComponent")

if __name__ == "__main__":
    main()
