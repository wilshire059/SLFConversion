# diagnose_and_fix_structs.py
# Comprehensive diagnostic and fix for broken struct types after migration
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\diagnose_and_fix_structs.py").read())

import unreal

# Blueprints to check/fix
BLUEPRINTS_TO_CHECK = [
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
]

# Expected variable types (from JSON exports)
# Maps variable name -> expected C++ struct path
EXPECTED_VARIABLE_TYPES = {
    "B_StatusEffect": {
        "TickStatChange": "/Script/SLFConversion.FStatusEffectTick",
        "OneShotStatChange": "/Script/SLFConversion.FStatusEffectStatChanges",
        "TickAndOneShotStatChange": "/Script/SLFConversion.FStatusEffectOneShotAndTick",
        "StatsToAdjust": "/Script/SLFConversion.FStatChange",  # This is TArray<FStatChange>
    },
    "W_LevelUp": {
        # Add any struct variables here
    }
}

# Old Blueprint struct paths -> New C++ struct paths
STRUCT_REDIRECTS = {
    "/Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectTick.FStatusEffectTick": "/Script/SLFConversion.FStatusEffectTick",
    "/Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectStatChanges.FStatusEffectStatChanges": "/Script/SLFConversion.FStatusEffectStatChanges",
    "/Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectOneShotAndTick.FStatusEffectOneShotAndTick": "/Script/SLFConversion.FStatusEffectOneShotAndTick",
    "/Game/SoulslikeFramework/Structures/Stats/FStatChange.FStatChange": "/Script/SLFConversion.FStatChange",
    "/Game/SoulslikeFramework/Structures/Stats/FLevelChangeData.FLevelChangeData": "/Script/SLFConversion.FLevelChangeData",
    "/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType": "/Script/SLFConversion.ESLFValueType",
}


def check_cpp_structs_exist():
    """Verify that the C++ structs are available."""
    print("\n" + "="*70)
    print("  CHECKING C++ STRUCT AVAILABILITY")
    print("="*70)

    structs_to_check = [
        "FStatusEffectTick",
        "FStatusEffectStatChanges",
        "FStatusEffectOneShotAndTick",
        "FStatChange",
        "FLevelChangeData",
        "ESLFValueType",
    ]

    all_found = True
    for struct_name in structs_to_check:
        try:
            struct_class = getattr(unreal, struct_name, None)
            if struct_class:
                print(f"  [OK] {struct_name} - found")
            else:
                print(f"  [MISSING] {struct_name} - NOT FOUND")
                all_found = False
        except Exception as e:
            print(f"  [ERROR] {struct_name} - {e}")
            all_found = False

    return all_found


