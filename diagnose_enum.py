# diagnose_enum.py
# Shows all enum pins in affected Blueprints and their current values
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\diagnose_enum.py").read())

import unreal

# All Blueprints that had errors
BLUEPRINTS_TO_CHECK = [
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Frostbite",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Plague",
    "/Game/SoulslikeFramework/Data/Buffs/Logic/B_Buff_AttackPower",
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_AdjustStat",
]

def main():
    print("\n" + "="*70)
    print("  E_ValueType Enum Pin Diagnostics - BEFORE STATE")
    print("="*70)

    try:
        fixer = unreal.BlueprintFixerLibrary
    except AttributeError:
        print("ERROR: BlueprintFixerLibrary not found!")
        return

    total_pins = 0

    for bp_path in BLUEPRINTS_TO_CHECK:
        bp_name = bp_path.split('/')[-1]

        asset = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not asset:
            print(f"\n[{bp_name}] ERROR: Could not load")
            continue

        # Try different Blueprint casts
        bp = unreal.Blueprint.cast(asset)
        if not bp:
            bp = unreal.WidgetBlueprint.cast(asset)
        if not bp:
            print(f"\n[{bp_name}] ERROR: Not a Blueprint type")
            continue

        # Search for E_ValueType pins
        results = fixer.get_enum_pin_info(bp, "E_ValueType")

        if results:
            print(f"\n[{bp_name}] Found {len(results)} E_ValueType pins:")
            for r in results:
                print(f"  {r}")
                total_pins += 1
        else:
            print(f"\n[{bp_name}] No E_ValueType pins found")

    print("\n" + "="*70)
    print(f"  TOTAL: {total_pins} E_ValueType pins across all Blueprints")
    print("="*70)

if __name__ == "__main__" or True:
    main()
