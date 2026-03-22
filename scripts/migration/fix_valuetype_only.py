# fix_valuetype_only.py
# TARGETED fix - only fixes pins named "ValueType"
# Does NOT affect other enums like E_Direction, E_MovementType, etc.
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\fix_valuetype_only.py").read())

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
    print("  E_ValueType TARGETED Pin Fixer")
    print("  Only fixes pins named 'ValueType'")
    print("="*70)

    try:
        fixer = unreal.BlueprintFixerLibrary
    except AttributeError:
        print("ERROR: BlueprintFixerLibrary not found! Rebuild C++.")
        return

    # Check if the new function exists
    if not hasattr(fixer, 'fix_pins_by_name_and_value'):
        print("ERROR: fix_pins_by_name_and_value function not found! Rebuild C++.")
        return

    print("Using C++ FixPinsByNameAndValue function")

    total_fixed = 0
    processed = 0

    for bp_path in BLUEPRINTS_TO_FIX:
        bp_name = bp_path.split('/')[-1]
        print(f"\n[{processed + 1}/{len(BLUEPRINTS_TO_FIX)}] {bp_name}")

        asset = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not asset:
            print(f"      ERROR: Could not load asset")
            processed += 1
            continue

        # Cast to Blueprint
        bp = unreal.Blueprint.cast(asset)
        if not bp:
            bp = unreal.WidgetBlueprint.cast(asset)
        if not bp:
            print(f"      ERROR: Not a Blueprint type")
            processed += 1
            continue

        # Fix only pins named "ValueType"
        # NewEnumerator0 -> 0 (CurrentValue)
        # NewEnumerator1 -> 1 (MaxValue)
        fixed0 = fixer.fix_pins_by_name_and_value(bp, "ValueType", "NewEnumerator0", "0")
        fixed1 = fixer.fix_pins_by_name_and_value(bp, "ValueType", "NewEnumerator1", "1")

        fixed_total = fixed0 + fixed1

        if fixed_total > 0:
            total_fixed += fixed_total
            print(f"      Fixed {fixed0} CurrentValue, {fixed1} MaxValue")

            # Save
            try:
                unreal.EditorAssetLibrary.save_asset(bp_path)
                print(f"      Saved")
            except Exception as e:
                print(f"      Save warning: {e}")
        else:
            print(f"      No ValueType pins needed fixing")

        processed += 1

    print("\n" + "="*70)
    print(f"  SUMMARY")
    print("="*70)
    print(f"Processed:    {processed} Blueprints")
    print(f"Fixed:        {total_fixed} ValueType pins")
    print("="*70)
    print("\nNext steps:")
    print("1. Close editor")
    print("2. Delete type files (E_ValueType, FStatChange, etc.)")
    print("3. Open editor")
    print("4. Manually fix Switch nodes in B_Stat")
    print("5. Save All")

if __name__ == "__main__" or True:
    main()
