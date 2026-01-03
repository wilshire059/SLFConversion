"""
migrate_inputbuffer_with_fixer.py

Full migration using C++ BlueprintFixerLibrary to repair dependent Blueprints.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_inputbuffer_with_fixer.py").read())

Prerequisites:
    1. C++ compiled (UInputBufferComponent + UBlueprintFixerLibrary)
    2. AC_InputBuffer has been manually cleaned:
       - Deleted: QueueAction, ConsumeInputBuffer, ToggleBuffer, ExecuteActionImmediately
       - Deleted: IncomingActionTag, IgnoreNextOfActions, BufferOpen?, OnInputBufferConsumed
       - Event Graph cleared
"""

import unreal


# Configuration
BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer"
CPP_CLASS_PATH = "/Script/SLFConversion.InputBufferComponent"

DEPENDENT_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Animations/Notifies/AN_TryGuard",
    "/Game/SoulslikeFramework/Animations/Notifies/ANS_InputBuffer",
    "/Game/SoulslikeFramework/Blueprints/B_Soulslike_Character",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_InventoryManager",
]


def step1_reparent():
    """Reparent AC_InputBuffer to UInputBufferComponent."""
    print("\n[Step 1] Reparenting AC_InputBuffer...")

    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        print("  [ERROR] UInputBufferComponent not found!")
        return False

    bp = unreal.load_asset(BP_PATH)
    if not bp:
        print(f"  [ERROR] Could not load: {BP_PATH}")
        return False

    success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
    if not success:
        print("  [ERROR] Reparent failed!")
        print("  Make sure you deleted the BP functions/variables first.")
        return False

    print("  [OK] Reparented successfully")

    # Compile and save
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.EditorAssetLibrary.save_asset(BP_PATH)
    print("  [OK] Compiled and saved")

    return True


def step2_analyze_dependents():
    """Analyze what needs fixing in dependent Blueprints."""
    print("\n[Step 2] Analyzing dependent Blueprints...")

    for bp_path in DEPENDENT_BLUEPRINTS:
        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"  [WARN] Could not load: {bp_path}")
            continue

        print(f"\n  {bp_path.split('/')[-1]}:")

        # Get function call info using our C++ library
        info = unreal.BlueprintFixerLibrary.get_function_call_info(bp)
        input_buffer_calls = [i for i in info if "InputBuffer" in i or "AC_InputBuffer" in i]

        if input_buffer_calls:
            for call in input_buffer_calls[:5]:  # Limit output
                print(f"    - {call}")
            if len(input_buffer_calls) > 5:
                print(f"    ... and {len(input_buffer_calls) - 5} more")
        else:
            print("    No InputBuffer calls found")


def step3_fix_dependents():
    """Use C++ BlueprintFixerLibrary to fix dependent Blueprints."""
    print("\n[Step 3] Fixing dependent Blueprints with C++...")

    total_fixed = 0

    for bp_path in DEPENDENT_BLUEPRINTS:
        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"  [WARN] Could not load: {bp_path}")
            continue

        bp_name = bp_path.split('/')[-1]
        print(f"\n  Processing: {bp_name}")

        # Method 1: Try ReconstructAllNodes first (simplest)
        print("    Reconstructing all nodes...")
        unreal.BlueprintFixerLibrary.reconstruct_all_nodes(bp)

        # Method 2: Specifically fix InputBuffer function calls
        print("    Fixing InputBuffer function calls...")
        fixed = unreal.BlueprintFixerLibrary.refresh_function_calls_for_class(
            bp, "AC_InputBuffer_C", "InputBufferComponent"
        )
        print(f"    Fixed {fixed} function call nodes")
        total_fixed += fixed

        # Method 3: Fix event dispatcher bindings
        print("    Fixing event dispatcher bindings...")
        events_fixed = unreal.BlueprintFixerLibrary.fix_event_dispatcher_bindings(
            bp, "AC_InputBuffer"
        )
        print(f"    Fixed {events_fixed} event bindings")

        # Compile and save
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"    [OK] Compiled and saved")
        except Exception as e:
            print(f"    [WARN] Compile/save error: {e}")

    print(f"\n  Total nodes fixed: {total_fixed}")
    return total_fixed


def run_migration():
    """Main migration function."""
    print("=" * 70)
    print("AC_InputBuffer Migration with C++ Blueprint Fixer")
    print("=" * 70)

    # Check prerequisites
    print("\n[Prerequisite Check]")

    # Check C++ class exists
    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        print("[ERROR] UInputBufferComponent not found! Compile C++ first.")
        return False
    print("  [OK] UInputBufferComponent found")

    # Check BlueprintFixerLibrary exists
    try:
        test = unreal.BlueprintFixerLibrary
        print("  [OK] BlueprintFixerLibrary found")
    except:
        print("[ERROR] BlueprintFixerLibrary not found! Compile C++ first.")
        return False

    # Step 1: Reparent
    if not step1_reparent():
        return False

    # Step 2: Analyze (optional, for debugging)
    step2_analyze_dependents()

    # Step 3: Fix dependents
    step3_fix_dependents()

    # Done
    print("\n" + "=" * 70)
    print("[DONE] Migration complete!")
    print("=" * 70)
    print()
    print("Next: Test in Play mode to verify input buffering works.")
    print()
    print("If there are still issues, check the Output Log for details.")
    print("You may need to manually refresh some nodes in the Blueprint Editor.")

    return True


# Run
run_migration()
