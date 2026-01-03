# diagnose_broken_nodes.py
# Diagnoses all broken nodes in the affected Blueprints
# Shows exactly what needs to be fixed
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\diagnose_broken_nodes.py").read())

import unreal

BLUEPRINTS_TO_CHECK = [
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
]

def main():
    print("\n" + "="*80)
    print("  BROKEN NODE DIAGNOSTIC")
    print("="*80)

    try:
        fixer = unreal.BlueprintFixerLibrary
    except AttributeError:
        print("ERROR: BlueprintFixerLibrary not found!")
        return

    for bp_path in BLUEPRINTS_TO_CHECK:
        bp_name = bp_path.split('/')[-1]
        print(f"\n{'='*60}")
        print(f"  {bp_name}")
        print(f"{'='*60}")

        asset = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not asset:
            print(f"  ERROR: Could not load asset")
            continue

        # Cast to Blueprint
        bp = unreal.Blueprint.cast(asset)
        if not bp:
            bp = unreal.WidgetBlueprint.cast(asset)
        if not bp:
            print(f"  ERROR: Not a Blueprint type")
            continue

        # Get detailed node info for all graphs
        node_info = fixer.get_detailed_node_info(bp, "")

        # Filter for problematic nodes
        problem_keywords = [
            "Switch", "Equal", "Break", "Make",
            "bad", "Enum", "FStatChange", "FLevelChangeData",
            "E_ValueType", "ESLFValueType", "Wildcard"
        ]

        print(f"\n  Nodes of interest:")
        found_any = False
        for info in node_info:
            info_str = str(info)
            # Check if this node might be problematic
            for keyword in problem_keywords:
                if keyword.lower() in info_str.lower():
                    print(f"    {info}")
                    found_any = True
                    break

        if not found_any:
            print("    (No obvious problem nodes found)")

        # Also get all pins with values to check for enum issues
        print(f"\n  Pins with enum-related values:")
        all_pins = fixer.get_all_pin_values(bp)
        enum_pins = [p for p in all_pins if any(kw in str(p) for kw in ["Enumerator", "ValueType", "CurrentValue", "MaxValue", "ESLFValueType"])]
        for pin in enum_pins[:20]:  # Limit output
            print(f"    {pin}")
        if len(enum_pins) > 20:
            print(f"    ... and {len(enum_pins) - 20} more")
        if not enum_pins:
            print("    (None found)")

    print("\n" + "="*80)
    print("  DIAGNOSTIC COMPLETE")
    print("="*80)
    print("\nPlease paste the full output so we can analyze it.")

if __name__ == "__main__" or True:
    main()