def diagnose_blueprint(bp_path):
    """Diagnose a single Blueprint for broken struct references."""
    bp_name = bp_path.split('/')[-1]
    print(f"\n{'='*70}")
    print(f"  DIAGNOSING: {bp_name}")
    print(f"{'='*70}")

    asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not asset:
        print(f"  ERROR: Could not load asset")
        return None

    # Cast to Blueprint
    bp = unreal.Blueprint.cast(asset)
    if not bp:
        bp = unreal.WidgetBlueprint.cast(asset)
    if not bp:
        print(f"  ERROR: Not a Blueprint type")
        return None

    results = {
        "name": bp_name,
        "path": bp_path,
        "variables": [],
        "broken_nodes": [],
        "issues": []
    }

    # Check variables
    print(f"\n  Variables:")
    try:
        # Get new variables from the Blueprint
        new_vars = bp.get_editor_property("new_variables")
        for var in new_vars:
            var_name = str(var.get_editor_property("var_name"))
            var_type = var.get_editor_property("var_type")

            # Get type info
            pin_category = str(var_type.get_editor_property("pin_category"))
            pin_subcategory_obj = var_type.get_editor_property("pin_sub_category_object")
            container_type = str(var_type.get_editor_property("container_type"))

            type_path = ""
            if pin_subcategory_obj:
                type_path = str(pin_subcategory_obj.get_path_name()) if hasattr(pin_subcategory_obj, 'get_path_name') else str(pin_subcategory_obj)

            is_broken = False
            # Check if this is a broken struct reference
            if pin_category == "struct" and type_path:
                if "/Game/SoulslikeFramework/Structures/" in type_path:
                    is_broken = True
                    results["issues"].append(f"Variable '{var_name}' uses deleted Blueprint struct: {type_path}")
                elif not type_path or type_path == "None":
                    is_broken = True
                    results["issues"].append(f"Variable '{var_name}' has missing type")

            status = "[BROKEN]" if is_broken else "[OK]"
            print(f"    {status} {var_name}: {pin_category} | {type_path} | container: {container_type}")

            results["variables"].append({
                "name": var_name,
                "category": pin_category,
                "type_path": type_path,
                "container": container_type,
                "is_broken": is_broken
            })
    except Exception as e:
        print(f"    Error reading variables: {e}")
        results["issues"].append(f"Error reading variables: {e}")

    # Check for broken nodes using BlueprintFixerLibrary
    try:
        fixer = unreal.BlueprintFixerLibrary
        node_info = fixer.get_detailed_node_info(bp, "")

        broken_keywords = ["<unknown struct>", "bad enum", "Wildcard"]
        print(f"\n  Potentially Broken Nodes:")
        for info in node_info:
            info_str = str(info)
            for keyword in broken_keywords:
                if keyword.lower() in info_str.lower():
                    print(f"    [BROKEN] {info}")
                    results["broken_nodes"].append(info_str)
                    break
    except Exception as e:
        print(f"    Error checking nodes: {e}")

    # Summary
    print(f"\n  Summary:")
    print(f"    Total variables: {len(results['variables'])}")
    print(f"    Broken variables: {sum(1 for v in results['variables'] if v['is_broken'])}")
    print(f"    Broken nodes found: {len(results['broken_nodes'])}")
    print(f"    Issues: {len(results['issues'])}")

    return results


def main():
    print("\n" + "="*70)
    print("  STRUCT MIGRATION DIAGNOSTIC")
    print("="*70)

    # First check if C++ structs are available
    if not check_cpp_structs_exist():
        print("\n[ERROR] Not all C++ structs are available!")
        print("Make sure C++ code is compiled and the editor was restarted.")
        return

    # Diagnose each Blueprint
    all_results = []
    for bp_path in BLUEPRINTS_TO_CHECK:
        result = diagnose_blueprint(bp_path)
        if result:
            all_results.append(result)

    # Overall summary
    print("\n" + "="*70)
    print("  OVERALL SUMMARY")
    print("="*70)

    total_issues = sum(len(r["issues"]) for r in all_results)
    total_broken_vars = sum(sum(1 for v in r["variables"] if v["is_broken"]) for r in all_results)
    total_broken_nodes = sum(len(r["broken_nodes"]) for r in all_results)

    print(f"  Blueprints checked: {len(all_results)}")
    print(f"  Total broken variables: {total_broken_vars}")
    print(f"  Total broken nodes: {total_broken_nodes}")
    print(f"  Total issues: {total_issues}")

    if total_issues > 0:
        print("\n  All Issues:")
        for r in all_results:
            if r["issues"]:
                print(f"\n    {r['name']}:")
                for issue in r["issues"]:
                    print(f"      - {issue}")

    print("\n" + "="*70)
    print("  DIAGNOSTIC COMPLETE")
    print("="*70)

    if total_issues > 0:
        print("\n  RECOMMENDATION:")
        print("  The Blueprint struct files were deleted before variables were migrated.")
        print("  Options:")
        print("    1. Restore from backup, fix variables FIRST, then delete struct files")
        print("    2. Manually recreate variables with C++ types")
        print("    3. Use a C++ function to programmatically fix variable types")


if __name__ == "__main__" or True:
    main()
