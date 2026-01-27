import unreal

# Migrate W_Resources - Proper widget migration
# Widget tree is preserved; only event graphs are cleared
#
# CRITICAL: Must use clear_graphs_keep_variables_no_compile NOT clear_all_blueprint_logic_no_compile
# because Widget Blueprints store widget element references as Blueprint variables.
# Removing variables breaks the widget tree!
#
# Strategy:
# 1. Clear graphs ONLY (keep variables for widget tree references)
# 2. Reparent to C++ class
# 3. Compile and save

bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"
cpp_path = "/Script/SLFConversion.W_Resources"

# Load Blueprint
bp = unreal.load_asset(bp_path)
if not bp:
    print(f"ERROR: Could not load {bp_path}")
else:
    print(f"Loaded: {bp.get_name()}")

    # Step 1: Clear graphs ONLY - KEEP VARIABLES for widget tree references!
    # This is the key difference from clear_all_blueprint_logic_no_compile
    print("Step 1: Clearing graphs ONLY (keeping variables for widget tree)...")
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

    # Step 3: Compile and save
    print("Step 3: Compile and save...")
    try:
        result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        print(f"  Result: {result}")
    except Exception as e:
        print(f"  Error: {e}")

    print("Done!")

# Check file size
import os
size = os.path.getsize("C:/scripts/SLFConversion/Content/SoulslikeFramework/Widgets/HUD/W_Resources.uasset")
print(f"Final file size: {size} bytes")
