# fix_enum_step1.py
# Step 1 of B_Stat migration: Fix E_ValueType enum references
#
# This script prepares Blueprints for E_ValueType deletion by:
# 1. Finding all pins that use E_ValueType
# 2. Converting default values: NewEnumerator0 -> CurrentValue, NewEnumerator1 -> MaxValue
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\fix_enum_step1.py").read())

import unreal

# All Blueprint paths that reference E_ValueType (from JSON analysis)
BLUEPRINTS_TO_FIX = [
    # Animation
    "/Game/SoulslikeFramework/Blueprints/Animation/Notifies/AN_AdjustStat",

    # Actions
    "/Game/SoulslikeFramework/Blueprints/Actions/B_Action",
    "/Game/SoulslikeFramework/Blueprints/Actions/B_Action_Dodge",
    "/Game/SoulslikeFramework/Blueprints/Actions/ConsumableActions/B_Action_DrinkFlask_HP",

    # Buffs/Effects
    "/Game/SoulslikeFramework/Blueprints/Buffs/B_Buff_AttackPower",
    "/Game/SoulslikeFramework/Blueprints/StatusEffects/B_StatusEffect",
    "/Game/SoulslikeFramework/Blueprints/StatusEffects/B_StatusEffect_Frostbite",
    "/Game/SoulslikeFramework/Blueprints/StatusEffects/B_StatusEffect_Plague",

    # Stats
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "/Game/SoulslikeFramework/Blueprints/Showcase/B_ShowcaseEnemy_StatDisplay",

    # Components
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
    "/Game/SoulslikeFramework/Blueprints/AI/Components/AC_AI_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/AI/Components/AC_AI_Boss",

    # Widgets
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "/Game/SoulslikeFramework/Widgets/HUD/W_Boss_Healthbar",
    "/Game/SoulslikeFramework/Widgets/LevelUp/W_LevelUp",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_LevelUp",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status",
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

    # Cast to Blueprint
    bp = unreal.Blueprint.cast(asset)
    if not bp:
        # Try WidgetBlueprint
        bp = unreal.WidgetBlueprint.cast(asset)
    if not bp:
        # Try AnimBlueprint
        try:
            bp = unreal.AnimBlueprint.cast(asset)
        except:
            pass
    if not bp:
        return 0, f"Not a Blueprint"

    fixed_count = 0

    # Get all graphs
    try:
        # For regular Blueprints
        if hasattr(unreal, 'BlueprintEditorLibrary'):
            graphs = unreal.BlueprintEditorLibrary.get_all_graphs(bp)
        else:
            graphs = []
    except Exception as e:
        return 0, f"Could not get graphs: {e}"

    for graph in graphs:
        if not graph:
            continue

        try:
            nodes = graph.get_editor_property('nodes')
        except:
            continue

        if not nodes:
            continue

        for node in nodes:
            if not node:
                continue

            try:
                pins = node.get_editor_property('pins')
            except:
                continue

            if not pins:
                continue

            for pin in pins:
                if not pin:
                    continue

                try:
                    # Get pin type info
                    pin_type = pin.get_editor_property('pin_type')
                    if not pin_type:
                        continue

                    # Check if this is an E_ValueType pin
                    sub_cat_obj = pin_type.get_editor_property('pin_sub_category_object')
                    if sub_cat_obj and 'E_ValueType' in str(sub_cat_obj):
                        # Get current default value
                        default_val = pin.get_editor_property('default_value')

                        if default_val in VALUE_MAP:
                            new_val = VALUE_MAP[default_val]
                            pin.set_editor_property('default_value', new_val)
                            fixed_count += 1
                            print(f"      Fixed: {default_val} -> {new_val}")
                except Exception as e:
                    # Skip pins we can't process
                    pass

    if fixed_count > 0:
        # Compile
        try:
            if hasattr(unreal, 'BlueprintEditorLibrary'):
                unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        except Exception as e:
            print(f"      Compile warning: {e}")

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
        except Exception as e:
            print(f"      Save warning: {e}")

    return fixed_count, None

def main():
    print("\n" + "="*70)
    print("  STEP 1: E_ValueType Enum Migration")
    print("  Converting NewEnumerator0/1 -> CurrentValue/MaxValue")
    print("="*70)

    total_fixed = 0
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
            print(f"      No enum pins found (may need manual check)")
        else:
            print(f"      Fixed {fixed} pin(s)")
            total_fixed += fixed

        processed += 1

    print("\n" + "="*70)
    print(f"  SUMMARY")
    print("="*70)
    print(f"Processed: {processed} Blueprints")
    print(f"Fixed: {total_fixed} enum pins")
    print(f"Errors: {len(errors)}")

    if errors:
        print("\nErrors encountered:")
        for name, err in errors:
            print(f"  - {name}: {err}")

    print("\n" + "="*70)
    print("  NEXT STEPS")
    print("="*70)
    print("1. Close Unreal Editor")
    print("2. Delete these files from disk:")
    print("   - Content/SoulslikeFramework/Enums/E_ValueType.uasset")
    print("   - Content/SoulslikeFramework/Structures/Stats/FStatChange.uasset")
    print("   - Content/SoulslikeFramework/Structures/Stats/FStatChangePercent.uasset")
    print("   - Content/SoulslikeFramework/Structures/Stats/FLevelChangeData.uasset")
    print("3. Open Unreal Editor")
    print("4. Fix any remaining compilation errors manually")
    print("="*70)

if __name__ == "__main__" or True:
    main()
