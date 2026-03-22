# fix_enum_cpp.py
# E_ValueType migration using C++ BlueprintFixerLibrary
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\fix_enum_cpp.py").read())

import unreal

# Corrected paths for current project
BLUEPRINTS_TO_FIX = [
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

def main():
    print("\n" + "="*70)
    print("  E_ValueType Enum Migration (C++ Version)")
    print("  Using BlueprintFixerLibrary")
    print("="*70)

    # Check if BlueprintFixerLibrary is available
    try:
        fixer = unreal.BlueprintFixerLibrary
        print("BlueprintFixerLibrary found!")
    except AttributeError:
        print("ERROR: BlueprintFixerLibrary not found!")
        print("Make sure C++ code is compiled with the new FixEnumPinValues function.")
        return

    total_fixed = 0
    processed = 0
    errors = []

    for bp_path in BLUEPRINTS_TO_FIX:
        bp_name = bp_path.split('/')[-1]
        print(f"\n[{processed + 1}/{len(BLUEPRINTS_TO_FIX)}] {bp_name}")

        # Load the blueprint
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

        # Fix NewEnumerator0 -> CurrentValue
        fixed0 = fixer.fix_enum_pin_values(bp, "E_ValueType", "NewEnumerator0", "CurrentValue")

        # Fix NewEnumerator1 -> MaxValue
        fixed1 = fixer.fix_enum_pin_values(bp, "E_ValueType", "NewEnumerator1", "MaxValue")

        fixed_total = fixed0 + fixed1

        if fixed_total > 0:
            print(f"      Fixed {fixed_total} pins ({fixed0} CurrentValue, {fixed1} MaxValue)")
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

    print("\n" + "="*70)
    if total_fixed > 0:
        print("  SUCCESS! Pin values updated.")
        print("  Next steps:")
        print("  1. File -> Save All")
        print("  2. Close Unreal Editor")
        print("  3. Delete Blueprint enum/struct files from disk")
        print("  4. Add CoreRedirects to DefaultEngine.ini")
        print("  5. Reopen Editor")
    else:
        print("  No pins needed fixing (may already be correct)")
    print("="*70)

if __name__ == "__main__" or True:
    main()
