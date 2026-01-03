"""
migrate_inputbuffer_automated.py

FULLY AUTOMATED migration - no manual steps required.
Uses C++ BlueprintFixerLibrary for all Blueprint manipulation.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_inputbuffer_automated.py").read())

Prerequisites:
    1. C++ compiled (UInputBufferComponent + UBlueprintFixerLibrary)
    2. Content folder restored from backup (clean state)
"""

import unreal

# Configuration
BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer"
CPP_CLASS_PATH = "/Script/SLFConversion.InputBufferComponent"

# Things to delete from AC_InputBuffer before reparenting
FUNCTIONS_TO_DELETE = [
    "QueueAction",
    "ConsumeInputBuffer",
    "ToggleBuffer",
    "ExecuteActionImmediately",
]

VARIABLES_TO_DELETE = [
    "IncomingActionTag",
    "IgnoreNextOfActions",
    "BufferOpen?",  # BP uses ? in name
]

EVENT_DISPATCHERS_TO_DELETE = [
    "OnInputBufferConsumed",
]

# Dependent Blueprints to fix after migration
DEPENDENT_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Animations/Notifies/AN_TryGuard",
    "/Game/SoulslikeFramework/Animations/Notifies/ANS_InputBuffer",
    "/Game/SoulslikeFramework/Blueprints/B_Soulslike_Character",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_InventoryManager",
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

    # Check UInputBufferComponent
    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        print("[FAIL] UInputBufferComponent not found!")
        print("       Compile C++ first.")
        return False
    print("[OK] UInputBufferComponent found")

    # Check BlueprintFixerLibrary
    try:
        _ = unreal.BlueprintFixerLibrary
        print("[OK] BlueprintFixerLibrary found")
    except AttributeError:
        print("[FAIL] BlueprintFixerLibrary not found!")
        print("       Compile C++ first.")
        return False

    # Check AC_InputBuffer exists
    bp = unreal.load_asset(BP_PATH)
    if not bp:
        print(f"[FAIL] Could not load {BP_PATH}")
        return False
    print(f"[OK] AC_InputBuffer loaded")

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
    log_step(2, 7, "CLEANING UP AC_InputBuffer")

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

    # CRITICAL: Do NOT compile here!
    # If we compile after deleting functions, Unreal will cascade-compile dependents
    # which still reference the deleted functions -> compilation failure
    # Instead, we skip compile and immediately reparent. The reparent operation
    # will compile, and by then AC_InputBuffer will inherit C++ functions.
    print("\n--- Skipping Compile (will compile after reparent) ---")
    print("  [INFO] Reparent will trigger compile with inherited C++ functions")

    # Don't save yet - keep the Blueprint in memory only
    # Saving without compile might put the asset in an inconsistent state
    print("\n--- Keeping changes in memory (will save after reparent) ---")
    print("  [INFO] Changes are in memory, reparent will finalize them")

    # Don't reload - keep using the same Blueprint reference
    # Reloading after save-without-compile can cause issues
    return bp


