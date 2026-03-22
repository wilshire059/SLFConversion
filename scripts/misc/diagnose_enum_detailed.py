# diagnose_enum_detailed.py
# Shows EXACTLY what enum pin values look like
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\diagnose_enum_detailed.py").read())

import unreal

TEST_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
]

def main():
    print("\n" + "="*70)
    print("  DETAILED ENUM PIN DIAGNOSTIC")
    print("="*70)

    try:
        fixer = unreal.BlueprintFixerLibrary
        print("BlueprintFixerLibrary found!")
    except AttributeError:
        print("ERROR: BlueprintFixerLibrary not found!")
        return

    for bp_path in TEST_BLUEPRINTS:
        bp_name = bp_path.split('/')[-1]
        print(f"\n{'='*60}")
        print(f"  {bp_name}")
        print(f"{'='*60}")

        asset = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not asset:
            print(f"  ERROR: Could not load asset")
            continue

        bp = unreal.Blueprint.cast(asset)
        if not bp:
            bp = unreal.WidgetBlueprint.cast(asset)
        if not bp:
            print(f"  ERROR: Not a Blueprint type")
            continue

        # Use C++ function to get all enum pins
        print("\n  Looking for E_ValueType pins:")
        pins = fixer.get_enum_pin_info(bp, "E_ValueType")
        if pins:
            for pin_info in pins:
                print(f"    {pin_info}")
        else:
            print("    No E_ValueType pins found")

        # Also look for ESLFValueType (C++ enum name)
        print("\n  Looking for ESLFValueType pins:")
        pins = fixer.get_enum_pin_info(bp, "ESLFValueType")
        if pins:
            for pin_info in pins:
                print(f"    {pin_info}")
        else:
            print("    No ESLFValueType pins found")

        # Look for ANY byte/enum pins
        print("\n  Looking for ALL enum pins (empty search):")
        pins = fixer.get_enum_pin_info(bp, "")
        if pins:
            for pin_info in pins[:20]:  # Limit to first 20
                print(f"    {pin_info}")
            if len(pins) > 20:
                print(f"    ... and {len(pins) - 20} more")
        else:
            print("    No enum pins found at all")

    print("\n" + "="*70)
    print("  DIAGNOSTIC COMPLETE")
    print("="*70)

if __name__ == "__main__" or True:
    main()
