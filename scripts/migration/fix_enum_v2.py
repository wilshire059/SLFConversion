# fix_enum_v2.py
# E_ValueType migration script with corrected paths for current project
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\fix_enum_v2.py").read())

import unreal

# Corrected paths for the current project structure
BLUEPRINTS_TO_FIX = [
    # Animation Notifies
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_AdjustStat",

    # Actions (in Data folder)
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_DrinkFlask_HP",

    # Buffs
    "/Game/SoulslikeFramework/Data/Buffs/Logic/B_Buff_AttackPower",

    # Status Effects
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Frostbite",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Plague",

    # Stats
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_ShowcaseEnemy_StatDisplay",

    # Components
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_Boss",

    # Widgets
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "/Game/SoulslikeFramework/Widgets/HUD/W_Boss_Healthbar",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_StatEntry_LevelUp",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status",
]

VALUE_MAP = {
    "NewEnumerator0": "CurrentValue",
    "NewEnumerator1": "MaxValue",
}

def get_all_graphs_from_blueprint(bp):
    """Get all graphs from a Blueprint using direct property access."""
    graphs = []

    # Try different properties that might contain graphs
    for prop_name in ['ubergraph_pages', 'function_graphs', 'macro_graphs', 'delegate_signature_graphs']:
        try:
            prop_graphs = bp.get_editor_property(prop_name)
            if prop_graphs:
                graphs.extend(prop_graphs)
        except Exception as e:
            pass

    # For WidgetBlueprints, also try widget_tree
    try:
        widget_tree = bp.get_editor_property('widget_tree')
        if widget_tree:
            # Widget trees might have event graphs
            pass
    except:
        pass

    return graphs

def fix_blueprint(bp_path):
    """Fix E_ValueType enum references in a Blueprint."""
    # Load the blueprint
    asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not asset:
        return 0, "Could not load asset"

    # Determine Blueprint type and cast
    bp = None
    bp_type = "Unknown"

    try:
        bp = unreal.Blueprint.cast(asset)
        if bp:
            bp_type = "Blueprint"
    except:
        pass

    if not bp:
        try:
            bp = unreal.WidgetBlueprint.cast(asset)
            if bp:
                bp_type = "WidgetBlueprint"
        except:
            pass

    if not bp:
        try:
            bp = unreal.AnimBlueprint.cast(asset)
            if bp:
                bp_type = "AnimBlueprint"
        except:
            pass

    if not bp:
        return 0, f"Not a Blueprint type (got {type(asset).__name__})"

    print(f"      Type: {bp_type}")

    fixed_count = 0
    graphs_checked = 0
    e_valuetype_pins = 0

    # Get all graphs
    graphs = get_all_graphs_from_blueprint(bp)

    if not graphs:
        # Try alternative: access the generated class
        print(f"      No graphs found via properties, checking generated class...")
        try:
            gen_class = bp.get_editor_property('generated_class')
            if gen_class:
                print(f"      Generated class: {gen_class.get_name()}")
        except Exception as e:
            print(f"      Could not access generated class: {e}")
        return 0, "No graphs accessible"

    print(f"      Found {len(graphs)} graphs")

    for graph in graphs:
        if not graph:
            continue
        graphs_checked += 1

        try:
            graph_name = graph.get_name()
            print(f"      Checking graph: {graph_name}")
        except:
            pass

        try:
            nodes = graph.get_editor_property('nodes')
        except Exception as e:
            print(f"        Could not get nodes: {e}")
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
                    pin_type = pin.get_editor_property('pin_type')
                    if not pin_type:
                        continue

                    sub_cat_obj = pin_type.get_editor_property('pin_sub_category_object')
                    if sub_cat_obj and 'E_ValueType' in str(sub_cat_obj):
                        e_valuetype_pins += 1
                        default_val = pin.get_editor_property('default_value')

                        if default_val in VALUE_MAP:
                            new_val = VALUE_MAP[default_val]
                            pin.set_editor_property('default_value', new_val)
                            fixed_count += 1
                            print(f"        Fixed: {default_val} -> {new_val}")
                        elif default_val and default_val not in ['', 'CurrentValue', 'MaxValue']:
                            print(f"        Unknown value: {default_val}")
                except Exception as e:
                    pass

    print(f"      Graphs checked: {graphs_checked}, E_ValueType pins found: {e_valuetype_pins}")

    if fixed_count > 0:
        # Compile
        try:
            unreal.KismetEditorUtilities.compile_blueprint(bp)
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
    print("  E_ValueType Enum Migration v2")
    print("  Corrected paths for current project")
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
            print(f"      No pins needed fixing")
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

    print("="*70)

if __name__ == "__main__" or True:
    main()
