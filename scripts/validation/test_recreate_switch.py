"""
test_recreate_switch.py

Test the delete/recreate approach for Switch on Enum nodes.
This tests migrating a single Switch node from Blueprint E_ValueType to C++ E_ValueType.

UPDATED: Also migrates ALL other pins in the graph to C++ enum first,
so the Switch node connections are type-compatible.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\test_recreate_switch.py").read())
"""

import unreal

print("=" * 70)
print("  TEST: Recreate Switch Node with C++ Enum")
print("=" * 70)

# Target: B_Stat has a Switch on E_ValueType in the "AdjustValue" function
BP_PATH = "/Game/SoulslikeFramework/Data/Stats/B_Stat"
GRAPH_NAME = "AdjustValue"

print(f"\nTarget Blueprint: {BP_PATH}")
print(f"Target Graph: {GRAPH_NAME}")

# Load the Blueprint
bp = unreal.load_asset(BP_PATH)
if not bp:
    print("\n[ERROR] Could not load Blueprint!")
    raise Exception("Blueprint not found")

print(f"[OK] Blueprint loaded: {bp.get_name()}")

# Get the fixer library
bfl = unreal.BlueprintFixerLibrary

# First, let's see what enum usages exist
print("\n--- Current E_ValueType Usage ---")
try:
    usages = bfl.find_enum_usage(bp, "E_ValueType")
    for usage in usages:
        print(f"  {usage}")
except Exception as e:
    print(f"  Error: {e}")

# STEP 1: Migrate ALL enum pin types first (except Switch nodes which we'll recreate)
# This uses MigrateEnumInBlueprint which updates PinSubCategoryObject
print("\n--- Step 1: Migrate ALL pin types to C++ enum ---")
try:
    updated = bfl.migrate_enum_in_blueprint(bp, "E_ValueType", "/Script/SLFConversion.E_ValueType", False)
    print(f"[OK] Updated {updated} pin types to C++ enum")
except Exception as e:
    print(f"[ERROR] {e}")
    import traceback
    traceback.print_exc()

# STEP 2: Recreate Switch node (delete old, create new with C++ enum)
print("\n--- Step 2: Recreate Switch Node ---")
try:
    result = bfl.test_recreate_switch_node(bp, GRAPH_NAME)
    if result:
        print("[SUCCESS] Switch node recreated!")
    else:
        print("[FAILED] TestRecreateSwitchNode returned false")
except Exception as e:
    print(f"[ERROR] {e}")
    import traceback
    traceback.print_exc()

# Check enum usages again
print("\n--- E_ValueType Usage After Migration ---")
try:
    usages = bfl.find_enum_usage(bp, "E_ValueType")
    for usage in usages:
        print(f"  {usage}")
except Exception as e:
    print(f"  Error: {e}")

# Try to compile
print("\n--- Compiling Blueprint ---")
try:
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    print("[OK] Blueprint compiled successfully!")
except Exception as e:
    print(f"[ERROR] Compile failed: {e}")

print("\n" + "=" * 70)
print("  TEST COMPLETE")
print("=" * 70)
print("""
If successful:
1. Open B_Stat in the editor
2. Navigate to "Get Value" function
3. Check if the Switch node is working (no "bad enum" error)
4. Check the Output Log for detailed info

If it works, we can expand this to handle ALL node types!
""")
