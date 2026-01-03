# comprehensive_struct_fix.py
# Comprehensive fix for all broken struct/enum types after migration
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\comprehensive_struct_fix.py").read())

import unreal

# ============================================================================
# CONFIGURATION
# ============================================================================

BLUEPRINTS_TO_FIX = {
    "B_StatusEffect": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "W_HUD": "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "W_LevelUp": "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
}

# Variables that need type fixes (from JSON exports)
# Old Blueprint struct path -> New C++ struct class name
BROKEN_VARIABLE_FIXES = {
    "B_StatusEffect": {
        "TickStatChange": "FStatusEffectTick",
        "OneShotStatChange": "FStatusEffectStatChanges",
        "TickAndOneShotStatChange": "FStatusEffectOneShotAndTick",
        "StatsToAdjust": "FStatChange",  # TArray<FStatChange>
    },
}

# Pin value fixes still needed (NewEnumerator values in Make nodes)
PIN_VALUE_FIXES = [
    # (Blueprint name, pin name pattern, old value, new value)
    ("W_LevelUp", "ValueType", "NewEnumerator0", "0"),
    ("W_LevelUp", "ValueType", "NewEnumerator1", "1"),
]

# ============================================================================
# HELPER FUNCTIONS
# ============================================================================

def check_cpp_types():
    """Verify C++ types are available."""
    print("\n" + "="*70)
    print("  CHECKING C++ TYPES")
    print("="*70)

    types_to_check = [
        ("ESLFValueType", "enum"),
        ("FStatChange", "struct"),
        ("FLevelChangeData", "struct"),
        ("FStatusEffectTick", "struct"),
        ("FStatusEffectStatChanges", "struct"),
        ("FStatusEffectOneShotAndTick", "struct"),
    ]

    all_found = True
    for type_name, type_kind in types_to_check:
        try:
            type_class = getattr(unreal, type_name, None)
            if type_class:
                print(f"  [OK] {type_name} ({type_kind})")
            else:
                print(f"  [MISSING] {type_name} ({type_kind})")
                all_found = False
        except Exception as e:
            print(f"  [ERROR] {type_name}: {e}")
            all_found = False

    return all_found


def load_blueprint(bp_path):
    """Load a Blueprint asset."""
    asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not asset:
        return None

    bp = unreal.Blueprint.cast(asset)
    if not bp:
        bp = unreal.WidgetBlueprint.cast(asset)
    return bp


def reconstruct_all_nodes(bp):
    """Try ReconstructAllNodes - may fix issues via CoreRedirects."""
    try:
        fixer = unreal.BlueprintFixerLibrary
        result = fixer.reconstruct_all_nodes(bp)
        return result
    except Exception as e:
        print(f"    ReconstructAllNodes failed: {e}")
        return False


def fix_pin_values(bp, pin_name, old_value, new_value):
    """Fix pin values by name and value."""
    try:
        fixer = unreal.BlueprintFixerLibrary
        count = fixer.fix_pins_by_name_and_value(bp, pin_name, old_value, new_value)
        return count
    except Exception as e:
        print(f"    FixPinsByNameAndValue failed: {e}")
        return 0


def get_broken_nodes_info(bp):
    """Get information about broken nodes."""
    try:
        fixer = unreal.BlueprintFixerLibrary
        node_info = fixer.get_detailed_node_info(bp, "")

        broken = []
        for info in node_info:
            info_str = str(info)
            if any(kw in info_str for kw in ["<unknown struct>", "bad enum", "Wildcard"]):
                broken.append(info_str)

        return broken
    except Exception as e:
        print(f"    GetDetailedNodeInfo failed: {e}")
        return []