def reparent_blueprint(bp):
    """Reparent AC_InputBuffer to UInputBufferComponent."""
    log_step(3, 7, "REPARENTING TO C++")

    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        print("[FAIL] Could not load C++ class")
        return False

    # Get old parent - try different methods
    try:
        old_parent = bp.parent_class
        old_parent_name = old_parent.get_name() if old_parent else "Unknown"
    except:
        old_parent_name = "Unknown"

    print(f"  Old parent: {old_parent_name}")
    print(f"  New parent: {cpp_class.get_name()}")

    # Show current state before reparent
    print("\n  Pre-reparent Blueprint state:")
    funcs = unreal.BlueprintFixerLibrary.list_blueprint_functions(bp)
    print(f"    Functions remaining: {len(funcs)}")
    for f in funcs:
        print(f"      - {f}")

    vars_list = unreal.BlueprintFixerLibrary.list_blueprint_variables(bp)
    print(f"    Variables remaining: {len(vars_list)}")

    # Check Blueprint validity
    print("\n  Blueprint validity check:")
    print(f"    bp is valid: {bp is not None}")
    print(f"    bp type: {type(bp)}")

    # Check generated class
    try:
        gen_class = bp.generated_class
        print(f"    generated_class: {gen_class.get_name() if gen_class else 'None'}")
    except Exception as e:
        print(f"    generated_class: Error - {e}")

    # Check parent class via different methods
    try:
        parent = bp.parent_class
        print(f"    parent_class (direct): {parent.get_name() if parent else 'None'}")
    except Exception as e:
        print(f"    parent_class (direct): Error - {e}")

    try:
        parent = getattr(bp, 'ParentClass', None)
        print(f"    ParentClass (attr): {parent.get_name() if parent else 'None'}")
    except Exception as e:
        print(f"    ParentClass (attr): Error - {e}")

    # Check if new parent is compatible
    print(f"\n  C++ class check:")
    print(f"    cpp_class is valid: {cpp_class is not None}")
    print(f"    cpp_class name: {cpp_class.get_name()}")
    try:
        cpp_parent = cpp_class.get_super_class()
        print(f"    cpp_class parent: {cpp_parent.get_name() if cpp_parent else 'None'}")
    except Exception as e:
        print(f"    cpp_class parent: Error - {e}")

    print("\n  Attempting reparent (this will trigger compile)...")
    print("  [INFO] Watch Output Log for 'LogBlueprintFixer' and 'LogBlueprint' messages")

    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        print(f"  Reparent returned: {result} (type: {type(result)})")

        # None might indicate success (void function) - check actual state
        print("\n  Checking post-reparent state...")

        # Get parent class using diagnostics
        diag = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
        print(f"  Post-reparent diagnostics:\n{diag}")

        # Check if parent changed to InputBufferComponent
        if "InputBufferComponent" in diag:
            print("[OK] Reparent successful! Parent is now InputBufferComponent")
            return True
        elif result is None:
            # None return but parent didn't change - might still need to compile
            print("  Reparent returned None - attempting to compile...")
            try:
                unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                print("  Compiled - checking state again...")
                diag2 = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
                print(f"  Post-compile diagnostics:\n{diag2}")
                if "InputBufferComponent" in diag2:
                    print("[OK] Reparent successful after compile!")
                    return True
            except Exception as ce:
                print(f"  Compile error: {ce}")

            print("[FAIL] Parent did not change to InputBufferComponent")
            return False
        elif result is False:
            print("[FAIL] Reparent explicitly returned False")
            return False
        else:
            # Unexpected return value
            print(f"[WARN] Unexpected return value: {result}")
            return False

    except Exception as e:
        print(f"[FAIL] Exception: {e}")
        import traceback
        traceback.print_exc()
        print("\n  Exception during reparent - check Output Log for details")
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
    log_step(4, 7, "POST-MIGRATION DIAGNOSTICS (AC_InputBuffer)")

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

        # Get pre-fix diagnostics
        print("  Pre-fix function calls:")
        info = unreal.BlueprintFixerLibrary.get_function_call_info(bp)
        input_buffer_calls = [i for i in info if "InputBuffer" in i]
        for call in input_buffer_calls[:5]:
            print(f"    {call}")

        # Method 1: Reconstruct all nodes
        print("  Reconstructing all nodes...")
        unreal.BlueprintFixerLibrary.reconstruct_all_nodes(bp)

        # Method 2: Fix specific function calls
        print("  Fixing InputBuffer function calls...")
        fixed = unreal.BlueprintFixerLibrary.refresh_function_calls_for_class(
            bp, "AC_InputBuffer_C", "InputBufferComponent"
        )
        print(f"  Fixed {fixed} function call nodes")

        # Method 3: Fix event dispatcher bindings
        print("  Fixing event dispatcher bindings...")
        events_fixed = unreal.BlueprintFixerLibrary.fix_event_dispatcher_bindings(
            bp, "AC_InputBuffer"
        )
        print(f"  Fixed {events_fixed} event bindings")

        # Compile and save
        compile_and_save_blueprint(bp, bp_path)

        # Post-fix diagnostics
        print("  Post-fix function calls:")
        info = unreal.BlueprintFixerLibrary.get_function_call_info(bp)
        input_buffer_calls = [i for i in info if "InputBuffer" in i]
        for call in input_buffer_calls[:5]:
            print(f"    {call}")


