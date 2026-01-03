# fix_all_broken_structs.py
# Automated fix for all broken struct/enum types after migration
#
# PREREQUISITES:
# 1. Rebuild C++ code (new BlueprintFixerLibrary functions)
# 2. Restart Unreal Editor
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\fix_all_broken_structs.py").read())

import unreal

# ============================================================================
# CONFIGURATION - Based on JSON exports and diagnostic logs
# ============================================================================

# Variable type fixes for B_StatusEffect
# Format: (variable_name, c++_struct_path, is_array)
B_STATUSEFFECT_VARIABLE_FIXES = [
    ("TickStatChange", "/Script/SLFConversion.FStatusEffectTick", False),
    ("OneShotStatChange", "/Script/SLFConversion.FStatusEffectStatChanges", False),
    ("TickAndOneShotStatChange", "/Script/SLFConversion.FStatusEffectOneShotAndTick", False),
    ("StatsToAdjust", "/Script/SLFConversion.FStatChange", True),  # TArray<FStatChange>
]

# Blueprints with paths
BLUEPRINTS = {
    "B_StatusEffect": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "W_HUD": "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "W_LevelUp": "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
}


def check_prerequisites():
    """Check if new C++ functions are available."""
    print("\n" + "="*70)
    print("  CHECKING PREREQUISITES")
    print("="*70)

    try:
        fixer = unreal.BlueprintFixerLibrary

        # Check new functions
        required_funcs = [
            'fix_variable_struct_type',
            'get_variable_type_info',
            'delete_node_by_guid',
            'get_broken_node_guids',
        ]

        all_ok = True
        for func_name in required_funcs:
            if hasattr(fixer, func_name):
                print(f"  [OK] {func_name}")
            else:
                print(f"  [MISSING] {func_name}")
                all_ok = False

        if not all_ok:
            print("\n  ERROR: C++ functions not available!")
            print("  Please rebuild C++ and restart the editor.")
            return False

        return True

    except AttributeError:
        print("  ERROR: BlueprintFixerLibrary not found!")
        print("  Please rebuild C++ and restart the editor.")
        return False


def load_blueprint(bp_path):
    """Load a Blueprint asset."""
    asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not asset:
        return None

    bp = unreal.Blueprint.cast(asset)
    if not bp:
        bp = unreal.WidgetBlueprint.cast(asset)
    return bp


def fix_b_statuseffect():
    """Fix B_StatusEffect - broken struct variables."""
    print("\n" + "="*70)
    print("  FIXING B_StatusEffect")
    print("="*70)

    bp = load_blueprint(BLUEPRINTS["B_StatusEffect"])
    if not bp:
        print("  ERROR: Could not load Blueprint")
        return False

    fixer = unreal.BlueprintFixerLibrary

    # First, show current variable info
    print("\n  Current variable types:")
    var_info = fixer.get_variable_type_info(bp)
    for info in var_info:
        if "struct" in str(info).lower():
            print(f"    {info}")

    # Fix each broken variable
    print("\n  Fixing variables:")
    fixed_count = 0
    for var_name, struct_path, is_array in B_STATUSEFFECT_VARIABLE_FIXES:
        print(f"\n  Fixing {var_name} -> {struct_path.split('.')[-1]}...")
        if fixer.fix_variable_struct_type(bp, var_name, struct_path):
            print(f"    [OK] Variable type updated")
            fixed_count += 1
        else:
            print(f"    [FAIL] Could not fix variable")

    # Reconstruct all nodes to pick up the new types
    print("\n  Reconstructing all nodes...")
    fixer.reconstruct_all_nodes(bp)

    # Save
    if fixed_count > 0:
        try:
            unreal.EditorAssetLibrary.save_asset(BLUEPRINTS["B_StatusEffect"])
            print(f"\n  Saved B_StatusEffect ({fixed_count} variables fixed)")
        except Exception as e:
            print(f"\n  Save warning: {e}")

    return fixed_count > 0


