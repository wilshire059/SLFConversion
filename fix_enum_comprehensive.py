# fix_enum_comprehensive.py
# Comprehensive E_ValueType migration script based on JSON analysis
#
# This script fixes ALL E_ValueType enum pin values across ALL affected Blueprints
# by converting NewEnumerator0 -> CurrentValue and NewEnumerator1 -> MaxValue
#
# Based on JSON analysis findings:
#   - 22 unique Blueprint files reference E_ValueType
#   - 121 total E_ValueType pin references
#   - 3 structs use E_ValueType as a member: FStatChange, FStatChangePercent, FLevelChangeData
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\fix_enum_comprehensive.py").read())

import unreal

# Complete list of Blueprint paths from JSON analysis (22 files)
BLUEPRINTS_TO_FIX = [
    # Animation Notifies (3 E_ValueType refs)
    "/Game/SoulslikeFramework/Blueprints/Animation/Notifies/AN_AdjustStat",

    # Actions (8 E_ValueType refs total)
    "/Game/SoulslikeFramework/Blueprints/Actions/B_Action",                              # 2 refs
    "/Game/SoulslikeFramework/Blueprints/Actions/B_Action_Dodge",                        # 2 refs
    "/Game/SoulslikeFramework/Blueprints/Actions/ConsumableActions/B_Action_DrinkFlask_HP",  # 4 refs

    # Buffs/Effects (12 E_ValueType refs total)
    "/Game/SoulslikeFramework/Blueprints/Buffs/B_Buff_AttackPower",                      # 2 refs
    "/Game/SoulslikeFramework/Blueprints/StatusEffects/B_StatusEffect",                  # 6 refs
    "/Game/SoulslikeFramework/Blueprints/StatusEffects/B_StatusEffect_Frostbite",        # 2 refs
    "/Game/SoulslikeFramework/Blueprints/StatusEffects/B_StatusEffect_Plague",           # 2 refs

    # Stats (22 E_ValueType refs total)
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",                                        # 21 refs
    "/Game/SoulslikeFramework/Blueprints/Showcase/B_ShowcaseEnemy_StatDisplay",          # 1 ref

    # Components (44 E_ValueType refs total)
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",                   # 1 ref
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",                   # 12 refs
    "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",                # 1 ref
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",                     # 23 refs
    "/Game/SoulslikeFramework/Blueprints/AI/Components/AC_AI_CombatManager",             # 6 refs
    "/Game/SoulslikeFramework/Blueprints/AI/Components/AC_AI_Boss",                      # 1 ref

    # Widgets (29 E_ValueType refs total)
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",                                        # 7 refs
    "/Game/SoulslikeFramework/Widgets/HUD/W_Boss_Healthbar",                             # 1 ref
    "/Game/SoulslikeFramework/Widgets/LevelUp/W_LevelUp",                                # 18 refs
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry",                                # 1 ref
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_LevelUp",                        # 1 ref
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status",                         # 1 ref
]

# Value mapping
VALUE_MAP = {
    "NewEnumerator0": "CurrentValue",
    "NewEnumerator1": "MaxValue",
}

