import unreal

# Migrate W_Resources - REPARENT ONLY with NO clearing at all
# This preserves the full widget tree AND all styling data
#
# Strategy:
# 1. Load Blueprint (no modifications)
# 2. Reparent to C++ class ONLY
# 3. Compile and save
#
# The C++ BlueprintNativeEvent functions will override Blueprint implementations
# but all visual styling should be preserved

bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"
cpp_path = "/Script/SLFConversion.W_Resources"

# Load Blueprint
bp = unreal.load_asset(bp_path)
if not bp:
    print(f"ERROR: Could not load {bp_path}")
else:
    print(f"Loaded: {bp.get_name()}")

    # Check current parent
    gen_class = bp.generated_class()
    if gen_class:
        parent = gen_class.get_super_class()
        print(f"Current parent: {parent.get_name() if parent else 'None'}")

    # Step 1: Reparent to C++ ONLY - NO clearing whatsoever
    print("Step 1: Reparenting to C++ (preserving ALL data)...")
    try:
        ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
        print(f"  Result: {ok}")
    except Exception as e:
        print(f"  Error: {e}")
        ok = False

    if ok:
        # Step 2: Compile and save
        print("Step 2: Compile and save...")
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
