import unreal

# Migrate W_Resources - Clear graphs, keep variables, then reparent
#
# The bp_only Blueprint has compile errors due to old pins (Buff, etc.)
# We MUST clear graphs before reparenting. Use clear_graphs_keep_variables
# to preserve widget tree references stored as variables.

bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"
cpp_path = "/Script/SLFConversion.W_Resources"

# Load Blueprint
bp = unreal.load_asset(bp_path)
if not bp:
    print(f"ERROR: Could not load {bp_path}")
else:
    print(f"Loaded: {bp.get_name()}")

    # Get file size before
    import os
    size_before = os.path.getsize("C:/scripts/SLFConversion/Content/SoulslikeFramework/Widgets/HUD/W_Resources.uasset")
    print(f"File size before: {size_before} bytes")

    # Step 1: Clear graphs ONLY - KEEP VARIABLES for widget tree references
    print("Step 1: Clearing graphs (keeping variables)...")
    try:
        unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
        print("  Done")
    except Exception as e:
        print(f"  Warning: {e}")

    # Step 2: Reparent to C++
    print("Step 2: Reparenting to C++...")
    try:
        ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
        print(f"  Result: {ok}")
    except Exception as e:
        print(f"  Error: {e}")
        ok = False

    if ok:
        # Step 3: Compile and save
        print("Step 3: Compile and save...")
        try:
            result = unreal.SLFAutomationLibrary.compile_and_save(bp)
            print(f"  Result: {result}")
        except Exception as e:
            print(f"  Error: {e}")

    # Get file size after
    size_after = os.path.getsize("C:/scripts/SLFConversion/Content/SoulslikeFramework/Widgets/HUD/W_Resources.uasset")
    print(f"File size after: {size_after} bytes")
    print(f"Size reduction: {size_before - size_after} bytes ({100 - (size_after * 100 / size_before):.1f}%)")

    print("Done!")
