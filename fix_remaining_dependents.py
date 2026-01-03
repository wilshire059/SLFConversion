"""
fix_remaining_dependents.py

Fix the remaining dependent Blueprints after AC_InputBuffer migration.
The main migration succeeded - this just fixes the dependent BPs that had wrong paths.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\fix_remaining_dependents.py").read())
"""

import unreal

# Correct paths for dependent Blueprints
DEPENDENT_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_TryGuard",
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_InputBuffer",
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
]

print("=" * 70)
print("  FIXING REMAINING DEPENDENT BLUEPRINTS")
print("=" * 70)

for bp_path in DEPENDENT_BLUEPRINTS:
    bp_name = bp_path.split('/')[-1]
    print(f"\n--- Processing: {bp_name} ---")

    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"  [WARN] Could not load Blueprint at {bp_path}")
        continue

    # Get pre-fix diagnostics
    print("  Pre-fix function calls:")
    info = unreal.BlueprintFixerLibrary.get_function_call_info(bp)
    input_buffer_calls = [i for i in info if "InputBuffer" in i]
    for call in input_buffer_calls[:5]:
        print(f"    {call}")
    if not input_buffer_calls:
        print("    (none found)")

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
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print("  [OK] Compiled and saved")
    except Exception as e:
        print(f"  [WARN] Compile/save error: {e}")

    # Post-fix diagnostics
    print("  Post-fix function calls:")
    info = unreal.BlueprintFixerLibrary.get_function_call_info(bp)
    input_buffer_calls = [i for i in info if "InputBuffer" in i]
    for call in input_buffer_calls[:5]:
        print(f"    {call}")
    if not input_buffer_calls:
        print("    (none found)")

print("\n" + "=" * 70)
print("  DONE - Now test in Play mode!")
print("=" * 70)
