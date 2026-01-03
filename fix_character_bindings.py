"""
fix_character_bindings.py

Fix the OnInputBufferConsumed event binding in B_Soulslike_Character.
The binding may have broken when we migrated AC_InputBuffer to C++.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\fix_character_bindings.py").read())
"""

import unreal

print("=" * 60)
print("  FIXING CHARACTER EVENT BINDINGS")
print("=" * 60)

# Fix B_Soulslike_Character
BP_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

bp = unreal.load_asset(BP_PATH)
if not bp:
    print(f"[FAIL] Could not load {BP_PATH}")
else:
    print(f"[OK] Loaded: {bp.get_name()}")

    # Get function call info before fix
    print("\n  Pre-fix InputBuffer-related nodes:")
    info = unreal.BlueprintFixerLibrary.get_function_call_info(bp)
    for i in info:
        if "InputBuffer" in i or "OnInputBuffer" in i:
            print(f"    {i}")

    # Reconstruct all nodes
    print("\n  Reconstructing all nodes...")
    unreal.BlueprintFixerLibrary.reconstruct_all_nodes(bp)
    print("  [OK] Nodes reconstructed")

    # Fix event dispatcher bindings specifically
    print("\n  Fixing event dispatcher bindings for AC_InputBuffer...")
    events_fixed = unreal.BlueprintFixerLibrary.fix_event_dispatcher_bindings(bp, "AC_InputBuffer")
    print(f"  Fixed {events_fixed} event bindings")

    # Also try fixing with InputBufferComponent (the C++ class)
    print("\n  Fixing event dispatcher bindings for InputBufferComponent...")
    events_fixed2 = unreal.BlueprintFixerLibrary.fix_event_dispatcher_bindings(bp, "InputBufferComponent")
    print(f"  Fixed {events_fixed2} event bindings")

    # Refresh function calls
    print("\n  Refreshing function calls...")
    fixed = unreal.BlueprintFixerLibrary.refresh_function_calls_for_class(bp, "AC_InputBuffer_C", "InputBufferComponent")
    print(f"  Fixed {fixed} function call nodes")

    # Compile
    print("\n  Compiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("  [OK] Compiled")
    except Exception as e:
        print(f"  [WARN] Compile error: {e}")

    # Get function call info after fix
    print("\n  Post-fix InputBuffer-related nodes:")
    info = unreal.BlueprintFixerLibrary.get_function_call_info(bp)
    for i in info:
        if "InputBuffer" in i or "OnInputBuffer" in i:
            print(f"    {i}")

    # Save
    print("\n  Saving...")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_PATH)
        print("  [OK] Saved")
    except Exception as e:
        print(f"  [WARN] Save error: {e}")

print("\n" + "=" * 60)
print("  DONE - Test jump/roll/attack now!")
print("=" * 60)