def analyze_variables(bp, bp_name):
    """Analyze Blueprint variables for broken struct types."""
    broken_vars = []

    try:
        new_vars = bp.get_editor_property("new_variables")
        for var in new_vars:
            var_name = str(var.get_editor_property("var_name"))
            var_type = var.get_editor_property("var_type")

            pin_category = str(var_type.get_editor_property("pin_category"))
            pin_subcategory_obj = var_type.get_editor_property("pin_sub_category_object")

            type_path = ""
            if pin_subcategory_obj:
                type_path = str(pin_subcategory_obj.get_path_name()) if hasattr(pin_subcategory_obj, 'get_path_name') else str(pin_subcategory_obj)

            # Check if broken (still references deleted Blueprint struct)
            if pin_category == "struct" and "/Game/SoulslikeFramework/Structures/" in type_path:
                broken_vars.append({
                    "name": var_name,
                    "type_path": type_path,
                    "expected_cpp": BROKEN_VARIABLE_FIXES.get(bp_name, {}).get(var_name, "Unknown")
                })
    except Exception as e:
        print(f"    Error analyzing variables: {e}")

    return broken_vars


# ============================================================================
# MAIN FIX LOGIC
# ============================================================================

def fix_blueprint(bp_name, bp_path):
    """Attempt to fix a single Blueprint."""
    print(f"\n{'='*70}")
    print(f"  FIXING: {bp_name}")
    print(f"{'='*70}")

    bp = load_blueprint(bp_path)
    if not bp:
        print(f"  ERROR: Could not load Blueprint")
        return False, []

    issues = []
    fixed_count = 0

    # Step 1: Analyze current state
    print(f"\n  Analyzing...")
    broken_vars = analyze_variables(bp, bp_name)
    broken_nodes = get_broken_nodes_info(bp)

    if broken_vars:
        print(f"  Found {len(broken_vars)} broken variable(s):")
        for var in broken_vars:
            print(f"    - {var['name']}: {var['type_path']}")
            print(f"      Expected C++: {var['expected_cpp']}")
            issues.append(f"Variable '{var['name']}' needs manual fix to use {var['expected_cpp']}")

    if broken_nodes:
        print(f"  Found {len(broken_nodes)} broken node(s)")

    # Step 2: Try ReconstructAllNodes
    print(f"\n  Attempting ReconstructAllNodes...")
    if reconstruct_all_nodes(bp):
        print(f"    ReconstructAllNodes completed")

    # Step 3: Fix any remaining pin values
    for fix_bp, pin_name, old_val, new_val in PIN_VALUE_FIXES:
        if fix_bp == bp_name:
            count = fix_pin_values(bp, pin_name, old_val, new_val)
            if count > 0:
                print(f"  Fixed {count} pins: {pin_name} {old_val} -> {new_val}")
                fixed_count += count

    # Step 4: Re-check after fixes
    broken_nodes_after = get_broken_nodes_info(bp)

    if broken_nodes_after:
        print(f"\n  Still broken after auto-fix: {len(broken_nodes_after)} node(s)")
        for node in broken_nodes_after[:5]:  # Show first 5
            issues.append(f"Broken node: {node[:100]}...")

    # Step 5: Save if changes made
    if fixed_count > 0:
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"  Saved Blueprint")
        except Exception as e:
            print(f"  Save warning: {e}")

    return len(issues) == 0, issues


