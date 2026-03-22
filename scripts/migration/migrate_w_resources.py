import unreal

# Migrate W_Resources - Clear FIRST, then reparent
# This preserves the widget tree better
bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"
cpp_path = "/Script/SLFConversion.W_Resources"

# Load Blueprint
bp = unreal.load_asset(bp_path)
if not bp:
    print(f"ERROR: Could not load {bp_path}")
else:
    print(f"Loaded: {bp.get_name()}")

    # Step 1: Clear graphs BEFORE reparenting (no compile)
    # This removes the conflicting Blueprint functions/events
    print("Step 1: Clearing graphs (keeping variables/widget tree)...")
    unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
    print("  Done")

    # Step 2: Reparent to C++
    print("Step 2: Reparenting to C++...")
    ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
    print(f"  Result: {ok}")

    if ok:
        # Step 3: Compile and save
        print("Step 3: Compile and save...")
        result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        print(f"  Result: {result}")

    print("Done!")

# Check file size
import os
size = os.path.getsize("C:/scripts/SLFConversion/Content/SoulslikeFramework/Widgets/HUD/W_Resources.uasset")
print(f"Final file size: {size} bytes")
