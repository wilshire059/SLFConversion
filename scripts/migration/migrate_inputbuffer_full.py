"""
migrate_inputbuffer_full.py

Full migration with automatic dependent Blueprint fixing.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_inputbuffer_full.py").read())
"""

import unreal


# Dependent Blueprints that call InputBuffer functions
DEPENDENT_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Animations/Notifies/AN_TryGuard",
    "/Game/SoulslikeFramework/Animations/Notifies/ANS_InputBuffer",
    "/Game/SoulslikeFramework/Blueprints/B_Soulslike_Character",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_InventoryManager",
]

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer"


def refresh_and_compile_blueprint(bp_path):
    """Load, refresh nodes, and recompile a Blueprint."""
    print(f"  Processing: {bp_path}")

    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"    [WARN] Could not load: {bp_path}")
        return False

    try:
        # Try to refresh all nodes in the Blueprint
        # This should rebind function calls to inherited implementations
        if hasattr(unreal.BlueprintEditorLibrary, 'refresh_all_nodes'):
            unreal.BlueprintEditorLibrary.refresh_all_nodes(bp)
            print("    Refreshed nodes")
    except Exception as e:
        print(f"    [INFO] refresh_all_nodes: {e}")

    try:
        # Compile the Blueprint
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("    Compiled")
    except Exception as e:
        print(f"    [WARN] Compile error: {e}")
        return False

    try:
        # Save
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print("    Saved")
    except Exception as e:
        print(f"    [WARN] Save error: {e}")

    return True


def check_blueprint_subsystem():
    """Check what Blueprint manipulation methods are available."""
    print("[DEBUG] Available BlueprintEditorLibrary methods:")
    methods = [m for m in dir(unreal.BlueprintEditorLibrary) if not m.startswith('_')]
    for m in sorted(methods):
        print(f"  - {m}")
    print()


def run_migration():
    """Main migration with dependent BP fixing."""
    print("=" * 70)
    print("AC_InputBuffer Full Migration")
    print("=" * 70)
    print()

    # Debug: show available methods
    check_blueprint_subsystem()

    # Step 1: Load C++ class
    print("[Step 1] Loading C++ class...")
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.InputBufferComponent")
        if not cpp_class:
            print("[ERROR] UInputBufferComponent not found!")
            return False
        print("  Found UInputBufferComponent")
    except Exception as e:
        print(f"[ERROR] {e}")
        return False

    # Step 2: Load AC_InputBuffer
    print("\n[Step 2] Loading AC_InputBuffer...")
    bp = unreal.load_asset(BP_PATH)
    if not bp:
        print(f"[ERROR] Could not load: {BP_PATH}")
        return False
    print("  Loaded")

    # Step 3: Reparent (this will fail if functions conflict - that's expected)
    print("\n[Step 3] Attempting reparent...")
    print("  Note: This may fail due to function conflicts. That's expected.")
    print("  We'll try to work around it.")

    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if success:
            print("  [OK] Reparent succeeded!")
        else:
            print("  [INFO] Reparent returned False - need to delete BP functions first")
            print("\n  Please manually delete these from AC_InputBuffer:")
            print("    Functions: QueueAction, ConsumeInputBuffer, ToggleBuffer, ExecuteActionImmediately")
            print("    Variables: IncomingActionTag, IgnoreNextOfActions, BufferOpen?")
            print("    (Keep OnInputBufferConsumed event dispatcher)")
            print("\n  Then run this script again.")
            return False
    except Exception as e:
        print(f"  [ERROR] {e}")
        return False

    # Step 4: Compile AC_InputBuffer
    print("\n[Step 4] Compiling AC_InputBuffer...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("  Compiled")
    except Exception as e:
        print(f"  [WARN] {e}")

    # Step 5: Save AC_InputBuffer
    print("\n[Step 5] Saving AC_InputBuffer...")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_PATH)
        print("  Saved")
    except Exception as e:
        print(f"  [WARN] {e}")

    # Step 6: Fix dependent Blueprints
    print("\n[Step 6] Fixing dependent Blueprints...")
    for dep_path in DEPENDENT_BLUEPRINTS:
        refresh_and_compile_blueprint(dep_path)

    print("\n" + "=" * 70)
    print("[DONE] Migration complete!")
    print("=" * 70)
    print()
    print("Next: Test in Play mode to verify input buffering works.")
    print()

    return True


# Run
run_migration()