def fix_blueprint(bp_path):
    """
    Fix E_ValueType enum references in a Blueprint.
    Returns (fixed_count, error_message)
    """
    # Load the blueprint
    asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not asset:
        return 0, f"Could not load asset"

    # Determine Blueprint type
    bp = None
    bp_type = "Unknown"

    # Try regular Blueprint first
    bp = unreal.Blueprint.cast(asset)
    if bp:
        bp_type = "Blueprint"
    else:
        # Try WidgetBlueprint
        bp = unreal.WidgetBlueprint.cast(asset)
        if bp:
            bp_type = "WidgetBlueprint"
        else:
            # Try AnimBlueprint
            try:
                bp = unreal.AnimBlueprint.cast(asset)
                if bp:
                    bp_type = "AnimBlueprint"
            except:
                pass

    if not bp:
        return 0, f"Not a Blueprint type"

    fixed_count = 0
    graphs_checked = 0
    nodes_checked = 0
    pins_checked = 0
    e_valuetype_pins = 0

    # Get all graphs
    graphs = []
    try:
        if hasattr(unreal, 'BlueprintEditorLibrary'):
            graphs = unreal.BlueprintEditorLibrary.get_all_graphs(bp)
    except Exception as e:
        return 0, f"Could not get graphs: {e}"

    for graph in graphs:
        if not graph:
            continue
        graphs_checked += 1

        try:
            nodes = graph.get_editor_property('nodes')
        except:
            continue

        if not nodes:
            continue

        for node in nodes:
            if not node:
                continue
            nodes_checked += 1

            try:
                pins = node.get_editor_property('pins')
            except:
                continue

            if not pins:
                continue

            for pin in pins:
                if not pin:
                    continue
                pins_checked += 1

                try:
                    # Get pin type info
                    pin_type = pin.get_editor_property('pin_type')
                    if not pin_type:
                        continue

                    # Check if this is an E_ValueType pin
                    sub_cat_obj = pin_type.get_editor_property('pin_sub_category_object')
                    if sub_cat_obj and 'E_ValueType' in str(sub_cat_obj):
                        e_valuetype_pins += 1

                        # Get current default value
                        default_val = pin.get_editor_property('default_value')

                        if default_val in VALUE_MAP:
                            new_val = VALUE_MAP[default_val]
                            pin.set_editor_property('default_value', new_val)
                            fixed_count += 1
                            print(f"      Fixed: {default_val} -> {new_val}")
                        elif default_val:
                            # Value already converted or empty
                            print(f"      Skipped: {default_val} (already correct or empty)")

                except Exception as e:
                    # Skip pins we can't process
                    pass

    print(f"      Graphs: {graphs_checked}, Nodes: {nodes_checked}, Pins: {pins_checked}, E_ValueType pins: {e_valuetype_pins}")

    if fixed_count > 0:
        # Compile
        try:
            if hasattr(unreal, 'BlueprintEditorLibrary'):
                unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                print(f"      Compiled successfully")
        except Exception as e:
            print(f"      Compile warning: {e}")

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"      Saved successfully")
        except Exception as e:
            print(f"      Save warning: {e}")

    return fixed_count, None

def main():
    print("\n" + "="*70)
    print("  COMPREHENSIVE E_ValueType Enum Migration")
    print("  Based on JSON Analysis: 22 files, 121+ pin references")
    print("  Converting NewEnumerator0 -> CurrentValue")
    print("  Converting NewEnumerator1 -> MaxValue")
    print("="*70)

    total_fixed = 0
    total_e_valuetype_pins = 0
    processed = 0
    errors = []

    for bp_path in BLUEPRINTS_TO_FIX:
        bp_name = bp_path.split('/')[-1]
        print(f"\n[{processed + 1}/{len(BLUEPRINTS_TO_FIX)}] {bp_name}")

        fixed, error = fix_blueprint(bp_path)

        if error:
            print(f"      ERROR: {error}")
            errors.append((bp_name, error))
        elif fixed == 0:
            print(f"      No pins needed fixing (may already be correct)")
        else:
            total_fixed += fixed

        processed += 1

    print("\n" + "="*70)
    print(f"  SUMMARY")
    print("="*70)
    print(f"Processed:    {processed} Blueprints")
    print(f"Fixed:        {total_fixed} enum pin values")
    print(f"Errors:       {len(errors)}")

    if errors:
        print("\nErrors encountered:")
        for name, err in errors:
            print(f"  - {name}: {err}")

    print("\n" + "="*70)
    print("  MIGRATION STEP 1 COMPLETE")
    print("="*70)
    if total_fixed > 0:
        print("\nPin values have been updated. Now:")
        print("1. Save all modified Blueprints (File -> Save All)")
        print("2. Close Unreal Editor completely")
        print("3. Delete these Blueprint type files from disk:")
        print("   - Content/SoulslikeFramework/Enums/E_ValueType.uasset")
        print("   - Content/SoulslikeFramework/Structures/Stats/FStatChange.uasset")
        print("   - Content/SoulslikeFramework/Structures/Stats/FStatChangePercent.uasset")
        print("   - Content/SoulslikeFramework/Structures/Stats/FLevelChangeData.uasset")
        print("4. Reopen Unreal Editor")
        print("5. Fix any remaining compilation errors manually")
    else:
        print("\nNo pins needed fixing. Pins may already have correct values")
        print("or may need manual inspection.")
    print("="*70)

if __name__ == "__main__" or True:
    main()
