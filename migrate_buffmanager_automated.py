"""
migrate_buffmanager_automated.py

FULLY AUTOMATED migration - no manual steps required.
Reparents AC_BuffManager to C++ UBuffManagerComponent.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_buffmanager_automated.py").read())

Prerequisites:
    1. C++ compiled (UBuffManagerComponent + UBlueprintFixerLibrary)
    2. Content folder restored from backup (clean state)
"""

import unreal

# Configuration
BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_BuffManager"
CPP_CLASS_PATH = "/Script/SLFConversion.BuffManagerComponent"

# Things to delete from AC_BuffManager before reparenting
FUNCTIONS_TO_DELETE = [
    "TryAddBuff",
    "RemoveBuffOfType",
    "RemoveAllBuffsOfType",
    "RemoveBuffWithTag",
    "RemoveAllBuffsWithTag",
    "GetBuffsWithTag",
]

VARIABLES_TO_DELETE = [
    "ActiveBuffs",
]

EVENT_DISPATCHERS_TO_DELETE = [
    "OnBuffDetected",
]

# Dependent Blueprints to fix after migration
DEPENDENT_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/B_Soulslike_Character",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
]


def log_section(title):
    """Print a section header."""
    print()
    print("=" * 70)
    print(f"  {title}")
    print("=" * 70)


def log_step(step_num, total, description):
    """Print a step header."""
    print()
    print(f"[Step {step_num}/{total}] {description}")
    print("-" * 50)


def check_prerequisites():
    """Verify C++ classes are available."""
    log_section("PREREQUISITE CHECK")

    # Check UBuffManagerComponent
    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        print("[FAIL] UBuffManagerComponent not found!")
        print("       Compile C++ first.")
        return False
    print("[OK] UBuffManagerComponent found")

    # Check BlueprintFixerLibrary
    try:
        _ = unreal.BlueprintFixerLibrary
        print("[OK] BlueprintFixerLibrary found")
    except AttributeError:
        print("[FAIL] BlueprintFixerLibrary not found!")
        print("       Compile C++ first.")
        return False

    # Check AC_BuffManager exists
    bp = unreal.load_asset(BP_PATH)
    if not bp:
        print(f"[FAIL] Could not load {BP_PATH}")
        return False
    print(f"[OK] AC_BuffManager loaded")

    return True


def get_pre_migration_diagnostics(bp):
    """Get diagnostics before migration for comparison."""
    log_step(1, 7, "PRE-MIGRATION DIAGNOSTICS")

    print("\n--- Blueprint Structure ---")
    diag = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
    print(diag)

    print("\n--- Functions ---")
    funcs = unreal.BlueprintFixerLibrary.list_blueprint_functions(bp)
    for f in funcs:
        print(f"  {f}")

    print("\n--- Variables ---")
    vars = unreal.BlueprintFixerLibrary.list_blueprint_variables(bp)
    for v in vars:
        print(f"  {v}")

    print("\n--- Detailed Node Info (Event Graph) ---")
    nodes = unreal.BlueprintFixerLibrary.get_detailed_node_info(bp, "EventGraph")
    for n in nodes[:20]:  # Limit output
        print(f"  {n}")
    if len(nodes) > 20:
        print(f"  ... and {len(nodes) - 20} more nodes")


def cleanup_blueprint(bp):
    """Delete functions, variables, dispatchers, and clear event graph."""
    log_step(2, 7, "CLEANING UP AC_BuffManager")

    # Delete functions
    print("\n--- Deleting Functions ---")
    for func_name in FUNCTIONS_TO_DELETE:
        result = unreal.BlueprintFixerLibrary.delete_blueprint_function(bp, func_name)
        status = "[OK]" if result else "[SKIP]"
        print(f"  {status} {func_name}")

    # Delete variables
    print("\n--- Deleting Variables ---")
    for var_name in VARIABLES_TO_DELETE:
        result = unreal.BlueprintFixerLibrary.delete_blueprint_variable(bp, var_name)
        status = "[OK]" if result else "[SKIP]"
        print(f"  {status} {var_name}")

    # Delete event dispatchers
    print("\n--- Deleting Event Dispatchers ---")
    for disp_name in EVENT_DISPATCHERS_TO_DELETE:
        result = unreal.BlueprintFixerLibrary.delete_event_dispatcher(bp, disp_name)
        status = "[OK]" if result else "[SKIP]"
        print(f"  {status} {disp_name}")

    # Clear event graph
    print("\n--- Clearing Event Graph ---")
    nodes_removed = unreal.BlueprintFixerLibrary.clear_event_graph(bp)
    print(f"  Removed {nodes_removed} nodes from event graph")

    print("\n--- Skipping Compile (will compile after reparent) ---")
    print("  [INFO] Reparent will trigger compile with inherited C++ functions")

    print("\n--- Keeping changes in memory (will save after reparent) ---")
    print("  [INFO] Changes are in memory, reparent will finalize them")

    return bp


