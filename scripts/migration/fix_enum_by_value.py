# fix_enum_by_value.py
# Fixes enum pin values by searching for pins with specific VALUE content
# This works regardless of pin type - searches ALL pins in the Blueprint
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\fix_enum_by_value.py").read())

import unreal

BLUEPRINTS_TO_FIX = [
    # Stats
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",

    # Animation Notifies
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_AdjustStat",

    # Actions
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_DrinkFlask_HP",

    # Buffs
    "/Game/SoulslikeFramework/Data/Buffs/Logic/B_Buff_AttackPower",

    # Status Effects
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Frostbite",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Plague",

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

    # Demo
    "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_ShowcaseEnemy_StatDisplay",
]

def main():
    print("\n" + "="*70)
    print("  E_ValueType Enum Pin Fixer (By Value)")
    print("  Searches for pins by VALUE content, not by type")
    print("="*70)

    try:
        fixer = unreal.BlueprintFixerLibrary
        print("BlueprintFixerLibrary found!")
    except AttributeError:
        print("ERROR: BlueprintFixerLibrary not found!")
        return

    # Check if the new function exists
    if not hasattr(fixer, 'fix_pins_by_value'):
        print("ERROR: fix_pins_by_value function not found! Rebuild C++.")
        return

    total_fixed = 0
    processed = 0
    errors = []

    for bp_path in BLUEPRINTS_TO_FIX:
        bp_name = bp_path.split('/')[-1]
        print(f"\n[{processed + 1}/{len(BLUEPRINTS_TO_FIX)}] {bp_name}")

        asset = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not asset:
            print(f"      ERROR: Could not load asset")
            errors.append((bp_name, "Could not load"))
            processed += 1
            continue

        # Cast to Blueprint
        bp = unreal.Blueprint.cast(asset)
        if not bp:
            bp = unreal.WidgetBlueprint.cast(asset)
        if not bp:
            try:
                bp = unreal.AnimBlueprint.cast(asset)
            except:
                pass
        if not bp:
            print(f"      ERROR: Not a Blueprint type")
            errors.append((bp_name, "Not a Blueprint"))
            processed += 1
            continue

        fixed_total = 0

        # Fix enum values by VALUE - use numeric values for C++ enum compatibility
        # NewEnumerator0 (CurrentValue) = 0
        # NewEnumerator1 (MaxValue) = 1
        fixed0 = fixer.fix_pins_by_value(bp, "NewEnumerator0", "0")
        fixed1 = fixer.fix_pins_by_value(bp, "NewEnumerator1", "1")

        if fixed0 > 0 or fixed1 > 0:
            print(f"      Fixed {fixed0} NewEnumerator0->0, {fixed1} NewEnumerator1->1")
            fixed_total = fixed0 + fixed1

        if fixed_total > 0:
            total_fixed += fixed_total

            # Compile
            try:
                unreal.KismetEditorUtilities.compile_blueprint(bp)
                print(f"      Compiled")
            except Exception as e:
                print(f"      Compile warning: {e}")

            # Save
            try:
                unreal.EditorAssetLibrary.save_asset(bp_path)
                print(f"      Saved")
            except Exception as e:
                print(f"      Save warning: {e}")
        else:
            print(f"      No pins needed fixing")

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