def verify_migration():
    """Verify the migration was successful."""
    log_step(6, 7, "VERIFICATION")

    bp = unreal.load_asset(BP_PATH)
    if not bp:
        print("[FAIL] Could not load AC_InputBuffer")
        return False

    # Get parent class - try different methods
    try:
        parent = bp.parent_class
        parent_name = parent.get_name() if parent else "None"
    except:
        parent_name = "Unknown"

    print(f"  AC_InputBuffer parent class: {parent_name}")

    if "InputBufferComponent" in parent_name:
        print("[OK] Migration verified - parent is InputBufferComponent")
        return True
    else:
        print(f"[FAIL] Expected InputBufferComponent, got {parent_name}")
        return False


def run_migration():
    """Main migration function - fully automated."""
    log_section("AC_InputBuffer FULLY AUTOMATED MIGRATION")
    print("This script will:")
    print("  1. Get pre-migration diagnostics")
    print("  2. Delete BP functions, variables, dispatchers, event graph")
    print("  3. Reparent to UInputBufferComponent")
    print("  4. Get post-migration diagnostics")
    print("  5. Fix all dependent Blueprints")
    print("  6. Verify migration")
    print()
    print("All logs go to Output Log under 'LogBlueprintFixer'")

    # Check prerequisites
    if not check_prerequisites():
        print("\n[ABORT] Prerequisites not met")
        return False

    # Load Blueprint
    bp = unreal.load_asset(BP_PATH)
    if not bp:
        print(f"\n[ABORT] Could not load {BP_PATH}")
        return False

    # Step 1: Pre-migration diagnostics
    get_pre_migration_diagnostics(bp)

    # Step 2: Cleanup Blueprint (returns reloaded BP)
    bp = cleanup_blueprint(bp)
    if not bp:
        print("\n[ABORT] Failed to reload Blueprint after cleanup")
        return False

    # Step 3: Reparent
    if not reparent_blueprint(bp):
        print("\n[ABORT] Reparent failed")
        print("\nDIAGNOSTICS: Check Output Log for 'LogBlueprintFixer' messages")
        print("You can compare against JSON exports in:")
        print("  C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA/")
        return False

    # Compile and save AC_InputBuffer
    print("\n  Compiling and saving AC_InputBuffer...")
    compile_and_save_blueprint(bp, BP_PATH)

    # Step 4: Post-migration diagnostics
    # Reload to get fresh state
    bp = unreal.load_asset(BP_PATH)
    get_post_migration_diagnostics(bp)

    # Step 5: Fix dependent Blueprints
    fix_dependent_blueprints()

    # Step 6: Verify
    success = verify_migration()

    # Summary
    log_step(7, 7, "SUMMARY")
    if success:
        print("[SUCCESS] Migration complete!")
        print()
        print("Next steps:")
        print("  1. Check Output Log for any warnings/errors")
        print("  2. Test in Play mode - verify input buffering works")
        print("  3. If issues, share logs and I'll compare against JSON exports")
    else:
        print("[FAILED] Migration had issues")
        print()
        print("Debug steps:")
        print("  1. Check Output Log for 'LogBlueprintFixer' messages")
        print("  2. Share the logs with me")
        print("  3. I'll compare against JSON exports to identify the issue")

    return success


# Run migration
run_migration()