def reparent_blueprint(bp):
    """Reparent AC_BuffManager to UBuffManagerComponent."""
    log_step(3, 7, "REPARENTING TO C++")

    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        print("[FAIL] Could not load C++ class")
        return False

    try:
        old_parent = bp.parent_class
        old_parent_name = old_parent.get_name() if old_parent else "Unknown"
    except:
        old_parent_name = "Unknown"

    print(f"  Old parent: {old_parent_name}")
    print(f"  New parent: {cpp_class.get_name()}")

    print("\n  Pre-reparent Blueprint state:")
    funcs = unreal.BlueprintFixerLibrary.list_blueprint_functions(bp)
    print(f"    Functions remaining: {len(funcs)}")

    print("\n  Attempting reparent (this will trigger compile)...")

    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        print(f"  Reparent returned: {result}")

        print("\n  Checking post-reparent state...")
        diag = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
        print(f"  Post-reparent diagnostics:\n{diag}")

        if "BuffManagerComponent" in diag:
            print("[OK] Reparent successful! Parent is now BuffManagerComponent")
            return True
        elif result is None:
            print("  Reparent returned None - attempting to compile...")
            try:
                unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                diag2 = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
                if "BuffManagerComponent" in diag2:
                    print("[OK] Reparent successful after compile!")
                    return True
            except Exception as ce:
                print(f"  Compile error: {ce}")
            print("[FAIL] Parent did not change to BuffManagerComponent")
            return False
        elif result is False:
            print("[FAIL] Reparent explicitly returned False")
            return False
        else:
            print(f"[WARN] Unexpected return value: {result}")
            return False

    except Exception as e:
        print(f"[FAIL] Exception: {e}")
        import traceback
        traceback.print_exc()
        return False


def compile_and_save_blueprint(bp, bp_path):
    """Compile and save a Blueprint."""
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("  [OK] Compiled")
    except Exception as e:
        print(f"  [WARN] Compile: {e}")

    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print("  [OK] Saved")
    except Exception as e:
        print(f"  [WARN] Save: {e}")


def get_post_migration_diagnostics(bp):
    """Get diagnostics after migration for comparison."""
    log_step(4, 7, "POST-MIGRATION DIAGNOSTICS (AC_BuffManager)")

    print("\n--- Blueprint Structure ---")
    diag = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
    print(diag)


def fix_dependent_blueprints():
    """Fix all dependent Blueprints."""
    log_step(5, 7, "FIXING DEPENDENT BLUEPRINTS")

    for bp_path in DEPENDENT_BLUEPRINTS:
        bp_name = bp_path.split('/')[-1]
        print(f"\n--- Processing: {bp_name} ---")

        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"  [WARN] Could not load Blueprint")
            continue

        print("  Pre-fix function calls:")
        info = unreal.BlueprintFixerLibrary.get_function_call_info(bp)
        buff_calls = [i for i in info if "Buff" in i]
        for call in buff_calls[:5]:
            print(f"    {call}")

        print("  Reconstructing all nodes...")
        unreal.BlueprintFixerLibrary.reconstruct_all_nodes(bp)

        print("  Fixing BuffManager function calls...")
        fixed = unreal.BlueprintFixerLibrary.refresh_function_calls_for_class(
            bp, "AC_BuffManager_C", "BuffManagerComponent"
        )
        print(f"  Fixed {fixed} function call nodes")

        print("  Fixing event dispatcher bindings...")
        events_fixed = unreal.BlueprintFixerLibrary.fix_event_dispatcher_bindings(
            bp, "AC_BuffManager"
        )
        print(f"  Fixed {events_fixed} event bindings")

        compile_and_save_blueprint(bp, bp_path)


def verify_migration():
    """Verify the migration was successful."""
    log_step(6, 7, "VERIFICATION")

    bp = unreal.load_asset(BP_PATH)
    if not bp:
        print("[FAIL] Could not load AC_BuffManager")
        return False

    try:
        parent = bp.parent_class
        parent_name = parent.get_name() if parent else "None"
    except:
        parent_name = "Unknown"

    print(f"  AC_BuffManager parent class: {parent_name}")

    if "BuffManagerComponent" in parent_name:
        print("[OK] Migration verified - parent is BuffManagerComponent")
        return True
    else:
        print(f"[FAIL] Expected BuffManagerComponent, got {parent_name}")
        return False


def run_migration():
    """Main migration function - fully automated."""
    log_section("AC_BuffManager FULLY AUTOMATED MIGRATION")
    print("This script will:")
    print("  1. Get pre-migration diagnostics")
    print("  2. Delete BP functions, variables, dispatchers, event graph")
    print("  3. Reparent to UBuffManagerComponent")
    print("  4. Get post-migration diagnostics")
    print("  5. Fix all dependent Blueprints")
    print("  6. Verify migration")

    if not check_prerequisites():
        print("\n[ABORT] Prerequisites not met")
        return False

    bp = unreal.load_asset(BP_PATH)
    if not bp:
        print(f"\n[ABORT] Could not load {BP_PATH}")
        return False

    get_pre_migration_diagnostics(bp)
    bp = cleanup_blueprint(bp)
    if not bp:
        print("\n[ABORT] Failed after cleanup")
        return False

    if not reparent_blueprint(bp):
        print("\n[ABORT] Reparent failed")
        return False

    print("\n  Compiling and saving AC_BuffManager...")
    compile_and_save_blueprint(bp, BP_PATH)

    bp = unreal.load_asset(BP_PATH)
    get_post_migration_diagnostics(bp)

    fix_dependent_blueprints()

    success = verify_migration()

    log_step(7, 7, "SUMMARY")
    if success:
        print("[SUCCESS] Migration complete!")
        print("\nNext steps:")
        print("  1. Check Output Log for any warnings/errors")
        print("  2. Test in Play mode - verify buff system works")
    else:
        print("[FAILED] Migration had issues")
        print("\nDebug steps:")
        print("  1. Check Output Log for 'LogBlueprintFixer' messages")
        print("  2. Compare against JSON exports")

    return success


# Run migration
run_migration()