def generate_manual_fix_guide():
    """Generate detailed manual fix instructions."""
    guide = """
================================================================================
  MANUAL FIX GUIDE
================================================================================

The following issues cannot be automatically fixed and require manual attention:

--------------------------------------------------------------------------------
B_StatusEffect - BROKEN VARIABLES (4)
--------------------------------------------------------------------------------
These variables reference deleted Blueprint structs and need to be recreated:

1. TickStatChange (FStatusEffectTick)
   - Delete the existing variable
   - Create new variable: Name="TickStatChange", Type="FStatusEffectTick" (C++)
   - Properties: Category="Runtime (Do Not Edit)", Instance Editable=false

2. OneShotStatChange (FStatusEffectStatChanges)
   - Delete the existing variable
   - Create new variable: Name="OneShotStatChange", Type="FStatusEffectStatChanges" (C++)

3. TickAndOneShotStatChange (FStatusEffectOneShotAndTick)
   - Delete the existing variable
   - Create new variable: Name="TickAndOneShotStatChange", Type="FStatusEffectOneShotAndTick" (C++)

4. StatsToAdjust (TArray<FStatChange>)
   - Delete the existing variable
   - Create new variable: Name="StatsToAdjust", Type=Array of "FStatChange" (C++)

After recreating variables, the Break nodes referencing them should start working.

--------------------------------------------------------------------------------
W_HUD - BROKEN ENUM COMPARISONS (4)
--------------------------------------------------------------------------------
Equal (Enum) nodes have Wildcard type instead of ESLFValueType.

For each broken node:
1. Find the "Equal (Enum)" node with Wildcard pins
2. Delete the node
3. Add new "Equal (Enum)" node
4. Set the enum type to "ESLFValueType"
5. Reconnect the A and B pins
6. Reconnect the output Boolean

Nodes are in functions related to stat display (checking CurrentValue vs MaxValue).

--------------------------------------------------------------------------------
W_LevelUp - BROKEN MAKE/BREAK FLevelChangeData (6)
--------------------------------------------------------------------------------
Break nodes (4):
- NodeGuid: 92E4BA3E4F98F0ACCB45A9BD485233DD
- NodeGuid: C6F9B8B64C1053123B094A9D562DD6EF
- NodeGuid: D0D1E078492679F22B74C8BE6540A91E
(plus one more)

Make nodes (2):
- NodeGuid: EEED9A294BFE2DE75B89C196D9EF72A1
- NodeGuid: 36E82A5947429857C8B9C3A3ACA748BB

For each broken Break/Make node:
1. Note what pins are connected to what
2. Delete the broken node
3. Search for "Break Level Change Data" or "Make Level Change Data" (C++ version)
4. Add the new node
5. Reconnect pins:
   - FLevelChangeData (struct input/output)
   - Change (double)
   - ValueType (ESLFValueType)

FLevelChangeData C++ struct has:
- Change: double
- ValueType: ESLFValueType

--------------------------------------------------------------------------------
ALTERNATIVE: RESTORE AND MIGRATE PROPERLY
--------------------------------------------------------------------------------
If manual fixes are too tedious, consider:

1. Close Unreal Editor
2. Restore from backup:
   rm -rf "C:/scripts/SLFConversion/Content"
   cp -r "C:/scripts/SLFConversion_Migration/Backups/blueprint_only/Content" "C:/scripts/SLFConversion/"
3. Open Editor (Blueprint structs will exist again)
4. Run a migration script that:
   - Creates new variables with C++ types
   - Copies data from old variables
   - Updates node references
   - Then deletes old Blueprint struct files

================================================================================
"""
    return guide


def main():
    print("\n" + "="*70)
    print("  COMPREHENSIVE STRUCT/ENUM FIX")
    print("="*70)

    # Check prerequisites
    if not check_cpp_types():
        print("\n[ERROR] C++ types not available. Rebuild C++ and restart editor.")
        return

    # Try to fix each Blueprint
    all_success = True
    all_issues = []

    for bp_name, bp_path in BLUEPRINTS_TO_FIX.items():
        success, issues = fix_blueprint(bp_name, bp_path)
        if not success:
            all_success = False
            all_issues.extend([(bp_name, issue) for issue in issues])

    # Summary
    print("\n" + "="*70)
    print("  SUMMARY")
    print("="*70)

    if all_success:
        print("  All Blueprints fixed successfully!")
        print("  Compile all Blueprints and test.")
    else:
        print(f"  {len(all_issues)} issue(s) remain that need manual attention:")
        for bp_name, issue in all_issues[:10]:
            print(f"    [{bp_name}] {issue[:70]}...")

        print(generate_manual_fix_guide())


if __name__ == "__main__" or True:
    main()