def fix_w_hud():
    """Fix W_HUD - broken enum comparison nodes."""
    print("\n" + "="*70)
    print("  FIXING W_HUD")
    print("="*70)

    bp = load_blueprint(BLUEPRINTS["W_HUD"])
    if not bp:
        print("  ERROR: Could not load Blueprint")
        return False

    fixer = unreal.BlueprintFixerLibrary

    # Get broken nodes
    print("\n  Finding broken nodes...")
    broken = fixer.get_broken_node_guids(bp)

    if not broken:
        print("  No broken nodes found!")
        return True

    print(f"  Found {len(broken)} broken node(s):")
    for info in broken:
        print(f"    {info}")

    # For enum comparison nodes, deleting and recreating is needed
    # But we can try reconstructing first
    print("\n  Attempting ReconstructAllNodes...")
    fixer.reconstruct_all_nodes(bp)

    # Check again
    broken_after = fixer.get_broken_node_guids(bp)
    if len(broken_after) < len(broken):
        print(f"  Reduced from {len(broken)} to {len(broken_after)} broken nodes")
    else:
        print("  ReconstructAllNodes did not fix the nodes")
        print("\n  MANUAL FIX REQUIRED:")
        print("  The Equal (Enum) nodes lost their type reference.")
        print("  For each broken node:")
        print("    1. Delete the broken 'Equal (Enum)' node")
        print("    2. Add new 'Equal (Enum)' node")
        print("    3. The type should auto-detect from connected pins")
        print("    4. If not, check what E_ValueType/ESLFValueType comparisons exist")

    # Save
    try:
        unreal.EditorAssetLibrary.save_asset(BLUEPRINTS["W_HUD"])
        print(f"\n  Saved W_HUD")
    except Exception as e:
        print(f"\n  Save warning: {e}")

    return len(broken_after) == 0


def fix_w_levelup():
    """Fix W_LevelUp - broken Make/Break FLevelChangeData nodes."""
    print("\n" + "="*70)
    print("  FIXING W_LevelUp")
    print("="*70)

    bp = load_blueprint(BLUEPRINTS["W_LevelUp"])
    if not bp:
        print("  ERROR: Could not load Blueprint")
        return False

    fixer = unreal.BlueprintFixerLibrary

    # First fix any remaining pin values
    print("\n  Fixing pin values...")
    fixed0 = fixer.fix_pins_by_name_and_value(bp, "ValueType_6_DD5AF4FE43302E61E614B9B7135A155B", "NewEnumerator0", "0")
    fixed1 = fixer.fix_pins_by_name_and_value(bp, "ValueType_6_DD5AF4FE43302E61E614B9B7135A155B", "NewEnumerator1", "1")
    print(f"  Fixed {fixed0 + fixed1} pin values")

    # Get broken nodes
    print("\n  Finding broken nodes...")
    broken = fixer.get_broken_node_guids(bp)

    if not broken:
        print("  No broken nodes found!")
        return True

    print(f"  Found {len(broken)} broken node(s):")
    for info in broken:
        print(f"    {info}")

    # Try reconstructing
    print("\n  Attempting ReconstructAllNodes...")
    fixer.reconstruct_all_nodes(bp)

    # Check again
    broken_after = fixer.get_broken_node_guids(bp)
    if len(broken_after) < len(broken):
        print(f"  Reduced from {len(broken)} to {len(broken_after)} broken nodes")

    if broken_after:
        print("\n  MANUAL FIX REQUIRED:")
        print("  Break/Make <unknown struct> nodes need manual replacement:")
        print("    1. Note what each pin connects to")
        print("    2. Delete the broken node")
        print("    3. Search for 'Break Level Change Data' or 'Make Level Change Data'")
        print("    4. Add the C++ version (should show 'Change' and 'Value Type' pins)")
        print("    5. Reconnect the pins")
        print("\n  FLevelChangeData struct has:")
        print("    - Change (double)")
        print("    - ValueType (ESLFValueType)")

    # Save
    try:
        unreal.EditorAssetLibrary.save_asset(BLUEPRINTS["W_LevelUp"])
        print(f"\n  Saved W_LevelUp")
    except Exception as e:
        print(f"\n  Save warning: {e}")

    return len(broken_after) == 0


def main():
    print("\n" + "="*70)
    print("  AUTOMATED STRUCT MIGRATION FIX")
    print("="*70)

    # Check prerequisites
    if not check_prerequisites():
        return

    # Fix each Blueprint
    results = {}

    results["B_StatusEffect"] = fix_b_statuseffect()
    results["W_HUD"] = fix_w_hud()
    results["W_LevelUp"] = fix_w_levelup()

    # Summary
    print("\n" + "="*70)
    print("  SUMMARY")
    print("="*70)

    all_ok = True
    for bp_name, success in results.items():
        status = "[FIXED]" if success else "[NEEDS MANUAL FIX]"
        print(f"  {status} {bp_name}")
        if not success:
            all_ok = False

    if all_ok:
        print("\n  All Blueprints fixed!")
        print("  Compile all Blueprints and test.")
    else:
        print("\n  Some Blueprints need manual fixes.")
        print("  See the output above for specific instructions.")

    print("\n" + "="*70)


if __name__ == "__main__" or True:
    main()
