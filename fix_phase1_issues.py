"""
fix_phase1_issues.py

Fix B_BaseCharacter and W_DebugWindow after Phase 1 StatManager migration.
The issue: Get nodes reference old Blueprint variables that now conflict with C++ properties.

This script tries to delete the problematic variables and reconstruct nodes.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\fix_phase1_issues.py").read())
"""

import unreal

print("=" * 60)
print("  FIXING PHASE 1 MIGRATION ISSUES")
print("=" * 60)

# The problematic variables that were moved to C++
VARS_TO_DELETE = ["SpeedAsset", "SelectedClassAsset", "StatTable"]

BLUEPRINTS_TO_FIX = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
    "/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow",
]

bfl = unreal.BlueprintFixerLibrary

for bp_path in BLUEPRINTS_TO_FIX:
    bp_name = bp_path.split('/')[-1]
    print(f"\n--- {bp_name} ---")

    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"  [FAIL] Could not load")
        continue

    # List current variables
    print("  Current variables:")
    try:
        vars_list = bfl.list_blueprint_variables(bp)
        for v in vars_list:
            if any(name in v for name in VARS_TO_DELETE):
                print(f"    {v} <- WILL DELETE")
            else:
                print(f"    {v}")
    except Exception as e:
        print(f"    Could not list: {e}")

    # Try to delete the problematic variables
    print("\n  Deleting problematic variables...")
    for var_name in VARS_TO_DELETE:
        try:
            deleted = bfl.delete_blueprint_variable(bp, var_name)
            if deleted:
                print(f"    [OK] Deleted {var_name}")
            else:
                print(f"    [SKIP] {var_name} not found or couldn't delete")
        except Exception as e:
            print(f"    [SKIP] {var_name}: {e}")

    # Reconstruct all nodes
    print("\n  Reconstructing all nodes...")
    try:
        bfl.reconstruct_all_nodes(bp)
        print("    [OK] Nodes reconstructed")
    except Exception as e:
        print(f"    [ERROR] {e}")

    # Compile
    print("\n  Compiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("    [OK] Compiled successfully")
    except Exception as e:
        print(f"    [ERROR] {e}")

    # Save
    print("\n  Saving...")
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print("    [OK] Saved")
    except Exception as e:
        print(f"    [ERROR] {e}")

print("\n" + "=" * 60)
print("  DONE")
print("=" * 60)
print("""
If errors persist, manually fix in editor:
1. Open the Blueprint (B_BaseCharacter or W_DebugWindow)
2. Find nodes with red X error markers
3. Delete the broken Get nodes
4. The properties are now on AC_StatManager (inherited from C++)
   - Get a reference to AC_StatManager component
   - Drag off to get SpeedAsset or SelectedClassAsset
""")
